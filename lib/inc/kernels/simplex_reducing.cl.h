#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

// Reducing simplexes to their triangles
const string simplex_reducing = R"__CODE__(

#define TRIANGLES_PER_SIMPLEX (RENDER_DIMENSION * (RENDER_DIMENSION - 1) * (RENDER_DIMENSION - 2) / 6) // There are nC3 triangles in an n-1 simplex

void copy_point(const global Point p, global Point res)
{
	for (uint i = 0; i < ELEMENTS_PER_POINT; ++i)
	{
		res[i] = p[i];
	}
}

void emit_triangle(const global Point p0, const global Point p1, const global Point p2, global Triangle result)
{
	copy_point(p0, result[0]);
	copy_point(p1, result[1]);
	copy_point(p2, result[2]);
}

kernel void reduce_simplex(
	const global Simplex* simplexes, 
	global Triangle* result)
{
	const uint index = get_global_id(0);
	uint res = get_global_id(0) * TRIANGLES_PER_SIMPLEX;
	
	for (uint i = 0; i < RENDER_DIMENSION - 2; ++i)
	{
		for (uint j = i + 1; j < RENDER_DIMENSION - 1; ++j)
		{
			for (uint k = j + 1; k < RENDER_DIMENSION; ++k)
			{
				emit_triangle(simplexes[index][i], simplexes[index][j], simplexes[index][k], result[res++]);
			}
		}	
	}
}

)__CODE__";

}

}

}
