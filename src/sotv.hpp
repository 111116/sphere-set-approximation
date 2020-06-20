// calculate Sphere Outside Triangle Volume (SOTV)
// described in *Variational Sphere Set Approximation for Solid Objects*

#pragma once

#include <cmath>
#include <algorithm>
#include "math/vecmath.hpp"
#include "consolelog.hpp"

class SOTV
{
public:
	// parameters: triangle vertices, sphere center & radius
	double operator()(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		bool out1 = (sqrlen(v1-o) > r*r);
		bool out2 = (sqrlen(v2-o) > r*r);
		bool out3 = (sqrlen(v3-o) > r*r);
		int n_out = out1 + out2 + out3;
		assert(n_out == 3);
		return sotv_case_b(v1, v2, v3, o, r);
	}

private:

	static double cot(double x)
	{
		return cos(x) / sin(x);
	}
	static double csc(double x)
	{
		return 1 / sin(x);
	}

	static double solid_angle_tetrahedron(vec3f oa, vec3f ob, vec3f oc)
	{
		// https://math.stackexchange.com/a/3305625
		double cosa = dot(ob,oc) / (norm(ob)*norm(oc));
		double cosb = dot(oa,oc) / (norm(oa)*norm(oc));
		double cosc = dot(oa,ob) / (norm(oa)*norm(ob));
		double sina = std::sqrt(std::max(0.0, 1.0 - cosa * cosa));
		double sinb = std::sqrt(std::max(0.0, 1.0 - cosb * cosb));
		double sinc = std::sqrt(std::max(0.0, 1.0 - cosc * cosc));
		double phi_a = acos((cosa - cosb * cosc) / (sinb * sinc));
		double phi_b = acos((cosb - cosa * cosc) / (sina * sinc));
		double phi_c = acos((cosc - cosa * cosb) / (sina * sinb));
		double omega = phi_a + phi_b + phi_c - PI;
		return omega;
	}

	static double volume_tetrahedron(vec3f oa, vec3f ob, vec3f oc)
	{
		return std::abs(dot(oa, cross(ob, oc))) / 6;
	}

	// params: end points of line segment, center of sphere, radius
	static vec3f line_sphere_intersection_1o(vec3f a, vec3f b, vec3f o, double r)
	{
		// solve for non-negative d: ||a+d(b-a)-o||=r
		// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
		// rearrange as quadratic equation
		double A = sqrlen(b-a);
		double B = 2 * dot(b-a, a-o);
		double C = sqrlen(a-o) - r*r;
		double delta = B*B - 4*A*C;
		if (delta < -1e-5 * B*B)
			throw "failed computing line sphere intersection";
		delta = std::max(0.0, delta);
		double d = (-B + std::sqrt(delta)) / (2*A);
		if (d < -1e-5)
			throw "failed computing segment sphere intersection";
		return a + d * (b-a);
	}

	// params: end points of line segment, center of sphere, radius
	static bool line_sphere_intersection_2o(vec3f a, vec3f b, vec3f o, double r, vec3f& p1, vec3f& p2)
	{
		// solve for d: ||a+d(b-a)-o||=r
		// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
		// rearrange as quadratic equation
		double A = sqrlen(b-a);
		double B = 2 * dot(b-a, a-o);
		double C = sqrlen(a-o) - r*r;
		double delta = B*B - 4*A*C;
		if (delta < 0)
			return false;
		double d1 = (-B - std::sqrt(delta)) / (2*A);
		double d2 = (-B + std::sqrt(delta)) / (2*A);
		p1 = a + d1 * (b-a);
		p2 = a + d2 * (b-a);
		return true;
	}


	// c is to the other side of swing volume, relative to line p0-p1
	static double swing_volume(vec3f p0, vec3f p1, vec3f c, vec3f o, double r)
	{
		// volume in sphere between p0-o-p1 & p0-c-p1, where p0,p1 are on sphere
		// https://planetcalc.com/7847/
		vec3f p = (p0 + p1) / 2;
		vec3f oc = c - o;
		oc -= (p1-p0) / sqrlen(p1-p0) * dot(oc, p1-p0);
		c = o + oc;
		double a = acos(dot(c-p,o-p) / (norm(c-p) * norm(o-p)));
		double r0 = norm(p - o);
		double h = r - r0;
		double phi0 = asin(r0 * sin(a) / r);
		auto sqr = [](double a){return a*a;};
		auto cub = [](double a){return a*a*a;};
		// final formula
		double K1 = sqrt(sqr(sin(a)) - sqr(sin(phi0)));
		double K2 = atan(cos(a) * sin(phi0) / K1);
		double V = cub(r0*sin(a))/3 * (sqr(cot(phi0)) * (K2-PI/2) + K1*csc(phi0)*cot(a)*csc(a))
			- 2.0/3*r*r*r0*sin(a) * (csc(phi0) * (asin(cos(a)/cos(phi0)) - PI/2) - K2 + PI/2);
		return V;
	}

	// (a) 3 vert. in
	double sotv_case_a(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		double omega = solid_angle_tetrahedron(v1-o, v2-o, v3-o);
		double V = volume_tetrahedron(v1-o, v2-o, v3-o);
		return r*r*r*omega/3 - V;
	}

	// (b) 3 vert. out
	double sotv_case_b(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		// first solve for infinite triangle
		double vol = sotv_case_b_original(v1, v2, v3, o, r);
		// then subtract swing volumes
		auto slice_remaining = [](vec3f a, vec3f b, vec3f c, vec3f o, double r)
		{
			vec3f p0, p1;
			bool intersect = line_sphere_intersection_2o(a,b,o,r, p0,p1);
			if (!intersect) return 0.0;
			return swing_volume(p0,p1,c,o,r);
		};
		vol -= slice_remaining(v2,v3,v1,o,r);
		vol -= slice_remaining(v1,v3,v2,o,r);
		vol -= slice_remaining(v1,v2,v3,o,r);
		return vol;
	}

	// (b) 3 vert. out
	double sotv_case_b_original(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		// method used in the paper, which is problematic
		// d: distance from center to triangle
		double d = 6 * volume_tetrahedron(v1-o, v2-o, v3-o) / norm(cross(v2-v1, v3-v1));
		if (d >= r) return 0;
		double h = r - d;
		// return volume of sphereical cap
		return PI*h*h*(r-h/3);
	}

	// (c) 2 vert. out
	double sotv_case_c(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		
	}

	// (d) 1 vert. out
	double sotv_case_d(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		
	}
};

SOTV sotv;
