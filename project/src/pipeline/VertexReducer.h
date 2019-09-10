/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief implements the vertex reducer kernel
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "../kernels/vertex_reducer.cl.h"
#include "../utils/StandardDispatch.h"
#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

/**
 * @brief Accessing the first stage in the raster pipeline
 * 
 * The vertex reducer is the first stage in the raster pipeline. It's job is to reduce each vertex dimensions - 
 * take the homogenous N dimensional input and transform it into a 2D position with some extra data (such as 'depth'), to be used in 
 * the next pipeline operations.
 * @par
 * As of right now, this stage runs repeating perspective projections on each dimension.
 * @par
 * This stage takes 4 inputs: the vertex input buffer, the perspective near and far planes and an output target buffer.
 * It executes naively - each vertex gets it's own work item to process.
 * Each simplex will be copied to the target buffer and it's vertecies will be reduced; simplex order in the input buffer is preserved.
 * @par
 * The vertecies are reduced to a D3D style 'NDC' form - the X,Y coordinates are between -1 and 1, while every other coordinate is between 0 and 1.
 * 
 * @note As of right now, no clipping is done at all - but this will change soon
 */
class NRAPI VertexReducer : public StandardDispatch<1, Buffer, cl_float3, cl_float3, Buffer>
{
public:
	/**
	 * @brief Find the vertex reduce kernel in a module and creates accessor to it
	 * 
	 * @param module kernel containing module
	 * @param[out] err internal OpenCL call status
	 */
	VertexReducer(const Module& module, cl_status& err);

	VertexReducer();

	/**
	 * @brief Set the kernel expected input buffer
	 * 
	 * The buffer is expected to contain plain simplexes: N points of homogenous N dimension (N + 1 coordinates)
	 * @param in data buffer
	 * @return internal OpenCL call status
	 */
	cl_status setSimplexInputBuffer(const Buffer& in);

	/**
	 * @brief Set the perspective near plane
	 * 
	 * The buffer is expected to contain N elements (N floats each).
	 * Each element specifices the lower bound of the repeating perspective projections at it's coordinate
	 * @param near perspective near plane
	 * @return internal OpenCL call status
	 */
	cl_status setNearPlane(const cl_float3& near);

	/**
	 * @brief Set the perspective far plane
	 * 
	 * The buffer is expected to contain N elements (N floats each).
	 * Each element specifices the upper bound of the repeating perspective projections at it's coordinate
	 * @param far perspective far plane
	 * @return internal OpenCL call status
	 */
	cl_status setFarPlane(const cl_float3& far);

	/**
	 * @brief Set the simplex output buffer
	 * 
	 * Sets the target buffer for this stage
	 * @param out simplex target buffer
	 * @return internal OpenCL call status
	 */
	cl_status setSimplexOutputBuffer(const Buffer& out);

	/**
	 * @brief Set the execution range
	 * 
	 * Since there is a one to one mapping between vertecies and work items, this just creates a single line of work items
	 * , with length the same as the vertex count
	 * @param vertexCount vertex count in batch
	 */
	void setExecutionRange(const nr_uint vertexCount);

	using TypesafeKernel::operator cl_kernel;
};

}

}