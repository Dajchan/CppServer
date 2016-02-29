//
//  timer.cpp
//  px
//
//  Created by Jonathan Cichon on 07.01.16.
//
//

#include "timer.hpp"
#include "logger.hpp"
#include "helper.hpp"

using namespace px;


Timer_p Timer::Create(double seconds, function<void()> block) {
    Timer_p p = Timer_p(new Timer(seconds, block));
    p->m_self_ref = p;
    p->_start();
    return p;
}

Timer::Timer(double seconds, function<void()> block) : m_interval(seconds), m_block(block) {
    delay(seconds);
}

Timer::~Timer() {}

void Timer::delay(double seconds) {
    m_fire_date = std::chrono::system_clock::now() + std::chrono::milliseconds((int)(seconds*1000));
}

void Timer::_start() {
    std::thread([this] () {
        _call();
    }).detach();
}

void Timer::_call() {
    Timer_p self = m_self_ref;
    while (true) {
        std::this_thread::sleep_until(m_fire_date);
        if (std::chrono::system_clock::now() >= m_fire_date) {
            m_block();
            delay(m_interval);
        }
    }
}


