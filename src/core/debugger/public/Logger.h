#pragma once

#ifdef NDEBUG
#define LOG_MSG(message,...) ;
#define LOG_TRACE(message, ...) ;
#define LOG_WARN(message,...) ;
#define LOG_ERR(message,...) ;
#define VAL_LAYER(message,...) ;
#define ASSERT_EQU(object, value, message, ...) ;
#define ASSERT_NOT_NULL(object, value, message, ...) ;
#define VK_CHECK(func,message,...) func

#else
#include <exception>
#include "core/debugger/private/LogEngine.h"

// Use this because in no debug mode will not exist any logger
#define LOG_MSG(message,...){ LogEngine::GetLogEngineInstance()->LogMsg(LOG_TYPE::LOG,message,##__VA_ARGS__);}
#define LOG_TRACE(message, ...){LogEngine::GetLogEngineInstance()->LogMsg(LOG_TYPE::TRACE,message,__VA_ARGS__);}
#define LOG_WARN(message,...){ LogEngine::GetLogEngineInstance()->LogMsg(LOG_TYPE::WARN,message,__VA_ARGS__);}
#define LOG_ERR(message,...) {LogEngine::GetLogEngineInstance()->LogMsg(LOG_TYPE::ERR,message,__VA_ARGS__); throw std::exception(message);}
#define VAL_LAYER(message,...){ LogEngine::GetLogEngineInstance()->LogMsg(LOG_TYPE::VAL,message ,__VA_ARGS__);}
#define ASSERT_EQU(object, value, message, ...) do { if ( object != value){LOG_ERR(message,__VA_ARGS__}}while(0)
#define ASSERT_NOT_NULL(object, message,...) do { if (!object){LOG_ERR(message,__VA_ARGS__)}}while(0)
#define VK_CHECK(func,message,...) do{ if((func) != 0) LOG_ERR("Failed to "#func ##"\n", __VA_ARGS__) }while(0)

#endif