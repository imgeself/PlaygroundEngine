#include "PGLog.h"

#include <time.h>

LogList PGLog::m_LogList = LogList();

PGLog::PGLog() {

}

void PGLog::Log(LogType type, const char* fmt, ...) {
    const size_t logBufferMaxSize = 1024;
    char logBuffer[logBufferMaxSize] = {0};

    // Get time
    time_t rawtime;
    tm timeinfo;
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    // Format [HH:MM:SS]
    // TODO: Print milliseconds!
    const size_t timeFormatMaxSize = 12; // length of "[HH:MM:SS] \0"
    const size_t timeFormatSize = timeFormatMaxSize - 1; // without terminating character
    strftime(logBuffer, timeFormatMaxSize, "[%T] ", &timeinfo);
    
    va_list args;
    va_start(args, fmt);
    vsnprintf(logBuffer + timeFormatSize, logBufferMaxSize - timeFormatSize, fmt, args);
    va_end(args);

    size_t logLength = strlen(logBuffer) + 2; // Length of the log text plus new line and termination character
    if (logBuffer[logLength - 3] != '\n') {
        logBuffer[logLength - 2] = '\n';
    }
    char* logData = (char*) malloc(logLength);
    memcpy((void*) logData, (const void*) logBuffer, logLength);

    // Print log to stdout
    printf(logData);

    LogItem logItem;
    logItem.type = type;
    logItem.logData = (const char*) logData;

    m_LogList.push_back(logItem);
}

void PGLog::ClearLogList() {
    for (LogItem item : m_LogList) {
        free((void*) item.logData);
    }
    m_LogList.clear();
}

