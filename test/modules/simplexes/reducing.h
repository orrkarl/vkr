#pragma once

#include <base/Module.h>
#include <base/Buffer.h>
#include <kernels/base.cl.h>
#include <kernels/simplex_reducing.cl.h>

#include <utility>

#include "../../includes.h"


using namespace nr;
using namespace nr::detail;
using namespace testing;
using namespace std::rel_ops;

template<nr_uint dim>
struct ReducedSimplex
{
	Triangle<dim> triangles[dim * (dim - 1) * (dim - 2) / 6];

	bool operator==(const ReducedSimplex<dim>& other) const
	{
		for (auto i = 0; i < 3; ++i)
			if (triangles[i] != other[i])
				return false;

		return true;
	}

	friend std::ostream& operator<<(std::ostream& os, const ReducedSimplex& simplex)
	{
		os << "ReducedSimplex: { ";
		for (auto i = 0u; i < dim * (dim - 1) * (dim - 2) / 6 - 1; ++i)
			os << simplex[i] << " ";
		return os << simplex[dim * (dim - 1) * (dim - 2) / 6 - 1] << " }";
	}

	Triangle<dim> operator[](const nr_uint index) const { return triangles[index]; }

	Triangle<dim>& operator[](const nr_uint index) { return triangles[index]; }
};

void setupSimplexes(const nr_uint count, Simplex<4>* original, ReducedSimplex<4>* result)
{
	auto curr = 0.0f;

	for (auto i = 0; i < count; ++i)
	{
		for (auto j = 0u; j < 4; ++j)
		{
			original[i][j][0] = curr;
			original[i][j][1] = curr;
			original[i][j][2] = curr;
			original[i][j][3] = curr++;
			original[i][j][4] = nanf("");
		}

		result[i][0][0] = original[i][0];
		result[i][0][1] = original[i][1];
		result[i][0][2] = original[i][2];

		result[i][1][0] = original[i][0];
		result[i][1][1] = original[i][1];
		result[i][1][2] = original[i][3];

		result[i][2][0] = original[i][0];
		result[i][2][1] = original[i][2];
		result[i][2][2] = original[i][3];

		result[i][3][0] = original[i][1];
		result[i][3][1] = original[i][2];
		result[i][3][2] = original[i][3];
	}
}

TEST(SimplexReducer, reducing)
{
	cl_status err = CL_SUCCESS;

	constexpr const nr_uint dim = 4;
	const nr_uint simplexCount = 3;

	Simplex<dim> orig[simplexCount];
	ReducedSimplex<dim> expected[simplexCount];
	ReducedSimplex<dim> result[simplexCount];
	setupSimplexes(simplexCount, orig, expected);
	
	Module::Options options =
	{
		Module::CL_VERSION_12,
		Module::RenderDimension(dim),
		Module::DEBUG
	};

	Module code(defaultContext, Module::Sources{ clcode::base, clcode::simplex_reducing }, &err);
	ASSERT_SUCCESS(err);

	ASSERT_SUCCESS(code.build(defaultDevice, options));

	auto testee = SimplexReduceKernel(code, &err);
	ASSERT_SUCCESS(err);

	auto q = defaultCommandQueue;
	ASSERT_SUCCESS(err);

	auto d_orig = Buffer::make<Simplex<dim>>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, simplexCount, orig, &err);
	ASSERT_SUCCESS(err); 
	auto d_res = Buffer::make<ReducedSimplex<dim>>(defaultContext, CL_MEM_WRITE_ONLY, simplexCount, &err);
	ASSERT_SUCCESS(err);

	testee.setExecutionRange(simplexCount);
	
	ASSERT_SUCCESS(testee.setSimplexInputBuffer(d_orig));
	ASSERT_SUCCESS(testee.setTriangleOutputBuffer(d_res));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
	ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_res, false, (nr_float*) result));
	ASSERT_SUCCESS(q.await());

	for (auto i = 0u; i < simplexCount; ++i)
	{
		ASSERT_EQ(expected[i], result[i]) << i;
	}
}
