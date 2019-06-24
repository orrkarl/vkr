#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "../rendering/Render.h"

namespace nr
{

namespace __internal
{

struct NRAPI VertexShader : Kernel
{
	VertexShader(Module module, cl_status* err = nullptr);
    
	cl_status load();

    Buffer<nr_float> points;
    Buffer<nr_float> near;
    Buffer<nr_float> far;
    Buffer<nr_float> result;
};

}

}