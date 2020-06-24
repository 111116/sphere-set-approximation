#pragma once

#include "math/vecmath.hpp"
#include "rtcore/ray.hpp"
#include "rtcore/mesh.hpp"
#include "rtcore/triangle.hpp"
#include "rtcore/sharedsampler.hpp"

bool point_in_mesh(vec3f p, const RTcore::Mesh& mesh)
{
	// method 1. shoot a ray and see odd/even number of intersections
	// for robustness we shoot 5 rays
	int n_odd = 0, n_even = 0;
	for (int i=0; i<5; ++i)
	{
		RTcore::Ray ray(p, RTcore::SharedSampler::sampleUnitSphereSurface());
		if (mesh.intersect_all(ray).size() % 2 == 0)
			n_even += 1;
		else
			n_odd += 1;
	}
	// if (n_odd && n_even) // edge case
	// 	console.log("point_in_mesh: contradict",n_odd,n_even);
	return (n_odd > n_even);
}