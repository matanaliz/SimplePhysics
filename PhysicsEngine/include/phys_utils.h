#ifndef PHYS_UTILS_H
#define PHYS_UTILS_H

#include <phys_platform.h>
#include <phys_constants.h>

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
	using fAngle = Angle<double>;

	template <typename T>
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
	using fVec2D = Vec2D<double>;

	template <class T>
	class QuadTree
	{
	public:
		const int kMaxObjects = 4;
		QuadTree(int level, int x, int y, int right, int top)
			: m_level(level)
			, m_x(x)
			, m_y(y)
			, m_top(top)
			, m_right(right)
		{

		}

		std::vector<T> locate(const T& body)
		{
			fVec2D pos = body->GetPosition();
			if (pos.x < m_x || pos.x > m_right ||
				pos.y < m_y || pos.y > m_top)
			{
				return {};
			}

			if (!m_objects.empty())
				return m_objects;

			// TODO rewrite this has so much copy
			std::vector<T> res;
			if (!m_nodes.empty())
				for (auto node : m_nodes)
				{
					std::vector<T> tmp = node.locate(body);
					res.insert(res.begin(), tmp.cbegin(), tmp.cend());
				}

			return res;
		}

		bool insert(const T& body)
		{
			fVec2D pos = body->GetPosition();
			if (pos.x < m_x || pos.x > m_right ||
				pos.y < m_y || pos.y > m_top)
			{
				return false;
			}

			if (m_objects.size() < kMaxObjects)
			{
				m_objects.push_back(body);
				return true;
			}

			if (m_nodes.empty())
			{
				int mid_x = m_x + (m_right - m_x) / 2;
				int mid_y = m_y + (m_top - m_y) / 2;

				m_nodes.push_back(QuadTree(m_level + 1, m_x, m_y, mid_x, mid_y));
				m_nodes.push_back(QuadTree(m_level + 1, m_x, mid_y, mid_x, m_top));
				m_nodes.push_back(QuadTree(m_level + 1, mid_x, m_y, m_right, mid_y));
				m_nodes.push_back(QuadTree(m_level + 1, mid_x, mid_y, m_right, m_top));
			}

			for (auto& node : m_nodes)
				if (node.insert(body))
					return true;

			return false;
		}
		void clear()
		{
			m_objects.clear();

			for (auto node : m_nodes)
				node.clear();
		}

	private:
		int m_level;
		int m_x;
		int m_y;
		int m_top;
		int m_right;

		std::vector<T> m_objects;
		std::vector<QuadTree> m_nodes;

	};

} // namespace physic

#endif // PHYS_UTILS_H