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
		console.error("Usage:", argv[0], "<obj> <n_sphere> [n_point = 10000]");
		return 1;
	}
	srand(19260817);
	
	visualizer_mesh_filename = argv[1];
	RTcore::Mesh mesh = RTcore::objmesh(argv[1]);
	test_all_normal_outward(mesh);
	int ns = atoi(argv[2]);
	int np = 10000;
	if (argc > 3) np = atoi(argv[3]);

	auto spheres = sphere_set_approximate(mesh, ns, 10000, 5000, 10);

	console.log("Evaluating...");
	console.info("Relative Outside Volume:", volume(spheres)/volume(mesh)-1);
	for (auto s: spheres)
		std::cout << s.center << " " << s.radius << std::endl;
}
