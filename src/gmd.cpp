/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "gmd.h"
#include "gmd_ms_curl_getter.h"
#include "gmd_yahoo_curl_getter.h"
#include <string>

GetMarketData::GetMarketData(Providers provider, std::string apiKey, std::vector<std::string> symbols,
                             std::vector<std::string> outputFilePaths)
        : m_provider(provider),
          m_apiKey(std::move(apiKey)),
          m_symbols(std::move(symbols)),
          m_outputFilePaths(std::move(outputFilePaths)) {

    curl_global_init(CURL_GLOBAL_ALL);
}

GetMarketData::~GetMarketData() {
    curl_global_cleanup();
}

void GetMarketData::getEODData() {

    for (auto i = 0; i < m_symbols.size(); i++) {

        std::unique_ptr<GMDICurlGetter> getter;

        if (m_provider == Providers::Marketstack) {
            getter = std::make_unique<GMDMSCurlGetter>(m_apiKey);
        } else {
            getter = std::make_unique<GMDYahooCurlGetter>();
        }

        getter->createOrUpdateSymbolFile(m_symbols[i], m_outputFilePaths[i]);
    }
}