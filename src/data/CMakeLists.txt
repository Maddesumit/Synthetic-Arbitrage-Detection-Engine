# Data module for market data handling
add_library(arbitrage_data
    MarketData.cpp
    WebSocketClient.cpp
    BinanceClient.cpp
)

target_link_libraries(arbitrage_data 
    PRIVATE 
    arbitrage_utils
    nlohmann_json_lib
    spdlog_lib
    ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(arbitrage_data 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE
    ${CMAKE_SOURCE_DIR}/external/websocketpp
    ${CMAKE_SOURCE_DIR}/external/nlohmann_json/include
    ${CMAKE_SOURCE_DIR}/external/spdlog/include
)

# Add OpenSSL for TLS WebSocket connections
find_package(OpenSSL REQUIRED)
target_link_libraries(arbitrage_data PRIVATE OpenSSL::SSL OpenSSL::Crypto)
