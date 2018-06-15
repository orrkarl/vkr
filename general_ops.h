#ifndef MATH__GENERAL_OPS_H_
#define MATH__GENERAL_OPS_H_
#include <cmath>

inline int ipow(int base, int exp)
{
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

inline double round(const double value, const int digits)
{
	const double percision = ipow(10, digits);
	return round(value * percision) / percision;
}

#endif // MATH__GENERAL_OPS_H_
