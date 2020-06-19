#pragma once
#include "matfloat.hpp"

struct mat3x2f
{
	float data[3][2];
	mat3x2f() {
		memset(data, 0, sizeof data);
	}
	mat3x2f(const vec3f& a, const vec3f& b)
	{
		data[0][0] = a.x; data[1][0] = a.y; data[2][0] = a.z;
		data[0][1] = b.x; data[1][1] = b.y; data[2][1] = b.z;
	}
	vec3f column(int k) const
	{
		return vec3f(data[0][k], data[1][k], data[2][k]);
	}
	float* operator[](unsigned i) {
		return data[i];
	}
	float const* operator[](unsigned i) const {
		return data[i];
	}
};

inline mat3x2f operator * (const mat3x2f& a, const mat2f& b)
{
	mat3x2f res;
	for (int i=0; i<3; ++i)
		for (int j=0; j<2; ++j)
			for (int k=0; k<2; ++k)
				res[i][j] += a[i][k] * b[k][j];
	return res;
}