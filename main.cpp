#include <Windows.h>
#include "core/engine/VEngine.h"

int WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hprevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	try
	{
		VEngine engine("vEngine (vulkan)",hInstance);
		engine.Run();
	}
	catch (...)
	{
		system("pause");
		return EXIT_FAILURE;
	}
	system("pause");

	return EXIT_SUCCESS;
}