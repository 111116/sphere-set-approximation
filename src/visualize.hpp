#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include "math/vecmath.hpp"
#include "rtcore/mesh.hpp"
#include "sphere.hpp"

typedef std::vector<vec3f> PointSet;

std::string visualizer_mesh_filename = "";

void visualize(std::vector<Sphere> spheres, std::string outfile = "scene.json", vec3f camerapos = vec3f(3,2,-5.76543))
{
	console.info("For visualization, run: tungsten", outfile);
	std::ofstream fout(outfile);
	fout << "{\"primitives\":[\n";
	if (visualizer_mesh_filename != "")
	{
		fout << "{\"type\":\"mesh\",\"file\":\"" << visualizer_mesh_filename << "\",\"bsdf\":{\"type\":\"lambert\",\"albedo\":0.8}},\n";
	}
	for (auto sph : spheres)
	{
		fout << "{\"type\": \"sphere\",\"transform\":{\"position\":["
		<< sph.center.x << "," << sph.center.y << "," << sph.center.z << "],\"scale\":" << sph.radius << "},\"bsdf\":{\"type\":\"lambert\",\"ior\":1.3,\"albedo\":[1,0.6,0.6]}},\n";
	}
    fout << "{\"type\": \"skydome\",\"temperature\": 4777.0,\"gamma_scale\": 1.0,\"turbidity\": 3.0,\"intensity\": 6.0,\"sample\": true},";
	fout << "],\n";
	fout << "\"camera\":{\"resolution\":[2048,2048],\"transform\":{";
	// print camera pos
	fout << "\"position\": [" << camerapos.x << "," << camerapos.y << "," << camerapos.z << "], \"look_at\": [0,0,0], \"up\": [0,1,0]";

	fout << "},";
	fout << "\"type\":\"pinhole\",\"fov\":40},\n";
    fout << "\"integrator\": {\"min_bounces\": 0,\"max_bounces\": 16,\"type\": \"path_tracer\"},\n";
    fout << "\"renderer\": {\"scene_bvh\": true,\"spp\": 16,\"output_file\": \"demo.png\"},\n";
    fout << "}\n";
}

void visualize(const PointSet& set, double visualradius = 0.01)
{
	std::string tmp = visualizer_mesh_filename;
	visualizer_mesh_filename = "";
	std::vector<Sphere> s;
	for (auto p: set)
		s.push_back(Sphere(p, visualradius));
	visualize(s);
	visualizer_mesh_filename = tmp;
}

void visualize_with_mesh(const PointSet& set, double visualradius = 0.01)
{
	std::vector<Sphere> s;
	for (auto p: set)
		s.push_back(Sphere(p, visualradius));
	visualize(s);
}