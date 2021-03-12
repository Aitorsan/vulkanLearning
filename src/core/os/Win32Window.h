#pragma once

#include <Windows.h>
struct VkInstance_T;
struct VkSurfaceKHR_T;
class VEngine;


class Win32Window
{
	HINSTANCE WindowInstance;
	HWND WindowHandle;
	LPCTSTR ApplicationName;
	bool CloseWindow;
public:
	unsigned int Width{ 1280 }, Height{720};

	Win32Window(LPCTSTR appName);
	~Win32Window();
	void CreateWin32Window(HINSTANCE hInstance);
	void CreateWindowSurface(VkInstance_T* vulkanInstance, VkSurfaceKHR_T** windowSurface);
	bool ShouldClose() const { return CloseWindow; }
	void OnDestroy();
	void PoolEvents();
	HWND GetHandleToWindow() const;
	HINSTANCE GetWindowInstance()const;
};

