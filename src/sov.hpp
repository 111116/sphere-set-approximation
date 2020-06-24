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
	auto sgn = [](double x){
		if (x>0) return 1;
		if (x<0) return -1;
		console.warn("sov: accumulating zero sign");
		return 0;
	};
	for (auto t : mesh.list)
		s += sotv(t->v1, t->v2, t->v3, o,r) * sgn(dot(t->v1-o, t->planeNormal));
	if (!point_in_mesh(o, mesh)) {
		if (s > 1e-6) console.warn("sov: ERR SIGN positive", s);
		s += 4.0/3*PI * r*r*r;
	}
	else {
		// silently fix edge case which point_in_mesh fails to handle
		if (s < -1e-6) console.warn("sov: ERR SIGN negative", s);
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
