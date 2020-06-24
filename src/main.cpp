#include <vector>
#include <algorithm>

#include "sov.hpp"
#include "rtcore/objmesh.hpp"
#include "rtcore/mesh.hpp"
#include "visualize.hpp"
#include "normal_outward_test.hpp"
#include "util.hpp"
#include "pointset.hpp"
#include "powell.hpp"

std::tuple<std::vector<Sphere>, std::vector<PointSet>>
	points_assign(const std::vector<vec3f>& center, const PointSet& points, std::function<double(Sphere)> loss)
{
	// initialize
	const int n = center.size();
	std::vector<Sphere> sphere;
	std::vector<double> curloss(n,0);
	std::vector<PointSet> cluster(n);
	std::vector<bool> assigned(points.size(), false);
	std::vector<std::vector<int>> psorted; // indices of sorted points
	std::vector<std::vector<int>::iterator> pcur(n);
	for (int i=0; i<n; ++i) {
		sphere.push_back(Sphere(center[i], 0));
		// sort points by distance
		psorted.push_back(std::vector<int>(points.size()));
		std::iota(psorted[i].begin(), psorted[i].end(), 0);
		auto cmp = [&](int a, int b){
			return sqrlen(points[a] - center[i]) < sqrlen(points[b] - center[i]);
		};
		std::sort(psorted[i].begin(), psorted[i].end(), cmp);
		pcur[i] = psorted[i].begin();
	}
	std::vector<double> nextloss(n);
	for (int i=0; i<n; ++i)
		nextloss[i] = loss(Sphere(center[i], norm(points[*pcur[i]] - center[i])));
	// assign all points
	for (int _=0; _<points.size(); ++_)
	{
		// if (_ % 100 == 0) console.log("_",_);
		// find point-center pair of minimum increment in loss function
		int best = 0;
		double bestdelta = nextloss[best] - curloss[best];
		for (int i=1; i<n; ++i) {
			double t = nextloss[i] - curloss[i];
			if (t < bestdelta) {
				bestdelta = t;
				best = i;
			}
		}
		assert(!isinf(bestdelta));
		// add this point to corresponding cluster
		cluster[best].push_back(points[*pcur[best]]);
		sphere[best].radius = norm(points[*pcur[best]] - center[best]);
		curloss[best] = nextloss[best];
		assigned[*pcur[best]] = true;
		// update pcur so that they all point to unassigned points
		for (int i=0; i<n; ++i) {
			bool recompute = false;
			while (pcur[i] != psorted[i].end() && assigned[*pcur[i]]) {
				recompute = true;
				pcur[i]++;
			}
			if (recompute) {
				if (pcur[i] == psorted[i].end())
					nextloss[i] = INF;
				else
					nextloss[i] = loss(Sphere(center[i], norm(points[*pcur[i]] - center[i])));
			}
		}
	}
	return {sphere, cluster};
}

