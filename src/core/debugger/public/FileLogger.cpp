
#include "FileLogger.h"

FileLogger::FileLogger()
	:LogFile{}
{
	//active = false;
	if (active)
	{
		LogFile.open("vkEngineLog.log", std::ios::out);
		if (!LogFile.is_open()) throw std::runtime_error("Can't open log file\n");
	}

}
FileLogger::~FileLogger()
{
	if (LogFile.is_open())
		LogFile.close();
}

void FileLogger::LogMsg(LOG_TYPE type, const char* message)
{
	if ( type == LOG_TYPE::LOG && LogFile.is_open())
		LogFile << message;
}

void FileLogger::CloseFile()
{
	if (LogFile.is_open())
		LogFile.close();
}
