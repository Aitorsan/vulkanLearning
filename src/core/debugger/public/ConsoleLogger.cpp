#include "Logger.h"
#include <Windows.h>

#include <string>
#include "ConsoleLogger.h"


ConsoleLogger::ConsoleLogger()
{

	CreateAndAttachConsoleToWin32Program();
}

void ConsoleLogger::CreateAndAttachConsoleToWin32Program()
{
	FILE* fpstdin = stdin;
	FILE* fpstdout = stdout;
	FILE* fpstderr = stderr;
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen_s(&fpstdin, "CONIN$", "r", stdin);
	freopen_s(&fpstdout, "CONOUT$", "w", stdout);
	freopen_s(&fpstderr, "CONOUT$", "w", stderr);
	SetConsoleTitle("appconsole");
}

void ConsoleLogger::LogMsg(LOG_TYPE type, const char* message) 
{

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(WORD) type);
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE),(WORD) type);
	
		std::string header{};
		switch (type)
		{
		case LOG_TYPE::LOG: header = "[LOG]"; break;
		case LOG_TYPE::TRACE: header = "[TRACE]"; break;
		case LOG_TYPE::WARN:header = "[WARN]"; break;
		case LOG_TYPE::VAL: header = "[VALIDATION LAYER]"; break;
		case LOG_TYPE::ERR:header = "[ERROR]"; break;
		}
		printf("%s",message);
	
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), 15);
}

ConsoleLogger::~ConsoleLogger()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), 15);
}

