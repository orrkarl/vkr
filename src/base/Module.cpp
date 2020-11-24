#include "Module.h"
#include "../util/Containers.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace nr::base {

BuildFailedException::BuildFailedException(std::vector<std::string> buildLog)
    : m_buildLogs(std::move(buildLog)) {
}

const char* BuildFailedException::what() const noexcept {
    return "could not build OpenCL program";
}
const std::vector<std::string>& BuildFailedException::allBuildLogs() const {
    return m_buildLogs;
}

cl_program createProgram(cl_context context, const std::vector<std::string>& sources) {
    Status status = CL_SUCCESS;

    std::vector<const char*> sourcesRaw(sources.size());
    std::transform(sources.cbegin(), sources.cend(), sourcesRaw.begin(), std::mem_fn(&std::string::c_str));
    auto ret = clCreateProgramWithSource(context, static_cast<U32>(sources.size()), sourcesRaw.data(),
                                         nullptr, &status);
    if (status != CL_SUCCESS) {
        throw ModuleCreateException(status);
    }

    return ret;
}

Module::Module(Context& context, const std::string& code)
    : m_object(createProgram(context.rawHandle(), { code })) {
}

Module::Module(Context& context, const Sources& codes)
    : m_object(createProgram(context.rawHandle(), codes)) {
}

cl_program Module::rawHandle() {
    return m_object;
}

void Module::build(const Device& device, const char* options) {
    Devices devices { device };
    build(devices, options);
}

void Module::build(const Devices& devices, const char* options) {
    auto rawDevices = util::extractSizeLimited<Device, cl_device_id>(devices,
                                                                     std::mem_fn(&Device::rawHandle));

    auto status = clBuildProgram(m_object, static_cast<U32>(devices.size()), rawDevices.data(), options,
                                 nullptr, nullptr);
    if (status == CL_BUILD_PROGRAM_FAILURE) {
        std::vector<std::string> buildLogs(devices.size());
        std::transform(devices.cbegin(), devices.cend(), buildLogs.begin(),
                       [this](const Device& dev) { return getBuildLog(dev); });
        throw BuildFailedException(std::move(buildLogs));
    } else if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not build module");
    }
}

std::string Module::getBuildLog(const Device& device) const {
    size_t len = 0;

    auto status = clGetProgramBuildInfo(m_object, device.rawHandle(), CL_PROGRAM_BUILD_LOG, 0, nullptr, &len);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query build log length");
    }

    auto str = std::make_unique<char[]>(len);
    status = clGetProgramBuildInfo(m_object, device.rawHandle(), CL_PROGRAM_BUILD_LOG, len, str.get(),
                                   nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query build log");
    }

    return std::string(str.get(), len);
}

std::vector<std::string> Module::getKernelNames() const {
    size_t rawNamesSize = 0;
    auto status = clGetProgramInfo(m_object, CL_PROGRAM_KERNEL_NAMES, 0, nullptr, &rawNamesSize);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query kernel names size");
    }

    auto rawNames = std::make_unique<char[]>(rawNamesSize);
    status = clGetProgramInfo(m_object, CL_PROGRAM_KERNEL_NAMES, sizeof(char) * rawNamesSize, rawNames.get(),
                              nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query kernel names");
    }

    std::vector<std::string> ret;

    std::istringstream splittingNames(rawNames.get());
    std::string kernelName;
    while (std::getline(splittingNames, kernelName, ';')) {
        ret.push_back(kernelName);
    }

    return ret;
}

}
