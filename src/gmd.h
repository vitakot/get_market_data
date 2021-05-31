/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef GET_MARKET_DATA_H
#define GET_MARKET_DATA_H

#include <vector>
#include <string>
#include <nlohmann/json_fwd.hpp>
#include "gmd_types.h"

class GetMarketData {

private:
    const Providers m_provider;
    const std::string m_apiKey;
    const std::vector<std::string> m_symbols;
    const std::vector<std::string> m_outputFilePaths;

public:

    explicit GetMarketData(Providers provider, std::string apiKey, std::vector<std::string> symbols,
                           std::vector<std::string> outputFilePaths);

    ~GetMarketData();

    void getEODData();
};

#endif //GET_MARKET_DATA_H
