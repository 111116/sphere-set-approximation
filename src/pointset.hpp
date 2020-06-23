#pragma once

#include <unordered_set>
#include <algorithm>
#include <tuple>
#include "rtcore/mesh.hpp"
#include "rtcore/mt19937sampler.hpp"
#include "math/vecmath.hpp"
#include "visualize.hpp"
#include "util.hpp"


typedef std::vector<vec3f> PointSet;


std::tuple<vec3f, vec3f> farthest_points_apart(const PointSet& p)
{
	std::tuple<vec3f, vec3f> bestresult;
	double maxsqrlen = 0;
	if (p.size()<2000) {
		// bruteforcing
		for (int i=0; i<p.size(); ++i)
			for (int j=i+1; j<p.size(); ++j)
				if (sqrlen(p[i] - p[j]) > maxsqrlen) {
					maxsqrlen = sqrlen(p[i] - p[j]);
					bestresult = {p[i], p[j]};
				}
		return bestresult;
	}
	// approximate algorithm (precision can be improved by stratified sampling)
	for (int _=0; _<1000; ++_)
	{
		vec3f axis = RTcore::SharedSampler::sampleUnitSphereSurface();
		double minproj = std::numeric_limits<double>::infinity();
		double maxproj = -std::numeric_limits<double>::infinity();
		vec3f minpoint, maxpoint;
		for (vec3f x: p) {
			double proj = dot(x, axis);
			if (proj > maxproj) {
				maxproj = proj;
				maxpoint = x;
			}
			if (proj < minproj) {
				minproj = proj;
				minpoint = x;
			}
		}
		if (sqrlen(maxpoint - minpoint) > maxsqrlen) {
			maxsqrlen = sqrlen(maxpoint - minpoint);
			bestresult = {minpoint, maxpoint};
		}
	}
	return bestresult;
}


PointSet allvertices(const RTcore::Mesh& mesh)
{
	auto trigs = mesh.list;
	std::vector<vec3f> p;
	for (auto t: trigs)
	{
		p.push_back(t->v1);
		p.push_back(t->v2);
		p.push_back(t->v3);
	}
	auto cmp = [](const vec3f& a, const vec3f& b) {
		return a.x<b.x or (a.x==b.x and a.y<b.y) or (a.x==b.x and a.y==b.y and a.z<b.z);
	};
	sort(p.begin(), p.end(), cmp);
	p.erase(unique(p.begin(), p.end()), p.end());
	return p;
}

double diameter(const RTcore::Mesh& mesh)
{
	auto [a,b] = farthest_points_apart(allvertices(mesh));
	return norm(a-b);
}


PointSet voxelized(const RTcore::Mesh& mesh, double vxsize)
{
	auto aabb = mesh.boundingVolume();
	PointSet points;
	for (double x = aabb.x1 - vxsize; x < aabb.x2 + vxsize; x += vxsize)
	for (double y = aabb.y1 - vxsize; y < aabb.y2 + vxsize; y += vxsize)
	for (double z = aabb.z1 - vxsize; z < aabb.z2 + vxsize; z += vxsize)
		if (point_in_mesh(vec3f(x,y,z), mesh))
			points.push_back(vec3f(x,y,z));
	return points;
}

PointSet get_inner_points(const RTcore::Mesh& mesh, int n_approx = 10000)
{
	// voxel number estimate
	auto aabb = mesh.boundingVolume();
	double vxsize = std::cbrt((aabb.x2-aabb.x1)*(aabb.y2-aabb.y1)*(aabb.z2-aabb.z1))/100;
	PointSet points = voxelized(mesh, vxsize);
	// resample with estimated density
	vxsize *= std::cbrt(points.size() / n_approx);
	points = voxelized(mesh, vxsize);
	console.log(points.size(), "inner points");
	return points;
}


PointSet sample_surface(const RTcore::Mesh& mesh, int n_approx)
{
	RTcore::MT19937Sampler sampler(rand());
	double area = 0;
	auto trigs = mesh.list;
	for (auto p: trigs)
		area += p->surfaceArea();
	PointSet result;
	for (auto p: trigs) {
		double n_tosample = n_approx * p->surfaceArea() / area;
		int n = floor(n_tosample) + (sampler.get1f() < n_tosample - floor(n_tosample));
		for (int i=0; i<n; ++i)
			result.push_back(p->sampleSurface(sampler));
	}
	return result;
}

PointSet sample_surface_bestcandidate(const RTcore::Mesh& mesh, int n_approx)
{
	int n_candid = 10;
	PointSet pool = sample_surface(mesh, n_approx*n_candid);
	while (pool.size() < n_approx * n_candid) {
		PointSet spool = sample_surface(mesh, 100);
		pool.insert(pool.end(), spool.begin(), spool.end());
	}
	std::shuffle(pool.begin(), pool.end(), std::mt19937(rand()));
	PointSet result;
	for (int i=0; i<n_approx; ++i) {
		double bestdist = 0;
		int best = 0;
		for (int j=i*n_candid; j<(i+1)*n_candid; j++) {
			double dist = std::numeric_limits<double>::infinity();
			for (auto p: result)
				dist = std::min(dist, sqrlen(p - pool[j]));
			if (dist > bestdist) {
				bestdist = dist;
				best = j;
			}
		}
		result.push_back(pool[best]);
	}
	return result;
}

PointSet get_surface_points(const RTcore::Mesh& mesh, int n_approx = 10000)
{
	PointSet points = sample_surface_bestcandidate(mesh, n_approx);
	// don't need all vertices if we don't want to be strict
	// PointSet points = allvertices(mesh);
	console.log(points.size(), "surface points");
	return points;
}
