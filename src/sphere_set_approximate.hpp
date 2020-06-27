#include <vector>
#include <algorithm>
#include <map>

#include "sov.hpp"
#include "rtcore/mesh.hpp"
#include "visualize.hpp"
#include "util.hpp"
#include "pointset.hpp"
#include "powell.hpp"


std::tuple<std::vector<Sphere>, std::vector<PointSet>>
	points_assign_MO(const std::vector<vec3f>& center, const PointSet& points, std::function<double(Sphere)> loss)
{
	// initialize
	const int n = center.size();
	std::vector<Sphere> sphere;
	std::vector<double> curloss(n,0);
	std::vector<PointSet> cluster(n);
	for (int i=0; i<n; ++i) {
		sphere.push_back(Sphere(center[i], 0));
	}
	// sorted points
	PointSet psorted;
	{
		std::vector<std::pair<double, vec3f>> ps;
		for (auto p: points) {
			double minsqrdist = INF;
			for (auto c: center)
				minsqrdist = std::min(minsqrdist, sqrlen(c - p));
			ps.push_back({minsqrdist, p});
		}
		std::sort(ps.begin(), ps.end(), [](const std::pair<double, vec3f>& a, const std::pair<double, vec3f>& b){return a.first < b.first;});
		for (auto pair: ps)
			psorted.push_back(pair.second);
	}
	// cached loss values (lower bound is looked up)
	std::vector<std::map<double,double>> cache(n);
	for (int i=0; i<n; ++i)
		cache[i][0] = 0;
	auto eval = [&](int id, const vec3f& p) {
		double r = norm(p - center[id]);
		double t = loss(Sphere(center[id], r));
		cache[id][r] = t;
		return t;
	};
	// assign all points
	for (auto p: psorted)
	{
		// find sphere of minimum loss increment
		int best = 0;
		double bestdelta = INF;
		std::vector<double> lower_bound(n);
		for (int i=0; i<n; ++i) {
			lower_bound[i] = std::prev(cache[i].upper_bound(norm(p - center[i]))) -> second;
			double incre = std::max(0.0, lower_bound[i] - curloss[i]);
			if (incre < bestdelta || (incre == bestdelta && norm(p-center[i]) < norm(p-center[best]))) {
				bestdelta = incre;
				best = i;
			}
		}
		// now we have the best candidate according to lower bound of increase in loss
		bestdelta = std::max(0.0, eval(best, p) - curloss[best]);
		for (int i=0; i<n; ++i) {
			if (i != best && lower_bound[i] - curloss[i] < bestdelta) {
				double t = eval(i, p);
				double incre = std::max(0.0, t - curloss[i]);
				if (incre < bestdelta || (incre == bestdelta && norm(p-center[i]) < norm(p-center[best]))) {
					bestdelta = incre;
					best = i;
				}
			}
		}
		// console.log("assign to",best," incre",bestdelta);
		// add this point to best cluster
		cluster[best].push_back(p);
		sphere[best].radius = std::max(sphere[best].radius, norm(p - center[best]));
		curloss[best] = std::max(curloss[best], cache[best][norm(p - center[best])]);
	}
	return {sphere, cluster};
}


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

