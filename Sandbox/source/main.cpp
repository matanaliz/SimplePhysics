#include <graphics.h>

#include <phys_engine.h>
#include <phys_constants.h>
#include <phys_utils.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <string>

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
		draw::Render::Instance()->Crear();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:

		break;
	case WM_TIMER:
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0L;
}


void printCommandHelp()
{
	std::cout << "Command line options :" << std::endl;
	std::cout << "-a : angle in degrees, must be 0 < angle < 90" << std::endl;
	std::cout << "-v : start velocity in m/s" << std::endl;
	std::cout << "-m : set mass of object (optional parameter)" << std::endl;
}

// Check command line for parameter and value
char* getCommandParam(int argc, char* argv[], const std::string& param)
{
	char** end = argv + argc;
	char** it = std::find(argv, end, param);
	if (it != end && ++it != end)
		return *it;

	return 0;
}

int failedCommandParam()
{
	printCommandHelp();
	return -1;
}

int main(int argc, char* argv[])
{
	/*
		Command line options:
		-a : angle in degrees, must be 0 < angle < 90
		-v : start velocity in m/s
		-m : set mass of object (optional parameter)
	*/
	physic::fAngle argAngle = { 0. };
	physic::fVec2D argVelocity(0, 0);
	float argMass = 1.;

	if (argc < 4)
		return failedCommandParam();

	// Parse angle parameter
	char* param = getCommandParam(argc, argv, "-a");
	if (0 != param)
	{
		float value = std::stof(param);
		if (value > 90.f || value < 0.f)
		{
			std::cout << "Angle in degrees, must be 0 < angle < 90" << std::endl;
			return failedCommandParam();
		}

		argAngle = { value };
	}
	else
		return failedCommandParam();

	// Parse velocity parameter
	param = getCommandParam(argc, argv, "-v");
	if (0 != param)
	{
		float value = std::stof(param);
		argVelocity = { value, argAngle };
	}
	else
		return failedCommandParam();

	// Parse mass parameter (optional)
	param = getCommandParam(argc, argv, "-m");
	if (0 != param)
	{
		float value = std::stof(param);
		if ( value < 0.f + 0.1f )
		{
			std::cout << "Mass argument is too small, try bigger value." << std::endl;
			return failedCommandParam();
		}

		argMass = value;
	}

	// Get engine object
	physic::IEngine* engine = physic::IEngine::Instance();

	{
		// Physical body. Should be wrapped for correct drawing.
		physic::BodyPtr body = physic::IBody::GetBody();

		// Set initial position of body
		body->SetPosition(physic::fVec2D(
			static_cast<float>(draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius),
			static_cast<float>(draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius)
			));

		// Set command line argument velocity vector
		body->SetVelocityVector(argVelocity);

		// Set body mass
		body->SetMass(argMass);

		// Add body into engine for simulation
		engine->AddBody(body);

		// Add body to drawing queue
		draw::Render* render = draw::Render::Instance();
		render->AddBody(body);
	}

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
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0);

		if (hWnd)
		{
			// Set up render
			draw::Render* render = draw::Render::Instance();
			render->SetWindowsHandle(hWnd);

			RECT clientArea;
			GetClientRect(hWnd, &clientArea);

			// Set physical simulation constrains be same as graphics
			engine->SetWorldSize(
				draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius,
				draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius,
				clientArea.right,
				clientArea.bottom);

			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			render->Crear();
			
			long long sleep = static_cast<long long>(10);
			auto frameStart = std::chrono::steady_clock::now();
			auto outerLoopStart = std::chrono::steady_clock::now();

			MSG msg = { 0 };
			while (TRUE)
			{
				if (GetMessage(&msg, NULL, 0, 0) >= 0)
				{
					DispatchMessage(&msg);

					// Constant framerate at ~16.7 ms, 60 fps
					static const double dt = 1.0f / 60.0;
					static const double dtMs = dt * 1000;
					// Speed up simulation
					static const double timeScale = 5;

					namespace sc = std::chrono;

					auto frameStop = sc::steady_clock::now();
					long long frameMs = sc::duration_cast<sc::milliseconds>(frameStop - frameStart).count();
					long long outerLoopMs = sc::duration_cast<sc::milliseconds>(frameStop - outerLoopStart).count();

					std::cout << "Frame loop: " << frameMs << std::endl;
					std::cout << "Outer loop: " << outerLoopMs << std::endl;

					frameStart = std::chrono::steady_clock::now();
					
					if (frameMs > dtMs)
						sleep -= sleep / 4;
					else
						sleep += sleep / 4;
					
					// Call engine step with time delta parameter to calculate physics
					engine->Step(dt * timeScale);

					// Render all objects
					draw::Render::Instance()->Draw();

					std::this_thread::sleep_for(sc::milliseconds(sleep));

					std::cout << "Thread sleep: "
						<< std::chrono::duration_cast<sc::milliseconds>(std::chrono::steady_clock::now() - frameStart).count() << std::endl;

					outerLoopStart = std::chrono::steady_clock::now();
				}
			}
		}
	}

	return 0;
}