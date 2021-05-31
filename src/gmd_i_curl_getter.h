/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef GET_MARKET_DATA_I_CURL_GETTER_H
#define GET_MARKET_DATA_I_CURL_GETTER_H

struct GMDICurlGetter {

    virtual ~GMDICurlGetter() = default;

    virtual void createOrUpdateSymbolFile(const std::string &symbol, const std::string &path) = 0;
};

#endif //GET_MARKET_DATA_I_CURL_GETTER_H
