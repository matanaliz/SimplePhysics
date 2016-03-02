#include <Windows.h>
#include <thread>
#include <chrono>
#include <phys_engine.h>

// Simple wrapper class for physical body
class Entity
{
public:
	explicit Entity(BodyPtr, HWND);
	~Entity() = default;

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
	Entity(Entity&&) = delete;
	Entity& operator=(Entity&&) = delete;

	void Draw();

private:
	HWND m_hWnd;
	BodyPtr m_body;
	fVec2D m_prevPosition;
};

Entity::Entity(BodyPtr body, HWND hWnd)
	: m_hWnd(hWnd)
	, m_body(body)
	, m_prevPosition(m_body->GetPosition())
{

}

void Entity::Draw()
{
	// TODO move to header
	int WhiteColor = RGB(255, 255, 255);
	int BlackColor = RGB(0, 0, 0);

	HDC hdc;
	hdc = GetDC(m_hWnd);

	// 1. Clean rect for previous position
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, WhiteColor);

	Rectangle(hdc, 
		(int)m_prevPosition.x,
		(int)m_prevPosition.y,
		(int)m_prevPosition.x + 20,
		(int)m_prevPosition.y + 20);

	SetDCPenColor(hdc, BlackColor);

	// 2. Draw new object
	fVec2D pos = m_body->GetPosition();
	Ellipse(hdc,
		(int)pos.x,
		(int)pos.y,
		(int)pos.x + 20,
		(int)pos.y + 20);

	// 3. Save state to previous position
	m_prevPosition = pos;
	ReleaseDC(m_hWnd, hdc);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
	case WM_CREATE:
		// Init timers?
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		// Init paint with coord lines?
		break;
	case WM_TIMER:
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0L;
}

int main(int argc, char* argv[])
{
	const wchar_t* wndClassName = L"wndcls";
	WNDCLASSEX wndClass = {
		sizeof(WNDCLASSEX),
		CS_DBLCLKS,
		WinProc,
		0,
		0,
		GetModuleHandle(0),
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		HBRUSH(COLOR_WINDOW + 1),
		0,
		wndClassName,
		LoadIcon(0, IDI_APPLICATION)
	};

	if (RegisterClassEx(&wndClass))
	{
		HWND hWnd = CreateWindowEx(0, wndClassName, L"Sandbox",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0);

		if (hWnd)
		{
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			MSG msg;

			// Simple gravity constant. Should be moved to some engine header.
			const float kGravity = 9.81;

			// Get engine object
			IEngine* engine = IEngine::Instance();

			// Set physical simulation constrains e.g. gravity and world size.
			engine->SetWorldConstrains(kGravity, 0, 0, 840, 480);

			// Physical body. Should be wrapped for correct drawing.
			BodyPtr body = IBody::GetBody();

			// Set initial position of body
			body->SetPosition(fVec2D(0, 0));

			// Setting velocity vector as vX = 10 m/s, vY = 60 m/s
			body->SetVelocityVector(fVec2D(10, 60));

			// Add body into engine for simulation
			engine->AddBody(body);

			Entity e((body), hWnd);

			while (TRUE)
			{
				if (GetMessage(&msg, NULL, 0, 0))
				{
					DispatchMessage(&msg);
				}

				// Stepping should be implemented with native timer
				static const float dt = 1.0 / 30.0;
				std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt * 1000)));

				// Entity object holds body object
				e.Draw();

				// Call engine step with time delta parameter to calculate physics
				engine->Step(dt);
			}
		}
	}

	return 0;
}