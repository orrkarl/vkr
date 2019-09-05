/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief public interface for storing client vertecies data
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../rendering/Render.h"
#include "../utils/rendermath.h"

namespace nr
{

/**
 * @brief Vertex data buffer, allocating all of the memory required to process vertecies
 * 
 * This class contains, in fact, 3 different buffers: 
 * A public vertex data buffer, and 2 private buffers - one for reduced vertecies and one for reduced simplices.
 * Accesing the data in the vertex buffer can be done by passing this class to a command queue - it will be implicitly casted to it's vertex data Buffer
 */
class VertexBuffer
{
public:
	/**
	 * @brief Factory method for creating buffers
	 * 
	 * Since, as of right now, there is now way to pass anything other than simplices to NR, the vertex buffer can only contain those.
	 * @param context parent context
	 * @param primitiveCount how many simplex should be allocated
	 * @param data simplices pointer to be copied
	 * @param[out] err internal OpenCL call status
	 * @return VertexBuffer result (will be a null buffer if an error has occured)
	 */
	static VertexBuffer make(const Context& context, const nr_uint primitiveCount, Triangle* data, cl_status& err)
	{
		auto vertecies = Buffer::make<Triangle>(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, primitiveCount, data, err);
		if (error::isFailure(err)) return VertexBuffer();
		auto reducedVertecies = Buffer::make<Triangle>(context, CL_MEM_READ_WRITE, primitiveCount, err);
		if (error::isFailure(err)) return VertexBuffer();

		return VertexBuffer(vertecies, reducedVertecies);
	}

	/**
	 * @brief Factory method for creating buffers
	 * 
	 * Since, as of right now, there is now way to pass anything other than simplices to NR, the vertex buffer can only contain those.
	 * @param context parent context
	 * @param primitiveCount how many simplex should be allocated
	 * @param[out] err internal OpenCL call status
	 * @return VertexBuffer result (will be a null buffer if an error has occured)
	 */
	static VertexBuffer make(const Context& context, const nr_uint primitiveCount, cl_status& err)
	{
		auto vertecies = Buffer::make<Triangle>(context, CL_MEM_READ_WRITE, primitiveCount, err);
		if (error::isFailure(err)) return VertexBuffer();
		auto reducedVertecies = Buffer::make<Triangle>(context, CL_MEM_READ_WRITE, primitiveCount, err);
		if (error::isFailure(err)) return VertexBuffer();

		return VertexBuffer(vertecies, reducedVertecies);
	}

	VertexBuffer() {}

	/**
	 * @brief Casts this buffer to it's vertex data buffer
	 * 
	 * This function exposes the vertex data buffer while keeping the other members hidden
	 * @return Buffer vertex data buffer
	 */
	operator Buffer() const { return vertecies; }

private:
	VertexBuffer(const Buffer vertecies, const Buffer reducedVertecies)
		: vertecies(vertecies), reducedVertecies(reducedVertecies)
	{
	}

	friend class Pipeline;

	Buffer vertecies;
	Buffer reducedVertecies;
};	

}