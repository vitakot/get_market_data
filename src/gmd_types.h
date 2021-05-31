/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef GET_MARKET_DATA_TYPES_H
#define GET_MARKET_DATA_TYPES_H

#include <ctime>

static const std::time_t HISTORY_LENGTH_IN_S = 31556926; // Number of seconds in 1 year

enum class Providers : char {
    Yahoo = 'y',
    Marketstack = 'm'
};

#endif //GET_MARKET_DATA_TYPES_H
