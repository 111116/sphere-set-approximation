
#define VOXELIZER_IMPLEMENTATION
#include "lib/voxelizer.h"
#include "sotv.hpp"
#include "sov_test.hpp"

#include "rtcore/objmesh.hpp"
#include "rtcore/mesh.hpp"
#include "visualize.hpp"

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		console.error("Usage:", argv[0], "<obj>");
		return 1;
	}
	visualizer_mesh_filename = argv[1];
	RTcore::Mesh m = RTcore::objmesh(argv[1]);
	testsov(m);
}
