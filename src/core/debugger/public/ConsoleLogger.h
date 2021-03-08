#pragma once

#include "ILogListener.h"

class ConsoleLogger : public ILogListener
{
	void CreateAndAttachConsoleToWin32Program();
public:
	ConsoleLogger();
	virtual ~ConsoleLogger();
	void LogMsg(LOG_TYPE type, const char* message) override;
	
};





