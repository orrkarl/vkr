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

	VertexShader();
    
	cl_status load();

    Buffer points;
    Buffer near;
    Buffer far;
    Buffer result;
};

}

}