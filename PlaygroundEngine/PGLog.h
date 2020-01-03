#pragma once

#include "Core.h"

#include <string.h>
#include <stdarg.h>
#include <vector>

enum LogType {
    LOG_DEBUG,
    LOG_WARNING,
    LOG_ERROR
};

struct LogItem {
    LogType type;
    const char* logData;
};

typedef std::vector<LogItem> LogList;

class PG_API PGLog {
public:
    static void Log(LogType type, const char* fmt, ...);

    static void ClearLogList();
    static const LogList& GetLogList() { return m_LogList; };

private:
    PGLog();

    static LogList m_LogList;
};

#define PG_LOG_DEBUG(...) PGLog::Log(LogType::LOG_DEBUG, __VA_ARGS__)
#define PG_LOG_WARNING(...) PGLog::Log(LogType::LOG_WARNING, __VA_ARGS__)
#define PG_LOG_ERROR(...) PGLog::Log(LogType::LOG_ERROR, __VA_ARGS__)


