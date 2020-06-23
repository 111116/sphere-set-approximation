#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include "math/vecmath.hpp"
#include "lib/consolelog.hpp"
#include "util.hpp"

double optimize(double initial, std::function<double(double)> loss)
{
	// determine search interval
	double f_initial = loss(initial);
	double l = initial-1, r = initial+1;
	double fl = loss(l), fr = loss(r);
	for (double d=1; fl <= f_initial; d*=2) {
		l -= d;
		fl = loss(l);
	}
	for (double d=1; fr <= f_initial; d*=2) {
		r += d;
		fr = loss(r);
	}
	// Golden-section search
	const double gr = (std::sqrt(5.0) - 1) / 2;
    double c = r - (r-l) * gr;
    double d = l + (r-l) * gr;
    double fc = loss(c);
    double fd = loss(d);
    while (std::abs(c - d) > 1e-5) {
        if (fc < fd)
        {
            r = d;
            d = c;
            fd = fc;
            c = l + (d-l) * gr;
            fc = loss(c);
        }
        else
        {
            l = c;
            c = d;
            fc = fd;
            d = r - (r-c) * gr;
            fd = loss(d);
        }
    }
    // check if we have really done at least some optimization
    if (loss((l+r)/2) > loss(initial))
    	return initial;
    return (l+r)/2;
}

vec3f optimize(vec3f initial, std::function<double(vec3f)> loss)
{
	// Powell's method
	std::vector<vec3f> dir = {{1,0,0},{0,1,0},{0,0,1}};
	vec3f x0 = initial;
	double totalimprove = 0;
	while (true) {
		std::vector<double> improvement;
		vec3f x = x0;
		double f = loss(x0);
		for (auto d: dir) {
			double t = optimize(0, [&](double t){return loss(x+t*d);});
			vec3f x1 = x+t*d;
			double f1 = loss(x1);
			improvement.push_back(f - f1);
			x = x1;
			f = f1;
		}
		double improve = std::accumulate(improvement.begin(), improvement.end(), 0.0);
		// console.log("  improve",improve);
		totalimprove += improve;
		dir[argmax(improvement) - improvement.begin()] = normalized(x-x0);
		x0 = x;
		if (improve < 1e-5) break;
	}
	// console.log("total improve:", totalimprove);
	return x0;
}
