#ifndef PHYS_UTILS_H
#define PHYS_UTILS_H

#include <phys_platform.h>
//#include <phys_constants.h>

#include <cmath>
#include <tuple>
#include <stdexcept>
#include <cassert>
#include <vector>

namespace physic
{

	template <typename T>
	T Clip(const T& n, const T& lower, const T& upper) {
		return std::max(lower, std::min(n, upper));
	}

	template <typename T = double>
	class PHYS_API Angle
	{
	public:
		Angle(T angle) : m_angle(angle) {}
		~Angle() = default;
		T inRad() const { return static_cast<T>(m_angle * kPi / 180.f); }
		T inDeg() const { return m_angle; }

	private:
		T m_angle;
	};

	// Useful aliases
	using fAngle = Angle<float>;

	template <typename T>
	class PHYS_API Vec2D
	{
	public:
		T x;
		T y;

		Vec2D(T vx = 0, T vy = 0) : x(vx), y(vy) {}

		template <typename U>
		Vec2D(T length, Angle<U> angle)
			: x(static_cast<T>(length * std::cos(angle.inRad())))
			, y(static_cast<T>(length * std::sin(angle.inRad())))
		{}

		//template <typename U>
		//Vec2D(U length, U angle)
		//	: x(static_cast<T>(length * std::cos(angle.inRad())))
		//	, y(static_cast<T>(length * std::sin(angle.inRad())))
		//{}

		~Vec2D() = default;

		Vec2D(const Vec2D& v) : x(v.x), y(v.y) {}
		Vec2D& operator=(const Vec2D& v) { x = v.x; y = v.y; return *this; }

		Vec2D(Vec2D&& v) : x(std::move(v.x)), y(std::move(v.y)) {}
		Vec2D& operator=(Vec2D&& v) { x = std::move(v.x); y = std::move(v.y); return *this; }

		// Unary arithmetic operators
		Vec2D& operator+=(const Vec2D& v) { x += v.x; y += v.y; return *this; }
		Vec2D& operator-=(const Vec2D& v) { x -= v.x; y -= v.y; return *this; }
		Vec2D operator-() const { return Vec2D<T>(-x, -y); }

		// Unary scalar multiplication and division operators
		template <typename S> Vec2D& operator*=(S s) { x *= static_cast<T>(s); y *= static_cast<T>(s); return *this; }
		template <typename S> Vec2D& operator/=(S s)
		{
			assert(0 != s);
			x /= s; y /= s; return *this;
		}

		typedef T type;
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

	// Get length of vector
	template<class T> T EuclideanNorm(const Vec2D<T>& v) { return std::sqrt(v.x * v.x + v.y * v.y); }
	template<class T> T DotProduct(const Vec2D<T>& a, const Vec2D<T>& b) { return a.x * b.x + a.y * b.y; }
	template<class T> T GetAngle(const Vec2D<T>& a, const Vec2D<T>& b) { return std::acos(DotProduct(a, b) / (EuclideanNorm(a) * EuclideanNorm(b))) * 180.f / kPi; }
	template<class T> Vec2D<T> Normalized(const Vec2D<T>& v) { T norm = EuclideanNorm(v); return norm != 0 ? Vec2D<T>(v) / norm : Vec2D<T>(static_cast<T>(0), static_cast<T>(0)); }
	template<class T> Vec2D<T> Round(const Vec2D<T>& v) { return Vec2D<T>(std::round(v.x), std::round(v.y)); }

	// Useful aliases
	using fVec2D = Vec2D<float>;

	template <typename T>
	class PHYS_API Point2D
	{
	public:
		T x;
		T y;

		Point2D(T vx = 0, T vy = 0) : x(vx), y(vy) {}
		~Point2D() = default;

		Point2D(const Point2D& v) : x(v.x), y(v.y) {}
		Point2D& operator=(const Point2D& v) { x = v.x; y = v.y; return *this; }

		Point2D(Point2D&& v) : x(std::move(v.x)), y(std::move(v.y)) {}
		Point2D& operator=(Point2D&& v) { x = std::move(v.x); y = std::move(v.y); return *this; }

		// Unary arithmetic operators
		Point2D& operator+=(const Point2D& v) { x += v.x; y += v.y; return *this; }
		Point2D& operator-=(const Point2D& v) { x -= v.x; y -= v.y; return *this; }
		
		template <typename U>
		Point2D& operator+=(const Vec2D<U>& v) { x += v.x; y += v.y; return *this; }
		template <typename U>
		Point2D& operator-=(const Vec2D<U>& v) { x -= v.x; y -= v.y; return *this; }

		Point2D operator-() const { return Point2D<T>(-x, -y); }

		typedef T type;
	};

	// Binary arithmetic operators 
	template<typename T> 
	Vec2D<T> operator+(const Point2D<T>& l, const Point2D<T>& r) { return Vec2D<T>(l.x + r.x, l.y + r.y); }
	template<typename T>
	Vec2D<T> operator-(const Point2D<T>& l, const Point2D<T>& r) { return Vec2D<T>(l.x - r.x, l.y - r.y); }

	template<class T> bool IsPointInRect(const Point2D<T>& p, const Point2D<T>& bot_left, const Point2D<T>& top_right)
	{
		return  p.x >= bot_left.x && p.x <= top_right.x &&
				p.y >= bot_left.y && p.y <= top_right.y;
	}

	using Point = Point2D<float>;

	class PHYS_API Mass
	{
	public:
		float mass;
		float inv_mass;

		Mass(float m) : mass(m), inv_mass(m ? 1.f / m : 0.f) {}
	};

} // namespace physic

#endif // PHYS_UTILS_H