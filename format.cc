#include "format.h"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

namespace util {

int format(char** str_p, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	const auto ret = format(str_p, fmt, va);
	va_end(va);
	return ret;
}

int format(char** str_p, const char* fmt, va_list args)
{
	const auto len = _vscprintf(fmt, args);
	if (len == -1)
	{
		return -1;
	}
	const auto size = size_t(len) + 1;

	const auto buf = static_cast<char*>(malloc(size));
	if (buf == nullptr)
	{
		return -1;
	}

		const auto ret = _vsnprintf(buf, size_t(len), fmt, args);
	if (ret == -1)
	{
		free(buf);
		return -1;
	}

	buf[len] = 0;
	*str_p = buf;

	return ret;
}

char* format(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	const auto ret = format(fmt, va);
	va_end(va);
	return ret;
}

char* format(const char* fmt, va_list args)
{
	const auto str_p = static_cast<char**>(malloc(sizeof(char*)));
	if (str_p == nullptr) return nullptr;

		const auto count = format(str_p, fmt, args);
	if (count == -1) return nullptr;
	const auto ret = *str_p;
	free(str_p);

	return ret;
}

}
