#pragma once

#include <base/Module.h>
#include <base/Buffer.h>
#include <kernels/base.cl.h>
#include <kernels/simplex_reducer.cl.h>
#include <pipeline/SimplexReducer.h>

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
		return triangles[0] == other.triangles[0];
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

void setupSimplexes(const nr_uint count, Simplex<3>* original, ReducedSimplex<3>* result)
{
	auto curr = 0.0f;

	for (auto i = 0; i < count; ++i)
	{
		for (auto j = 0u; j < 3; ++j)
		{
			original[i][j][0] = curr;
			original[i][j][1] = curr;
			original[i][j][2] = curr++;
			original[i][j][3] = 3.1415f;
		}

		result[i][0][0] = original[i][0];
		result[i][0][1] = original[i][1];
		result[i][0][2] = original[i][2];
	}
}

TEST(SimplexReducer, reducing)
{
	cl_status err = CL_SUCCESS;

	const nr_uint simplexCount = 3;

	Simplex<3> orig[simplexCount];
	ReducedSimplex<3> expected[simplexCount];
	ReducedSimplex<3> result[simplexCount];
	setupSimplexes(simplexCount, orig, expected);
	
	Module::Options options =
	{
		Module::CL_VERSION_12,
		Module::DEBUG
	};

	Module code(defaultContext, Module::Sources{ clcode::base, clcode::simplex_reduce }, err);
	ASSERT_SUCCESS(err);

	ASSERT_SUCCESS(code.build(defaultDevice, options));

	auto testee = SimplexReducer(code, err);
	ASSERT_SUCCESS(err);

	auto q = defaultCommandQueue;
	ASSERT_SUCCESS(err);

	auto d_orig = Buffer::make<Simplex<3>>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, simplexCount, orig, err);
	ASSERT_SUCCESS(err); 
	auto d_res = Buffer::make<ReducedSimplex<3>>(defaultContext, CL_MEM_WRITE_ONLY, simplexCount, err);
	ASSERT_SUCCESS(err);

	testee.setExecutionRange(simplexCount);
	
	ASSERT_SUCCESS(testee.setSimplexInputBuffer(d_orig));
	ASSERT_SUCCESS(testee.setTriangleOutputBuffer(d_res));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
	ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_res, false, (nr_float*) result));
	ASSERT_SUCCESS(q.await());

	for (auto i = 0u; i < simplexCount; ++i)
	{
		ASSERT_EQ(expected[i], result[i]);
	}
}
