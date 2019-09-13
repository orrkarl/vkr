#pragma once

#include "../includes.h"
#include <rendering/Render.h>


void fillTriangleBuffer(
	const nr::ScreenDimension& screenDim, const nr::detail::BinQueueConfig config,
	const nr_uint workGroupCount, const nr_uint batchSize,
	const nr_float distance,
	nr::Triangle* triangles
);
