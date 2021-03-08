#pragma once


#define DISABLE_COPY(classname)\
classname (const classname&) = delete;\
classname & operator=(const classname&) = delete;


#define DISABLE_COPY_GEN_DEFAULT_CONSTRUCTOR(classname)\
classname() = default;\
classname (const classname&) = delete;\
classname & operator=(const classname&) = delete;