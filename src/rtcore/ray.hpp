#pragma once

#include "math/vecmath.hpp"

namespace RTcore
{
typedef vec3f point;

struct Ray
{
	point origin;
	vec3f dir; // must be normalized
	Ray(point p, vec3f d): origin(p), dir(d) {}
	point atParam(double t) const {
		return origin + dir * t;
	}
};

}
