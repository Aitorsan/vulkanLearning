#pragma once


#define DISABLE_COPY(classname)\
classname (const classname&) = delete;\
classname & operator=(const classname&) = delete;


#define DISABLE_COPY_GEN_DEFAULT_CONSTRUCT(classname)\
classname() = default;\
classname (const classname&) = delete;\
classname & operator=(const classname&) = delete;