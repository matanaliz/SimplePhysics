#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <Windows.h>
#include <phys_engine.h>

enum Colors
{
	Black = RGB(0, 0, 0),
	White = RGB(255, 255, 255)
};

// Place start of coordinates at margin value from left-bot corner
const unsigned kViewportMargin = 20;
// How much more pixels radius will be cleaned around basic shape
const unsigned kCleanupMargin = 10;

const unsigned kDefaultEntityRadius = 25;

class Entity
{
public:
	explicit Entity(physic::BodyPtr, HWND, unsigned radius = kDefaultEntityRadius);
	~Entity() = default;

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
	Entity(Entity&&) = delete;
	Entity& operator=(Entity&&) = delete;

	void Draw();

private:
	HWND m_hWnd;
	physic::BodyPtr m_body;
	physic::fVec2D m_prevPosition;
	unsigned m_radius;
};

#endif // GRAPHICS_H