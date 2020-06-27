#include <iostream>
#include <fstream>
#include "rtcore/objmesh.hpp"
#include "visualize.hpp"
#include "mesh_test.hpp"
#include "util.hpp"
#include "pointset.hpp"
#include "sphere_set_approximate.hpp"
#include "lib/argparse.h"

static const char *const usage[] = {
    "main -i <obj> -m <manifold> -n <n_sphere> [options]",
    NULL,
};

int main(int argc, const char* argv[])
{
	// default parameters
	int n_sphere = 64;
	int n_innersample = 8000;
	int n_surfacesample = 4000;
	int n_finalsample = 100000;
	int n_mutate = 10;
	int seed = 19260817;
	const char *objpath = NULL;
	const char *manifoldpath = NULL;

	// parse command line arguments
	argparse_option opt[] = {
        OPT_GROUP("Options:"),
        OPT_HELP(),
        OPT_STRING('i', "obj", &objpath, "input mesh to approximate"),
        OPT_STRING('m', "manifold", &manifoldpath, "simplified manifold of input mesh"),
        OPT_INTEGER('n', "sphere", &n_sphere, "number of spheres to construct, default=64"),
        OPT_INTEGER(0, "inner", &n_innersample, "number of inner sample points, default=8000"),
        OPT_INTEGER(0, "surface", &n_surfacesample, "number of surface sample points, default=4000"),
        OPT_INTEGER(0, "final", &n_finalsample, "number of final coverage samples, default=100000"),
        OPT_INTEGER(0, "mutate", &n_mutate, "number of global optima explorations, default=10"),
        OPT_INTEGER(0, "seed", &seed, "seed of random number generator"),
        OPT_END(),
    };
    argparse parser;
    argparse_init(&parser, opt, usage, 0);
	argc = argparse_parse(&parser, argc, argv);
	if (!objpath || !manifoldpath) {
		argparse_usage(&parser);
		return 1;
	}

	// load meshs
	RTcore::Mesh mesh = RTcore::objmesh(objpath);
	RTcore::Mesh manifold = RTcore::objmesh(manifoldpath);
	visualizer_mesh_filename = objpath;
	// examine mesh orientation
	test_all_normal_outward(manifold);

	// sphere construction
	srand(seed);
	auto spheres = sphere_set_approximate(mesh, manifold, n_sphere, n_innersample, n_surfacesample, n_finalsample, n_mutate);

	// output spheres
	for (auto s: spheres)
		std::cout << s.center << " " << s.radius << std::endl;

	// evaluate how well the spheres fit the model
	visualize(spheres);
	console.log("Evaluating...");
	console.info("Relative Outside Volume (Er):", volume(spheres)/volume(manifold)-1);
}
