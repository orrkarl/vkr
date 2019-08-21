#include "utils.h"

nr::Simplex<3> mkTriangleInExactCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, const nr_float distance)
{
	nr::Simplex<3> triangle;
	triangle.points[0].values[0] = p0.x;
	triangle.points[0].values[1] = p0.y;

	triangle.points[1].values[0] = p1.x;
	triangle.points[1].values[1] = p1.y;

	triangle.points[2].values[0] = p2.x;
	triangle.points[2].values[1] = p2.y;

	for (nr_uint i = 0; i < 3; ++i)
	{
		for (nr_uint j = 2; j < 4; ++j)
		{
			triangle.points[i].values[j] = distance;
		}
	}

	return triangle;
}

nr::Simplex<3> mkTriangleFullyInBin(const nr::ScreenDimension & screenDim, const Bin & bin, const nr_float distance)
{
	ScreenPosition top_left_screen = { bin.x + 1, bin.y + bin.height - 2 };
	ScreenPosition bottom_left_screen = { bin.x + 1, bin.y + 1 };
	ScreenPosition bottom_right_screen = { bin.x + bin.width - 2, bin.y + 1 };

	NDCPosition top_left = ndcFromScreen(top_left_screen, screenDim);
	NDCPosition bottom_left = ndcFromScreen(bottom_left_screen, screenDim);
	NDCPosition bottom_right = ndcFromScreen(bottom_right_screen, screenDim);

	return mkTriangleInExactCoordinates(top_left, bottom_right, bottom_left, distance);
}

void fillTriangleBuffer(
	const nr::ScreenDimension& screenDim, const nr::detail::BinQueueConfig config,
	const nr_uint workGroupCount, const nr_uint batchSize,
	const nr_float distance,
	nr::Simplex<3> * triangles)
{
	Bin current;
	current.width = config.binWidth;
	current.height = config.binHeight;
	current.x = 0;
	current.y = 0;

	const nr_uint copyFactor = workGroupCount * batchSize;

	nr_uint currentIndex = 0;

	for (; current.x < screenDim.width; current.x += config.binWidth)
	{
		for (; current.y < screenDim.height; current.y += config.binHeight)
		{
			triangles[currentIndex] = mkTriangleFullyInBin(screenDim, current, distance);
			for (auto i = 1u; i < copyFactor; ++i)
			{
				triangles[currentIndex + i] = triangles[currentIndex];
			}
			currentIndex += copyFactor;
		}
	}
}