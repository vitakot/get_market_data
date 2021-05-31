/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef GET_MARKET_DATA_MS_CURL_GETTER_H
#define GET_MARKET_DATA_MS_CURL_GETTER_H

#include <curl/curl.h>
#include <nlohmann/json_fwd.hpp>
#include "gmd_types.h"
#include "gmd_i_curl_getter.h"

class GMDMSCurlGetter : public GMDICurlGetter {

    CURL *m_curl = nullptr;
    const std::string m_apiKey;

    static void writeEodData(const nlohmann::json &json, const std::string &path);

    void getEODSymbolData(const std::string &symbol, const std::string &path, std::time_t from, std::string &eodData);

public:
    explicit GMDMSCurlGetter(std::string m_apiKey);

    ~GMDMSCurlGetter() override;

    void createOrUpdateSymbolFile(const std::string &symbol, const std::string &path) override;
};

#endif //GET_MARKET_DATA_MS_CURL_GETTER_H
