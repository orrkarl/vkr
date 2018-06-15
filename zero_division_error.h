#ifndef UTIL_EXCEPTIONS__ZERO_DIVISION_ERROR_H_
#define UTIL_EXCEPTIONS__ZERO_DIVISION_ERROR_H_

#include "arithmetic_error.h"

#include <cstdarg>

namespace util {

class ZeroDivisionError : ArithmeticError
{
public:
	ZeroDivisionError() : ArithmeticError("cannot divide by zero") {}

	explicit ZeroDivisionError(const char* msg) : ArithmeticError(msg) {}

	explicit ZeroDivisionError(const std::string& msg) : ArithmeticError(msg) {}

	ZeroDivisionError(const char* msg, va_list args) : ArithmeticError(msg, args) {}
};

}
#endif // UTIL_EXCEPTIONS__ZERO_DIVISION_ERROR_H_
