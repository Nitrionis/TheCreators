#pragma once

#include <stdint.h>


template <typename T = uint8_t>
struct Vector2 {
	T x, y;

	T operator[] (int i) { return (&x)[i]; }

	Vector2(T x = 0, T y = 0) : x(x), y(y) {}

	//Vector2<T> yx() { return Vector2<T>(y, x); }
};

template <typename T = uint8_t>
struct Vector3 {
	T x, y, z;

	T operator[] (int i) { return (&x)[i]; }

	Vector3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}

	Vector3(Vector2<T> v, T z = 0) : x(v.x), y(v.y), z(z) {}

	Vector3(T x, Vector2<T> v) : x(x), y(v.x), z(v.y) {}

	//Vector2<T> xy() { return Vector2<T>(x, y); }

	//Vector2<T> yx() { return Vector2<T>(y, x); }
};

template <typename T = uint8_t>
struct Vector4 {
	T x, y, z, w;

	T operator[] (int i) { return (&x)[i]; }

	Vector4(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}

	Vector4(Vector2<T> v, T z = 0, T w = 0) : x(v.x), y(v.y), z(z), w(w) {}

	Vector4(T x, Vector2<T> v, T w = 0) : x(x), y(v.x), z(v.y), w(w) {}

	Vector4(T x, T y, Vector2<T> v) : x(x), y(y), z(v.x), w(v.y) {}

	Vector4(Vector3<T> v, T w = 0) : x(v.x), y(v.y), z(v.z), w(w) {}

	Vector4(T x, Vector3<T> v) : x(x), y(v.x), z(v.y), w(v.z) {}
};