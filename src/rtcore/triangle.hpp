#pragma once

#include "aabox.hpp"
#include "math/matmath.hpp"
#include "ray.hpp"

namespace RTcore
{

class Triangle
{
	vec3f v1,v2,v3;
	mat3f tMatrix;
	double one_by_2S;

public:
	vec3f planeNormal;

	Triangle(vec3f v1, vec3f v2, vec3f v3, vec3f frontvec = 0): v1(v1), v2(v2), v3(v3)
	// pre-calculation to accelerate intersection / interpolation computation
	{
		// NOTE: be careful when dealing with tiny triangles!
		assert(norm(v1-v2)>0);
		assert(norm(v1-v3)>0);
		assert(norm(v2-v3)>0);
		assert(norm(cross(normalized(v2-v1), normalized(v3-v1))) > 1e-9);

		planeNormal = normalized(cross(v2-v1, v3-v1));
		if (dot(planeNormal, frontvec) < 0)
			planeNormal = -planeNormal;
		// make sure plane normal points outward
		one_by_2S = 1 / norm(cross(v3-v1, v2-v1));
		tMatrix = inverse(mat3f(v2-v1, v3-v1, cross(v2-v1, normalized(v3-v1))));
	}

	bool intersect(const Ray& ray, double& result) const
	{
		vec3f o = tMatrix * (ray.origin - v1);
		vec3f dir = tMatrix * ray.dir;
		double t = o.z / dir.z;
		vec3f p = o - dir * t;
		if (t<0 && p.x >= 0 && p.y >= 0 && p.x + p.y <= 1)
		{
			result = -t;
			return true;
		}
		return false;
	}

	AABox boundingVolume() const
	{
		AABox v;
		v.x1 = std::min(v1.x, std::min(v2.x, v3.x));
		v.x2 = std::max(v1.x, std::max(v2.x, v3.x));
		v.y1 = std::min(v1.y, std::min(v2.y, v3.y));
		v.y2 = std::max(v1.y, std::max(v2.y, v3.y));
		v.z1 = std::min(v1.z, std::min(v2.z, v3.z));
		v.z2 = std::max(v1.z, std::max(v2.z, v3.z));
		return v;
	}

private:
	bool onsurface(const point& p) const {
		return norm(cross(p-v1,v2-v1)) + norm(cross(p-v2,v3-v2)) + norm(cross(p-v3,v1-v3)) < (1+1e-4) * norm(cross(v2-v1,v3-v1));
	}
};

}