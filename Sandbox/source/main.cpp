#include <Windows.h>
#include <thread>
#include <chrono>
#include <phys_engine.h>
#include <phys_constants.h>
#include <graphics.h>

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

			// Get engine object
			physic::IEngine* engine = physic::IEngine::Instance();

			// Set physical simulation constrains e.g. gravity and world size.
			engine->SetWorldConstrains(physic::kGravity, 0, 0, 2048, 2048);

			// Physical body. Should be wrapped for correct drawing.
			physic::BodyPtr body = physic::IBody::GetBody();

			// Set initial position of body
			body->SetPosition(physic::fVec2D(0, 0));

			// Setting velocity vector as vX = 10 m/s, vY = 60 m/s
			body->SetVelocityVector(physic::fVec2D(10, 100));

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
				static const float dt = 1.0f / 60.0;
				std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt * 250)));

				// Entity object holds body object
				e.Draw();

				// Call engine step with time delta parameter to calculate physics
				engine->Step(dt);
			}
		}
	}

	return 0;
}