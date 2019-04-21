#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"
#include "../base/Kernel.h"
#include "../rendering/Render.h"

namespace nr
{

namespace __internal
{

struct BinQueueConfig
{
    NRuint binWidth;
    NRuint binHeight;
    NRuint queueSize;
};

class BinRasterizerParams
{
public:

    static NRuint getTotalBinQueueSize(const NRuint workGroupCount, const ScreenDimension& dim, const NRuint binWidth, const NRuint binHeight, const NRuint binQueueSize)
    {
        return workGroupCount * getBinCount(dim, binWidth, binHeight) * binQueueSize * sizeof(NRuint);
    }
    
    BinRasterizerParams()
    {
    }

    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }

    cl_int load(cl::Kernel kernel);

    Error allocateBinQueues(const NRuint workGroupCount, const ScreenDimension& dim, const BinQueueConfig& config)
    {
        Error ret = Error::NO_ERROR;
        ret = m_binQueues.resize(CL_MEM_READ_WRITE, getTotalBinQueueSize(workGroupCount, dim, config.binWidth, config.binHeight, config.queueSize));
        if (error::isFailure(ret)) return ret;
        ret = m_hasOverflow.resize(CL_MEM_READ_WRITE, sizeof(cl_bool));
        if (error::isFailure(ret)) return ret;
        
        m_dim = dim;
        m_binQueueConfig = config;

        return ret;
    }

    void configureSimplexBuffer(Buffer data, const NRuint& count)
    {
        m_simplexData = data;
        m_simplexCount = count;
    }

    Buffer getBinQueues()
    {
        return m_binQueues;
    }

    void setOverflowBuffer(Buffer overflow)
    {
        m_hasOverflow = overflow;
    }

    NRbool isOverflowing(cl::CommandQueue q, cl_int* err)
    {
        NRbool ret = false;
        if (err != nullptr) *err = q.enqueueReadBuffer(m_hasOverflow.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &ret);
        return ret;
    }

private:
    static NRuint getBinCount(const ScreenDimension& dim, const NRuint& binWidth, const NRuint& binHeight)
    {
        NRuint xCount = (NRuint) ceil(((NRfloat) dim.width) / binWidth);
        NRuint yCount = (NRuint) ceil(((NRfloat) dim.height) / binHeight);
        return xCount * yCount;
    }

    // Screen Dimensions
    ScreenDimension m_dim;

    // Bin Queues Data
    BinQueueConfig m_binQueueConfig;
    Buffer m_binQueues;

    // Simplex data
    Buffer m_simplexData;
    NRuint m_simplexCount;

    // Overflow handling
    Buffer m_hasOverflow;
};

typedef Kernel<BinRasterizerParams> BinRasterizer;

}

}