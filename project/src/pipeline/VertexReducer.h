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
 * take the homogenous 3 dimensional input and transform it into a 2D position with some extra data (such as 'depth'), to be used in 
 * the next pipeline operations.
 * @par
 * This stage takes 6 inputs: the vertex input buffer, aspect ratio, fov angle, the perspective z near and z far planes and an output target buffer.
 * It executes naively - each vertex gets it's own work item to process.
 * Each simplex will be copied to the target buffer and it's vertecies will be reduced; vertex order in the input buffer is preserved.
 * @par
 * The vertecies are reduced to a D3D style 'NDC' form - the X,Y coordinates are between -1 and 1, while every other coordinate is between 0 and 1.
 * 
 * @note As of right now, no clipping is done at all - but this will change soon
 */
class NRAPI VertexReducer : public StandardDispatch<1, Buffer, nr_float, nr_float, nr_float, nr_float, Buffer>
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
	 * The buffer is expected to contain plain triangles: 3 points of homogenous 3 dimensional space (4 coordinates total)
	 * @param in data buffer
	 * @return internal OpenCL call status
	 */
	cl_status setVertexInputBuffer(const Buffer& in);

	/**
	 * @brief Set the perspective projection aspect ratio
	 *
	 * @param screenDimension current viewport size
	 * @return internal OpenCL call status
	 */
	cl_status setAspectRatio(const ScreenDimension& screenDimension);

	/**
	 * @brief Set the perspective projection horizontal field of view
	 *
	 * @param angle fov angle (in radians)
	 * @return internal OpenCL call status
	 */
	cl_status setFieldOfView(const nr_float angle);

	/**
	 * @brief Set the perspective projection near z plane
	 *
	 * @param zNear near Z plane
	 * @return internal OpenCL call status
	 */
	cl_status setZNearPlane(const nr_float zNear);

	/**
	 * @brief Set the perspective projection far z plane
	 *
	 * @param zFar far Z plane
	 * @return internal OpenCL call status
	 */
	cl_status setZFarPlane(const nr_float zFar);


	/**
	 * @brief Set the simplex output buffer
	 * 
	 * Sets the target buffer for this stage
	 * @param out simplex target buffer
	 * @return internal OpenCL call status
	 */
	cl_status setVertexOutputBuffer(const Buffer& out);

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