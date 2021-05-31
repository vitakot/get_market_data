/*
Get Market Data
https://github.com/vitakot/get_market_data

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "gmd.h"
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

int main(int argc, char **argv) {

    std::string apiKey;
    Providers provider;
    std::vector<std::string> symbols;
    std::vector<std::string> outputFilePaths;
    cxxopts::ParseResult parseResult;
    std::string pr(" ");

    cxxopts::Options options("get_market_data", "Utility for downloading market data from https://marketstack.com/");

    pr[0] = static_cast<char>(Providers::Yahoo);

    options.add_options()
            ("p,provider", "Data provider, y - yahoo (default), m - marketstack, example: -p y",
             cxxopts::value<char>()->default_value(pr))
            ("k,key", "Your API key, example: -k 12456424", cxxopts::value<std::string>())
            ("s,symbols", "Symbols of assets to download, example: -s \"vix,vix3m\" ",
             cxxopts::value<std::vector<std::string>>())
            ("o,outputs",
             R"(Paths to output asset files, example: -s "C:\Users\Public\vix.csv,C:\Users\Public\vix3m.csv" )",
             cxxopts::value<std::vector<std::string>>())
            ("h,help", "Print usage");
    try {
        parseResult = options.parse(argc, argv);
    }
    catch (cxxopts::OptionException &ex) {
        std::cout << "Wrong parameters!" << std::endl << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    provider = (Providers) parseResult["provider"].as<char>();

    if (parseResult.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (static_cast<Providers>(provider) != Providers::Yahoo &&
        static_cast<Providers>(provider) != Providers::Marketstack) {
        std::cout << "Wrong provider!" << std::endl << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (!parseResult.count("key") &&
        static_cast<Providers>(provider) == Providers::Marketstack) {
        std::cout << "No API key! MarketStack provider requires your unique API key." << std::endl << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    } else if (parseResult.count("key")) {
        apiKey = parseResult["key"].as<std::string>();
    }

    if (parseResult.count("symbols")) {
        symbols = parseResult["symbols"].as<std::vector<std::string>>();
    } else {
        std::cout << "No symbols!" << std::endl << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (parseResult.count("outputs")) {
        outputFilePaths = parseResult["outputs"].as<std::vector<std::string>>();

        if (symbols.size() != outputFilePaths.size()) {
            std::cout << "Number of output files must match number of symbols!" << std::endl << std::endl;
            std::cout << options.help() << std::endl;
            exit(0);
        }
    } else {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    try {
        std::unique_ptr<GetMarketData> getMarketData;
        getMarketData = std::make_unique<GetMarketData>(provider, apiKey, symbols, outputFilePaths);
        getMarketData->getEODData();
    }
    catch (std::runtime_error &e) {
        spdlog::critical(e.what());
    }
    catch (nlohmann::json::exception &e) {
        spdlog::critical(e.what());
    }
    catch (std::exception &e) {
        spdlog::critical(e.what());
    }

    return 0;
}
