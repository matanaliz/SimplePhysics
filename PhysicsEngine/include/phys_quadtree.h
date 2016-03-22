#ifndef PHYS_QUADTREE_H
#define PHYS_QUADTREE_H

#include <phys_utils.h>
#include <vector>

namespace physic
{
	template <class T>
	class QuadTree
	{
	public:
		const size_t kMaxObjects = 8;
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
				return{};

			if (!m_objects.empty() && m_objects.size() > 1)
				return m_objects;

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
				return false;

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

#endif // PHYS_QUADTREE_H