#include "WebSocketClient.hpp"
#include "BinanceClient.hpp"
#include "OKXClient.hpp"
#include "BybitClient.hpp"
#include <chrono>
#include <thread>

namespace arbitrage {
namespace data {

WebSocketClient::WebSocketClient(Exchange exchange) 
    : exchange_(exchange), current_reconnect_delay_(reconnect_initial_delay_) {
}

WebSocketClient::~WebSocketClient() {
    // Stop reconnection if active
    reconnection_active_ = false;
    if (reconnection_thread_.joinable()) {
        reconnection_thread_.join();
    }
}

void WebSocketClient::setReconnectParameters(int initial_delay, int max_delay, double backoff_multiplier) {
    reconnect_initial_delay_ = initial_delay;
    reconnect_max_delay_ = max_delay;
    reconnect_backoff_multiplier_ = backoff_multiplier;
    current_reconnect_delay_ = initial_delay;
}

void WebSocketClient::updateConnectionStatus(ConnectionStatus status) {
    ConnectionStatus old_status = connection_status_.load();
    connection_status_ = status;
    
    LOG_INFO("WebSocket client for {} status changed from {} to {}", 
             exchangeToString(exchange_), 
             static_cast<int>(old_status), 
             static_cast<int>(status));
    
    if (connection_callback_) {
        connection_callback_(status);
    }
    
    // Start reconnection if connection lost and auto-reconnect is enabled
    if (status == ConnectionStatus::ERROR || status == ConnectionStatus::DISCONNECTED) {
        if (auto_reconnect_ && !reconnection_active_) {
            startReconnection();
        }
    } else if (status == ConnectionStatus::CONNECTED) {
        // Reset reconnection delay on successful connection
        current_reconnect_delay_ = reconnect_initial_delay_;
        reconnection_active_ = false;
    }
}

void WebSocketClient::handleError(const std::string& error_message) {
    LOG_ERROR("WebSocket client for {} error: {}", exchangeToString(exchange_), error_message);
    
    if (error_callback_) {
        error_callback_(error_message);
    }
    
    updateConnectionStatus(ConnectionStatus::ERROR);
}

void WebSocketClient::onOrderBookReceived(const OrderBook& orderbook) {
    if (orderbook_callback_) {
        orderbook_callback_(orderbook);
    }
}

void WebSocketClient::onTradeReceived(const Trade& trade) {
    if (trade_callback_) {
        trade_callback_(trade);
    }
}

void WebSocketClient::onTickerReceived(const Ticker& ticker) {
    if (ticker_callback_) {
        ticker_callback_(ticker);
    }
}

void WebSocketClient::onFundingRateReceived(const FundingRate& funding_rate) {
    if (funding_rate_callback_) {
        funding_rate_callback_(funding_rate);
    }
}

void WebSocketClient::onMarkPriceReceived(const MarkPrice& mark_price) {
    if (mark_price_callback_) {
        mark_price_callback_(mark_price);
    }
}

void WebSocketClient::startReconnection() {
    if (reconnection_active_) {
        return;  // Already reconnecting
    }
    
    reconnection_active_ = true;
    updateConnectionStatus(ConnectionStatus::RECONNECTING);
    
    // Start reconnection in a separate thread
    if (reconnection_thread_.joinable()) {
        reconnection_thread_.join();
    }
    
    reconnection_thread_ = std::thread(&WebSocketClient::reconnectionLoop, this);
}

void WebSocketClient::reconnectionLoop() {
    LOG_INFO("Starting reconnection loop for {}", exchangeToString(exchange_));
    
    while (reconnection_active_ && connection_status_ != ConnectionStatus::CONNECTED) {
        LOG_INFO("Attempting to reconnect to {} in {}ms", 
                 exchangeToString(exchange_), current_reconnect_delay_);
        
        // Wait before attempting reconnection
        std::this_thread::sleep_for(std::chrono::milliseconds(current_reconnect_delay_));
        
        if (!reconnection_active_) {
            break;
        }
        
        // Attempt to reconnect
        if (connect()) {
            LOG_INFO("Reconnection to {} successful", exchangeToString(exchange_));
            reconnection_active_ = false;
            break;
        } else {
            LOG_WARN("Reconnection to {} failed, will retry", exchangeToString(exchange_));
            
            // Increase delay with backoff
            current_reconnect_delay_ = std::min(
                static_cast<int>(current_reconnect_delay_ * reconnect_backoff_multiplier_),
                reconnect_max_delay_
            );
        }
    }
    
    LOG_INFO("Reconnection loop for {} ended", exchangeToString(exchange_));
}

std::unique_ptr<WebSocketClient> createWebSocketClient(Exchange exchange) {
    switch (exchange) {
        case Exchange::BINANCE:
            return std::make_unique<BinanceClient>();
        case Exchange::OKX:
            return std::make_unique<OKXClient>();
        case Exchange::BYBIT:
            return std::make_unique<BybitClient>();
        default:
            throw std::invalid_argument("Unsupported exchange");
    }
}

} // namespace data
} // namespace arbitrage
