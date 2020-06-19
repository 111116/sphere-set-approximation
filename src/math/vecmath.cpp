
#include <cstring>
#include <iostream>
#include "vecmath.hpp"



// IO operations

std::istream& operator >> (std::istream& is, vec2f& v) {
	return is >> v.x >> v.y;
}
std::istream& operator >> (std::istream& is, vec3f& v) {
	return is >> v.x >> v.y >> v.z;
}
std::istream& operator >> (std::istream& is, vec4f& v) {
	return is >> v.x >> v.y >> v.z >> v.w;
}
std::ostream& operator << (std::ostream& os, const vec2f& v) {
	return os << '(' << v.x << ',' << v.y << ')';
}
std::ostream& operator << (std::ostream& os, const vec3f& v) {
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
}
std::ostream& operator << (std::ostream& os, const vec4f& v) {
	return os << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
}


