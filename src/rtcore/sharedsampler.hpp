#pragma once

#include <random>
#include <functional>
#include <cstdlib>
#include "sampler.hpp"

namespace RTcore
{

class SharedSampler
{
public:
	static double get1f() {
		return (double)rand() / RAND_MAX;
	}
	static vec2f get2f() {
		return vec2f(get1f(), get1f());
	}
	// limit shouldn't be loo large
	static unsigned get1u(unsigned limit) {
		return rand() % limit;
	}
	static vec3f sampleUnitSphereSurface() {
		// TODO optimize
		vec3f v;
		do v = vec3f(get1f()*2-1, get1f()*2-1, get1f()*2-1);
		while (norm(v)>1 || norm(v)<1e-3);
		return normalized(v);
	}
	static vec2f sampleUnitTriangle() {
		vec2f t = get2f();
		if (t.x + t.y > 1) {
			t.x = 1 - t.x;
			t.y = 1 - t.y;
		}
		return t;
	}
	static vec2f sampleUnitDisk() {
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

