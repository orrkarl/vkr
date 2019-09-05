/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Helper class which collects all of the code and kernels required for a Pipeline setup 
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "../base/Context.h"
#include "../base/Device.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "BinRasterizer.h"
#include "FineRasterizer.h"
#include "VertexReducer.h"

namespace nr
{

namespace detail
{

/**
 * @brief Pipeine helper class
 * 
 * This class abstracts the access to a complete NR device code module, providing and easy way to setup a pipeline 
 */
class Source : private Module
{
public:
	/**
	 * @brief Construct a NR source object, bound to a context
	 * 
	 * The actual code is collected from the cl files
	 * @param context parent context
	 */
	Source(const Context& context, cl_status& status);

	/**
	 * @brief builds the source module
	 * 
	 * @param dev target device
	 * @param renderDimension rendering dimension
	 * @param binBatchSize bin rasterizer batch size (refer to BinRasterizer for detailed explenation)
	 * @param debug should compile in debug mode
	 * @return cl_status internal OpenCL call status
	 */
	cl_status build(const Device& dev, const nr_uint renderDimension, const nr_uint binBatchSize, const nr_bool debug = true);

	/**
	 * @brief Aquires a BinRasterizer kernel
	 * 
	 * @param[out] status 
	 * @return BinRasterizer bin rasterizer kernel
	 */
	BinRasterizer binRasterizer(cl_status& status) const;

	/**
	 * @brief Aquires a FineRasterizer kernel
	 * 
	 * @param[out] status 
	 * @return FineRasterizer fine rasterizer kernel
	 */
	FineRasterizer fineRasterizer(cl_status& status) const;

	/**
	 * @brief Aquires a VertexReducer kernel
	 * 
	 * @param[out] status 
	 * @return VertexReducer vertex reducer kernel
	 */
	VertexReducer vertexReducer(cl_status& status) const;

	operator cl_program() const;
};

}

}
