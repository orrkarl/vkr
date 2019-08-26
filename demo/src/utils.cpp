#include "utils.h"

#include <ostream>

void reduce4Simplex(const Tetrahedron& tetrahedron, Triangle4d result[4])
{
    result[0].points[0] = tetrahedron.points[0];
    result[0].points[1] = tetrahedron.points[1];
    result[0].points[2] = tetrahedron.points[2];

    result[1].points[0] = tetrahedron.points[0];
    result[1].points[1] = tetrahedron.points[1];
    result[1].points[2] = tetrahedron.points[3];

    result[2].points[0] = tetrahedron.points[0];
    result[2].points[1] = tetrahedron.points[2];
    result[2].points[2] = tetrahedron.points[3];

    result[3].points[0] = tetrahedron.points[1];
    result[3].points[1] = tetrahedron.points[2];
    result[3].points[2] = tetrahedron.points[3];
}

std::ostream& operator<<(std::ostream& os, const Triangle4d& self)
{
    return os << "Triangle4d{" << self.points[0] << ", " << self.points[1] << ", " << self.points[2] << "}";
}

std::ostream& operator<<(std::ostream& os, const Tetrahedron& self)
{
    return os << "Tetrahedron{" << self.points[0] << ", " << self.points[1] << ", " << self.points[2] << ", " << self.points[3] << "}";
}

bool isCubeFace(const Vector& p0, const Vector& p1, const Vector& p2, const Vector& p3)
{
    auto d01 = p0.distanceSquared(p1);
    auto d02 = p0.distanceSquared(p2);
    auto d03 = p0.distanceSquared(p3);
    auto d12 = p1.distanceSquared(p2);
    auto d13 = p1.distanceSquared(p3);
    auto d23 = p2.distanceSquared(p3);

    std::vector<nr_float> diffs{ d01, d02, d03, d12, d13, d23 };
    auto min = *std::min_element(diffs.cbegin(), diffs.cend());
    auto max = *std::max_element(diffs.cbegin(), diffs.cend());

    if (fabs(min - 0.5 * max) > 10e-4) return false;

    auto countMin = 0, countMax = 0;
    for (auto i = 0; i < 6; ++i)
    {
        if (diffs[i] - min <= 10e-4) countMin++;
        else if (max - diffs[i] <= 10e-4) countMax++;
        else return false;
    }   

    return countMax == 2 && countMin == 4; 
}

void reduceToFaces(const Vector cube[8], Vector result[6 * 4])
{    
    auto result_idx = 0;
    for (auto i = 0; i < 8 && result_idx < 24; ++i)
    {
        for (auto j = i + 1; j < 8 && result_idx < 24; ++j)
        {
            for (auto k = j + 1; k < 8 && result_idx < 24; ++k)
            {
                for (auto l = k + 1; l < 8 && result_idx < 24; ++l)
                {
                    if (isCubeFace(cube[i], cube[j], cube[k], cube[l]))
                    {
                        result[result_idx++] = cube[i];
                        result[result_idx++] = cube[j];
                        result[result_idx++] = cube[k];
                        result[result_idx++] = cube[l];
                    }
                }   
            }   
        }   
    }
}

bool isVertexInFace(const Vector& initial, const Vector faces[4])
{
    for (auto vec = 0; vec < 4; ++vec)
        if (initial == faces[vec])
            return true;
    
    return false;
}

void getNextFace(const Vector& initial, const Vector faces[24], const nr_uint lastFaceIndex, nr_uint& result)
{
    for (auto face = lastFaceIndex + 1; face < 6; ++face)
    {
        if (!isVertexInFace(initial, faces + 4 * face))
        {
            result = face;
            return;
        }
    }
}

void tetrahadrlize3Cube(const Vector cube[8], Tetrahedron result[6])
{
    Vector faces[24];
    reduceToFaces(cube, faces);

    nr_uint i0, i1, i2;

    getNextFace(faces[0], faces, 0, i0);
    getNextFace(faces[0], faces, i0, i1);
	if (i1 > 5)
	{
		std::cerr << "Oopsy!" << std::endl;
	}
    getNextFace(faces[0], faces, i1, i2);
	if (i2 > 5)
	{
		std::cerr << "Oopsy!" << std::endl;
	}
    result[0].points[0] = faces[0];
    result[0].points[1] = faces[i0 * 4];
    result[0].points[2] = faces[i0 * 4 + 1];
    result[0].points[3] = faces[i0 * 4 + 2];

    result[1].points[0] = faces[0];
    result[1].points[1] = faces[i0 * 4 + 2];
    result[1].points[2] = faces[i0 * 4 + 3];
    result[1].points[3] = faces[i0 * 4];

    result[2].points[0] = faces[0];
    result[2].points[1] = faces[i1 * 4];
    result[2].points[2] = faces[i1 * 4 + 1];
    result[2].points[3] = faces[i1 * 4 + 2];

    result[3].points[0] = faces[0];
    result[3].points[1] = faces[i1 * 4 + 2];
    result[3].points[2] = faces[i1 * 4 + 3];
    result[3].points[3] = faces[i1 * 4];

    result[4].points[0] = faces[0];
    result[4].points[1] = faces[i2 * 4];
    result[4].points[2] = faces[i2 * 4 + 1];
    result[4].points[3] = faces[i2 * 4 + 2];

    result[5].points[0] = faces[0];
    result[5].points[1] = faces[i2 * 4 + 2];
    result[5].points[2] = faces[i2 * 4 + 3];
    result[5].points[3] = faces[i2 * 4];
}

void generate3cube(const Vector cube[16], const nr_uint diff, const nr_uint offset, Vector cube3d[8])
{
    auto cube3_idx = 0;
    for (nr_uint i = offset * diff; i < 16; i += 2 * diff)
    {
        for (nr_uint j = 0; j < diff; ++j)
        {
            cube3d[cube3_idx++] = cube[i + j];
        }
    }
}

void cube4dToSimplices(const Vector cube[16], Tetrahedron simplices[6 * 8])
{
    Vector cube3d[8];

    auto result_idx = 0;
    for (auto diff = 1; diff <= 8; diff *= 2)
    {
		generate3cube(cube, diff, 0, cube3d);
        tetrahadrlize3Cube(cube3d, simplices + result_idx);
        result_idx += 6;

		generate3cube(cube, diff, 1, cube3d);
        tetrahadrlize3Cube(cube3d, simplices + result_idx);
        result_idx += 6;
    }
}
