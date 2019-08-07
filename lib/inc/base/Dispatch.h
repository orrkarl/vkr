#pragma once

#include "../general/predefs.h"

namespace nr
{

class CommandQueue;

template <typename Consumer>
class Dispatch
{
public:
	cl_status operator()(const nr::CommandQueue& q)
	{
		return static_cast<Consumer*>(this)->consume(q);
	}
};

}
