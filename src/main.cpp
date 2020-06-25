#include <iostream>
#include <fstream>
#include "rtcore/objmesh.hpp"
#include "visualize.hpp"
#include "normal_outward_test.hpp"
#include "util.hpp"
#include "pointset.hpp"
#include "sphere_set_approximate.hpp"
#include "lib/argparse.h"

static const char *const usage[] = {
    "main [options] <obj>",
    NULL,
};

int main(int argc, const char* argv[])
{
	int n_sphere = 64;
	int n_innersample = 12000;
	int n_surfacesample = 6000;
	int n_finalsample = 200000;
	int n_mutate = 10;
	int seed = 19260817;

	argparse_option opt[] = {
        OPT_GROUP("Options:"),
        OPT_HELP(),
        OPT_INTEGER('n', "sphere", &n_sphere, "number of spheres to create"),
        OPT_INTEGER(0, "inner", &n_innersample, "number of inner sample points"),
        OPT_INTEGER(0, "surface", &n_surfacesample, "number of surface sample points"),
        OPT_INTEGER(0, "final", &n_finalsample, "number of final samples"),
        OPT_INTEGER(0, "mutate", &n_mutate, "number of attempts to find minimum"),
        OPT_INTEGER(0, "seed", &seed, "seed of random number generator"),
        OPT_END(),
    };
    argparse parser;
    argparse_init(&parser, opt, usage, 0);
	argc = argparse_parse(&parser, argc, argv);

	if (argc == 0) {
		argparse_usage(&parser);
		return 1;
	}
	visualizer_mesh_filename = argv[1];
	RTcore::Mesh mesh = RTcore::objmesh(argv[1]);
	test_all_normal_outward(mesh);


	srand(seed);
	auto spheres = sphere_set_approximate(mesh, n_sphere, n_innersample, n_surfacesample, n_finalsample, n_mutate);

	for (auto s: spheres)
		std::cout << s.center << " " << s.radius << std::endl;

	console.log("Evaluating...");
	console.info("Relative Outside Volume (Er):", volume(spheres)/volume(mesh)-1);
}
