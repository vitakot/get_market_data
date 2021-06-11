# Get Market Data
Utility for downloading EOD market data from https://marketstack.com/ and https://finance.yahoo.com/.

## Usage

    get_market_data [OPTION...]

    -p, --provider arg  Data provider, y - yahoo (default), m - marketstack,
                        example: -p y (default: y)  
    -k, --key arg       Your API key, example: -k 12456424  
    -s, --symbols arg   Symbols of assets to download, example: -s
                        "^vix,^vix3m" (actual symbols depends on provider)  
    -o, --outputs arg   Paths to output asset files, example: -s
                        "C:\Users\Public\vix.csv,C:\Users\Public\vix3m.csv"  
    -h, --help          Print usage  

## Dependencies

https://github.com/curl/curl  
https://github.com/gabime/spdlog  
https://github.com/nlohmann/json  
https://github.com/jarro2783/cxxopts  
https://github.com/j-ulrich/http-status-codes-cpp