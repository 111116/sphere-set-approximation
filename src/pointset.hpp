#pragma once

#include <unordered_set>
#include <tuple>
#include "rtcore/mesh.hpp"
#include "rtcore/sharedsampler.hpp"
#include "math/vecmath.hpp"
#define VOXELIZER_IMPLEMENTATION
#include "lib/voxelizer.h"
#include "visualize.hpp"


typedef std::vector<vec3f> PointSet;


vx_vertex_t vxvec(const vec3f& a)
{
	vx_vertex_t b;
	b.x = a.x;
	b.y = a.y;
	b.z = a.z;
	return b;
}

vec3f vecvx(const vx_vertex_t& a)
{
	vec3f b;
	b.x = a.x;
	b.y = a.y;
	b.z = a.z;
	return b;
}

vx_mesh_t* meshconvert(const RTcore::Mesh& mesh)
{
	auto trigs = mesh.list;
	vx_mesh_t* m = vx_mesh_alloc(trigs.size()*3, trigs.size()*3);
	int id = 0;
	for (auto t: trigs) {
		for (int i=0; i<3; ++i) {
			m->indices[id] = id;
			m->normalindices[id] = id;
			m->vertices[id] = vxvec((i==0)?t->v1: (i==1)?t->v2: t->v3);
			m->normals[id] = vxvec(t->planeNormal);
			m->colors[id] = vxvec(0);
			id++;
		}
	}
	return m;
}

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


PointSet get_inner_points(const RTcore::Mesh& mesh)
{
	// convert mesh to vxmesh
	vx_mesh_t* vxmesh = meshconvert(mesh);
	// voxelize
	double vxsize = 0.1;
	vx_point_cloud_t* pointcloud = vx_voxelize_pc(vxmesh, vxsize, vxsize, vxsize, vxsize/10);
	PointSet points;
	for (int i=0; i<pointcloud->nvertices; ++i)
		points.push_back(vecvx(pointcloud->vertices[i]));
	// free memory
	vx_mesh_free(vxmesh);
	vx_point_cloud_free(pointcloud);
	return points;
}

PointSet get_surface_points(const RTcore::Mesh& mesh)
{
	PointSet points = allvertices(mesh);
	visualize(points);
	return points;
}
