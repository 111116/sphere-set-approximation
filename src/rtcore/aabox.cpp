
#include <limits>
#include "aabox.hpp"

namespace RTcore
{

const AABox AABox::infAAB (
	-std::numeric_limits<float>::infinity(),
	 std::numeric_limits<float>::infinity(),
	-std::numeric_limits<float>::infinity(),
	 std::numeric_limits<float>::infinity(),
	-std::numeric_limits<float>::infinity(),
	 std::numeric_limits<float>::infinity()
);

// AABB of union geometry
AABox operator+ (const AABox& a, const AABox& b) {
	AABox t;
	t.x1 = fmin(a.x1, b.x1);
	t.x2 = fmax(a.x2, b.x2);
	t.y1 = fmin(a.y1, b.y1);
	t.y2 = fmax(a.y2, b.y2);
	t.z1 = fmin(a.z1, b.z1);
	t.z2 = fmax(a.z2, b.z2);
	return t;
}

// AABB of union geometry
AABox operator+ (const AABox& a, const point& p) {
	AABox t;
	t.x1 = fmin(a.x1, p.x);
	t.x2 = fmax(a.x2, p.x);
	t.y1 = fmin(a.y1, p.y);
	t.y2 = fmax(a.y2, p.y);
	t.z1 = fmin(a.z1, p.z);
	t.z2 = fmax(a.z2, p.z);
	return t;
}

}
