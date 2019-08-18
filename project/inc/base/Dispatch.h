#pragma once

#include "../general/predefs.h"

namespace nr
{

class CommandQueue;

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
