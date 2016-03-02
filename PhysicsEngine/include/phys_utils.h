#ifndef PHYS_UTILS_H
#define PHYS_UTILS_H

#include <cmath>
#include <tuple>

// TODO move implementation to cpp
template <class T>
class PHYS_API Vec2D
{
public:
	T x;
	T y;

	explicit Vec2D(T vx = 0, T vy = 0) : x(vx), y(vy) {}
	Vec2D(const Vec2D& v) : x(v.x), y(v.y) {}
	Vec2D& operator=(const Vec2D& v) { x = v.x; y = v.y; return *this; }

	// Unary arithmetic operators
	Vec2D& operator+=(const Vec2D& v) { x += v.x; y += v.y; return *this; }
	Vec2D& operator-=(const Vec2D& v) { x -= v.x; y -= v.y; return *this; }

	// Unary scalar multiplication and division operators
	template <typename S> Vec2D& operator*=(S s) { x *= s; y *= s; return *this; }
	template <typename S> Vec2D& operator/=(S s) { x /= s; y /= s; return *this; }
};

	// Compare operators
	template<class T> bool operator==(const Vec2D<T>& l, const Vec2D<T>& r) { return std::tie(l.x, l.y) == std::tie(r.x, r.y); }
	template<class T> bool operator!=(const Vec2D<T>& l, const Vec2D<T>& r) { return !(l == r); }

	// Binary arithmetic operators 
	template<class T> Vec2D<T> operator+(const Vec2D<T>& l, const Vec2D<T>& r) { return Vec2D<T>(l) += r; }
	template<class T> Vec2D<T> operator-(const Vec2D<T>& l, const Vec2D<T>& r) { return Vec2D<T>(l) -= r; }

	// Binary scalar multiplication and division operators
	template<class T, typename S> Vec2D<T> operator*(const S& s, const Vec2D<T>& v) { return Vec2D<T>(v) *= s; }
	template<class T, typename S> Vec2D<T> operator*(const Vec2D<T>& v, const S& s) { return Vec2D<T>(v) *= s; }
	template<class T, typename S> Vec2D<T> operator/(const S& s, const Vec2D<T>& v) { return Vec2D<T>(v) /= s; }
	template<class T, typename S> Vec2D<T> operator/(const Vec2D<T>& v, const S& s) { return Vec2D<T>(v) /= s; }
	
	template<class T> T EuclideanNorm(const Vec2D<T>& v) { return std::sqrt(v.x * v.x + v.y * v.y); }

	// Useful aliases
	using fVec2D = Vec2D<float>;

#endif // PHYS_UTILS_H