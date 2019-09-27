/**
* @file
* @author Orr Karl (karlor041@gmail.com)
* @brief implements the vertex post processing kernel
* @version 0.7.0
* @date 2019-09-27
*
* @copyright Copyright (c) 2019
*
*/
#pragma once

#include "../predefs.h"

#include "../kernels/vertex_post_processor.cl.h"
#include "../utils/StandardDispatch.h"
#include "../rendering/Render.h"

namespace nr
{

namespace detail
{


class NRAPI VertexPostProcessor : public StandardDispatch<1, Buffer, Buffer>
{
public:
	VertexPostProcessor(const Module& module, cl_status& err);

	VertexPostProcessor();

	cl_status setTriangleInputBuffer(const Buffer& triangleBuffer);

	cl_status setTriangleRecordOutputBuffer(const Buffer& recordBuffer);

	void setExecutionRange(const nr_uint triangleCount);
};

}

}