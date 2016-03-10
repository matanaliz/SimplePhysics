#include <graphics.h>

#include <iostream>
#include <cassert>

namespace draw
{
	Render::Render()
		: m_hWnd(0)
		, m_enteties()
	{

	}

	Render* Render::Instance()
	{
		static Render _instance;
		return &_instance;
	}

	void Render::AddBody(const physic::BodyPtr& body)
	{
		m_enteties.push_back(std::shared_ptr<Entity>(new (std::nothrow) Entity(body)));
	}

	void Render::SetWindowsHandle(HWND hWnd)
	{
		m_hWnd = hWnd;
	}

	void Render::Draw()
	{
		assert(0 != m_hWnd);
		for (const auto& entity : m_enteties)
		{
			entity->Draw(m_hWnd);
		}

		DrawCoordinates();
	}

	void Render::Crear()
	{
		assert(0 != m_hWnd);
		HDC hdc;
		hdc = GetDC(m_hWnd);

		RECT windowArea;
		GetClientRect(m_hWnd, &windowArea);
		FillRect(hdc, &windowArea, (HBRUSH)(COLOR_WINDOW + 1));
		ReleaseDC(m_hWnd, hdc);
	}

	void Render::DrawCoordinates()
	{
		assert(0 != m_hWnd);
		HDC hdc;
		hdc = GetDC(m_hWnd);

		RECT clientArea;
		GetClientRect(m_hWnd, &clientArea);

		// Map logical and device coordinates
		SetGraphicsMode(hdc, GM_ADVANCED);
		SetMapMode(hdc, MM_ISOTROPIC);

		// Map logical (0, 0) to bottom-left corner
		SetWindowExtEx(hdc, 1, 1, NULL);
		SetViewportExtEx(hdc, 1, -1, NULL);

		SetViewportOrgEx(hdc, 0, clientArea.bottom, NULL);

		HPEN blackPen = CreatePen(PS_SOLID, 3, Colors::Black);
		HPEN origPen = (HPEN)SelectObject(hdc, blackPen);

		// Scale of client area is 1:2
		const float kAxisFactor = 1.8f;
		MoveToEx(hdc, kAxisCrossPoint.x, kAxisCrossPoint.y, NULL);
		LineTo(hdc, kAxisCrossPoint.x, static_cast<int>(clientArea.bottom * kAxisFactor));

		MoveToEx(hdc, kAxisCrossPoint.x, kAxisCrossPoint.y, NULL);
		LineTo(hdc, static_cast<int>(clientArea.right * kAxisFactor), kAxisCrossPoint.y);

		SelectObject(hdc, origPen);
		DeleteObject(blackPen);
		ReleaseDC(m_hWnd, hdc);
	}


	Entity::Entity(const physic::BodyPtr& body, unsigned radius)
		: m_body(body)
		, m_prevPosition(m_body->GetPosition())
		, m_radius(radius)
	{

	}

	void Entity::Draw(HWND hWnd)
	{
		HDC hdc;
		hdc = GetDC(hWnd);

		RECT clientArea;
		GetClientRect(hWnd, &clientArea);
		
		SetGraphicsMode(hdc, GM_ADVANCED);
		SetMapMode(hdc, MM_ISOTROPIC);

		// Map logical and device coordinates
		SetWindowExtEx(hdc, 1, 1, NULL);
		SetViewportExtEx(hdc, 1, -1, NULL);

		// Map logical (0, 0) to bottom-left corner
		SetViewportOrgEx(hdc, 0, clientArea.bottom, NULL);

		HPEN blackPen = CreatePen(PS_SOLID, 2, Colors::Black);
		HPEN whitePen = CreatePen(PS_SOLID, 2, Colors::White);
		// 1. Clean rect for previous position
		HPEN origPen = (HPEN)SelectObject(hdc, whitePen);

		int cleanupRadius = m_radius + kCleanupMargin;
		Rectangle(hdc,
			(int)m_prevPosition.x - cleanupRadius,
			(int)m_prevPosition.y - cleanupRadius,
			(int)m_prevPosition.x + cleanupRadius,
			(int)m_prevPosition.y + cleanupRadius);

		SelectObject(hdc, blackPen);
		
		// 2. Draw new object
		physic::fVec2D pos = m_body->GetPosition();
		Ellipse(hdc,
			(int)pos.x - m_radius,
			(int)pos.y - m_radius,
			(int)pos.x + m_radius,
			(int)pos.y + m_radius);

		// 3. Save state to previous position
		m_prevPosition = pos;

		SelectObject(hdc, origPen);
		DeleteObject(blackPen);
		DeleteObject(whitePen);
		ReleaseDC(hWnd, hdc);
	}
} // namespace Drawing
