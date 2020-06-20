// calculate Sphere Outside Volume (SOV)
// using sampling 
// It's inefficient and approximated, only used for validation purposes

#pragma once

#include "math/vecmath.hpp"
#include "rtcore/mt19937sampler.hpp"
#include "rtcore/triangle.hpp"

// parameters: mesh, sphere center & radius
double sotv_sampled(const RTcore::Mesh mesh&, vec3f o, double r)
{
	const int nsample = 10000000;
	RTcore::MT19937Sampler sampler(rand());
	double sum = 0;
	for (int i=0; i<nsample; ++i)
	{
		// sampler a point in the sphere
		vec3f u;
		do u = vec3f(sampler.get1f()*2-1, sampler.get1f()*2-1, sampler.get1f()*2-1);
		while (sqrlen(u) > 1);
		vec3f p = o + r * u;
		// integral
		sum += !point_in_mesh(p, mesh);
	}
	return sum / nsample;
}

