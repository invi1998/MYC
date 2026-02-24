
#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <iomanip>
#include <sstream>   // 用于错误处理示例

class Date {
private:
    int day;
    int month;
    int year;

public:
    Date(int day = 1, int month = 1, int year = 1970) : day(day), month(month), year(year) {
        if (!isValidDate(day, month, year)) {
            throw std::invalid_argument("Invalid date");
        }
    }
    // 是否是leap year（闰年）
    bool isLeapYear(int year) const {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    int daysInMonth(int month, int year) const {
        static const int daysPerMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (month == 2 && isLeapYear(year)) {
            return 29;
        }
        return daysPerMonth[month - 1];
    }

    // 输出格式: YYYY-MM-DD
    friend std::ostream& operator<<(std::ostream& os, const Date& date) {
        os << std::setfill('0') << std::setw(4) << date.year << "-"
           << std::setw(2) << date.month << "-"
           << std::setw(2) << date.day;
        return os;
    }

    // 输入格式: YYYY-MM-DD (例如: 2024-06-15) 
    friend std::istream& operator>>(std::istream& is, Date& date) {
        std::string input;
        is >> input;
        std::istringstream ss(input);
        char delimiter1, delimiter2;
        int year, month, day;
        if (ss >> year >> delimiter1 >> month >> delimiter2 >> day &&
            delimiter1 == '-' && delimiter2 == '-' &&
            date.isValidDate(day, month, year)) {
            date.year = year;
            date.month = month;
            date.day = day;
        } else {
            is.setstate(std::ios::failbit); // 设置输入流状态为失败
        }
        return is;
    }

private:
    bool isValidDate(int day, int month, int year) const {
        if (year < 1 || month < 1 || month > 12 || day < 1) {
            return false;
        }
        if (day > daysInMonth(month, year)) {
            return false;
        }
        return true;
    }
};


#endif // DATE_H
