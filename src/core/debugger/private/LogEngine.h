#pragma once
#ifdef LOG_ENGINE_INCLUDE
#error don't include LogEngine.h use Logger.h instead
#endif
#include "core/debugger/public/ILogListener.h"
#include <vector>

class LogEngine
{
	LogEngine();
	//it is a singleton can't be copied or move
	LogEngine(const LogEngine&) = delete;
	LogEngine& operator=(const LogEngine&) = delete;
	
	std::vector<ILogListener*> LogListeners = {};
	static LogEngine* LogEngineInstance;
public:

	~LogEngine();
	static LogEngine* GetLogEngineInstance();
	static void Destroy();

	void LogMsg(LOG_TYPE type, const char* message,...);

	void RegisterListener(ILogListener* listener);
};


#define REGISTER_LOGGER(classname, ...) LogEngine::GetLogEngineInstance()->RegisterListener(new classname(__VA_ARGS__));

#define LOG_ENGINE_INCLUDE