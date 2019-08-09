#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Kernel.h>

#include <rendering/Render.h>

#include <utils/StandardDispatch.h>

using namespace nr;
using namespace testing;


struct NDCFromScreen : StandardDispatch<1, ScreenPosition, ScreenDimension, Buffer>
{   
public:
    NDCFromScreen(const Module& module, cl_status* err = nullptr)
        : StandardDispatch(module, "ndc_from_screen_test", err)
    {
    }

	cl_status setPosition(const ScreenPosition& pos)
	{
		return setArg<0>(pos);
	}

	cl_status setDimension(const ScreenDimension& dim)
	{
		return setArg<1>(dim);
	}

	cl_status setResultBuffer(const Buffer& buffer)
	{
		return setArg<2>(buffer);
	}

	void setExecutionRange(const nr_uint s)
	{
		range.global.x = s;
		range.local.x = s;
	}
};

struct ScreenFromNDC : StandardDispatch<1, NDCPosition, ScreenDimension, Buffer>
{   
public:
    ScreenFromNDC(const Module& module, cl_status* err = nullptr)
        : StandardDispatch(module, "screen_from_ndc_test", err)
    {
    }

	cl_status setPosition(const NDCPosition& pos)
	{
		return setArg<0>(pos);
	}

	cl_status setDimension(const ScreenDimension& dim)
	{
		return setArg<1>(dim);
	}

	cl_status setResultBuffer(const Buffer& buffer)
	{
		return setArg<2>(buffer);
	}

	void setExecutionRange(const nr_uint s)
	{
		range.global.x = s;
		range.local.x = s;
	}
};


AssertionResult comparePoints(const NDCPosition& p1, const NDCPosition& p2, const ScreenDimension& screenDim)
{
    nr_bool x = std::abs(p1.x - p2.x) <= 2.0f / screenDim.width;
    nr_bool y = std::abs(p1.y - p2.y) <= 2.0f / screenDim.height;

    if (x && y) return AssertionSuccess();
    
    if (x) return AssertionFailure() << "Points" << p1 << ", " << p2 << " not in the same bin (X too far)";
    if (y) return AssertionFailure() << "Points" << p1 << ", " << p2 << " not in the same bin (y too far)";

    return AssertionFailure() << "Points " << p1 << ", " << p2 << " not in the same pixel (X and Y too far)";
}
