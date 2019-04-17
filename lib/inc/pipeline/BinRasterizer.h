#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"
#include "../base/Kernel.h"
#include "../rendering/RenderState.h"

namespace nr
{

namespace __internal
{

class BinRasterizerParams
{
public:

    static NRuint getTotalBinQueueSize(const NRuint workGroupCount, const ScreenDimension& dim, const NRuint binWidth, const NRuint binHeight, const NRuint binQueueSize)
    {
        return workGroupCount * getBinCount(dim, binWidth, binHeight) * binQueueSize * sizeof(NRuint);
    }
    
    BinRasterizerParams()
        : m_binQueues(Type::UINT), m_hasOverflow(Type::BOOL), m_simplexData(Type::FLOAT)
    {
    }

    cl_int init(cl::CommandQueue queue);

    cl_int load(cl::Kernel kernel);

    Error allocateBinQueues(const NRuint workGroupCount, const ScreenDimension& dim, const NRuint& binWidth, const NRuint& binHeight, const NRuint binQueueSize)
    {
        Error ret = Error::NO_ERROR;
        ret = m_binQueues.resize(CL_MEM_READ_WRITE, getTotalBinQueueSize(workGroupCount, dim, binWidth, binHeight, binQueueSize));
        if (error::isFailure(ret)) return ret;
        ret = m_hasOverflow.resize(CL_MEM_READ_WRITE, sizeof(cl_bool));
        if (error::isFailure(ret)) return ret;
        
        m_binQueueSize = binQueueSize;
        m_dim = dim;

        m_binWidth = binWidth;
        m_binHeight = binHeight;

        return ret;
    }

    void configureSimplexBuffer(Buffer data, const NRuint& count)
    {
        m_simplexData = data;
        m_simplexCount = count;
    }

    NRbool isOverflowing(cl::CommandQueue queue, cl_int* err = nullptr)
    {
        cl_bool ret;
        cl_int error = queue.enqueueReadBuffer(m_hasOverflow.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &ret);
        if (error != CL_SUCCESS && err != nullptr) *err = error;
        return ret;
    }

    Buffer getBinQueues()
    {
        return m_binQueues;
    }

private:
    static NRuint getBinCount(const ScreenDimension& dim, const NRuint& binWidth, const NRuint& binHeight)
    {
        NRuint xCount = (NRuint) ceil(((NRfloat) dim.width) / binWidth);
        NRuint yCount = (NRuint) ceil(((NRfloat) dim.height) / binHeight);
        return xCount * yCount;
    }

    static const NRuint ZERO;

    // Screen Dimensions
    ScreenDimension m_dim;

    // Bin Queues Data
    NRuint m_binQueueSize;
    Buffer m_binQueues;

    // Bin data
    NRuint m_binWidth;
    NRuint m_binHeight;

    // Simplex data
    Buffer m_simplexData;
    NRuint m_simplexCount;

    // Overflow handling
    Buffer m_hasOverflow;
};

typedef Kernel<BinRasterizerParams> BinRasterizer;

}

}