#include <pipeline/VertexProcessor.h>

namespace nr
{

namespace detail
{

cl_status VertexProcessor::setRenderDimension(const Source& source)
{
	if (source.getRenderDimension() == m_currentRenderDimension) return CL_SUCCESS;
	cl_status ret = CL_SUCCESS;
	auto pret = &ret;

	m_simplexReduce = source.simplexReduce(pret);
	if (nr::error::isFailure(ret)) return ret;
	m_vertexReduce = source.vertexReduce(pret);
	if (nr::error::isFailure(ret)) return ret;
	m_currentRenderDimension = source.getRenderDimension();

	auto ctx = source.getRenderContext();

	parameters.far = Buffer::make<nr_float>(ctx, CL_MEM_READ_WRITE, m_currentRenderDimension);
	parameters.near = Buffer::make<nr_float>(ctx, CL_MEM_READ_WRITE, m_currentRenderDimension);

	return CL_SUCCESS;
}

cl_status VertexProcessor::setNearPlane(const CommandQueue& q, const nr_float* near)
{
	return q.enqueueBufferWriteCommand(parameters.near, false, m_currentRenderDimension, near);
}

cl_status VertexProcessor::setFarPlane(const CommandQueue& q, const nr_float* far)
{
	return q.enqueueBufferWriteCommand(parameters.far, false, m_currentRenderDimension, far);
}

cl_status VertexProcessor::consume(const CommandQueue& q)
{	
	cl_status ret;

	ret = setup();
	if (nr::error::isFailure(ret)) return ret;

	ret = q.enqueueKernelCommand<1>(m_vertexReduce, m_vertexReduceRange);
	if (nr::error::isFailure(ret)) return ret;

	ret = q.enqueueKernelCommand<1>(m_simplexReduce, m_simplexReduceRange);
	if (nr::error::isFailure(ret)) return ret;

	return ret;
}

cl_status VertexProcessor::setup()
{
	cl_status err;
	if (nr::error::isFailure(err = setupVertexReduce())) return err;
	return setupSimplexReduce();
}

cl_status VertexProcessor::setupSimplexReduce()
{
	cl_status ret = CL_SUCCESS;

	ret = m_simplexReduce.setArg(0, inputs.simplexes);
	if (nr::error::isFailure(ret)) return ret;

	return m_simplexReduce.setArg(1, outputs.result);
}

cl_status VertexProcessor::setupVertexReduce()
{
	return cl_status();
}

}

}