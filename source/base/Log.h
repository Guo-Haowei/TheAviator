#pragma once

namespace internal {
enum class LogLevel {
    Log = 0,
    Success,
    Info,
    Warning,
    Error,
    Fatal,
};

void Log( LogLevel level, const char* fmt, ... );
}  // namespace internal

#define LOG( fmt, ... )  ::internal::Log( ::internal::LogLevel::Log, fmt, ##__VA_ARGS__ )
#define LOGI( fmt, ... ) ::internal::Log( ::internal::LogLevel::Info, fmt, ##__VA_ARGS__ )
#define LOGS( fmt, ... ) ::internal::Log( ::internal::LogLevel::Success, fmt, ##__VA_ARGS__ )
#define LOGW( fmt, ... ) ::internal::Log( ::internal::LogLevel::Warning, fmt, ##__VA_ARGS__ )
#define LOGE( fmt, ... ) ::internal::Log( ::internal::LogLevel::Error, fmt, ##__VA_ARGS__ )
#define LOGF( fmt, ... ) ::internal::Log( ::internal::LogLevel::Fatal, fmt, ##__VA_ARGS__ )