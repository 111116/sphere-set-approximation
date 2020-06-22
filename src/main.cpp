#include <vector>
#include <algorithm>

#include "sov.hpp"
#include "rtcore/objmesh.hpp"
#include "rtcore/mesh.hpp"
#include "visualize.hpp"
#include "normal_outward_test.hpp"
#include "util.hpp"
#include "pointset.hpp"


std::tuple<std::vector<Sphere>, std::vector<PointSet>>
	points_assign(const std::vector<vec3f>& center, const PointSet& points, std::function<double(Sphere)> loss)
{
	std::vector<Sphere> sphere;
	std::vector<PointSet> cluster;
}

Sphere sphere_fit(const Sphere& initial, const PointSet& points)
{
	return initial;
}

void checkContain(const Sphere& s, const PointSet& points)
{
	for (auto p: points) {
		if (sqrlen(p - s.center) > s.radius * s.radius * (1+1e-5)) {
			console.warn("check contain failed");
			return;
		}
	}
}


// Note: radii are invalidated after teleportation
void teleport(std::vector<Sphere>& sphere, std::vector<PointSet>& points, std::function<double(Sphere)> loss)
{
	auto overlap_ratio = [&](Sphere s){
		double overlap = 0;
		for (auto s1: sphere)
			overlap = std::max(overlap, sphere_overlap_volume(s, s1));
		return overlap / s.volume();
	};
	auto to_remove = argmax(sphere, overlap_ratio);
	auto to_split = argmax(sphere, loss);
	if (to_remove == to_split) {
		console.warn("to_remove = to_split");
		return;
	}
	int index_to_split = to_split - sphere.begin();
	auto [p1,p2] = farthest_points_apart(points[index_to_split]);
	// replace these 2 spheres with the splited
	*to_remove = Sphere(p1, 0);
	*to_split = Sphere(p2, 0);
}


std::vector<Sphere> sphere_set_approximate(const RTcore::Mesh& mesh, int ns)
{
	double bestsumloss = std::numeric_limits<double>::infinity();
	std::vector<Sphere> bestresult;
	// initialize
	console.log("initializing...");
	auto loss = [&](Sphere s){return sov(mesh,s);};
	PointSet innerpoints = get_inner_points(mesh);
	PointSet surfacepoints = get_surface_points(mesh);
	std::vector<vec3f> center;
	std::sample(center.end(), innerpoints.begin(), innerpoints.end(), ns, std::mt19937(0));
	console.log("optimizing...");
	// iterate over 3 steps
	std::vector<Sphere> sphere;
	std::vector<PointSet> points;
	// step 1: point assignment
	std::tie(sphere, points) = points_assign(center, concat(innerpoints, surfacepoints), loss);
	// step 2: fitting & save results
	{
		double sumloss = 0;
		for (int i=0; i<ns; ++i) {
			checkContain(sphere[i], points[i]); // debug
			sphere[i] = sphere_fit(sphere[i], points[i]);
			checkContain(sphere[i], points[i]); // debug
			sumloss += loss(sphere[i]);
		}
		// save best result so far
		if (sumloss < bestsumloss) {
			bestsumloss = sumloss;
			bestresult = sphere;
		}
	}
	// step 3: teleportation
	teleport(sphere, points, loss);
	for (int i=0; i<ns; ++i)
		center[i] = sphere[i].center;

	return bestresult;
}

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		console.error("Usage:", argv[0], "<obj>");
		return 1;
	}
	visualizer_mesh_filename = argv[1];
	RTcore::Mesh mesh = RTcore::objmesh(argv[1]);
	test_all_normal_outward(mesh);

	auto spheres = sphere_set_approximate(mesh, 50);


}
