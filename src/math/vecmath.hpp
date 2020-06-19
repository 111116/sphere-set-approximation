// mathematics stuff
#pragma once

#include <cmath>
#include <iosfwd>


typedef double float_t;


const float_t PI = acos(-1);


struct vec2f
{
	float_t x,y;
	vec2f(): x(0), y(0) {}
	vec2f(float_t p): x(p), y(p) {}
	vec2f(float_t px, float_t py): x(px), y(py) {}
	bool nonzero() const { return x || y; }
};

struct vec3f
{
	float_t x,y,z;
	vec3f(): x(0), y(0), z(0) {}
	vec3f(float_t p): x(p), y(p), z(p) {}
	vec3f(float_t px, float_t py, float_t pz): x(px), y(py), z(pz) {}
	bool nonzero() const { return x || y || z; }
};

struct vec4f
{
	float_t x,y,z,w;
	vec4f(): x(0), y(0), z(0), w(0) {}
	vec4f(float_t p): x(p), y(p), z(p), w(p) {}
	vec4f(float_t px, float_t py, float_t pz, float_t pw): x(px), y(py), z(pz), w(pw) {}
	vec4f(const vec3f& a, float_t pw): x(a.x), y(a.y), z(a.z), w(pw) {}
	vec3f xyz() const { return vec3f(x,y,z); }
	bool nonzero() const { return x || y || z || w; }
};


// IO operations
std::istream& operator >> (std::istream& is, vec2f& v);
std::istream& operator >> (std::istream& is, vec3f& v);
std::istream& operator >> (std::istream& is, vec4f& v);
std::ostream& operator << (std::ostream& os, const vec2f& v);
std::ostream& operator << (std::ostream& os, const vec3f& v);
std::ostream& operator << (std::ostream& os, const vec4f& v);


// single float_t operations
vec2f operator * (float_t k, const vec2f& v);
vec2f operator * (const vec2f& v, float_t k);
vec2f operator / (const vec2f& v, float_t k);

vec3f operator * (float_t k, const vec3f& v);
vec3f operator * (const vec3f& v, float_t k);
vec3f operator / (const vec3f& v, float_t k);

vec4f operator * (float_t k, const vec4f& v);
vec4f operator * (const vec4f& v, float_t k);
vec4f operator / (const vec4f& v, float_t k);

vec2f& operator *= (vec2f& a, float_t k);
vec3f& operator *= (vec3f& a, float_t k);
vec4f& operator *= (vec4f& a, float_t k);


// element-wise operations
vec2f operator + (const vec2f& a, const vec2f& b);
vec3f operator + (const vec3f& a, const vec3f& b);
vec4f operator + (const vec4f& a, const vec4f& b);
vec2f operator - (const vec2f& a, const vec2f& b);
vec3f operator - (const vec3f& a, const vec3f& b);
vec4f operator - (const vec4f& a, const vec4f& b);
vec2f operator * (const vec2f& a, const vec2f& b);
vec3f operator * (const vec3f& a, const vec3f& b);
vec4f operator * (const vec4f& a, const vec4f& b);
vec2f operator / (const vec2f& a, const vec2f& b);
vec3f operator / (const vec3f& a, const vec3f& b);
vec4f operator / (const vec4f& a, const vec4f& b);

vec2f& operator += (vec2f& a, const vec2f& b);
vec3f& operator += (vec3f& a, const vec3f& b);
vec4f& operator += (vec4f& a, const vec4f& b);
vec2f& operator -= (vec2f& a, const vec2f& b);
vec3f& operator -= (vec3f& a, const vec3f& b);
vec4f& operator -= (vec4f& a, const vec4f& b);
vec2f& operator *= (vec2f& a, const vec2f& b);
vec3f& operator *= (vec3f& a, const vec3f& b);
vec4f& operator *= (vec4f& a, const vec4f& b);


// unary operations
vec2f operator-(const vec2f& v);
vec3f operator-(const vec3f& v);
vec4f operator-(const vec4f& v);

float_t sqrlen(const vec2f& v);
float_t sqrlen(const vec3f& v);
float_t sqrlen(const vec4f& v);

float_t norm(const vec2f& v);
float_t norm(const vec3f& v);
float_t norm(const vec4f& v);

vec2f normalized(const vec2f& v);
vec3f normalized(const vec3f& v);
vec4f normalized(const vec4f& v);


// other operations

float_t dot(const vec3f& a, const vec3f& b);

vec3f cross(const vec3f& a, const vec3f& b);

bool operator == (const vec3f& a, const vec3f& b);
bool operator != (const vec3f& a, const vec3f& b);



// inline implementations


// single float_t operations

inline vec2f operator * (float_t k, const vec2f& v) {
	return vec2f(k*v.x, k*v.y);
}
inline vec2f operator * (const vec2f& v, float_t k) {
	return vec2f(k*v.x, k*v.y);
}
inline vec2f operator / (const vec2f& v, float_t k) {
	return vec2f(v.x/k, v.y/k);
}

