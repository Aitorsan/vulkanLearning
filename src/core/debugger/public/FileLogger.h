#pragma once
#include "ILogListener.h"
#include <fstream>

class FileLogger: public ILogListener
{
	std::fstream LogFile;
public:
	FileLogger();
	virtual ~FileLogger();

	void LogMsg(LOG_TYPE type, const char* message) override;
	void CloseFile();

	
};