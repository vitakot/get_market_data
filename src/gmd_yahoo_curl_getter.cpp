/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include <ctime>
#include "gmd_yahoo_curl_getter.h"
#include "gmd_util.h"
#include "HttpStatusCodes_C++11.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <iomanip>

extern "C" {
#include "external/strlcpy.h"
}

static const char *YAHOO_API_EOD = "https://query1.finance.yahoo.com/v7/finance/download/";

static size_t writeCallback(void *ptr, size_t size, size_t nmemb, std::string *data) {
    data->append((char *) ptr, size * nmemb);
    return size * nmemb;
}

GMDYahooCurlGetter::GMDYahooCurlGetter() {
    m_curl = curl_easy_init();

    if (!m_curl) {
        throw std::runtime_error("Could not initialize CURL");
    }
}

GMDYahooCurlGetter::~GMDYahooCurlGetter() {
    curl_easy_cleanup(m_curl);
}

void GMDYahooCurlGetter::getEODSymbolData(const std::string &symbol, const std::string &path, std::time_t from,
                                          std::string &eodData) {
    CURLUcode result;
    CURLU *url = curl_url();

    std::string eodURLForSymbol = YAHOO_API_EOD + symbol;
    result = curl_url_set(url, CURLUPART_URL, eodURLForSymbol.c_str(), 0);

    if (result == CURLUE_OK) {
        char *urlStr;
        char paramValue1[64];
        char paramValue2[32];
        char paramValue3[32];
        char paramValue4[32];
        char paramValue5[32];

        std::time_t timeNow = std::time(nullptr);
        std::time_t timePast = from;

        strlcpy(paramValue1, (std::string("period1=") + std::to_string(timePast)).c_str(), sizeof paramValue1);
        strlcpy(paramValue2, (std::string("period2=") + std::to_string(timeNow)).c_str(), sizeof paramValue2);
        strlcpy(paramValue3, "interval=1d", sizeof paramValue3);
        strlcpy(paramValue4, "events=history", sizeof paramValue4);
        strlcpy(paramValue5, "includeAdjustedClose=true", sizeof paramValue5);

        if (curl_url_set(url, CURLUPART_QUERY, paramValue1, CURLU_APPENDQUERY) == CURLUE_OK) {
            if (curl_url_set(url, CURLUPART_QUERY, paramValue2, CURLU_APPENDQUERY) == CURLUE_OK) {
                if (curl_url_set(url, CURLUPART_QUERY, paramValue3, CURLU_APPENDQUERY) == CURLUE_OK) {
                    if (curl_url_set(url, CURLUPART_QUERY, paramValue4, CURLU_APPENDQUERY) == CURLUE_OK) {
                        if (curl_url_set(url, CURLUPART_QUERY, paramValue5, CURLU_APPENDQUERY) == CURLUE_OK) {
                            if (curl_url_get(url, CURLUPART_URL, &urlStr, 0) == CURLUE_OK) {

                                curl_easy_setopt(m_curl, CURLOPT_URL, urlStr);
                                curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 10L);
                                curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);
                                curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
                                curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &eodData);
                                curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
                                curl_easy_perform(m_curl);

                                int responseCode;
                                curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &responseCode);

                                if (responseCode == toInt(HttpStatus::Code::OK)) {
                                    spdlog::info("EOD Data for symbol {} downloaded", symbol);

                                } else {
                                    throw std::runtime_error(
                                            "API error, status: " + std::to_string(responseCode) + "-" +
                                            HttpStatus::reasonPhrase(responseCode));
                                }
                            } else {
                                throw std::runtime_error("Could not compose URL");
                            }
                        } else {
                            throw std::runtime_error("Could not set URL parameter: " + std::string(paramValue5));
                        }
                    } else {
                        throw std::runtime_error("Could not set URL parameter: " + std::string(paramValue4));
                    }
                } else {
                    throw std::runtime_error("Could not set URL parameter: " + std::string(paramValue3));
                }
            } else {
                throw std::runtime_error("Could not set URL parameter: " + std::string(paramValue2));
            }
        } else {
            throw std::runtime_error("Could not set URL parameter: " + std::string(paramValue1));
        }
    } else {
        throw std::runtime_error("Wrong URL: " + std::string(YAHOO_API_EOD));
    }
}

bool GMDYahooCurlGetter::checkExistingFile(const std::string &path, time_t &timeOfNewestRecord) {

    std::ifstream inFile(path);
    std::string row;
    std::string lastRow;

    //  Read last line
    if (inFile.is_open()) {

        while (std::getline(inFile, row)) {
            if (!row.empty()) {
                lastRow = std::move(row);
            }
        }

        auto records = splitString(lastRow, ',');

        if (!records.empty()) {
            std::string date = records[0];
            std::stringstream ss(date);

            std::tm tm = {};
            ss >> std::get_time(&tm, "%m/%d/%Y");
            timeOfNewestRecord = mkgmtime(&tm) + SECONDS_PER_DAY;

            if (timeOfNewestRecord != -1) {
                return true;
            }
        }
        inFile.close();
    }

    return false;
}

void GMDYahooCurlGetter::appendDataToFile(const std::string &data, std::ofstream &outFile) {

    std::stringstream dataStream(data);
    std::string row;
    int rowsCount = 0;

    while (std::getline(dataStream, row)) {
        rowsCount++;

        if (rowsCount == 1) {
            // Skip the comments
            continue;
        }

        auto records = splitString(row, ',');

        std::string date = records[0];
        double open = std::stod(records[1]);
        double high = std::stod(records[2]);
        double low = std::stod(records[3]);
        double close = std::stod(records[5]);  // Adjusted close

        std::tm tm = {};
        std::stringstream ss(date);
        ss >> std::get_time(&tm, "%Y-%m-%d");

        outFile << std::put_time(&tm, "%m/%d/%Y") << "," << open << "," << high << "," << low
                << "," << close << std::endl;
    }
}

void GMDYahooCurlGetter::createOrUpdateSymbolFile(const std::string &symbol, const std::string &path) {

    std::ofstream outFile;
    std::time_t timeOfNewestRecord;
    std::string eodData;

    if (checkExistingFile(path, timeOfNewestRecord)) {

        outFile.open(path, std::ofstream::out | std::ofstream::app);

        if (outFile.is_open()) {
            getEODSymbolData(symbol, path, timeOfNewestRecord, eodData);
            appendDataToFile(eodData, outFile);
            outFile.close();
        } else {
            throw std::runtime_error("Cannot open file: " + path);
        }

    } else {
        std::time_t timeNow = std::time(nullptr);
        timeOfNewestRecord = timeNow - HISTORY_LENGTH_IN_S;

        outFile.open(path, std::ofstream::out | std::ofstream::trunc);

        if (outFile.is_open()) {
            getEODSymbolData(symbol, path, timeOfNewestRecord, eodData);
            appendDataToFile(eodData, outFile);
            outFile.close();
        } else {
            throw std::runtime_error("Cannot open file: " + path);
        }
    }
}
