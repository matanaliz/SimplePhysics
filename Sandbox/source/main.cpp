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
		draw::Render::Instance()->Clear();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		break;
	case WM_CHAR:
		if (VK_SPACE == wParam)
		{
			physic::IEngine* engine = physic::IEngine::Instance();

			const physic::Point pos = { static_cast<physic::Point::type>(draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius + 1),
				static_cast<physic::Point::type>(draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius + 1) };
			const physic::fVec2D vel = physic::fVec2D(150, physic::fAngle(45));

			// Physical body. Should be wrapped for correct drawing.
			physic::BodyPtr body = physic::IBody::CreateBody(physic::IShape::ShapeType::Circle, pos, vel, 20);

			// Add body into engine for simulation
			engine->AddBody(body);

			// Add body to drawing queue
			draw::Render* render = draw::Render::Instance();
			render->AddBody(body);
		}
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
	physic::fAngle argAngle { 0.f };
	physic::fVec2D argVelocity{ 0.f, 0.f };
	float argMass { 1.f };

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

		argMass = { value };
	}

	// Get engine object
	physic::IEngine* engine = physic::IEngine::Instance();

	{
		// Set initial position of body
		physic::Point pos { 
			static_cast<physic::Point::type>(draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius + 1),
			static_cast<physic::Point::type>(draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius + 1) 
			};

		// Physical body. Should be wrapped for correct drawing.
		physic::BodyPtr body = physic::IBody::CreateBody(physic::IShape::ShapeType::Circle, pos, argVelocity, argMass);

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

			const physic::Point bot { 
				static_cast<physic::Point::type>(draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius),
				static_cast<physic::Point::type>(draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius) 
			};

			const physic::Point top { 
				static_cast<physic::Point::type>(clientArea.right), 
				static_cast<physic::Point::type>(clientArea.bottom) 
			};

			// Set physical simulation constrains be same as graphics
			engine->SetWorldBorders(bot, top);

			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			render->Clear();

			const double dt = 1.0f / 60.0;
			auto current_time = std::chrono::steady_clock::now();
			double accumulator = 0.0;

			MSG msg { 0 };

			while (TRUE)
			{
				if (GetMessage(&msg, NULL, 0, 0) >= 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);

					auto new_time = std::chrono::steady_clock::now();
					double frame_time = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(new_time - current_time).count();
					current_time = new_time;

					if (frame_time > dt)
						frame_time = dt;

					accumulator += frame_time;

					while (accumulator >= dt)
					{
						engine->Step(dt);
						accumulator -= dt;
					}

					// Render all objects
					draw::Render::Instance()->Draw();

				}
			}
		}
	}

	return 0;
}