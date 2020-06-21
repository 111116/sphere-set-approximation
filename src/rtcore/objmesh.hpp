#pragma once

#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

#include "lib/consolelog.hpp"
#include "triangle.hpp"
#include "mesh.hpp"

namespace RTcore
{

// load mesh from an ascii Wavefront .obj file
Mesh objmesh(const char* filename)
{
	console.log("Loading mesh:", filename);
	if (strcmp(filename+strlen(filename)-4, ".obj") != 0) {
		console.error("format must be obj");
		throw "format must be obj";
	}
	std::ifstream fin(filename);
	if (!fin) {
		console.error("Can't open file");
		throw "Can't open file";
	}
	std::vector<Triangle*> faces;
	std::vector<vec3f> v,vn;
	std::vector<vec2f> vt;
	std::string line;
	// reading file line by line
	while (std::getline(fin, line)) {
		if (isalpha(line[0])) {
			std::stringstream in(line);
			std::string cmd;
			in >> cmd;
			if (cmd == "v") { // vertex coordinate
				vec3f t;
				in >> t;
				v.push_back(t);
			}
			if (cmd == "vt") { // texture coordinate
				vec2f t;
				in >> t;
				vt.push_back(t);
			}
			if (cmd == "vn") { // normal vector
				vec3f t;
				in >> t;
				vn.push_back(t);
			}
			if (cmd == "f") { // polygon face
				std::vector<vec3f> vv, vvn;
				std::vector<vec2f> vvt;
				bool recompute_normal = false;
				while (!in.fail() && !in.eof()) {					
					// code from tungsten ObjLoader::loadFace
					int indices[] = {0, 0, 0};
					for (int i = 0; i < 3; ++i) {
						if (in.peek() != '/')
							in >> indices[i];
						if (in.peek() == '/')
							in.get();
						else
							break;
					}
					while (isspace(in.peek()))
						in.get();
					if (!indices[0]) {
						console.log("Obj line:", line);
						throw "error parsing obj";
					}
					int iv = indices[0];
					int ivt = indices[1];
					int ivn = indices[2];
					if (iv < 0) iv += v.size()+1;
					if (ivt < 0) ivt += vt.size()+1;
					if (ivn < 0) ivn += vn.size()+1;
					vv.push_back(v[iv-1]);
					vvt.push_back(ivt? vt[ivt-1]: vec2f());
					vvn.push_back(ivn? vn[ivn-1]: vec3f(0,1,0));
					recompute_normal |= !ivn;
				}
				for (int l=2; l<vv.size(); ++l) {
					// ignore triangles of zero surface area
					if (cross(vv[l-1]-vv[0],vv[l]-vv[0]) != vec3f(0)) {
						if (recompute_normal) {
							console.warn("Vertex normal isn't given");
							faces.push_back(new Triangle(vv[0], vv[l-1], vv[l]));
						}
						else {
							// console.info("building triangle");
							// console.log("v",vv[0]);
							// console.log("v",vv[l-1]);
							// console.log("v",vv[l]);
							// console.log("N:",vvn[0]);
							// console.log("N:",vvn[l-1]);
							// console.log("N:",vvn[l]);
							// console.log("frontHint:",vvn[0]+vvn[l-1]+vvn[l]);
							faces.push_back(new Triangle(vv[0], vv[l-1], vv[l], vvn[0]+vvn[l-1]+vvn[l]));
							// console.log("planeN:",faces.back()->planeNormal);
						}
					}
				}
			} // end reading face
		} // end reading line
	}
	return Mesh(faces);
}

}
