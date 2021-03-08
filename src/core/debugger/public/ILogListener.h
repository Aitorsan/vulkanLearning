#pragma once
enum class LOG_TYPE: unsigned short
{
	LOG = 10,
	TRACE = 15,
	WARN = 14,
	ERR =  12,
	VAL  = 8// validation layer log
};

class ILogListener
{
	friend class LogEngine;
protected:
	bool active = true;
public:
	virtual ~ILogListener(){}
	virtual void LogMsg(LOG_TYPE type, const char* message) = 0;
};