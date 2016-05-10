//
//  date.cpp
//  px
//
//  Created by Jonathan Cichon on 03.01.16.
//
//

#include "date.hpp"
#include "helper.hpp"
#include <iomanip>

bool GetDayAndMonthFromWeekInYear(int *year, int weekInYear, int *month, int *dayInMonth);
int GetDayOfTheWeek(int year, int month, int day);
int GetNumberOfWeeksInYear(int year);
bool GetDayAndMonthFromDayInYear(int year, int dayInYear, int *month, int *dayInMonth);
int GetDaysInMonth(int year, int month);
int GetDaysInYear(int year);
bool IsALeapYear(int year);

using namespace px;

Date::Date(const char *str) {
    
}


Date::Date(long uts): m_uts(uts) {}
Date::Date() : m_uts(static_cast<long>(px::timestamp())) {}
Date::Date(int year, int month, int day, int hour, int min, int sec) {
    time_t ts = px::timestamp();
    tm _t; tm *tm = gmtime_r(&ts, &_t);
    
    if (tm) {
        tm->tm_year = year-1900;
        tm->tm_mon = month-1;
        tm->tm_mday = day;
        tm->tm_hour = hour;
        tm->tm_min = min;
        tm->tm_sec = sec;
        ts = px::timegm(tm);
    }
    m_uts = static_cast<long>(ts);
}

