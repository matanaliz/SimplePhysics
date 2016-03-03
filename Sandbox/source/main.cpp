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


void printCommandHelp()
{
	std::cout << "Command line options :" << std::endl;
	std::cout << "--angle : angle in degrees, must be 0 < angle < 90" << std::endl;
	std::cout << "--vel   : start velocity in m/s" << std::endl;
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
		--angle : angle in degrees, must be 0 < angle < 90
		--vel   : start velocity in m/s
	*/
	physic::fAngle startAngle = { 0. };
	physic::fVec2D startVelocity(0, 0);

	if (argc < 4)
		return failedCommandParam();

	// Parse angle parameter
	char* param = getCommandParam(argc, argv, "--angle");
	if (0 != param)
	{
		float value = std::stof(param);
		if (value > 90.f || value < 0.f)
		{
			std::cout << "Angle in degrees, must be 0 < angle < 90" << std::endl;
			return failedCommandParam();
		}

		startAngle = { value };
	}
	else
		return failedCommandParam();

	// Parse velocity parameter
	param = getCommandParam(argc, argv, "--vel");
	if (0 != param)
	{
		float value = std::stof(param);
		startVelocity = { value, startAngle };
	}
	else
		return failedCommandParam();

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
			// Set world gravity
			engine->SetGravity(physic::kGravity);
			// Set physical simulation constrains e.g. gravity and world size.
			engine->SetWorldConstrains(
				draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius, 
				draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius,
				2048, 
				2048);

			// Physical body. Should be wrapped for correct drawing.
			physic::BodyPtr body = physic::IBody::GetBody();

			// Set initial position of body
			body->SetPosition(physic::fVec2D(
				static_cast<float>(draw::kAxisCrossPoint.x + draw::kDefaultEntityRadius),
				static_cast<float>(draw::kAxisCrossPoint.y + draw::kDefaultEntityRadius) 
			));

			// Set command line argument velocity vector
			body->SetVelocityVector(startVelocity);

			// Add body into engine for simulation
			engine->AddBody(body);

			// Set up render
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
					DispatchMessage(&msg);

					// Stepping should be implemented with native timer
					static const double dt = 1.0f / 30.0;
					std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt * 100)));

					// Call engine step with time delta parameter to calculate physics
					engine->Step(dt);

					// TODO Stop simulation
				}
			}
		}
	}

	return 0;
}