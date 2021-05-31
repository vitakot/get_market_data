/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef GET_MARKET_DATA_UTIL_H
#define GET_MARKET_DATA_UTIL_H

#include <sstream>

static const int SECONDS_PER_MINUTE = 60;
static const int SECONDS_PER_HOUR = 3600;
static const int SECONDS_PER_DAY = 86400;
static const int DAYS_OF_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool isLeapYear(short year) {
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    return (year % 400) == 0;
}

static time_t mkgmtime(const struct tm *ptm) {
    time_t secs = 0;
    // tm_year is years since 1900
    int year = ptm->tm_year + 1900;
    for (int y = 1970; y < year; ++y) {
        secs += (isLeapYear(static_cast<short>(y)) ? 366 : 365) * SECONDS_PER_DAY;
    }
    // tm_mon is month from 0..11
    for (int m = 0; m < ptm->tm_mon; ++m) {
        secs += DAYS_OF_MONTH[m] * SECONDS_PER_DAY;
        if (m == 1 && isLeapYear(static_cast<short>(year))) {
            secs += SECONDS_PER_DAY;
        }
    }
    secs += (ptm->tm_mday - 1) * SECONDS_PER_DAY;
    secs += ptm->tm_hour * SECONDS_PER_HOUR;
    secs += ptm->tm_min * SECONDS_PER_MINUTE;
    secs += ptm->tm_sec;
    return secs;
}

std::vector<std::string> splitString(const std::string &s, char delim) {

    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;

    while (std::getline(ss, item, delim)) {
        elems.push_back(std::move(item));
    }

    return elems;
}

#endif //GET_MARKET_DATA_UTIL_H
