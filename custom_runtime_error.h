#ifndef UTIL_EXCEPTIONS__CUSTOM_RUNTIME_ERROR_H_
#define UTIL_EXCEPTIONS__CUSTOM_RUNTIME_ERROR_H_

#include <stdexcept>
#include <string>

#include "format.h"

namespace util {

class CustomRuntimeError : std::runtime_error
{
public:
	explicit CustomRuntimeError(const char* msg) : std::runtime_error(msg) {}

	explicit CustomRuntimeError(const std::string& msg) : std::runtime_error(msg) {}

	CustomRuntimeError(const char* msg, va_list args) : std::runtime_error(format(msg, args)) {}
};

}

#endif // UTIL_EXCEPTIONS__CUSTOM_RUNTIME_ERROR_H_
