#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <Windows.h>
#include <phys_engine.h>
#include <vector>

namespace draw
{
	enum Colors
	{
		Black = RGB(0, 0, 0),
		White = RGB(255, 255, 255)
	};

	const POINT kAxisCrossPoint = { 50, 50 };
	// How much more pixels radius will be cleaned around basic shape
	const unsigned kCleanupMargin = 2;

	const unsigned kDefaultEntityRadius = 50;

	class Entity
	{
	public:
		explicit Entity(const physic::BodyPtr&, unsigned radius = kDefaultEntityRadius);
		virtual ~Entity() = default;

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(Entity&&) = delete;

		virtual void Draw(HWND);

	private:
		physic::BodyPtr m_body;
		physic::fVec2D m_prevPosition;
		unsigned m_radius;
	};

	class Render
	{
	public:

		static Render* Instance();
		void SetWindowsHandle(HWND);
		void AddBody(const physic::BodyPtr&);

		void Crear();
		void Draw();
		void DrawCoordinates();
		
	private:
		Render();
		~Render() {};

		HWND m_hWnd;

		using EntityPtr = std::shared_ptr<Entity>;
		std::vector<EntityPtr> m_enteties;
	};
}

#endif // GRAPHICS_H