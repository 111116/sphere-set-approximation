
// test sov

#pragma once

#include "lib/consolelog.hpp"
#include "sotv.hpp"
#include "sotv_ref.hpp"
#include "sov.hpp"
#include "sov_ref.hpp"
#include "sov_ref_ray.hpp"
#include "visualize.hpp"
#include "normal_outward_test.hpp"

double rnd()
{
	return (double)rand()/RAND_MAX*2-1;
}


double average(const std::vector<double>& a)
{
	if (a.empty()) throw "invalid";
	double sum = 0;
	for (double x: a)
		sum += x;
	return sum / a.size();
}

double stdev(const std::vector<double>& a)
{
	if (a.empty()) throw "invalid";
	double avg = average(a);
	double s = 0;
	for (double x: a)
		s += (x-avg) * (x-avg);
	return sqrt(s/(a.size()-1));
}

void testsov(const RTcore::Mesh& mesh)
{
	srand(0);
	test_all_normal_outward(mesh);
	for (int i=0; i<1000; ++i)
	{
		console.log("testcase",i);
		vec3f o(rnd(),rnd(),rnd());
		double r = std::abs(rnd());
		// override testcase
		o = vec3f(-0.26742,-0.521586,-0.294583);
		r = 0.939748;

		double res = sov(mesh,o,r);
		console.log("result:",res);
		// angle-based monte-carlo method
		for (int i=0; i<5; ++i) {
			console.log("raysampled:",sov_ray_sampled(mesh,o,r));
		}
		// volume-based monte-carlo method (as reference)
		std::vector<double> ans;
		for (int i=0; i<5; ++i) {
			ans.push_back(sov_sampled(mesh,o,r));
			console.log("sampled",ans.back());
		}
		double avg = average(ans);
		double s = stdev(ans);
		console.log("out:", res, "  ans:", avg,"+-",s);
		printscene({{o,r}});
		// probable error
		if (std::abs(res-avg) > 2*s)
		{
			console.error("error");
			console.warn('o',o);
			console.warn('r',r);
		}
	}
	
}
