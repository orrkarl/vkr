#ifndef PREDEFS__EXCEPT_H_
#define PREDEFS__EXCEPT_H_

#define _ENABLE_EXCEPTIONS_ true

#include <stdexcept>
#include "custom_runtime_error.h"
#include "arithmetic_error.h"
#include "zero_division_error.h"
#include "invalid_action_error.h"
#include "out_of_range.h"

#if _ENABLE_EXCEPTIONS_
#define THROW(ex) throw ex;
#define THROW_IF(cond, ex) if (cond) { throw ex; } else {}
#else
#define THROW(ex) 
#define THROW_IF(cond, ex)
#endif // _ENABLE_EXCEPTIONS_

#endif // PREDEFS__EXCEPT_H_