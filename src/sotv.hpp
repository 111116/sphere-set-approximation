// calculate Sphere Outside Triangle Volume (SOTV)
// described in *Variational Sphere Set Approximation for Solid Objects*

#pragma once

#include <cmath>
#include <algorithm>
#include "math/vecmath.hpp"
#include "consolelog.hpp"
#include "sotv_ref.hpp"

class SOTV
{
public:
	// parameters: triangle vertices, sphere center & radius
	double operator()(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		return sotv(v1,v2,v3,o,r);
	}

private:

	// parameters: triangle vertices, sphere center & radius
	static double sotv(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		bool out1 = (sqrlen(v1-o) > r*r);
		bool out2 = (sqrlen(v2-o) > r*r);
		bool out3 = (sqrlen(v3-o) > r*r);
		int n_out = out1 + out2 + out3;
		// console.log("n_vert_out:", n_out);
		if (n_out == 0) return sotv_case_a(v1, v2, v3, o,r);
		if (n_out == 3) return sotv_case_b(v1, v2, v3, o,r);
		if (n_out == 2) {
			if (!out1) return sotv_case_c(v1, v2,v3, o,r);
			if (!out2) return sotv_case_c(v2, v1,v3, o,r);
			if (!out3) return sotv_case_c(v3, v1,v2, o,r);
		}
		if (n_out == 1) {
			if (out1) return sotv_case_d(v1, v2,v3, o,r);
			if (out2) return sotv_case_d(v2, v1,v3, o,r);
			if (out3) return sotv_case_d(v3, v1,v2, o,r);
		}
		assert(false);
	}

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
		double d1 = (-B - std::sqrt(delta)) / (2*A);
		double d2 = (-B + std::sqrt(delta)) / (2*A);
		double d = (abs(d1-0.5) < abs(d2-0.5))? d1: d2;
		if (d < -1e-5 || d > 1+1e-5)
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


	// volume in sphere between p0-o-p1 & p0-c-p1, where p0,p1 are on sphere
	// c is to the other side of swing volume, relative to line p0-p1
	static double swing_volume(vec3f p0, vec3f p1, vec3f c, vec3f o, double r)
	{
		// analytic solution from https://planetcalc.com/7847/
		// in original paper this is implemented by tabulating sampled integral
		vec3f p = (p0 + p1) / 2;
		// transform to 2D view (OC perpendicular to p0-p1)
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
	static double sotv_case_a(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		double omega = solid_angle_tetrahedron(v1-o, v2-o, v3-o);
		double V = volume_tetrahedron(v1-o, v2-o, v3-o);
		return r*r*r*omega/3 - V;
	}

	// swing volume needed to subtract at an edge of triangle
	static double slice_remaining(vec3f a, vec3f b, vec3f c, vec3f o, double r)
	{
		vec3f p0, p1;
		bool intersect = line_sphere_intersection_2o(a,b,o,r, p0,p1);
		if (!intersect) return 0.0;
		// only accept p0!=p1 in between a,b
		if (dot(p0-a,p0-b) >= 0 || dot(p1-a,p1-b) >= 0 || p0==p1) return 0.0;
		return swing_volume(p0,p1,c,o,r);
	};

	// (b) 3 vert. out
	static double sotv_case_b(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		if (triangle_outside_sphere(v1,v2,v3, o,r))
			return 0.0;
		// first solve for infinite triangle
		double vol = sotv_case_b_original(v1, v2, v3, o,r);
		// then subtract swing volumes
		vol -= slice_remaining(v2,v3, v1, o,r);
		vol -= slice_remaining(v1,v3, v2, o,r);
		vol -= slice_remaining(v1,v2, v3, o,r);
		return vol;
	}

	// (b) 3 vert. out  (formula in original paper, which seems to assume infinite triangle)
	static double sotv_case_b_original(vec3f v1, vec3f v2, vec3f v3, vec3f o, double r)
	{
		// d: distance from center to triangle
		double d = 6 * volume_tetrahedron(v1-o, v2-o, v3-o) / norm(cross(v2-v1, v3-v1));
		if (d >= r) return 0;
		double h = r - d;
		// return volume of sphereical cap
		return PI*h*h*(r-h/3);
	}

	// (c) 2 vert. out
	static double sotv_case_c(vec3f vin, vec3f v2, vec3f v3, vec3f o, double r)
	{
		// first solve for infinite triangle
		double vol = sotv_case_c_original(vin, v2, v3, o,r);
		// then subtract swing volumes
		vol -= slice_remaining(v2,v3, vin, o,r);
		return vol;
	}

	// (c) 2 vert. out  (formula in original paper, which seems to assume infinite triangle)
	static double sotv_case_c_original(vec3f vin, vec3f v2, vec3f v3, vec3f o, double r)
	{
		vec3f p0 = line_sphere_intersection_1o(vin, v2, o,r);
		vec3f p1 = line_sphere_intersection_1o(vin, v3, o,r);
		return sotv_case_a(p0,p1,vin,o,r) + swing_volume(p0,p1,vin,o,r);
	}

	// (d) 1 vert. out
	static double sotv_case_d(vec3f vout, vec3f v2, vec3f v3, vec3f o, double r)
	{
		vec3f p0 = line_sphere_intersection_1o(vout, v2, o,r);
		return sotv_case_c(v3, vout,p0, o,r) + sotv_case_a(v3,v2,p0, o,r);
	}

	static bool triangle_outside_sphere(vec3f a, vec3f b, vec3f c, vec3f o, double r)
	{
		// case 1. nearest point to sphere on triangle is inside triangle
		// first draw line perpendicular to triangle
		vec3f p = normalized(cross(a-b, a-c));
		// which passes through o and intersects plane of triangle at d
		vec3f d = o + p * dot(a-o, p);
		// then we check if d is inside the triangle
		vec3f facedir_ref = cross(b-a, c-b); // abc
		double sgn1 = dot(facedir_ref, cross(a-d, b-a)); // dab
		double sgn2 = dot(facedir_ref, cross(b-d, c-b)); // dbc
		double sgn3 = dot(facedir_ref, cross(c-d, a-c)); // dca
		if ((sgn1>=0 && sgn2>=0 && sgn3>=0) || (sgn1<=0 && sgn2<=0 && sgn3<=0))
		{ // d is inside (or on edge of) the triangle
			// compute distance from o to plane
			double h = dot(a-o, p);
			return h>=r;
		}
		// case 2. nearest point to sphere on triangle is on an edge
		auto touch_edge = [](vec3f a, vec3f b, vec3f o, double r)
		{
			vec3f oa = a-o;
			vec3f op = oa - dot(oa, a-b) * (a-b) / sqrlen(a-b);
			vec3f p = o + op;
			if (dot(p-a, p-b)<=0)
				return norm(op)<r;
			return false;
		};
		if (touch_edge(a,b,o,r)) return false;
		if (touch_edge(b,c,o,r)) return false;
		if (touch_edge(c,a,o,r)) return false;
		// case 3. nearest point to sphere on triangle is among vertices
		if (sqrlen(a-o) < r*r) return false;
		if (sqrlen(b-o) < r*r) return false;
		if (sqrlen(c-o) < r*r) return false;
		return true;
	}
};

SOTV sotv;
