#include "Win32Window.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <cstdio>
#include "core/debugger/public/Logger.h"
#include "core/engine/VEngine.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_CREATE:
		{
			Win32Window* win = (Win32Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);
			//win->OnCreate();
			break;
		}
		case WM_DESTROY:
		{
			Win32Window* win = (Win32Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			win->OnDestroy();
			PostQuitMessage(0);
			break;
		}
		case WM_PAINT:
		{
			ValidateRect(hwnd, NULL);
			break;
		}
		case WM_SIZE:
		{
		
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return NULL;
}

Win32Window::Win32Window(LPCTSTR appName)
	: WindowInstance(NULL)
	, WindowHandle(nullptr)
	, ApplicationName(appName)
	, CloseWindow(false)
{
}

Win32Window::~Win32Window()
{
	DestroyWindow(WindowHandle);
}

void Win32Window::CreateWin32Window(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = ApplicationName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));


	if (!::RegisterClassEx(&wcex))
		LOG_ERR("failed to regsiter window");

	WindowInstance = hInstance;
	//center the screen
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowLeft = screenWidth / 2 - Width / 2;
	int windowTop = screenHeight / 2 - Height / 2;


	WindowHandle = ::CreateWindow(ApplicationName
	, ApplicationName
	, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
	, windowLeft
	, windowTop
	,Width
	,Height
	,NULL
	,NULL
	,WindowInstance
	,this);

	if (WindowHandle == 0)
	{
		LOG_ERR("Error can't create window");
	}
	else
	{
		ShowWindow(WindowHandle, SW_SHOW);
		SetForegroundWindow(WindowHandle);
		SetFocus(WindowHandle);
	}
	CloseWindow = false;

}

void Win32Window::PoolEvents()
{

	MSG message;

	while (PeekMessage(&message, NULL, 0, 0,PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}



void Win32Window::OnDestroy()
{
	CloseWindow = true;
}

HWND Win32Window::GetHandleToWindow() const
{
	return WindowHandle;
}

HINSTANCE Win32Window::GetWindowInstance() const
{
	return WindowInstance;
}

void Win32Window::CreateWindowSurface(VkInstance vulkanInstance, VkSurfaceKHR_T** windowSurface)
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = NULL;
	surfaceInfo.hinstance = WindowInstance;
	surfaceInfo.hwnd = WindowHandle;

	if (vkCreateWin32SurfaceKHR(vulkanInstance, &surfaceInfo, nullptr, &(*windowSurface)) != VK_SUCCESS)
		LOG_ERR("Unable to create window surface for win32 api!\n");
}
