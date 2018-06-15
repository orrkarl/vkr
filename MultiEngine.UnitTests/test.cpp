#include "pch.h"

#include "../vector.h"

#define vector(i) math::algebra::Vector<i>

GTEST_TEST(VectorDotProduct, VectorTest) {
	typedef vector(16) vector16;
	typedef vector(15) vector15;
	typedef vector(3) vector3;
	typedef vector(2) vector2;

	const vector3 vec0{ 1, -1, 1 };
	const vector3 vec1{ 1, 1, 1 };
	const vector2 vec2{ 1, -1 };
	vector2 vec3{ 1, 1 };
	vector15 vec4;
	vector15 vec5;
	vector16 vec6;
	vector16 vec7;

	for (auto i = 0; i < 15; ++i)
	{
		vec4[i] = vec6[i] = 1;
		vec5[i] = vec7[i] = pow(-1, i);
	}
	vec6[15] = 1;
	vec7[15] = pow(-1, 15);

	EXPECT_EQ(vec0.NormSquared(), 3);
	EXPECT_EQ(vec1.Norm(), sqrt(3));
	EXPECT_EQ(vector2::DotProduct(vec2, vec3), 0);
	EXPECT_TRUE(vec2.IsOrthogonalTo(vec3));
	EXPECT_EQ(vector15::DotProduct(vec4, vec5), 1);
	EXPECT_EQ(vec4.Norm(), sqrt(15));
	EXPECT_EQ(vec5.Norm(), sqrt(15));
	EXPECT_EQ(vector16::DotProduct(vec6, vec7), 0);

	EXPECT_TRUE(vec0.AsUnitVector().IsUnitVector());
	EXPECT_TRUE(vec1.AsUnitVector().IsUnitVector());
	EXPECT_TRUE(vec2.AsUnitVector().IsUnitVector());
	EXPECT_TRUE(vec3.AsUnitVector().IsUnitVector());
	EXPECT_TRUE(vec4.AsUnitVector().IsUnitVector());
	EXPECT_TRUE(vec5.AsUnitVector().IsUnitVector());
	EXPECT_TRUE(vec6.AsUnitVector().IsUnitVector());
	EXPECT_TRUE(vec7.AsUnitVector().IsUnitVector());

	vec7.Normalize();
	EXPECT_EQ(vec7.Norm(), 1);
}