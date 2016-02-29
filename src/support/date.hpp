#pragma once
#include "defines.hpp"

namespace px {
    class Date {
    public:
        Date(long uts);
        Date();
        Date(int year, int month, int day, int hour=0, int min=0, int sec=0);
        
        Date advance(int year, int month=0, int day=0, int hour=0, int min=0, int sec=0) const;
        Date advance_years(int years) const;
        Date advance_months(int months) const;
        Date advance_days(int days) const;
        Date advance_hours(int hours) const;
        Date advance_minutes(int minutes) const;
        Date advance_seconds(int seconds) const;
        
        Date beginning_of_day() const;
        Date end_of_day() const;
        
        Date beginning_of_month() const;
        Date end_of_month() const;
        
        static Date FromWeek(int year, int week, int day=0, int hour=0, int min=0, int sec=0);
        
        static const Date& Empty();
        
        bool is_empty() const;
        
        string to_string() const;
        
        long to_long() const;
        
        operator long() const;
        
        bool operator<=(const Date& other);
        bool operator>=(const Date& other);
        bool operator<(const Date& other);
        bool operator>(const Date& other);
        bool operator==(const Date& other);
        bool operator!=(const Date& other);
        
        bool operator<=(const long& other);
        bool operator>=(const long& other);
        bool operator<(const long& other);
        bool operator>(const long& other);
        bool operator==(const long& other);
        bool operator!=(const long& other);
        
    private:
        long m_uts;
    };
}