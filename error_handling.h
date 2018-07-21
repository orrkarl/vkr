#ifndef PREDEFS__ASSERET_H_
#define PREDEFS__ASSERET_H_

#include <cassert>

#ifdef _ASSERTIONS_ENABLED_
#define ASSERT(expr) assert(expr)
#define COND_ASSERT(enable, expr) if (enable) {} else ASSERT(expr)
#else
#define ASSERT(expr)
#define COND_ASSERT(enable, expr)
#endif

#endif // PREDEFS__ASSERET_H_
