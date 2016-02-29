#pragma once
#include "defines.hpp"
#include <chrono>
#include <future>

namespace px {
    
    class Timer;
    typedef std::shared_ptr<Timer> Timer_p;
    
    class Timer {
    public:
        static Timer_p Create(double seconds, function<void()> block);
        
        virtual ~Timer();
        
        void delay(double seconds);
        
    private:
        double m_interval;
        std::chrono::system_clock::time_point m_fire_date;
        function<void()> m_block;
        std::mutex m_mutex;
        Timer_p m_self_ref;
        
        void _call();
        void _start();
        
        Timer(double, function<void()>);
        Timer() = delete;
        Timer(const Timer& src) = delete;
        Timer& operator=(const Timer& rhs) = delete;
    };
}