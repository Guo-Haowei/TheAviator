#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

bool Com_Window_Create( const char* title, int width, int height );
void Com_Window_Shutdown();
