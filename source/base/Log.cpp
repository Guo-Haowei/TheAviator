#include "log.h"

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdarg>
#include <cstdio>
#include <ctime>

#define ENDPOINT stdout

namespace internal {
void Log( LogLevel level, const char* fmt, ... )
{
    char buffer1[1024];
    va_list args;
    va_start( args, fmt );
    vsnprintf( buffer1, sizeof( buffer1 ), fmt, args );
    va_end( args );

    time_t rawtime;
    struct tm* timeinfo;
    std::time( &rawtime );
    timeinfo = localtime( &rawtime );
    char timebuf[128];
    strftime( timebuf, sizeof( timebuf ), "%H:%M:%S", timeinfo );

    char buffer2[2048];
    snprintf( buffer2, sizeof( buffer2 ), "[%s] %s\n", timebuf, buffer1 );

    // print to debugger
    OutputDebugStringA( buffer2 );
    // print to console
    constexpr WORD defaultStyle = ( FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN );

    WORD style = defaultStyle;

    switch ( level )
    {
        case LogLevel::Fatal:
        case LogLevel::Error:
            style = FOREGROUND_RED;
            break;
        case LogLevel::Warning:
            style = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case LogLevel::Info:
            style = FOREGROUND_BLUE;
            break;
        case LogLevel::Success:
            style = FOREGROUND_GREEN;
            break;
        default:
            break;
    }

    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    SetConsoleTextAttribute( hConsole, style );
    fprintf( ENDPOINT, buffer2 );
    SetConsoleTextAttribute( hConsole, defaultStyle );

    if ( level >= LogLevel::Error )
    {
        __debugbreak();
    }
}
}  // namespace internal
