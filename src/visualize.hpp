#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include "math/vecmath.hpp"
#include "rtcore/mesh.hpp"

std::string visualizer_mesh_filename = "";

void printscene(std::vector<std::pair<vec3f,double>> spheres, std::string outfile = "scene.json", vec3f camerapos = vec3f(5,2,5))
{
	std::ofstream fout(outfile);
	fout << "{\"primitives\":[\n";
	if (visualizer_mesh_filename != "")
	{
		fout << "{\"type\":\"mesh\",\"file\":\"" << visualizer_mesh_filename << "\",\"bsdf\":{\"type\":\"lambert\",\"albedo\":0.8}},\n";
	}
	for (auto sph : spheres)
	{
		fout << "{\"type\": \"sphere\",\"transform\":{\"position\":["
		<< sph.first.x << "," << sph.first.y << "," << sph.first.z << "],\"scale\":" << sph.second << "},\"bsdf\":{\"type\":\"lambert\",\"ior\":1.3,\"albedo\":[1,0.6,0.6]}},\n";
	}
    fout << "{\"type\": \"skydome\",\"temperature\": 4777.0,\"gamma_scale\": 1.0,\"turbidity\": 3.0,\"intensity\": 6.0,\"sample\": true},";
	fout << "],\n";
	fout << "\"camera\":{\"resolution\":[512,512],\"transform\":{";
	// print camera pos
	fout << "\"position\": [" << camerapos.x << "," << camerapos.y << "," << camerapos.z << "], \"look_at\": [0,0,0], \"up\": [0,1,0]";

	fout << "},";
	fout << "\"type\":\"pinhole\",\"fov\":40},\n";
    fout << "\"integrator\": {\"min_bounces\": 0,\"max_bounces\": 16,\"type\": \"path_tracer\"},\n";
    fout << "\"renderer\": {\"scene_bvh\": true,\"spp\": 64,\"output_file\": \"demo.png\"},\n";
    fout << "}\n";
}

