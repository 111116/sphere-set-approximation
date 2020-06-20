
// test sotv

#include "consolelog.hpp"
#include "sotv.hpp"
#include "sotv_ref.hpp"

double rnd()
{
	return (double)rand()/RAND_MAX*10-5;
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

int main()
{

	// console.log(sotv(a,b,c,o,r));
	// console.log(sotv_sampled(a,b,c,o,r));
	// console.log(sotv_sampled(a,b,c,o,r));
	// console.log(sotv_sampled(a,b,c,o,r));

	srand(time(0));

	for (int i=0; i<1000; ++i)
	{
		console.log("testcase",i);
		vec3f o(rnd(),rnd(),rnd());
		double r = std::abs(rnd());
		vec3f a(rnd(),rnd(),rnd());
		vec3f b(rnd(),rnd(),rnd());
		vec3f c(rnd(),rnd(),rnd());
		a += o;
		b += o;
		c += o;
		double res = sotv(a,b,c,o,r);
		std::vector<double> ans;
		for (int i=0; i<5; ++i)
			ans.push_back(sotv_sampled(a,b,c,o,r));
		double avg = average(ans);
		double s = stdev(ans);
		console.log("out:", res, "  ans:", avg,"+-",s);
		if (std::abs(res-avg) > 2*s)
		{
			console.error("error");
			console.info('a',a);
			console.info('b',b);
			console.info('c',c);
			console.info('o',o);
			console.info('r',r);
		}
	}
	
}
