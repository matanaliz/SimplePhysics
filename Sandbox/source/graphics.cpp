#include <graphics.h>

Entity::Entity(physic::BodyPtr body, HWND hWnd, unsigned radius)
	: m_hWnd(hWnd)
	, m_body(body)
	, m_prevPosition(m_body->GetPosition())
	, m_radius(radius)
{

}

void Entity::Draw()
{

	HDC hdc;
	hdc = GetDC(m_hWnd);

	SetGraphicsMode(hdc, GM_ADVANCED);
	SetMapMode(hdc, MM_ISOTROPIC);

	RECT windowArea;
	GetClientRect(m_hWnd, &windowArea);
	SetViewportOrgEx(hdc, kViewportMargin, windowArea.bottom - kViewportMargin, NULL);

	// 1. Clean rect for previous position
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, Colors::White);

	// 10 pixels around shape will be cleaned
	int cleanupRadius = m_radius + kCleanupMargin;
	Rectangle(hdc,
		(int)m_prevPosition.x - cleanupRadius,
		(int)m_prevPosition.y - cleanupRadius,
		(int)m_prevPosition.x + cleanupRadius,
		(int)m_prevPosition.y + cleanupRadius);

	SetDCPenColor(hdc, Colors::Black);

	// 2. Draw new object
	physic::fVec2D pos = m_body->GetPosition();
	Ellipse(hdc,
		(int)pos.x - m_radius,
		(int)pos.y - m_radius,
		(int)pos.x + m_radius,
		(int)pos.y + m_radius);

	// 3. Save state to previous position
	m_prevPosition = pos;
	ReleaseDC(m_hWnd, hdc);
}