// Note: radii are invalidated after teleportation
void teleport_n(std::vector<Sphere>& sphere, std::vector<PointSet>& points, std::function<double(Sphere)> loss)
{
	auto t = unique_volume(sphere);
	auto to_remove = sphere.begin() + (argmax(t, [](double a){return -a;}) - t.begin());
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

std::vector<Sphere> sphere_set_approximate(const RTcore::Mesh& originalmesh, const RTcore::Mesh& manifold, int ns, int ninner, int nsurface, int n_finalsample, int n_mutate)
{
	double bestsumloss = INF;
	std::vector<Sphere> bestresult;
	auto loss = [&](Sphere s){return sov(manifold,s);};
	// sample points
	console.log("initializing...  ns:",ns);
	PointSet innerpoints = get_inner_points(originalmesh, ninner);
	PointSet surfacepoints = get_surface_points(originalmesh, nsurface);
	visualize_with_mesh(surfacepoints, 0.02);

	auto checkresult = [&](const std::vector<Sphere>& sphere){
		double sumloss = 0;
		for (int i=0; i<ns; ++i)
			sumloss += loss(sphere[i]);
		console.log("TOTAL LOSS:", sumloss);
		if (sumloss < 0) {
			console.warn("NEGATIVE LOSS!");
			throw "wtf";
		}
		// save best result so far
		if (sumloss < bestsumloss) {
			bestsumloss = sumloss;
			bestresult = sphere;
		}
		return sumloss;
	};
	auto getcenter = [](std::vector<Sphere> sphere) {
		std::vector<vec3f> center;
		for (int i=0; i<sphere.size(); ++i)
			center.push_back(sphere[i].center);
		return center;
	};
	auto step1 = [&](std::vector<vec3f> center) {
		console.time("point assignment");
		auto [sphere, points] = points_assign(center, concat(innerpoints, surfacepoints), loss);
		console.timeEnd("point assignment");
		return std::make_tuple(sphere, points);
	};
	auto step2 = [&](std::vector<Sphere> sphere, std::vector<PointSet> points) {
		console.time("sphere fit");
		for (int i=0; i<ns; ++i)
			sphere[i] = sphere_fit(sphere[i], points[i], loss);
		console.timeEnd("sphere fit");
		return std::make_tuple(sphere, points);
	};
	auto step12 = [&](std::vector<vec3f> center) {
		auto [sphere, points] = step1(center);
		return step2(sphere, points);
	};
	auto step3 = [&](std::vector<Sphere> sphere, std::vector<PointSet> points) {
		console.log("teleporting...");
		auto sphere_b = sphere;
		auto points_b = points;
		teleport(sphere, points, loss);
		teleport_n(sphere_b, points_b, loss);
		auto a = step12(getcenter(sphere));
		auto b = step12(getcenter(sphere_b));
		if (checkresult(std::get<0>(a)) < checkresult(std::get<0>(b)))
			return a;
		else
			return b;
	};
	auto getrandomcenter = [&](){
		std::vector<vec3f> center;
		std::sample(innerpoints.begin(), innerpoints.end(), std::back_inserter(center), ns, std::mt19937(rand()));
		return center;
	};
	// random initialize
	auto [sphere, points] = step12(getrandomcenter());
	double curloss = checkresult(sphere);
	// iterate 3 steps
	console.info("optimizing...");
	int risecnt = 0;
	for (int i=0;; i++) {
		auto [sphere1, points1] = (i%2==0)? step1(getcenter(sphere)): step2(sphere, points);
		double loss1 = checkresult(sphere1);
		if (loss1 < curloss - 1e-6) {
			curloss = loss1;
			sphere = sphere1;
			points = points1;
		}
		else {
			auto [sphere2, points2] = step3(sphere, points);
			double loss2 = checkresult(sphere2);
			if (loss2 < curloss - 1e-6) {
				curloss = loss2;
				sphere = sphere2;
				points = points2;
				i = 1; // next step: step1
			}
			else {
				if (risecnt < n_mutate) {
					curloss = loss2;
					sphere = sphere2;
					points = points2;
					i = 1; // next step: step1
					risecnt++;
					console.log("accepting worsening mutation...");
				}
				else {
					break;
				}
			}
		}
	}
	// final iteration
	points = std::vector<PointSet>(ns);
	for (auto p: concat(innerpoints, surfacepoints)) {
		bool acc = false;
		for (int i=0; i<ns; ++i)
			if (norm(p - bestresult[i].center) <= bestresult[i].radius)
			{
				acc = true;
				points[i].push_back(p);
				break;
			}
		if (not acc) console.warn("final allocation failed");
	}
	console.info("final iteration...");
	curloss = checkresult(bestresult);
	while (true) {
		auto [sphere1, points1] = step12(getcenter(bestresult));
		double loss1 = checkresult(sphere1);
		if (loss1 < curloss - 1e-6) {
			curloss = loss1;
			bestresult = sphere1;
			points = points1;
		}
		else {
			break;
		}
	}
	// final expanding
	visualize(bestresult);
	console.info("final expanding to cover all triangles...");
	PointSet finalpoints = get_surface_points(originalmesh, n_finalsample);
	for (auto p: finalpoints) {
		auto iter = argmax(bestresult, [&](Sphere s){
			return -std::max(0.0,norm(p-s.center)-s.radius);});
		iter->radius = std::max(iter->radius, norm(p - iter->center));
		points[iter - bestresult.begin()].push_back(p);
	}
	checkresult(bestresult);
	for (int i=0; i<ns; ++i) {
		checkContain(bestresult[i], points[i]);
		bestresult[i] = sphere_fit(bestresult[i], points[i], loss);
	}
	curloss = checkresult(bestresult);
	visualize(bestresult);
	PointSet allpoints;
	for (auto p: points)
		allpoints = concat(allpoints, p);
	while (true) {
		console.time("point assignment");
		auto [s1, p1] = points_assign(getcenter(bestresult), allpoints, loss);
		console.timeEnd("point assignment");
		auto [sphere1, points1] = step2(s1, p1);
		double loss1 = checkresult(sphere1);
		if (loss1 < curloss - 1e-6) {
			curloss = loss1;
			bestresult = sphere1;
			points = points1;
		}
		else {
			break;
		}
	}
	visualize(bestresult);
	return bestresult;
}