Date Date::FromString(const string& str) {
    std::tm t = {};
    std::istringstream ss(str);
    ss >> std::get_time(&t, "%a, %d %b %Y %H:%M:%S %T");
    return Date(t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

Date Date::FromWeek(int year, int week, int day, int hour, int min, int sec) {
    int month;
    int out_day;
    GetDayAndMonthFromWeekInYear(&year, week, &month, &out_day);
    return Date(year, month, out_day+day, hour, min, sec);
}

Date Date::advance(int year, int month, int day, int hour, int min, int sec) const {
    time_t ts = m_uts;
    tm _t; tm *tm = gmtime_r(&ts, &_t);
    if (tm) {
        
        tm->tm_year += year;
        tm->tm_mon += month;
        tm->tm_mday += day;
        tm->tm_hour += hour;
        tm->tm_min += min;
        tm->tm_sec += sec;
        
        ts = px::timegm(tm);
    }
    return Date(ts);
}

Date Date::advance_years(int years) const {
    return advance(years);
}

Date Date::advance_months(int months) const {
    time_t ts = m_uts;
    tm _t; tm *tm = gmtime_r(&ts, &_t);
    if (tm) {
        tm->tm_mon += months;
        
        int day_old = tm->tm_mday;
        if (day_old > 28) {
            int max_days = GetDaysInMonth(tm->tm_year+1900, tm->tm_mon+1);
            if (day_old > max_days) {
                tm->tm_mday = max_days;
            }
        }
        
        ts = px::timegm(tm);
    }
    return Date(ts);
}

Date Date::advance_days(int days) const {
    return advance(0, 0, days);
}

Date Date::advance_hours(int hours) const {
    return advance(0,0,0,hours);
}

Date Date::advance_minutes(int minutes) const {
    return advance(0,0,0,0,minutes);
}

Date Date::advance_seconds(int seconds) const {
    return advance(0,0,0,0,0,seconds);
}

Date Date::beginning_of_day() const {
    time_t ts = m_uts;
    tm _t; tm *tm = gmtime_r(&ts, &_t);
    if(tm) {
        tm->tm_hour = 0;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        
        ts = px::timegm(tm);
    }
    return Date(ts);
}

Date Date::end_of_day() const {
    time_t ts = m_uts;
    tm _t; tm *tm = gmtime_r(&ts, &_t);
    if(tm) {
        tm->tm_hour = 23;
        tm->tm_min = 59;
        tm->tm_sec = 59;
        
        ts = px::timegm(tm);
    }
    return Date(ts);
}

Date Date::beginning_of_month() const {
    time_t ts = m_uts;
    tm _t; tm *tm = gmtime_r(&ts, &_t);
    if(tm) {
        tm->tm_hour = 0;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        tm->tm_mday = 0;
        
        ts = px::timegm(tm);
    }
    return Date(ts);
}

Date Date::end_of_month() const {
    time_t ts = m_uts;
    tm _t; tm *tm = gmtime_r(&ts, &_t);
    if(tm) {
        tm->tm_hour = 23;
        tm->tm_min = 59;
        tm->tm_sec = 59;
        tm->tm_mday = GetDaysInMonth(tm->tm_year+1900, tm->tm_mon+1);
        
        ts = px::timegm(tm);
    }
    return Date(ts);
}

const Date& Date::Empty() {
    static Date d;
    static std::once_flag f;
    std::call_once(f, []{
        d = Date(LONG_MIN);
    });
    return d;
}

bool Date::is_empty() const {
    return m_uts == LONG_MIN;
}

string Date::to_string() const {
    std::time_t t = m_uts;
    char mbstr[100];
    tm _t;
    if (std::strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %H:%M:%S GMT", gmtime_r(&t, &_t))) {
        return mbstr;
    }
    return px::String::Empty();
}

long Date::to_long() const {
    if (this == nullptr) {
        return 0;
    }
    return m_uts;
}

Date::operator long() const {
    if (this == nullptr) {
        return 0;
    }
    return m_uts;
}

bool Date::operator<=(const Date& other) {
    return (*this==other) || (*this<other);
}
bool Date::operator>=(const Date& other) {
    return (*this==other) || (*this>other);
}
bool Date::operator<(const Date& other) {
    return m_uts < other.m_uts;
}
bool Date::operator>(const Date& other) {
    return m_uts > other.m_uts;
}
bool Date::operator==(const Date& other) {
    return m_uts == other.m_uts;
}
bool Date::operator!=(const Date& other) {
    return !(*this == other);
}

bool Date::operator<=(const long& other) {
    return (*this==other) || (*this<other);
}
bool Date::operator>=(const long& other) {
    return (*this==other) || (*this>other);
}
bool Date::operator<(const long& other) {
    return m_uts < other;
}
bool Date::operator>(const long& other) {
    return m_uts > other;
}
bool Date::operator==(const long& other) {
    return m_uts == other;
}
bool Date::operator!=(const long& other) {
    return !(*this == other);
}


/** ISO number of days in a year */
#define ISO_DAYS_A_YEAR 365

/** ISO number of days in a leap year */
#define ISO_DAYS_A_LEAP_YEAR 366

/** Number of ISO days a week */
#define ISO_DAYS_A_WEEK 7

/** Number of ISO days a week */
#define ISO_MONTHS_A_YEAR 12

/** ISO number of days in February */
#define ISO_DAYS_IN_FEBRUARY 28

/** ISO number of days in February in a leap year */
#define ISO_LEAP_YEAR_DAYS_IN_FEBRUARY 29

/** First month number in a ISO year */
#define ISO_FIRST_MONTH_IN_YEAR 1

/*!
 \brief Get the year, month, and day in month from a week in year
 \details An ISO week-numbering year has 52 or 53 full weeks (364 or 371 days)
 \param year [in, out] Gregorian year
 \param weekInYear [in] Gregorian week in a year
 \param month [out] Gregorian month
 \param dayInMonth [out] Gregorian day in month
 \returns \c True if success or \c False if error
 */
bool GetDayAndMonthFromWeekInYear(int *year, int weekInYear, int *month, int *dayInMonth)
{
    //    int daysInMonth;
    int weeksInYear;
    *month = ISO_FIRST_MONTH_IN_YEAR;
    bool hasChanged;
    
    do
    {
        hasChanged = false;
        
        weeksInYear = GetNumberOfWeeksInYear(*year);
        if (weekInYear > weeksInYear)
        {
            weekInYear -= weeksInYear;
            (*year)++;
            hasChanged = true;
        }
        
    } while(hasChanged);
    
    int dayInYear = (weekInYear - 1)  * ISO_DAYS_A_WEEK + 1;
    
    // Since the first day of week 1 in a year in the Gregorian calendar is not usually January 1st we need to handle the offset
    static int t[] = {0, 0, -1, -2, -3, 3, 2, 1};
    int jan1DayOfWeek = GetDayOfTheWeek(*year, 1, 1);
    dayInYear += t[jan1DayOfWeek];
    
    if (dayInYear <= 0)
    {
        // dayInYear is in the previous year
        (*year)--;
        dayInYear += GetDaysInYear(*year);
    }
    else
    {
        int daysInYear = GetDaysInYear(*year);
        if (dayInYear > daysInYear)
        {
            // dayInYear is in the next year
            (*year)++;
            dayInYear -= daysInYear;
        }
    }
    
    if (!GetDayAndMonthFromDayInYear(*year, dayInYear, month, dayInMonth))
        return false;
    
    return true;
}

enum EGregorianMonth
{
    EGregorianMonth_January = 1,
    EGregorianMonth_February = 2,
    EGregorianMonth_March = 3,
    EGregorianMonth_April = 4,
    EGregorianMonth_May = 5,
    EGregorianMonth_June = 6,
    EGregorianMonth_July = 7,
    EGregorianMonth_August = 8,
    EGregorianMonth_September = 9,
    EGregorianMonth_October = 10,
    EGregorianMonth_November = 11,
    EGregorianMonth_December = 12
};

/*!
 \brief Check if leap year
 \par Algorithm
 Checks if the year is divisible by 400 or by 4
 \code
 if year is divisible by 400 then
 is_leap_year
 else if year is divisible by 100 then
 not_leap_year
 else if year is divisible by 4 then
 is_leap_year
 else
 not_leap_year
 \endcode
 \returns \c True if a leap year or \c False if not a leap year
 \see http://en.wikipedia.org/wiki/Leap_year#Algorithm
 */
bool IsALeapYear(int year)
{
    return (!(year % 4) && (year % 100)) || !(year % 400);
}

/*!
 \brief Get number of days in a year in a Gregorian calendar
 */
int GetDaysInYear(int year)
{
    return IsALeapYear(year) ? ISO_DAYS_A_LEAP_YEAR : ISO_DAYS_A_YEAR;
}

/*!
 \brief Get number of days in a month according to the Gregorian calendar
 */
int GetDaysInMonth(int year, int month)
{
    int daysInMonth;
    
    switch (month)
    {
        case EGregorianMonth_February:
            daysInMonth = IsALeapYear(year) ? ISO_LEAP_YEAR_DAYS_IN_FEBRUARY : ISO_DAYS_IN_FEBRUARY;
            break;
        case EGregorianMonth_April:
        case EGregorianMonth_June:
        case EGregorianMonth_September:
        case EGregorianMonth_November:
            daysInMonth = 30;
            break;
        default:
            daysInMonth = 31;
            break;
    }
    
    return daysInMonth;
}

/*!
 \brief Gets the month and day in month from a day in year
 \param year [in] Gregorian year
 \param dayInYear [in] Gregorian day on year (1-365 or 1-366 i leap year)
 \param month [out] Gregorian month
 \param dayInMonth [out] Gregorian day in month
 \returns \c True if success or \c False if error
 */
bool GetDayAndMonthFromDayInYear(int year, int dayInYear, int *month,
                                 int *dayInMonth)
{
    int daysInMonth;
    *month = ISO_FIRST_MONTH_IN_YEAR;
    
    for (int i = ISO_FIRST_MONTH_IN_YEAR; i <= ISO_MONTHS_A_YEAR ; i++)
    {
        daysInMonth = GetDaysInMonth(year, i);
        
        if (dayInYear <= daysInMonth) break;
        
        (*month)++;
        dayInYear -= daysInMonth;
    }
    
    *dayInMonth = (int)dayInYear;
    
    return true;
}

/*!
 \brief Get the number of weeks in a year
 \par Algorithm
 There are 52 weeks in most years however, years that begin on a Thursday and leap years that begin on a Wednesday have 53 weeks.
 */
int GetNumberOfWeeksInYear(int year)
{
    int jan1DayOfWeek = GetDayOfTheWeek(year, 1, 1);
    return jan1DayOfWeek == 4 || (jan1DayOfWeek == 3 && IsALeapYear(year)) ? 53 : 52;
}

/*!
 \brief Get the day of week in a Gregorian calendar
 \par Algorithm
 Uses the Claus Tøndering algorithm
 \see http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week#T.C3.B8ndering.27s_algorithm
 */
int GetDayOfTheWeek(int year, int month, int day)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    return (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
}



