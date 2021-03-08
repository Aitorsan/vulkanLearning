#pragma once

#include <Windows.h>

struct VkInstance_T;
struct VkSurfaceKHR_T;


class Win32Window
{
	HINSTANCE WindowInstance;
	HWND WindowHandle;
	LPCTSTR ApplicationName;
public:
	unsigned int Width{ 1280 }, Height{720};

	Win32Window(LPCTSTR appName);

	void CreateWin32Window(HINSTANCE hInstance);
	void CreateWindowSurface(VkInstance_T* vulkanInstance, VkSurfaceKHR_T** windowSurface);
	HWND GetHandleToWindow() const;
	HINSTANCE GetWindowInstance()const;
};

