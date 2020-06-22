#pragma once

#include "math/vecmath.hpp"

struct Sphere
{
	vec3f center;
	double radius;
	Sphere(vec3f center, double radius): center(center), radius(radius) {}
	Sphere& operator= (const Sphere& s)
	{
		center = s.center;
		radius = s.radius;
		return *this;
	}
	double volume() const
	{
		return 4.0/3*PI*radius*radius*radius;
	}
};

double sphere_overlap_volume(vec3f o1, double r1, vec3f o2, double r2)
{
	double d = norm(o1 - o2);
	if (d >= r1 + r2) return 0.0;
	if (d + r1 <= r2) return 4*PI/3*r1*r1*r1;
	if (d + r2 <= r1) return 4*PI/3*r2*r2*r2;
	// https://mathworld.wolfram.com/Sphere-SphereIntersection.html
	double a = 0.5/d * std::sqrt((-d+r1-r2)*(-d-r1+r2)*(-d+r1+r2)*(d+r1+r2));
	double h1 = (r2-r1+d)*(r2+r1-d)/(2*d);
	double h2 = (r1-r2+d)*(r1+r2-d)/(2*d);
	return PI*h1*h1*(r1-h1/3) + PI*h2*h2*(r2-h2/3);
}

double sphere_overlap_volume(Sphere a, Sphere b)
{
	return sphere_overlap_volume(a.center, a.radius, b.center, b.radius);
}

