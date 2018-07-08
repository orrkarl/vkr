#ifndef PREDEFS__ASSERET_H_
#define PREDEFS__ASSERET_H_

#include <cassert>

#ifdef _DEBUG
#define _ASSERTIONS_ENABLED_
#endif

#ifdef _ASSERTIONS_ENABLED_
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr)
#endif

#endif // PREDEFS__ASSERET_H_
