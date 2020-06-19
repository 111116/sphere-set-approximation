// calculate Sphere Outside Triangle Volume (SOTV)
// using sampling (inefficient, approximated)

#pragma once

#include "vecmath.hpp"

// parameters: triangle vertices, sphere center & radius
double sotv_sampled(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
{
	// define ray intersection function
	mat3f tMatrix = inverse(mat3f(v2-v1, v3-v1, cross(v2-v1, normalized(v3-v1))));
	auto intersect = [=](vec3f dir){
		vec3f proj = tMatrix * (o - v1);
		vec3f dirproj = tMatrix * dir;
		float t = proj.z / dirproj.z;
		vec3f p = proj - dirproj * t;
		if (t<0 && p.x >= 0 && p.y >= 0 && p.x + p.y <= 1)
		{
			t*t*sqrlen(dir)
			return true;
		}
		return false;
	}
	const int n_step = 500; // we sample total n_step^3 points
	const double step_size = (double)2*r / n_step;
	long long sum = 0;
	for (double x = o.x - r + 0.5 * stepsize; x < o.x + r; x += stepsize)
	for (double y = o.y - r + 0.5 * stepsize; y < o.y + r; y += stepsize)
	for (double z = o.z - r + 0.5 * stepsize; z < o.z + r; z += stepsize)
	{
		vec3f p(x,y,z);
		bool inside_sphere = (sqrlen(p - o) < r*r);
		bool outside_triangle = point_is_outside_triangle(p, v1, v2, v3);
		sum += (inside_sphere && outside_triangle);
	}
	return stepsize * stepsize * stepsize * sum;
}

bool point_is_outside_triangle(vec3f p, vec3f v1, vec3f v2, vec3f v3)
{

}
