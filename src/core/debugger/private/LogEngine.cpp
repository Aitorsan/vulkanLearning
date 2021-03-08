
#include "LogEngine.h"

#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <string>
#include "core/debugger/public/LoggListeners.h"

LogEngine* LogEngine::LogEngineInstance = nullptr;


LogEngine::LogEngine()
	:LogListeners
	{
		#include "../configFiles/LoggRegister.config"
	}
{

}

LogEngine::~LogEngine()
{
	for (ILogListener* l : LogListeners)
	{
		if (l) delete l;
		l = nullptr;
	}
}


LogEngine* LogEngine::GetLogEngineInstance()
{
	if (LogEngineInstance == nullptr)
		LogEngineInstance = new LogEngine;

	return LogEngineInstance;
}

 void LogEngine::Destroy()
{
	if (LogEngineInstance)
		delete LogEngineInstance;
	LogEngineInstance = nullptr;
}

 void LogEngine::LogMsg(LOG_TYPE type, const char* message, ...)
 {
	 va_list args;
	 va_start(args, message);

	 va_list temp;
	 va_copy(temp, args);

	 int size =  _vscprintf(message, args);

	std::vector<char> buffer ((uint32_t)size + 1);

	vsnprintf(buffer.data(), buffer.size(), message, temp);
	va_end(args);
	
	std::string str{ buffer.begin(),buffer.end() };
	 
	 for (int i = 0; i < LogListeners.size(); ++i)
	 {
		 ILogListener* l = LogListeners[i];
		 if(l && l->active)
			 l->LogMsg(type,  str.c_str());
	 }



 }

 void LogEngine::RegisterListener(ILogListener* listener)
 {
	 LogListeners.push_back(listener);
 }
