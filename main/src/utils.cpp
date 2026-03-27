#include "utils.hpp"

#ifdef _WIN32
#define CONSOLE_COLOR_RESET ""
#define CONSOLE_COLOR_GREEN ""
#define CONSOLE_COLOR_RED ""
#define CONSOLE_COLOR_PURPLE ""
#else
#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[1;32m"
#define CONSOLE_COLOR_RED "\033[1;31m"
#define CONSOLE_COLOR_PURPLE "\033[1;35m"
#endif

#include <cstdio>
#include <cstdarg>
#include <ctime>

static void LogMessage(int level, const char *msg, va_list args)
{
    time_t rawTime;
    struct tm *timeInfo;
    char timeBuffer[80];

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(timeBuffer, sizeof(timeBuffer), "[%H:%M:%S]", timeInfo);

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), msg, args);

    switch (level)
    {
    case 0:
        fprintf(stdout, "%s%s: %s%s\n", CONSOLE_COLOR_GREEN, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    case 1:
        fprintf(stderr, "%s%s: %s%s\n", CONSOLE_COLOR_RED, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    case 2:
        fprintf(stderr, "%s%s: %s%s\n", CONSOLE_COLOR_PURPLE, timeBuffer, buffer, CONSOLE_COLOR_RESET);
        break;
    }
}

void LogError(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    LogMessage(1, msg, args);
    va_end(args);
}

void LogWarning(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    LogMessage(2, msg, args);
    va_end(args);
}

void LogInfo(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    LogMessage(0, msg, args);
    va_end(args);
}
