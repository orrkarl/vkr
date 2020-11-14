#include "Module.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <string>

namespace nr::base {

BuildFailedException::BuildFailedException(Status status)
    : CLApiException(status, "could not build program") {
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

void Module::build(DeviceView device, const char* options) {
    Devices devices { device };
    build(devices, options);
}

void Module::build(Devices& devices, const char* options) {
    if (devices.size() > std::numeric_limits<U32>::max()) {
        throw BuildFailedException(CL_INVALID_VALUE);
    }

    std::vector<cl_device_id> rawDevices(devices.size());
    std::transform(devices.begin(), devices.end(), rawDevices.begin(), std::mem_fn(&DeviceView::rawHandle));

    auto status = clBuildProgram(m_object, static_cast<U32>(devices.size()), rawDevices.data(), options,
                                 nullptr, nullptr);
    if (status != CL_SUCCESS) {
        throw BuildFailedException(status);
    }
}

std::string Module::getBuildLog(DeviceView device) const {
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
    char* rawNames = nullptr;
    auto status = clGetProgramInfo(m_object, CL_PROGRAM_KERNEL_NAMES, sizeof(rawNames), &rawNames, nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query kernel names");
    }

    std::vector<std::string> ret;

    std::istringstream splittingNames(rawNames);
    std::string kernelName;
    while (std::getline(splittingNames, kernelName, ';')) {
        ret.push_back(kernelName);
    }

    return ret;
}

}
