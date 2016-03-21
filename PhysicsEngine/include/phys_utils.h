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
		T inRad() const { return static_cast<T>(m_angle * 3.141592 / 180.f); }
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

		Point2D operator-() { return Point2D<T>(-x, -y); }

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

		explicit Mass(float m) : mass(m), inv_mass(m ? 1.f / m : 0.f) {}
	};

	template <class T>
	class QuadTree
	{
	public:
		const size_t kMaxObjects = 4;
		QuadTree(int level, Point bot_left, Point top_right)
			: m_level(level)
			, m_botLeft(bot_left)
			, m_topRight(top_right)
		{

		}

		std::vector<T> locate(const T& body) const
		{
			Point pos = body->GetPosition();
			if (!IsPointInRect(pos, m_botLeft, m_topRight))
			{
				return {};
			}

			if (!m_objects.empty() && m_objects.size() > 1)
				return m_objects;

			// TODO rewrite this has so much copy
			std::vector<T> res;
			if (!m_nodes.empty())
				for (const auto& node : m_nodes)
				{
					std::vector<T> tmp = node.locate(body);
					res.insert(res.begin(), tmp.cbegin(), tmp.cend());
				}

			return res;
		}

		bool insert(const T& body)
		{
			Point pos = body->GetPosition();
			if (!IsPointInRect(pos, m_botLeft, m_topRight))
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
				const int m_x = m_botLeft.x;
				const int m_y = m_botLeft.y;
				const int m_right = m_topRight.x;
				const int m_top = m_topRight.y;
				const int mid_x = m_botLeft.x + (m_topRight.x - m_botLeft.x) / 2;
				const int mid_y = m_botLeft.y + (m_topRight.y - m_botLeft.y) / 2;

				m_nodes.push_back(QuadTree(m_level + 1, m_botLeft, Point(mid_x, mid_y)));
				m_nodes.push_back(QuadTree(m_level + 1, Point(m_x, mid_y), Point(mid_x, m_top)));
				m_nodes.push_back(QuadTree(m_level + 1, Point(mid_x, m_y), Point(m_right, mid_y)));
				m_nodes.push_back(QuadTree(m_level + 1, Point(mid_x, mid_y), m_topRight));
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
		Point m_botLeft;
		Point m_topRight;

		std::vector<T> m_objects;
		std::vector<QuadTree> m_nodes;

	};

} // namespace physic

#endif // PHYS_UTILS_H