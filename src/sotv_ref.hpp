// calculate Sphere Outside Triangle Volume (SOTV)
// using sampling 
// It's inefficient and approximated, only used for validation purposes

#pragma once

#include "math/vecmath.hpp"
#include "rtcore/mt19937sampler.hpp"
#include "rtcore/triangle.hpp"

// parameters: triangle vertices, sphere center & radius
double sotv_sampled(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
{
	const int nsample = 1000000;
	RTcore::Triangle trig(v1,v2,v3);
	RTcore::MT19937Sampler sampler;
	double sum = 0;
	for (int i=0; i<nsample; ++i)
	{
		RTcore::Ray ray(o, sampler.sampleUnitSphereSurface());
		double t;
		bool intersect = trig.intersect(ray, t);
		if (!intersect || t > r) continue;
		sum += 4.0/3*PI * (r*r*r - t*t*t) / nsample;
	}
	return sum;
}

