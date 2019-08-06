#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

template <typename Parameters, typename Inputs, typename Outputs>
class Stage
{
public:
	Inputs inputs;
	Outputs outputs;

protected:
	Parameters parameters;
};

}

}

