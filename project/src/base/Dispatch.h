/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief An extension for the CommandQueue API
 * @version 0.6.0
 * @date 2019-08-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "../predefs.h"

namespace nr
{

class CommandQueue;

/**
 * @brief CRTP class to enable a user-defined command
 * 
 * Here CRTP = Curiously recurring template pattern. In order to implement this class properly, 
 * the user needs to derive from it and pass his own type as the tmeplate argument. The user type needs to implement 
 * the following function: cl_status consume(const nr::CommandQueue& q) const
 * refer to StandardDispatch for an example
 * @tparam Consumer The underlying user type
 */
template <typename Consumer>
class Dispatch
{
public:
	cl_status operator()(const nr::CommandQueue& q) const
	{
		return static_cast<const Consumer*>(this)->consume(q);
	}
};

}
