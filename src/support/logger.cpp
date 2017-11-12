//
//  logger.cpp
//  px
//
//  Created by Jonathan Cichon on 03.01.16.
//
//

#include "logger.hpp"
#include <iostream>
#include <stdio.h>
#include "helper.hpp"
#include <cstdarg>

using px::Logger;

std::unique_ptr<Logger> Logger::m_instance;
std::once_flag Logger::m_onceFlag;

Logger& Logger::Instance() {
    std::call_once(m_onceFlag, [] {
        m_instance.reset(new Logger);
        m_instance->set_log_level(px::LogLevelInfo);
    });
    return *m_instance.get();
}

Logger::Logger(void) : m_log_fn(NULL) {}

void Logger::Log(px::LogLevel logLevel, const char *tag, const char *file_info, const char * msg) {
    Instance().log(logLevel, tag, file_info, msg);
}

const std::vector<std::string>& Logger::Files() {
    return Instance().m_files;
}

void Logger::log(px::LogLevel logLevel, const char *tag, const char *file_info, const char * msg) {
    if (m_log_fn) {
        m_log_fn(logLevel, tag, file_info, msg);
    } else {
        printf("[%s][%s] %s\n", tag, file_info, msg);
        //        std::cout << "[" << tag << "]";
        //        std::cout << "[" << file_info << "] ";
        //        std::cout << msg;
        //        std::cout << "\n";
    }
}

void Logger::set_log_function(const function<void(px::LogLevel logLevel, const char *tag, const char *file_info, const char * msg)> log_fn) {
    m_log_fn = log_fn;
}

void Logger::set_file_scopes(const std::vector<std::string>& files) {
    m_files = files;
}

void Logger::set_log_level(px::LogLevel level) {
    m_log_level = level;
}

px::LogLevel Logger::get_log_level() {
    return m_log_level;
}

void vpxLog(px::LogLevel logLevel, const char *file, int line, const char *tag, const char *fmt, va_list argList) {
    std::ostringstream file_info;
    std::ostringstream log_stream;
    
    std::string base_file_name = "";
    
    if (file && line > 0) {
        base_file_name = px::base_name(file);
        file_info << base_file_name << ":" << line;
    }
    
    if (logLevel >= px::LogLevelDebug && !base_file_name.empty() && Logger::Files().size() > 0 && !px::includes(px::Logger::Files(), px::file_stem(base_file_name))) {
        return;
    }
    
    va_list dupArgs;
    
#ifdef __GNUC__
    va_copy(dupArgs, argList); // Clone arguments for reuse by different call to vsprintf.
#else
    dupArgs = argList;        // Simply ptr copy for GCC compatibility
#endif
    int bufferSize = vsnprintf(NULL, 0, fmt, dupArgs);
#ifdef __GNUC__
    va_end(dupArgs); // cleanup
#endif
    
    if (bufferSize) {
        char *buffer;
        
        bufferSize++;
        buffer = (char *)malloc(sizeof(char)*bufferSize);
        
#ifdef __GNUC__
        va_copy(dupArgs, argList); // Clone arguments for reused by different call to vsprintf.
#endif
        vsnprintf(buffer, bufferSize, fmt, dupArgs);
#ifdef __GNUC__
        va_end(dupArgs); // cleanup
#endif
        log_stream << buffer;
        
        free(buffer);
    }
    Logger::Log(logLevel, tag, file_info.str().c_str(), log_stream.str().c_str());
}

void pxLog(px::LogLevel logLevel, char const *file, int line, const char * tag, const char * fmt, ...) {
    if (logLevel != px::LogLevelNone && logLevel <= Logger::Instance().get_log_level()) {
        va_list ap;
        va_start(ap, fmt);
        vpxLog(logLevel, file, line, tag, fmt, ap);
        va_end(ap);
    }
}
