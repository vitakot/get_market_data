/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <string>
#include <utility>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>
#include "gmd_ms_curl_getter.h"
#include "HttpStatusCodes_C++11.h"

extern "C" {
#include "external/strlcpy.h"
}

static const char *MS_API_EOD = "http://api.marketstack.com/v1/eod";

static size_t writeCallback(void *ptr, size_t size, size_t nmemb, std::string *data) {
    data->append((char *) ptr, size * nmemb);
    return size * nmemb;
}

GMDMSCurlGetter::GMDMSCurlGetter(std::string m_apiKey) : m_apiKey(std::move(m_apiKey)) {

    m_curl = curl_easy_init();

    if (!m_curl) {
        throw std::runtime_error("Could not initialize CURL");
    }
}

GMDMSCurlGetter::~GMDMSCurlGetter() {
    curl_easy_cleanup(m_curl);
}

void GMDMSCurlGetter::getEODSymbolData(const std::string &symbol, const std::string &path, std::time_t from,
                                       std::string &eodData) {

    CURLUcode result;
    char *urlStr;
    CURLU *url = curl_url();
    result = curl_url_set(url, CURLUPART_URL, MS_API_EOD, 0);

    if (result == CURLUE_OK) {
        char paramValue1[64];
        char paramValue2[32];

        strlcpy(paramValue1, (std::string("access_key=") + m_apiKey).c_str(), sizeof paramValue1);
        strlcpy(paramValue2, (std::string("symbols=") + symbol).c_str(), sizeof paramValue2);

        if (curl_url_set(url, CURLUPART_QUERY, paramValue1, CURLU_APPENDQUERY) == CURLUE_OK) {
            if (curl_url_set(url, CURLUPART_QUERY, paramValue2, CURLU_APPENDQUERY) == CURLUE_OK) {
                if (curl_url_get(url, CURLUPART_URL, &urlStr, 0) == CURLUE_OK) {

                    curl_easy_setopt(m_curl, CURLOPT_URL, urlStr);
                    curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 10L);
                    curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);
                    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &eodData);
                    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
                    curl_easy_perform(m_curl);

                    int responseCode;
                    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &responseCode);

                    if (responseCode == toInt(HttpStatus::Code::OK)) {
                        spdlog::info("EOD Data for symbol {} downloaded", symbol);

                        nlohmann::json json = nlohmann::json::parse(eodData);
                        writeEodData(json, path);

                    } else {
                        throw std::runtime_error("API error, status: " + std::to_string(responseCode) + "-" +
                                                 HttpStatus::reasonPhrase(responseCode));
                    }
                } else {
                    throw std::runtime_error("Could not compose URL");
                }
            } else {
                throw std::runtime_error("Could not set URL parameter: " + std::string(paramValue1));
            }
        } else {
            throw std::runtime_error("Could not set URL parameter: " + std::string(paramValue2));
        }

    } else {
        throw std::runtime_error("Wrong URL: " + std::string(urlStr));
    }
}

void GMDMSCurlGetter::writeEodData(const nlohmann::json &json, const std::string &path) {

    std::ofstream outFile;

    outFile.open(path, std::ofstream::out | std::ofstream::trunc);

    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }

    nlohmann::json data = json["data"];

    for (auto it = data.rbegin(); it != data.rend(); ++it) {
        auto dataRecord = *it;
        double open = dataRecord["open"];
        double high = dataRecord["high"];
        double low = dataRecord["low"];
        double close = dataRecord["adj_close"];

        std::string date = dataRecord["date"];

        std::tm tm = {};
        std::stringstream ss(date);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

        outFile << std::put_time(&tm, "%m/%d/%Y") << "," << open << "," << high << "," << low
                << "," << close << std::endl;
    }
}

void GMDMSCurlGetter::createOrUpdateSymbolFile(const std::string &symbol, const std::string &path) {
    std::string eodData;
    getEODSymbolData(symbol, path, -1, eodData);
    writeEodData(symbol, path);
}
