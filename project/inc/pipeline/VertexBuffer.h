#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"

#include "../rendering/Render.h"

#include "../utils/rendermath.h"

namespace nr
{

class VertexBuffer
{
public:
	template <nr_uint Dimension>
	static VertexBuffer make(const Context& context, const nr_uint primitiveCount, Simplex<Dimension>* data, cl_status* err)
	{
		auto vertecies = Buffer::make<Simplex<Dimension>>(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, primitiveCount, data, err);
		if (error::isFailure(*err)) return VertexBuffer();
		auto reducedVertecies = Buffer::make<Simplex<Dimension>>(context, CL_MEM_READ_WRITE, primitiveCount, err);
		if (error::isFailure(*err)) return VertexBuffer();
		auto reducedSimplices = Buffer::make<Triangle<Dimension>>(context, CL_MEM_READ_WRITE, detail::triangleCount(Dimension, primitiveCount), err);
		if (error::isFailure(*err)) return VertexBuffer();

		return VertexBuffer(vertecies, reducedVertecies, reducedSimplices);
	}

	template <nr_uint Dimension>
	static VertexBuffer make(const Context& context, const nr_uint primitiveCount, cl_status* err)
	{
		auto vertecies = Buffer::make<Simplex<Dimension>>(context, CL_MEM_READ_WRITE, primitiveCount, err);
		if (error::isFailure(*err)) return VertexBuffer();
		auto reducedVertecies = Buffer::make<Simplex<Dimension>>(context, CL_MEM_READ_WRITE, primitiveCount, err);
		if (error::isFailure(*err)) return VertexBuffer();
		auto reducedSimplices = Buffer::make<Triangle<Dimension>>(context, CL_MEM_READ_WRITE, detail::triangleCount(Dimension, primitiveCount), err);
		if (error::isFailure(*err)) return VertexBuffer();

		return VertexBuffer(vertecies, reducedVertecies, reducedSimplices);
	}

	VertexBuffer() {}

	operator Buffer() const { return vertecies; }

private:
	VertexBuffer(const Buffer vertecies, const Buffer reducedVertecies, const Buffer reducedSimplices)
		: vertecies(vertecies), reducedSimplices(reducedSimplices), reducedVertecies(reducedVertecies)
	{
	}

	friend class Pipeline;

	Buffer vertecies;
	Buffer reducedVertecies;
	Buffer reducedSimplices;
};	

}