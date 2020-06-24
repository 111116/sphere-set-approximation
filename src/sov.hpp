// calculate Sphere Outside Volume (SOV)

#pragma once

#include "math/vecmath.hpp"
#include "rtcore/mesh.hpp"
#include "rtcore/triangle.hpp"
#include "point_in_mesh.hpp"
#include "sotv.hpp"
#include "sphere.hpp"

// parameters: mesh, sphere center & radius
double sov(const RTcore::Mesh& mesh, vec3f o, double r)
{
	double s = 0;
	auto sgn = [](double x){return x>0?1:x<0?-1:0;};
	for (auto t : mesh.list)
		s += sotv(t->v1, t->v2, t->v3, o,r) * sgn(dot(t->v1-o, t->planeNormal));
	if (s < 0)
	if (!point_in_mesh(o, mesh)) {
		if (s > 0) console.log("sov ERR SIGN");
		s += 4.0/3*PI * r*r*r;
	}
	else {
		// silently fix edge case which point_in_mesh fails to handle
		if (s<0) {
			s += 4.0/3*PI * r*r*r;
		}
	}
	return s;
}

double sov(const RTcore::Mesh& mesh, const Sphere& sphere)
{
	return sov(mesh, sphere.center, sphere.radius);
}
