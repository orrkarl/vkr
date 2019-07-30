#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Buffer.h>
#include <kernels/base.cl.h>
#include <kernels/simplex_reducing.cl.h>
#include <pipeline/SimplexReducer.h>

#include <utility>

using namespace nr;
using namespace nr::__internal;
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

	const nr_uint dim = 4;
	const nr_uint point_count = dim + 1;
	const nr_uint simplexCount = 3;
	const nr_uint totalElementCount = point_count * 3 * (dim * (dim - 1) * (dim - 2) / 6) * simplexCount;

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

	Module code(defaultContext, { clcode::base, clcode::simplex_reducing }, &err);
	ASSERT_SUCCESS(err);

	ASSERT_SUCCESS(code.build(defaultDevice, options));

	auto testee = SimplexReducer(code, &err);
	ASSERT_SUCCESS(err);

	auto q = defaultCommandQueue;
	ASSERT_SUCCESS(err);

	Buffer<nr_float> d_orig(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, point_count * dim * simplexCount, (nr_float*)orig, &err);
	ASSERT_SUCCESS(err); 
	Buffer<nr_float> d_res(defaultContext, CL_MEM_WRITE_ONLY, totalElementCount, &err);
	ASSERT_SUCCESS(err);

	testee.simplexes = d_orig;
	testee.result = d_res;

	std::array<size_t, 1> global{ simplexCount };
	std::array<size_t, 1> local{ 1 };

	ASSERT_SUCCESS(testee.load());
	ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
	ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_res, false, point_count * 3 * (dim * (dim - 1) * (dim - 2) / 6) * simplexCount, (nr_float*) result));
	ASSERT_SUCCESS(q.await());

	for (auto i = 0u; i < simplexCount; ++i)
	{
		ASSERT_EQ(expected[i], result[i]) << i;
	}
}