Sphere sphere_fit(const Sphere& initial, const PointSet& points, std::function<double(Sphere)> loss)
{
	// function to optimize
	if (initial.radius == 0)
		return initial;
	auto target = [&](vec3f o){
		double r = 0;
		for (auto p: points)
			r = std::max(r, norm(p-o));
		return loss(Sphere(o,r));
	};
	Sphere sphere(optimize(initial.center, target), 0);
	for (auto p: points)
		sphere.radius = std::max(sphere.radius, norm(p-sphere.center));
	return sphere;
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
	auto t = overlap_ratio(sphere);
	auto to_remove = sphere.begin() + (argmax(t) - t.begin());
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


std::vector<Sphere> sphere_set_approximate(const RTcore::Mesh& mesh, int ns, int npoint)
{
	double bestsumloss = INF;
	std::vector<Sphere> bestresult;
	auto loss = [&](Sphere s){return sov(mesh,s);};
	// initialize
	console.log("initializing...  ns:",ns);
	PointSet innerpoints = get_inner_points(mesh, npoint/2);
	PointSet surfacepoints = get_surface_points(mesh, npoint/2);
	visualize_with_mesh(surfacepoints, 0.02);
	std::vector<vec3f> center;
	// iterate over 3 steps
	std::vector<Sphere> sphere;
	std::vector<PointSet> points;
	auto checkresult = [&](){
		double sumloss = 0;
		for (int i=0; i<ns; ++i)
			sumloss += loss(sphere[i]);
		console.log("TOTAL LOSS:", sumloss);
		// save best result so far
		if (sumloss < bestsumloss) {
			bestsumloss = sumloss;
			bestresult = sphere;
		}
		return sumloss;
	};
	for (int n_trial = 0; n_trial < 1; ++n_trial)
	{
		console.log("optimizing...");
		center.clear();
		std::sample(innerpoints.begin(), innerpoints.end(), std::back_inserter(center), ns, std::mt19937(rand()));
		double outer_bestloss = INF;
		for (int iter_a=0; iter_a<100; ++iter_a) {
			double inner_bestloss = INF;
			for (int iter_b=0; iter_b<100; ++iter_b) {
				// step 1: point assignment
				console.time("point assignment");
				std::tie(sphere, points) = points_assign(center, concat(innerpoints, surfacepoints), loss);
				console.timeEnd("point assignment");
				{
					double sumloss = checkresult();
					if (sumloss > inner_bestloss - 1e-5) break;
					inner_bestloss = sumloss;
				}
				// step 2: fitting & save results
				console.time("sphere fit");
				for (int i=0; i<ns; ++i) {
					sphere[i] = sphere_fit(sphere[i], points[i], loss);
					center[i] = sphere[i].center;
				}
				console.timeEnd("sphere fit");
				{
					double sumloss = checkresult();
					if (sumloss > inner_bestloss - 1e-5) break;
					inner_bestloss = sumloss;
				}
			}
			if (inner_bestloss > outer_bestloss - 1e-5) {
				break;
			}
			outer_bestloss = inner_bestloss;
			// step 3: teleportation
			console.time("teleportation");
			teleport(sphere, points, loss);
			for (int i=0; i<ns; ++i)
				center[i] = sphere[i].center;
			console.timeEnd("teleportation");
			visualize(bestresult);
		}
	}
	// final iteration
	console.log("final iteration...");
	for (int i=0; i<ns; ++i) {
		center[i] = bestresult[i].center;
	}
	double inner_bestloss = INF;
	for (int iter_b=0; iter_b<100; ++iter_b) {
		// step 1: point assignment
		console.time("point assignment");
		std::tie(sphere, points) = points_assign(center, concat(innerpoints, surfacepoints), loss);
		console.timeEnd("point assignment");
		// step 2: fitting & save results
		console.time("sphere fit");
		for (int i=0; i<ns; ++i) {
			sphere[i] = sphere_fit(sphere[i], points[i], loss);
			center[i] = sphere[i].center;
		}
		console.timeEnd("sphere fit");
		{
			double sumloss = checkresult();
			if (sumloss > inner_bestloss - 1e-5) break;
			inner_bestloss = sumloss;
		}
	}
	visualize(bestresult);
	return bestresult;
}

int main(int argc, char* argv[])
{
	if (argc <= 2)
	{
		console.error("Usage:", argv[0], "<obj> <n_sphere> [n_point = 10000]");
		return 1;
	}
	visualizer_mesh_filename = argv[1];
	RTcore::Mesh mesh = RTcore::objmesh(argv[1]);
	test_all_normal_outward(mesh);
	int ns = atoi(argv[2]);
	int np = 10000;
	if (argc > 3) np = atoi(argv[3]);

	auto spheres = sphere_set_approximate(mesh, ns, 10000);

	console.log("Evaluating...");
	console.info("Relative Outside Volume:", volume(spheres)/volume(mesh)-1);
	for (auto s: spheres)
		std::cout << s.center << " " << s.radius << std::endl;
}
