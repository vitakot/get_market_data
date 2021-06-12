/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef GET_MARKET_DATA_YAHOO_CURL_GETTER_H
#define GET_MARKET_DATA_YAHOO_CURL_GETTER_H

#include <curl/curl.h>
#include <nlohmann/json_fwd.hpp>
#include "gmd_types.h"
#include "gmd_i_curl_getter.h"

class GMDYahooCurlGetter : public GMDICurlGetter {

    CURL *m_curl = nullptr;

    static bool checkExistingFile(const std::string &path, time_t &timeOfNewestRecord);

    static void fixFileEnding(const std::string &path);

    void getEODSymbolData(const std::string &symbol, const std::string &path, std::time_t from, std::string &eodData);

    static void appendDataToFile(const std::string &data, std::ofstream &outFile);

public:

    explicit GMDYahooCurlGetter();

    ~GMDYahooCurlGetter() override;

    void createOrUpdateSymbolFile(const std::string &symbol, const std::string &path) override;
};


#endif //GET_MARKET_DATA_YAHOO_CURL_GETTER_H
