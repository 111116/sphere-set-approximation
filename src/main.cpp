#include <iostream>
#include <fstream>
#include "rtcore/objmesh.hpp"
#include "visualize.hpp"
#include "normal_outward_test.hpp"
#include "util.hpp"
#include "pointset.hpp"
#include "sphere_set_approximate.hpp"


int main(int argc, char* argv[])
{
	if (argc <= 2)
	{
		console.error("Usage:", argv[0], "<obj> <n_sphere>");
		return 1;
	}

	visualizer_mesh_filename = argv[1];
	RTcore::Mesh mesh = RTcore::objmesh(argv[1]);
	test_all_normal_outward(mesh);

	int n_sphere = atoi(argv[2]);
	int n_innersample = 10000;
	int n_surfacesample = 4000;
	int n_finalsample = 100000;
	int n_mutate = 10;
	int seed = 19260817;

	srand(seed);
	auto spheres = sphere_set_approximate(mesh, n_sphere, n_innersample, n_surfacesample, n_finalsample, n_mutate);

	for (auto s: spheres)
		std::cout << s.center << " " << s.radius << std::endl;

	console.log("Evaluating...");
	console.info("Relative Outside Volume (Er):", volume(spheres)/volume(mesh)-1);
}
