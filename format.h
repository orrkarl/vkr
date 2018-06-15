#ifndef UTIL__FORMAT_H_
#define UTIL__FORMAT_H_
#include <cstdarg>

namespace util {

int format(char** str_p, const char* fmt, ...);
int format(char** str_p, const char* fmt, va_list args);

char* format(const char* fmt, ...);
char* format(const char* fmt, va_list args);

}
#endif // UTIL__FORMAT_H_