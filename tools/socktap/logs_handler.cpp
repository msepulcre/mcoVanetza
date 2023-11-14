
// C++ Header File(s)
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

#include "logs_handler.hpp"

using namespace std;
using namespace Logging;

LogsHandler* LogsHandler::m_Instance = 0;

// Log file name. File name should be change from here only
//const string logFileName = "inpercept_stack.log";

LogsHandler::LogsHandler()
{
    std::time_t actualTime = std::time(nullptr);
    std::tm* timeInfo = std::localtime(&actualTime);
    char logFileName[80];
    std::strftime(logFileName, sizeof(logFileName), "inpercept_log.log", timeInfo); //inpercept_stack%Y-%m-%d-%H-%M-%S.txt", timeInfo);

    m_File.open(logFileName, ios::out|ios::app);
    m_LogLevel	= LOG_LEVEL_TRACE;
    m_LogType	= FILE_LOG;
    // Initialize mutex

    int ret=0;
    ret = pthread_mutexattr_settype(&m_Attr, PTHREAD_MUTEX_ERRORCHECK);
    if(ret != 0)
    {
        printf("LogsHandler::LogsHandler() -- Mutex attribute not initialize!!\n");
        exit(0);
    }
//    m_Mutex = PTHREAD_MUTEX_INITIALIZER;
    ret = pthread_mutex_init(&m_Mutex,&m_Attr);
    if(ret != 0)
    {
        printf("LogsHandler::LogsHandler() %d -- Mutex not initialize!!\n", ret);
        exit(0);
    }
}

LogsHandler::~LogsHandler()
{
    m_File.close();
    pthread_mutexattr_destroy(&m_Attr);
    pthread_mutex_destroy(&m_Mutex);

}

LogsHandler* LogsHandler::getInstance() throw ()
{
    if (m_Instance == 0)
    {
        m_Instance = new LogsHandler();
    }
    return m_Instance;
}

void LogsHandler::lock()
{
    pthread_mutex_lock(&m_Mutex);

}

void LogsHandler::unlock()
{
    pthread_mutex_unlock(&m_Mutex);
}

void LogsHandler::logIntoFile(std::string& data) //aqui se cambia getCurrentTime
{
    lock();
    m_File << getCurrentTime() << "  " << data << endl;
    unlock();
}

void LogsHandler::logOnConsole(std::string& data)
{
    cout << getCurrentTime() << "  " << data << endl;
}

string LogsHandler::getCurrentTime()
{
//    string currTime;
//    //Current date/time based on current time
//    time_t now = time(0);
//    // Convert current time to string
//    currTime.assign(ctime(&now));
//
//    // Last charactor of currentTime is "\n", so remove it
//    string currentTime = currTime.substr(0, currTime.size()-1);
    long long current_time;
    struct timeval te;
    gettimeofday(&te,NULL);
    current_time=te.tv_sec*1000000LL+te.tv_usec;
    std::stringstream ss;
    ss << current_time;

    //return to_string(current_time);
    return ss.str();
}

// Interface for Error Log
void LogsHandler::error(const char* text) throw()
{
    string data;
    data.append("[ERROR]: ");
    data.append(text);

    // ERROR must be capture
    if(m_LogType == FILE_LOG)
    {
        logIntoFile(data);
    }
    else if(m_LogType == CONSOLE)
    {
        logOnConsole(data);
    }
}

void LogsHandler::error(std::string& text) throw()
{
    error(text.data());
}

void LogsHandler::error(std::ostringstream& stream) throw()
{
    string text = stream.str();
    error(text.data());
}

// Interface for Alarm Log
void LogsHandler::alarm(const char* text) throw()
{
    string data;
    data.append("[ALARM]: ");
    data.append(text);

    // ALARM must be capture
    if(m_LogType == FILE_LOG)
    {
        logIntoFile(data);
    }
    else if(m_LogType == CONSOLE)
    {
        logOnConsole(data);
    }
}

void LogsHandler::alarm(std::string& text) throw()
{
    alarm(text.data());
}

void LogsHandler::alarm(std::ostringstream& stream) throw()
{
    string text = stream.str();
    alarm(text.data());
}

// Interface for Always Log
void LogsHandler::always(const char* text) throw()
{
    string data;
    data.append("[ALWAYS]: ");
    data.append(text);

    // No check for ALWAYS logs
    if(m_LogType == FILE_LOG)
    {
        logIntoFile(data);
    }
    else if(m_LogType == CONSOLE)
    {
        logOnConsole(data);
    }
}

void LogsHandler::always(std::string& text) throw()
{
    always(text.data());
}

void LogsHandler::always(std::ostringstream& stream) throw()
{
    string text = stream.str();
    always(text.data());
}

// Interface for Buffer Log
void LogsHandler::buffer(const char* text) throw()
{
    // Buffer is the special case. So don't add log level
    // and timestamp in the buffer message. Just log the raw bytes.
    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_BUFFER))
    {
        lock();
        m_File << text << endl;
        unlock();
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_BUFFER))
    {
        cout << text << endl;
    }
}

void LogsHandler::buffer(std::string& text) throw()
{
    buffer(text.data());
}

void LogsHandler::buffer(std::ostringstream& stream) throw()
{
    string text = stream.str();
    buffer(text.data());
}

// Interface for Info Log
void LogsHandler::info(const char* text) throw()
{
    string data;
    data.append("[INFO]: ");
    data.append(text);

    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_INFO))
    {
        logIntoFile(data);
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_INFO))
    {
        logOnConsole(data);
    }
}

void LogsHandler::info(std::string& text) throw()
{
    info(text.data());
}

void LogsHandler::info(std::ostringstream& stream) throw()
{
    string text = stream.str();
    info(text.data());
}

// Interface for Trace Log
void LogsHandler::trace(const char* text) throw()
{
    string data;
    data.append("[TRACE]: ");
    data.append(text);

    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_TRACE))
    {
        logIntoFile(data);
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_TRACE))
    {
        logOnConsole(data);
    }
}

void LogsHandler::trace(std::string& text) throw()
{
    trace(text.data());
}

void LogsHandler::trace(std::ostringstream& stream) throw()
{
    string text = stream.str();
    trace(text.data());
}

// Interface for Debug Log
void LogsHandler::debug(const char* text) throw()
{
    string data;
    data.append("[DEBUG]: ");
    data.append(text);

    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_DEBUG))
    {
        logIntoFile(data);
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_DEBUG))
    {
        logOnConsole(data);
    }
}

void LogsHandler::debug(std::string& text) throw()
{
    debug(text.data());
}

void LogsHandler::debug(std::ostringstream& stream) throw()
{
    string text = stream.str();
    debug(text.data());
}

// Interfaces to control log levels
void LogsHandler::updateLogLevel(LogLevel logLevel)
{
    m_LogLevel = logLevel;
}

// Enable all log levels
void LogsHandler::enaleLog()
{
    m_LogLevel = ENABLE_LOG;
}

// Disable all log levels, except error and alarm
void LogsHandler:: disableLog()
{
    m_LogLevel = DISABLE_LOG;
}

// Interfaces to control log Types
void LogsHandler::updateLogType(LogType logType)
{
    m_LogType = logType;
}

void LogsHandler::enableConsoleLogging()
{
    m_LogType = CONSOLE;
}

void LogsHandler::enableFileLogging()
{
    m_LogType = FILE_LOG ;
}

