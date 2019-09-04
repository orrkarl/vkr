/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief implements the simplex reducer kernel
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "../kernels/simplex_reducer.cl.h"
#include "../utils/StandardDispatch.h"

namespace nr
{

namespace detail
{

/**
 * @brief Accessing the second stage in the raster pipeline
 * 
 * The simplex reduce is the second pipeline stage. It's job is similar in principle to the vertex reducer job - to reduce information complexity.
 * However, this stage doens't reduce dimensions - it reduces simplices, shapes which are very complex to render generally on a 2D plane, to
 * Triangles - whose fragments are far easier to generate and traverse.
 * 
 * @par
 * This stage only moves vertecies in memory. Each simplex is splitted into many (nC3 where n is the render dimension) triangles, each 
 * containing 3 of the origianl simplex vertecies
 * 
 * @par
 * This stage takes 2 inputs: the simplex input buffer and the triangles output buffer
 * 
 * @note The order of vertecies in each triangle is preserved, in relation to the original simplex	
 */
class SimplexReducer : public StandardDispatch<1, Buffer, Buffer>
{
public:
	/**
	 * @brief Find the simplex reduce kernel in a module and creates accessor to it
	 * 
	 * @param module kernel containing module
	 * @param[out] err internal OpenCL call status
	 */
	SimplexReducer(const Module& module, cl_status& err)
		: StandardDispatch(module, SIMPLEX_REDUCE_KERNEL_NAME, err)
	{
	}

	SimplexReducer()
		: StandardDispatch()
	{
	}

	/**
	 * @brief Set the kernel expected input buffer
	 * 
	 * The buffer is expected to contain plain simplexes: N points of homogenous N dimensional space (N + 1 coordinates)
	 * @param in data buffer
	 * @return cl_status internal OpenCL call status
	 */
	cl_status setSimplexInputBuffer(const Buffer& in)
	{
		return setArg<INPUT_BUFFER>(in);
	}

	/**
	 * @brief Set the triangle target buffer
	 * 
	 * The buffer will contain triangles: 3 points of homogenous N dimensional space (N + 1 coordinates)
	 * @param out triangle target buffer
	 * @return cl_status internal OpenCL call status
	 */
	cl_status setTriangleOutputBuffer(const Buffer& out)
	{
		return setArg<OUTPUT_BUFFER>(out);
	}

	/**
	 * @brief Set the execution range
	 * 
	 * Since there is a one to one mapping between simplices and work items, this just creates a single line of work items
	 * , with length the same as the primitive count
	 * @note unlike the vertex reducer, this stage acts upon each primitive (instead of each vertex)
	 * @param primitiveCount primitive count in batch
	 */
	void setExecutionRange(const nr_uint primitiveCount)
	{
		range.global.x = primitiveCount;
		range.local.x = 1;
	}

	using TypesafeKernel<Buffer, Buffer>::operator cl_kernel;

private:
	static constexpr const nr_uint INPUT_BUFFER = 0;
	static constexpr const nr_uint OUTPUT_BUFFER = 1;

};

}

}