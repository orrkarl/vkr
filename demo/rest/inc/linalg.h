#pragma once

#include <math.h>
#include <string.h>
#include <utility>

int get_index(const int n, const int i, const int j)
{
    return n * i + j;
}

void matmul(const int n, const int m, const int p, const float* mat1, const float* mat2, float* result)
{
    float tmp;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            tmp = 0;
            for (int k = 0; k < p; ++k)
                tmp += mat1[get_index(n, i, j)] * mat2[get_index(n, k, j)];
            result[get_index(n, i, j)] = tmp;
        }
    }
}

void identity(const int dim, float* result)
{
    memset(result, 0, dim * dim * sizeof(float));
    for (int i = 0; i < dim; ++i)
        result[get_index(dim, i, i)] = 1.0f;
}

void rotation(const int dim, const std::pair<int, int>& axis, const float angle, float* result)
{
    identity(dim, result);
    
    auto cosine = cos(angle);
    auto sine   = sin(angle);
    result[get_index(dim, axis.first, axis.first)]   = cosine;
    result[get_index(dim, axis.second, axis.second)] = cosine;
    result[get_index(dim, axis.first, axis.second)]  = -sine;
    result[get_index(dim, axis.second, axis.first)]  = sine;
}


