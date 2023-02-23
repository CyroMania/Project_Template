#pragma once

#define GlCall(x) GlClearError();\
    x;\
    ASSERT(GlLogCall(#x, __FILE__, __LINE__))

#define ASSERT(x) if (!x) __debugbreak;

void GlClearError();

bool GlLogCall(const char* function, const char* file, int line);