#ifndef UTIL_EXCEPTIONS__INVALID_ACTION_ERROR_H_
#define UTIL_EXCEPTIONS__INVALID_ACTION_ERROR_H_
#include "custom_runtime_error.h"

namespace util {

class InvalidActionError : CustomRuntimeError
{
public:
	explicit InvalidActionError(const char* msg) : CustomRuntimeError(msg) {}

	explicit InvalidActionError(const std::string& msg)	: CustomRuntimeError(msg) {}

	InvalidActionError(const char* msg, va_list args) : CustomRuntimeError(msg, args) {}
};

}
#endif // UTIL_EXCEPTIONS__INVALID_ACTION_ERROR_H_
