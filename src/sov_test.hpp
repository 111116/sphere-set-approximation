
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
		console.info("testcase",i);
		vec3f o(rnd(),rnd(),rnd());
		double r = std::abs(rnd());
		// override testcase

		double res = sov(mesh,o,r);
		console.log("analytic result:", res);
		// probably skip trivial test
		if (std::abs(res - r*r*r*4/3*PI)<1e-6 && rand()%10!=0)
		{
			console.log("Skipping trivial case...");
			continue;
		}
		// angle-based monte-carlo method
		std::vector<double> ans_a;
		for (int i=0; i<5; ++i) {
			ans_a.push_back(sov_ray_sampled(mesh,o,r));
		}
		console.log("cone-numeric:   ", average(ans_a), "+-", stdev(ans_a));
		// volume-based monte-carlo method (as reference)
		std::vector<double> ans;
		for (int i=0; i<8; ++i) {
			ans.push_back(sov_sampled(mesh,o,r));
		}
		double avg = average(ans);
		double s = stdev(ans);
		console.log("vol-numeric:    ", avg,"+-",s);
		// probable error
		if (std::abs(res-avg) > 2*s)
		{
			console.error("error");
			console.warn('o',o);
			console.warn('r',r);
			printscene({{o,r}});
		}
	}
	
}
