#ifndef UTIL_EXCEPTIONS__OUT_OF_RANGE_H_
#define UTIL_EXCEPTIONS__OUT_OF_RANGE_H_
#include "custom_runtime_error.h"

namespace util {

class OutOfRange : CustomRuntimeError
{
public:
	static OutOfRange GetInstance(const char* msg, ...)
	{
		va_list va;
		va_start(va, msg);
		OutOfRange ret(msg, va);
		va_end(va);
		return ret;
	}

	explicit OutOfRange(const char* msg)
		: CustomRuntimeError(msg)
	{
	}

	explicit OutOfRange(const std::string& msg)
		: CustomRuntimeError(msg)
	{
	}

	OutOfRange(const char* msg, va_list args)
		: CustomRuntimeError(msg, args)
	{
	}
};

}

#endif // UTIL_EXCEPTIONS__OUT_OF_RANGE_H_