/*
 * General mathematical operations that are too important to be left on the 'math' namespace and library only.
 */

#ifndef MATH__GENERAL_OPS_H_
#define MATH__GENERAL_OPS_H_
#include <cmath>
#include "error_handling.h"

namespace util {

const double kDoublePercision = 0.0000000001;
const float kPercision = 0.00001f;
const int kPercisionDigit = 4;

//Integer power
inline int ipow(int base, int exp)
{
	ASSERT(exp > 0);

	auto result = 1;
	for (;;)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		if (!exp)
			break;
		base *= base;
	}

	return result;
}

// Rounds a float up to digits after decimal point
inline double round_float(const float value, const int digits)
{
	const auto percision = ipow(kPercisionDigit, digits);
	return round(value * percision) / percision;
}

// Rounds a float up to digits after decimal point
inline double round_float(const float value)
{
	return round_float(value, kPercisionDigit);
}

inline bool equals_rounded(const float num1, const float num2, const float tolerance)
{
	return abs(num1 - num2) <= tolerance;
}

inline bool equals_rounded(const float num1, const float num2)
{
	return equals_rounded(num1, num2, kPercision);
}

inline double round_double(const double value, const int digits)
{
	const double percision = ipow(kPercisionDigit, digits);
	return round(value * percision) / percision;
}

inline double round_double(const double value)
{
	return round_double(value, kPercisionDigit);
}

inline bool equals_rounded(const double num1, const double num2, const double tolerance)
{
	return abs(num1 - num2) <= tolerance;
}

inline bool equals_rounded(const double num1, const double num2)
{
	return equals_rounded(num1, num2, kDoublePercision);
}

}



#endif // MATH__GENERAL_OPS_H_
