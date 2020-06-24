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

// of each sphere, ratio of volume shared with at least one other spherein the set to its total volume
// complexity: O(|sphere|^2 * n_approx) but in fact fast enough
std::vector<double> overlap_ratio(const std::vector<Sphere>& sphere, int n_approx = 1000000)
{
	std::vector<double> ratio;
	for (int i=0; i<sphere.size(); ++i)
	{
		const Sphere s = sphere[i];
		int n_inside = 0, n_overlap = 0;
		std::vector<Sphere> others;
		for (int j=0; j<sphere.size(); ++j)
			if (j!=i && norm(sphere[j].center - s.center) < sphere[j].radius + s.radius)
				others.push_back(sphere[j]);
		if (others.empty()) {
			ratio.push_back(0.0);
			continue;
		}
		const double stepsize = 2*s.radius / std::cbrt(n_approx);
		const double r2 = s.radius * s.radius;
		for (double x = s.center.x - s.radius; x < s.center.x + s.radius; x += stepsize)
		for (double y = s.center.y - s.radius; y < s.center.y + s.radius; y += stepsize)
		for (double z = s.center.z - s.radius; z < s.center.z + s.radius; z += stepsize)
		{
			bool inside = (sqrlen(s.center - vec3f(x,y,z)) < r2);
			n_inside += inside;
			if (!inside) continue;
			bool overlap = false;
			if (sqrlen(others[0].center - vec3f(x,y,z)) < others[0].radius * others[0].radius)
			{
				overlap = true;
			}
			else {
				for (int j=1; j<others.size(); ++j) {
					if (sqrlen(others[j].center - vec3f(x,y,z)) < others[j].radius * others[j].radius)
					{
						overlap = true;
						std::swap(others[0], others[j]);
						break;
					}
				}
			}
			n_overlap += overlap;
		}
		ratio.push_back((double)n_overlap / n_inside);
	}
	return ratio;
}

// volume of union geometry
// complexity: O(|sphere|^2 * n_approx) but in fact fast enough
double volume(const std::vector<Sphere>& sphere, int n_approx = 10000000)
{
	double total = 0;
	// accumulate of each sphere: volume it doesn't shared with spheres later
	for (int i=0; i<sphere.size(); ++i)
	{
		const Sphere s = sphere[i];
		int n_inside = 0, n_overlap = 0;
		std::vector<Sphere> others;
		for (int j=i+1; j<sphere.size(); ++j)
			if (norm(sphere[j].center - s.center) < sphere[j].radius + s.radius)
				others.push_back(sphere[j]);
		if (others.empty()) {
			total += s.volume();
			continue;
		}
		const double stepsize = 2*s.radius / std::cbrt(n_approx);
		const double r2 = s.radius * s.radius;
		for (double x = s.center.x - s.radius; x < s.center.x + s.radius; x += stepsize)
		for (double y = s.center.y - s.radius; y < s.center.y + s.radius; y += stepsize)
		for (double z = s.center.z - s.radius; z < s.center.z + s.radius; z += stepsize)
		{
			bool inside = (sqrlen(s.center - vec3f(x,y,z)) < r2);
			n_inside += inside;
			if (!inside) continue;
			bool overlap = false;
			if (sqrlen(others[0].center - vec3f(x,y,z)) < others[0].radius * others[0].radius)
			{
				overlap = true;
			}
			else {
				for (int j=1; j<others.size(); ++j) {
					if (sqrlen(others[j].center - vec3f(x,y,z)) < others[j].radius * others[j].radius)
					{
						overlap = true;
						std::swap(others[0], others[j]);
						break;
					}
				}
			}
			n_overlap += overlap;
		}
		total += s.volume() * (1.0 - (double)n_overlap / n_inside);
	}
	return total;
}
