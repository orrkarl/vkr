#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"

#include "../pipeline/BinRasterizer.h"
#include "../pipeline/FineRasterizer.h"
#include "../pipeline/SimplexReducer.h"
#include "../pipeline/VertexShader.h"

namespace nr
{

namespace detail
{

class NRAPI Pipeline
{
public:
	Pipeline();

	cl_status setRenderDimension();

	void setInputBuffer(Buffer in);
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
