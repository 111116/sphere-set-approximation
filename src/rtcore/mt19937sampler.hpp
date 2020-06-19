#pragma once

#include <random>
#include <functional>
#include <cstdlib>
#include "sampler.hpp"

namespace RTcore
{

class MT19937Sampler : public Sampler
{
	std::mt19937 generator;
public:
	MT19937Sampler(unsigned seed = 0, int sampleID = 0):
		generator(seed ^ (std::hash<int>{}(sampleID) << 1)) {}
	
	double get1f() {
		return (double)generator() / generator.max();
	}
	vec2f get2f() {
		return vec2f(get1f(), get1f());
	}
	// limit shouldn't be loo large
	unsigned get1u(unsigned limit) {
		return generator() % limit;
	}
	vec3f sampleUnitSphereSurface() {
		// TODO optimize
		vec3f v;
		do v = vec3f(get1f()*2-1, get1f()*2-1, get1f()*2-1);
		while (norm(v)>1 || norm(v)<1e-3);
		return normalized(v);
	}
	vec2f sampleUnitTriangle() {
		vec2f t = get2f();
		if (t.x + t.y > 1) {
			t.x = 1 - t.x;
			t.y = 1 - t.y;
		}
		return t;
	}
	vec2f sampleUnitDisk() {
		double x,y;
		do {
			x = get1f()*2-1;
			y = get1f()*2-1;
		}
		while (x*x+y*y>1);
		return vec2f(x,y);
	}
};

}

