#include <Windows.h>
#include <thread>
#include <chrono>
#include <phys_engine.h>

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

			while (TRUE)
			{
				if (GetMessage(&msg, NULL, 0, 0))
				{
					DispatchMessage(&msg);
				}

				// Stepping should be implemented with native timer
				static const float dt = 1.0 / 30.0;
				std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt * 1000)));

				// Get current position of object
				fVec2D pos = body->GetPosition();

				// TODO wrap up body object into propper entity class for correct drawing
				// TODO get rid of the "tail"
				HDC hdc;
				hdc = GetDC(hWnd);
				Ellipse(hdc,
					(int)pos.x,
					(int)pos.y,
					(int)pos.x + 20,
					(int)pos.y + 20);
				ReleaseDC(hWnd, hdc);

				// Call engine step with time delta parameter to calculate physics
				engine->Step(dt);
			}
		}
	}

	return 0;
}