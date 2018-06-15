#ifndef UTIL_EXCEPTIONS__ARITHMETIC_EXCEPTION_H_
#define UTIL_EXCEPTIONS__ARITHMETIC_EXCEPTION_H_
#include "custom_runtime_error.h"

namespace util {

class ArithmeticError : CustomRuntimeError
{
public:
	explicit ArithmeticError(const char* msg) : CustomRuntimeError(msg) {}

	explicit ArithmeticError(const std::string& msg) : CustomRuntimeError(msg) {}

	ArithmeticError(const char* msg, va_list args) : CustomRuntimeError(msg, args) {}
};

}

#endif // UTIL_EXCEPTIONS__ARITHMETIC_EXCEPTION_H_
