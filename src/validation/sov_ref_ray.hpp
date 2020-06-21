// calculate Sphere Outside Volume (SOV)
// using sampling (cone method)
// It's inefficient and approximated, only used for validation purposes

#pragma once

#include <vector>
#include <algorithm>
#include "math/vecmath.hpp"
#include "rtcore/mesh.hpp"
#include "rtcore/mt19937sampler.hpp"
#include "rtcore/triangle.hpp"
#include "point_in_mesh.hpp"

// parameters: mesh, sphere center & radius
double sov_ray_sampled(const RTcore::Mesh& mesh, vec3f o, double r)
{
	const int nsample = 1000000;
	RTcore::MT19937Sampler sampler(rand());
	bool o_in_mesh = point_in_mesh(o, mesh);
	double sum = 0;
	for (int i=0; i<nsample; ++i)
	{
		// shoot a ray
		RTcore::Ray ray(o, sampler.sampleUnitSphereSurface());
		auto trigs = mesh.intersect_all(ray);
		if (trigs.size()%2 != o_in_mesh) {
			console.warn("edge case");
			--i;
			continue;
		}
		if (trigs.empty())
		{
			sum += r*r*r;
			continue;
		}
		// get all intersections
		std::vector<double> tlist;
		for (auto trig: trigs)
		{
			double t;
			bool intersect = trig->intersect(ray, t);
			assert(intersect);
			tlist.push_back(t);
		}
		// sort from far to near
		std::sort(tlist.begin(), tlist.end(), std::greater<double>());
		bool current_inside = false;
		bool r3_counted = false;
		for (double t: tlist)
		{
			if (t < r and not r3_counted) {
				r3_counted = true;
				if (not current_inside)
					sum += r*r*r;
			}
			current_inside = not current_inside;
			if (r3_counted) {
				if (current_inside)
					sum -= t*t*t;
				else
					sum += t*t*t;
			}
		}
		if (not r3_counted) {
			r3_counted = true;
			if (not current_inside)
				sum += r*r*r;
		}
	}
	return sum / nsample * 4.0/3*PI;
}

