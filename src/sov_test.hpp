
// test sov

#include "lib/consolelog.hpp"
#include "sov.hpp"
#include "sov_ref.hpp"
#include "visualize.hpp"

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

	for (int i=0; i<1000; ++i)
	{
		console.log("testcase",i);
		vec3f o(rnd(),rnd(),rnd());
		double r = std::abs(rnd());

		double res = sov(mesh,o,r);
		console.log("result",res);
		std::vector<double> ans;
		for (int i=0; i<5; ++i) {
			ans.push_back(sov_sampled(mesh,o,r));
			console.log("sampled",ans.back());
		}
		double avg = average(ans);
		double s = stdev(ans);
		console.log("out:", res, "  ans:", avg,"+-",s);
		if (std::abs(res-avg) > 2*s)
		{
			printscene({{o,r}});
			console.error("error");
			console.warn('o',o);
			console.warn('r',r);
		}
	}
	
}
