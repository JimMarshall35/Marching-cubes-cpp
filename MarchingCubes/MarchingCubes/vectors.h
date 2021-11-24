#pragma once
#include "typedefs.h"
#include <math.h>

struct ivec3 {
	ivec3() : x(0), y(0), z(0) {}
	explicit ivec3(i32 val) : x(val), y(val), z(val) {}
	ivec3(i32 x, i32 y, i32 z) : x(x), y(y), z(z) {}
	i32 x;
	i32 y;
	i32 z;
	ivec3 operator+(const ivec3& other) const {
		return ivec3(x + other.x, y + other.y, z + other.z);
	}
	ivec3 operator-(const ivec3& other) const {
		return ivec3(y - other.x, y - other.y, z - other.z);
	}
};


struct vec3 {
	vec3() : x(0), y(0), z(0) {}
	explicit vec3(f32 val) : x(val), y(val), z(val) {}
	vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}
	f32 x;
	f32 y;
	f32 z;
	vec3 operator*(const f32 scalar) const {
		return vec3(x * scalar, y * scalar, z * scalar);
	}
	vec3 operator+(const vec3& other) const {
		return vec3(x + other.x, y + other.y, z + other.z);
	}
	vec3 operator-(const vec3& other) const {
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	void operator+=(const vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}
	f32 magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}
	vec3 normalize() const {
		return vec3(x, y, z) * (1/magnitude());
	};
	static f32 distance(vec3 v0, vec3 v1) {
		return (v1 - v0).magnitude();
	}
};