cmake_minimum_required(VERSION 3.14)
project(get_market_data)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS -DCURL_STATICLIBS)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(json)
add_subdirectory(spdlog)
add_subdirectory(cxxopts)

include_directories(curl/include)
link_directories(curl/lib)
include_directories(src)
include_directories(json/include)
include_directories(http-status-codes-cpp)

add_executable(get_market_data
        main.cpp
        src/gmd.cpp
        src/gmd.h
        src/external/strlcpy.h
        src/external/strlcpy.c
        src/gmd_types.h
        src/gmd_yahoo_curl_getter.cpp
        src/gmd_yahoo_curl_getter.h
        src/gmd_ms_curl_getter.h
        src/gmd_ms_curl_getter.cpp
        src/gmd_i_curl_getter.h
        src/gmd_util.h)

target_link_libraries(get_market_data -lcurl spdlog cxxopts)