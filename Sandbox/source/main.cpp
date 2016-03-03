#include <graphics.h>

#include <phys_engine.h>
#include <phys_constants.h>

#include <thread>
#include <chrono>

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_SIZING:
	case WM_MOVING:
	case WM_ENTERSIZEMOVE:
	case WM_EXITSIZEMOVE:
		//
		draw::Render::Instance()->Crear();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		// Render all objects
		draw::Render::Instance()->Draw();
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
		0,
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
			// Get engine object
			physic::IEngine* engine = physic::IEngine::Instance();

			// Set physical simulation constrains e.g. gravity and world size.
			engine->SetWorldConstrains(physic::kGravity, 
				draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius, 
				draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius,
				2048, 
				2048);

			// Physical body. Should be wrapped for correct drawing.
			physic::BodyPtr body = physic::IBody::GetBody();

			// Set initial position of body
			body->SetPosition(physic::fVec2D(
				static_cast<float>(draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius), 
				static_cast<float>(draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius)));

			// Setting velocity vector as vX = 10 m/s, vY = 100 m/s
			body->SetVelocityVector(physic::fVec2D(10, 100));

			// Add body into engine for simulation
			engine->AddBody(body);

			draw::Render* render = draw::Render::Instance();
			render->SetWindowsHandle(hWnd);
			// Clear background
			render->Crear();

			// Add body to drawing queue
			render->AddBody(body);
		
			MSG msg = { 0 };
			while (TRUE)
			{
				if (GetMessage(&msg, NULL, 0, 0) >= 0)
				{
					// Stepping should be implemented with native timer
					static const double dt = 1.0f / 30.0;
					std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt * 100)));

					// Call engine step with time delta parameter to calculate physics
					engine->Step(dt);

					DispatchMessage(&msg);
				}
			}
		}
	}

	return 0;
}