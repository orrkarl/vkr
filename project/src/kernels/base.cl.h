/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Containing general utilities for NR device code
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

namespace nr
{

namespace detail
{

namespace clcode
{

/**
 * @brief NR basic device code
 * 
 * This piece of code contains most utility functions, basic type definitions and macros.
 * In addition, some of the utility functions also have Kernel versions, to be used only in testing.
 */
NRAPI extern const char* base;

}

}

}