inline vec3f operator * (float_t k, const vec3f& v) {
	return vec3f(k*v.x, k*v.y, k*v.z);
}
inline vec3f operator * (const vec3f& v, float_t k) {
	return vec3f(k*v.x, k*v.y, k*v.z);
}
inline vec3f operator / (const vec3f& v, float_t k) {
	return vec3f(v.x/k, v.y/k, v.z/k);
}

inline vec4f operator * (float_t k, const vec4f& v) {
	return vec4f(k*v.x, k*v.y, k*v.z, k*v.w);
}
inline vec4f operator * (const vec4f& v, float_t k) {
	return vec4f(k*v.x, k*v.y, k*v.z, k*v.w);
}
inline vec4f operator / (const vec4f& v, float_t k) {
	return vec4f(v.x/k, v.y/k, v.z/k, v.w/k);
}

inline vec2f& operator *= (vec2f& a, float_t k)
{
	a.x *= k;
	a.y *= k;
	return a;
}
inline vec3f& operator *= (vec3f& a, float_t k)
{
	a.x *= k;
	a.y *= k;
	a.z *= k;
	return a;
}
inline vec4f& operator *= (vec4f& a, float_t k)
{
	a.x *= k;
	a.y *= k;
	a.z *= k;
	a.w *= k;
	return a;
}



// element-wise operations

inline vec2f operator + (const vec2f& a, const vec2f& b) {
	return vec2f(a.x + b.x, a.y + b.y);
}
inline vec3f operator + (const vec3f& a, const vec3f& b) {
	return vec3f(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline vec4f operator + (const vec4f& a, const vec4f& b) {
	return vec4f(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
inline vec2f operator - (const vec2f& a, const vec2f& b) {
	return vec2f(a.x - b.x, a.y - b.y);
}
inline vec3f operator - (const vec3f& a, const vec3f& b) {
	return vec3f(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline vec4f operator - (const vec4f& a, const vec4f& b) {
	return vec4f(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}
inline vec2f operator * (const vec2f& a, const vec2f& b) {
	return vec2f(a.x*b.x, a.y*b.y);
}
inline vec3f operator * (const vec3f& a, const vec3f& b) {
	return vec3f(a.x*b.x, a.y*b.y, a.z*b.z);
}
inline vec4f operator * (const vec4f& a, const vec4f& b) {
	return vec4f(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}
inline vec2f operator / (const vec2f& a, const vec2f& b) {
	return vec2f(a.x/b.x, a.y/b.y);
}
inline vec3f operator / (const vec3f& a, const vec3f& b) {
	return vec3f(a.x/b.x, a.y/b.y, a.z/b.z);
}
inline vec4f operator / (const vec4f& a, const vec4f& b) {
	return vec4f(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w);
}

inline vec2f& operator += (vec2f& a, const vec2f& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
inline vec3f& operator += (vec3f& a, const vec3f& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}
inline vec4f& operator += (vec4f& a, const vec4f& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

inline vec2f& operator -= (vec2f& a, const vec2f& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
inline vec3f& operator -= (vec3f& a, const vec3f& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}
inline vec4f& operator -= (vec4f& a, const vec4f& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

inline vec2f& operator *= (vec2f& a, const vec2f& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}
inline vec3f& operator *= (vec3f& a, const vec3f& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
}
inline vec4f& operator *= (vec4f& a, const vec4f& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}



// unary operations
inline vec2f operator-(const vec2f& v) {
	return vec2f(-v.x, -v.y);
}
inline vec3f operator-(const vec3f& v) {
	return vec3f(-v.x, -v.y, -v.z);
}
inline vec4f operator-(const vec4f& v) {
	return vec4f(-v.x, -v.y, -v.z, -v.w);
}
inline float_t sqrlen(const vec2f& v) {
	return v.x*v.x + v.y*v.y;
}
inline float_t sqrlen(const vec3f& v) {
	return v.x*v.x + v.y*v.y + v.z*v.z;
}
inline float_t sqrlen(const vec4f& v) {
	return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}
inline float_t norm(const vec2f& v) {
	return sqrt(v.x*v.x + v.y*v.y);
}
inline float_t norm(const vec3f& v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
inline float_t norm(const vec4f& v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}
inline vec2f normalized(const vec2f& v)
{
	float_t lambda = 1/norm(v);
	return vec2f(lambda*v.x, lambda*v.y);
}
inline vec3f normalized(const vec3f& v)
{
	float_t lambda = 1/norm(v);
	return vec3f(lambda*v.x, lambda*v.y, lambda*v.z);
}
inline vec4f normalized(const vec4f& v)
{
	float_t lambda = 1/norm(v);
	return vec4f(lambda*v.x, lambda*v.y, lambda*v.z, lambda*v.w);
}



// other operations

inline float_t dot(const vec3f& a, const vec3f& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline vec3f cross(const vec3f& a, const vec3f& b)
{
	return vec3f(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

inline bool operator == (const vec3f& a, const vec3f& b) {
	return a.x==b.x && a.y==b.y && a.z==b.z;
}

inline bool operator != (const vec3f& a, const vec3f& b) {
	return a.x!=b.x || a.y!=b.y || a.z!=b.z;
}