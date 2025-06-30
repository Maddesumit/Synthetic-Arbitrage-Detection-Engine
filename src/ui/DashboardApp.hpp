#pragma once

#include "HttpServer.hpp"
#include "DataExporter.hpp"
#include "../core/PricingEngine.hpp"
#include "../data/MarketData.hpp"
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

namespace arbitrage {
namespace ui {

/**
 * @brief Main dashboard application that serves the web UI
 */
class DashboardApp {
public:
    DashboardApp(int port = 8080);
    ~DashboardApp();

    /**
     * @brief Initialize the dashboard with engine components
     */
    void initialize(std::shared_ptr<core::PricingEngine> pricing_engine);

    /**
     * @brief Start the dashboard server
     */
    void start();

    /**
     * @brief Start the dashboard server in background
     */
    void startAsync();

    /**
     * @brief Stop the dashboard server
     */
    void stop();

    /**
     * @brief Check if dashboard is running
     */
    bool isRunning() const;

    /**
     * @brief Update market data for display
     */
    void updateMarketData(const std::vector<data::MarketDataPoint>& data);

    /**
     * @brief Update pricing results for display
     */
    void updatePricingResults(const std::vector<core::PricingResult>& results);

    /**
     * @brief Update arbitrage opportunities for display
     */
    void updateArbitrageOpportunities(const std::vector<core::ArbitrageOpportunity>& opportunities);

    /**
     * @brief Get dashboard URL
     */
    std::string getDashboardUrl() const;

private:
    std::unique_ptr<HttpServer> http_server_;
    std::unique_ptr<DataExporter> data_exporter_;
    std::shared_ptr<core::PricingEngine> pricing_engine_;
    
    int port_;
    std::atomic<bool> running_;
    std::thread update_thread_;
    
    // Data storage for demo
    std::mutex data_mutex_;
    std::vector<data::MarketDataPoint> latest_market_data_;
    std::vector<core::PricingResult> latest_pricing_results_;
    std::vector<core::ArbitrageOpportunity> latest_opportunities_;
    
    void setupRoutes();
    void runUpdateLoop();
    void updateDemoData();
    
    // Route handlers
    HttpResponse handleApiStatus(const HttpRequest& request);
    HttpResponse handleApiMarketData(const HttpRequest& request);
    HttpResponse handleApiPricingResults(const HttpRequest& request);
    HttpResponse handleApiOpportunities(const HttpRequest& request);
    HttpResponse handleApiPerformance(const HttpRequest& request);
    HttpResponse handleApiRisk(const HttpRequest& request);
    HttpResponse handleStaticFiles(const HttpRequest& request);
    
    // Utility methods
    HttpResponse createJsonResponse(const nlohmann::json& data) const;
    HttpResponse createErrorResponse(int status_code, const std::string& message) const;
};

} // namespace ui
} // namespace arbitrage
