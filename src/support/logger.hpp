#pragma once
#include "defines.hpp"

namespace px {
    typedef enum {
        LogLevelNone,
        LogLevelError,
        LogLevelWarning,
        LogLevelInfo,
        LogLevelDebug,
        LogLevelVerbose,
    } LogLevel;
    
    class Logger final {
    public:
        ~Logger() {}
        static Logger& Instance();
        static void Log(LogLevel logLevel, const char *tag, const char *file_info, const char * msg);
        static const std::vector<std::string>& Files();
        
        void set_log_function(function<void(LogLevel logLevel, const char *tag, const char *file_info, const char * msg)> log_fn);
        
        void set_file_scopes(const std::vector<std::string>& files);
        
        LogLevel get_log_level();
        void set_log_level(LogLevel level);
        
    private:
        LogLevel m_log_level;
        std::vector<std::string> m_files;
        
        static std::unique_ptr<Logger> m_instance;
        static std::once_flag m_onceFlag;
        Logger(void);
        Logger(const Logger& src);
        Logger& operator=(const Logger& rhs);
        
        void log(LogLevel logLevel, const char *tag, const char *file_info, const char * msg);
        function<void(LogLevel logLevel, const char *tag, const char *file_info, const char * msg)> m_log_fn;
    };
    
}

#ifdef __printflike
void pxLog(px::LogLevel logLevel, char const *file, int line, const char * tag, const char * fmt, ...) __printflike(5, 6);
#else
void pxLog(px::LogLevel logLevel, char const *file, int line, const char * tag, const char * fmt, ...);
#endif

/** Logging Errors */
#define LogError(...) pxLog(px::LogLevelError, __FILE__, __LINE__, "SYNC", __VA_ARGS__)

/** Logging Warnings */
#define LogWarn(...) pxLog(px::LogLevelWarning, __FILE__, __LINE__, "SYNC", __VA_ARGS__)

/** Logging Infos */
#define LogInfo(...) pxLog(px::LogLevelInfo, __FILE__, __LINE__, "SYNC", __VA_ARGS__)

/** Logging Debug Infos */
#define LogDebug(...) pxLog(px::LogLevelDebug, __FILE__, __LINE__, "SYNC", __VA_ARGS__)

/** Logging Without Automatic Formating */
#define LogPlain(...) pxLog(pxLogLevelVerbose, NULL, -1, NULL, __VA_ARGS__)
