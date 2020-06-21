
#define VOXELIZER_IMPLEMENTATION
#include "lib/voxelizer.h"
#include "sov.hpp"

#include "rtcore/objmesh.hpp"
#include "rtcore/mesh.hpp"
#include "visualize.hpp"
#include "normal_outward_test.hpp"

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		console.error("Usage:", argv[0], "<obj>");
		return 1;
	}
	visualizer_mesh_filename = argv[1];
	RTcore::Mesh m = RTcore::objmesh(argv[1]);
	test_all_normal_outward(m);
}
