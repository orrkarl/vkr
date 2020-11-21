#include <iostream>

#include <catch2/catch.hpp>

#include <base/CommandQueue.h>
#include <base/Context.h>
#include <base/Module.h>
#include <base/Platform.h>

using namespace nr;
using namespace nr::base;

constexpr auto TEST_KERNEL = R"__CODE__(

kernel void ved_add(global int* a, global int* b, global int* dest) {
    int id = get_global_id(0);
    dest[id] = a[id] + 2 * b[id];
}

)__CODE__";

constexpr auto BUFFER_SIZE = 100;

TEST_CASE("Base", "[sanity]") {
    auto platforms = Platform::getAvailablePlatforms();
    REQUIRE(!platforms.empty());
    auto platform = platforms[0];

    Context ctx(platform, DeviceTypeFlag::All);

    // Creating devices
    auto devices = platform.getDevicesByType(DeviceTypeFlag::All);
    REQUIRE(!devices.empty());

    // Creating command queues
    std::vector<CommandQueue> queues;
    for (auto& dev : devices) {
        queues.emplace_back(ctx, dev, CommandQueue::PropertyFlag::ProfilingEnabled);
    }

    // Creating test OpenCL module
    Module testModule(ctx, TEST_KERNEL);
    {
        std::vector<DeviceView> devViews;
        for (auto& dev : devices) {
            devViews.emplace_back(dev.view());
        }
        testModule.build(devViews, "-D DEBUG -w -Werror -cl-std=CL1.2");
    }

    std::vector<I32> hostA(BUFFER_SIZE);
    std::vector<I32> hostB(BUFFER_SIZE);
    std::vector<I32> expectedHostC(BUFFER_SIZE);

    {
        std::random_device rdv;
        std::mt19937 engine(rdv());
        std::uniform_int_distribution<I32> dist(std::numeric_limits<I32>::min() / 3,
                                                std::numeric_limits<I32>::max() / 3);

        for (size_t i = 0; i < BUFFER_SIZE; ++i) {
            hostA[i] = dist(engine);
            hostB[i] = dist(engine);
            expectedHostC[i] = hostA[i] + 2 * hostB[i];
        }
    }

    Buffer devA(ctx, Memory::AccessFlag::HostWriteOnly | Memory::AccessFlag::ReadOnly,
                Memory::AllocateFlag::None, BUFFER_SIZE * sizeof(I32), nullptr);
    Buffer devB(ctx, Memory::AccessFlag::HostWriteOnly | Memory::AccessFlag::ReadOnly,
                Memory::AllocateFlag::CopyHostPtr, BUFFER_SIZE * sizeof(I32), hostB.data());

    std::vector<Buffer> devCBuffers;
    std::vector<std::vector<I32>> hostCBuffers;
    for (size_t i = 0; i < devices.size(); ++i) {
        devCBuffers.emplace_back(ctx, Memory::AccessFlag::HostReadOnly | Memory::AccessFlag::WriteOnly,
                                 Memory::AllocateFlag::None, BUFFER_SIZE * sizeof(I32), nullptr);
        hostCBuffers.push_back(std::vector<I32>(BUFFER_SIZE));
    }

    auto waitBWrite = queues[0].enqueueBufferWriteCommand(devA, BUFFER_SIZE * sizeof(I32), hostA.data(), {});

    std::vector<Kernel> kernels;
    std::vector<ApiEvent> kernelsDoneEvents;
    std::vector<EventView> kernelsDoneViews;
    NDExecutionRange<1> range { .global = NDRange<1> { BUFFER_SIZE }, .local = NDRange<1> { BUFFER_SIZE } };
    for (size_t i = 0; i < devices.size(); ++i) {
        Kernel test(testModule, testModule.getKernelNames()[0]);
        test.setArg(0, devA);
        test.setArg(1, devB);
        test.setArg(2, devCBuffers[i]);
        kernelsDoneEvents.push_back(
            queues[i].enqueueKernelCommand(test, range, { waitBWrite.view() }, { 0 }));
        kernelsDoneEvents.push_back(
            queues[i].enqueueBufferReadCommand(devCBuffers[i], BUFFER_SIZE * sizeof(I32),
                                               hostCBuffers[i].data(), { kernelsDoneEvents.back().view() }));
        kernelsDoneViews.push_back(kernelsDoneEvents.back().view());
        kernels.push_back(std::move(test));
    }

    Event::await(kernelsDoneViews);

    for (auto& hostC : hostCBuffers) {
        REQUIRE(hostC == expectedHostC);
    }
}
