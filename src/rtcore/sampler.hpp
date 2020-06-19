#pragma once

#include "../math/vecmath.hpp"

class Sampler
{
public:
	// sample [0,1]
	virtual double get1f() = 0;
	// sample [0,1]x[0,1]
	virtual vec2f get2f() = 0;
	// sample {0,1,...,limit-1}
	virtual unsigned get1u(unsigned limit) = 0;
	// sample {v: norm(v)==1}
	virtual vec3f sampleUnitSphereSurface() = 0;
	// sample {x>=0, y>=0, x+y<=1}
	virtual vec2f sampleUnitTriangle() = 0;
	// sample {x^2+y^2<=1}
	virtual vec2f sampleUnitDisk() = 0;
	// sample {x^2+y^2+z^2=1, z>=0} with weight propto z
	virtual vec3f cosSampleHemisphereSurface()
	{
		vec2f t = sampleUnitDisk();
		return vec3f(t.x,t.y,std::sqrt(fmax(0.0f, 1-t.x*t.x-t.y*t.y)));
	}
};

