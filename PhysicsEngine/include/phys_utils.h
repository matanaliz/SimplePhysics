#ifndef PHYS_UTILS_H
#define PHYS_UTILS_H

#include <phys_platform.h>
#include <phys_constants.h>

#include <cmath>
#include <tuple>
#include <stdexcept>

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
	using fAngle = Angle<double>;

	template <typename T = double>
	class PHYS_API Vec2D
	{
	public:
		T x;
		T y;

		Vec2D(T vx = 0, T vy = 0) : x(vx), y(vy) {}

		template <typename A>
		Vec2D(T length, Angle<A> angle) 
			: x(static_cast<T>(length * std::cos(angle.inRad())))
			, y(static_cast<T>(length * std::sin(angle.inRad())))
		{}

		~Vec2D() = default;

		Vec2D(const Vec2D& v) : x(v.x), y(v.y) {}
		Vec2D& operator=(const Vec2D& v) { x = v.x; y = v.y; return *this; }

		Vec2D(Vec2D&& v) : x(std::move(v.x)), y(std::move(v.y)) {}
		Vec2D& operator=(Vec2D&& v) { x = std::move(v.x); y = std::move(v.y); return *this; }

		// Unary arithmetic operators
		Vec2D& operator+=(const Vec2D& v) { x += v.x; y += v.y; return *this; }
		Vec2D& operator-=(const Vec2D& v) { x -= v.x; y -= v.y; return *this; }
		Vec2D operator-() { return Vec2D<T>(-x, -y); }

		// Unary scalar multiplication and division operators
		template <typename S> Vec2D& operator*=(S s) { x *= static_cast<T>(s); y *= static_cast<T>(s); return *this; }
		template <typename S> Vec2D& operator/=(S s)
		{
			if (0 == s)
				throw std::invalid_argument("Vec2D was divided by zero.");

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
	template<class T> Vec2D<T> Normalized(const Vec2D<T>& v) { return Vec2D<T>(v) / EuclideanNorm(v); }

	// Useful aliases
	using fVec2D = Vec2D<double>;

} // namespace physic

#endif // PHYS_UTILS_H