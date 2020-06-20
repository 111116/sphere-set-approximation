
// test sotv

#include "consolelog.hpp"
#include "sotv.hpp"
#include "sotv_ref.hpp"

int main() {
	// case b with potential bug
	vec3f O = vec3f(0,0,0);
	double R = 4;
	vec3f C = vec3f(5.71295,1.50167,2);
	vec3f D = vec3f(1.04163,-5.61882,1.30032);
	vec3f E = vec3f(-6.24324,2.82544,2.25269);
	console.log(sotv(C,D,E,O,R));
	console.log(sotv_sampled(C,D,E,O,R));
	console.log(sotv_sampled(C,D,E,O,R));
	console.log(sotv_sampled(C,D,E,O,R));
}