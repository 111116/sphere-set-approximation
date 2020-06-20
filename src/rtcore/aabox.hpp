#pragma once

#include "ray.hpp"

namespace RTcore
{

struct AABox
{
	// order matters!
	float x1,x2,y1,y2,z1,z2;
	AABox(){}
	AABox(float x1, float x2, float y1, float y2, float z1, float z2):
		x1(x1), x2(x2), y1(y1), y2(y2), z1(z1), z2(z2) {}
	AABox(point p): x1(p.x), x2(p.x), y1(p.y), y2(p.y), z1(p.z), z2(p.z) {}
	bool intersect(const Ray& r)
	{
		float tmin, tmax;
		if (r.dir.x > 0)
			tmin = (x1 - r.origin.x) / r.dir.x,
			tmax = (x2 - r.origin.x) / r.dir.x;
		else
			tmin = (x2 - r.origin.x) / r.dir.x,
			tmax = (x1 - r.origin.x) / r.dir.x;
		if (r.dir.y > 0)
			tmin = fmax(tmin, (y1 - r.origin.y) / r.dir.y),
			tmax = fmin(tmax, (y2 - r.origin.y) / r.dir.y);
		else
			tmin = fmax(tmin, (y2 - r.origin.y) / r.dir.y),
			tmax = fmin(tmax, (y1 - r.origin.y) / r.dir.y);
		if (r.dir.z > 0)
			tmin = fmax(tmin, (z1 - r.origin.z) / r.dir.z),
			tmax = fmin(tmax, (z2 - r.origin.z) / r.dir.z);
		else
			tmin = fmax(tmin, (z2 - r.origin.z) / r.dir.z),
			tmax = fmin(tmax, (z1 - r.origin.z) / r.dir.z);
		return tmin <= tmax && tmax > 0;
	}
	bool intersect(const Ray& r, float& t)
	{
		float tmin, tmax;
		if (r.dir.x > 0)
			tmin = (x1 - r.origin.x) / r.dir.x,
			tmax = (x2 - r.origin.x) / r.dir.x;
		else
			tmin = (x2 - r.origin.x) / r.dir.x,
			tmax = (x1 - r.origin.x) / r.dir.x;
		if (r.dir.y > 0)
			tmin = fmax(tmin, (y1 - r.origin.y) / r.dir.y),
			tmax = fmin(tmax, (y2 - r.origin.y) / r.dir.y);
		else
			tmin = fmax(tmin, (y2 - r.origin.y) / r.dir.y),
			tmax = fmin(tmax, (y1 - r.origin.y) / r.dir.y);
		if (r.dir.z > 0)
			tmin = fmax(tmin, (z1 - r.origin.z) / r.dir.z),
			tmax = fmin(tmax, (z2 - r.origin.z) / r.dir.z);
		else
			tmin = fmax(tmin, (z2 - r.origin.z) / r.dir.z),
			tmax = fmin(tmax, (z1 - r.origin.z) / r.dir.z);
		if (tmin > tmax || tmax <= 0) return false;
		t = tmin;
		return true;
	}
	float surfaceArea() {
		float dx = x2 - x1;
		float dy = y2 - y1;
		float dz = z2 - z1;
		return 2 * (dx*dy + dx*dz + dy*dz);
	}
	bool finite(const float U = 1e20) {
		return x2<U && y2<U && z2<U && x1>-U && y1>-U && z1>-U;
	}
	static const AABox infAAB;
};


// AABB of union geometry
AABox operator+ (const AABox& a, const AABox& b);

// AABB of union geometry
AABox operator+ (const AABox& a, const point& p);

}

