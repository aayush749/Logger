#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>
#include <string_view>

enum class Log
{
    INFO, WARNING, ERR
};

/**
 * A singleton Logger class, which can log simple messages to console
 */
class Logger
{
public:

    template <Log level, size_t sz, typename... Args>
    static int logfmt(const char* fmt, Args... args)
    {
        char MSG[sz] = {0};
        int val = snprintf(MSG, sz, fmt, args...);
        Logger::log(MSG, level);
        return val;
    }
    
    template <Log level, typename... Args>
    static int logfmt(const char* fmt, Args... args)
    {
        constexpr size_t sz = 1024;
        char MSG[sz] = {0};
        int val = snprintf(MSG, sz, fmt, args...);
        Logger::log(MSG, level);
        return val;
    }

    static void init(bool breakOnError = false)
    {
        s_BreakOnError = breakOnError;
    }

    template <typename T = std::string_view, typename... Args>
    static void LogInfo(const T& msg, const Args&... args)
    {
        std::string wholeMessage = Logger::join(args...);
        wholeMessage = msg + wholeMessage;
        log(wholeMessage.c_str(), Log::INFO);
    }

    static void LogWarning(std::string_view msg)
    {
        log(msg, Log::WARNING);
    }

    static void LogError(std::string_view msg)
    {
        log(msg, Log::ERR);
    }

    static void log(std::string_view msg, Log level = s_DefaultLogLevel)
    {
        log(msg.data(), level);
    }

    static void log(const char* msg = nullptr, Log level = s_DefaultLogLevel)
    {
        if(msg)
        {
            updateCurrentTime();
            char timeBuf[80];
            tm tmVar;
            localtime_s(&tmVar, &s_currentTime);
            strftime(timeBuf, sizeof(timeBuf), "%X", &tmVar);
            switch (level)
            {
            case Log::INFO:
                std::cerr << "\x1b[32m"<<timeBuf<<' '
                          <<"INFO: "
                          <<msg<<"\x1b[0m"<<std::endl;
                break;
            
            case Log::WARNING:
                std::cerr << "\x1b[33m"<<timeBuf<<' '
                          <<"WARNING: "
                          <<msg<<"\x1b[0m"<<std::endl;
                break;
            
            case Log::ERR:
                std::cerr << "\x1b[1;4;31m"<<timeBuf<<' '
                          <<"ERR: "
                          <<msg<<"\x1b[0m"<<std::endl;
#if DEBUG
                if(s_BreakOnError)
                    __debugbreak();   
#endif
                break;

            default:
                break;
            }
        }
    }
private:
    static void updateCurrentTime()
    {
        time(&s_currentTime);
    }

    template<typename... Args>
    static std::string join(Args... args)
    {
        return (std::string(args) + ...);
    }

private:
    inline constexpr static Log s_DefaultLogLevel = Log::INFO;
    inline static bool s_BreakOnError = false;
    inline static time_t s_currentTime;
};