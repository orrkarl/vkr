#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"

#include "../pipeline/BinRasterizer.h"
#include "../pipeline/FineRasterizer.h"
#include "../pipeline/SimplexReducer.h"
#include "../pipeline/VertexShader.h"

namespace nr
{

namespace __internal
{

class Pipeline
{
public:
	Pipeline();

	cl_status setRenderDimension();

	void setInputBuffer();
protected:
	cl_status render();

private:
	nr::__internal::BinRasterizer;
	nr::__internal::FineRasterizer;
	nr::__internal::SimplexReducer;
	nr::__internal::VertexShader;
};

}


}
