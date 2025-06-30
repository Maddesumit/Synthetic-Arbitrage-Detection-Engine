#include "DashboardApp.hpp"
#include "../utils/Logger.hpp"
#include <sstream>

namespace arbitrage {
namespace ui {

DashboardApp::DashboardApp(int port) 
    : port_(port), running_(false) {
    http_server_ = std::make_unique<HttpServer>(port);
    data_exporter_ = std::make_unique<DataExporter>();
    
    // Enable CORS for web dashboard
    http_server_->enableCORS(true);
    
    setupRoutes();
}

DashboardApp::~DashboardApp() {
    stop();
}

void DashboardApp::initialize(std::shared_ptr<core::PricingEngine> pricing_engine) {
    pricing_engine_ = pricing_engine;
    utils::Logger::info("Dashboard initialized with pricing engine");
}

void DashboardApp::start() {
    running_ = true;
    utils::Logger::info("Starting dashboard on port " + std::to_string(port_));
    
    // Start update loop in separate thread
    update_thread_ = std::thread([this]() { runUpdateLoop(); });
    
    // Start HTTP server (blocking)
    http_server_->start();
}

void DashboardApp::startAsync() {
    running_ = true;
    utils::Logger::info("Starting dashboard async on port " + std::to_string(port_));
    
    // Start update loop
    update_thread_ = std::thread([this]() { runUpdateLoop(); });
    
    // Start HTTP server in background
    http_server_->startAsync();
}

void DashboardApp::stop() {
    running_ = false;
    if (update_thread_.joinable()) {
        update_thread_.join();
    }
    if (http_server_) {
        http_server_->stop();
    }
    utils::Logger::info("Dashboard stopped");
}

void DashboardApp::setupRoutes() {
    // API routes
    http_server_->addRoute("GET", "/api/status", 
        [this](const HttpRequest& req) { return handleApiStatus(req); });
    http_server_->addRoute("GET", "/api/market-data", 
        [this](const HttpRequest& req) { return handleApiMarketData(req); });
    http_server_->addRoute("GET", "/api/pricing-results", 
        [this](const HttpRequest& req) { return handleApiPricingResults(req); });
    http_server_->addRoute("GET", "/api/opportunities", 
        [this](const HttpRequest& req) { return handleApiOpportunities(req); });
    http_server_->addRoute("GET", "/api/performance", 
        [this](const HttpRequest& req) { return handleApiPerformance(req); });
    http_server_->addRoute("GET", "/api/risk", 
        [this](const HttpRequest& req) { return handleApiRisk(req); });
    
    // Static files (for serving dashboard HTML/JS/CSS)
    http_server_->addRoute("GET", "/", 
        [this](const HttpRequest& req) { return handleStaticFiles(req); });
    http_server_->addRoute("GET", "/dashboard", 
        [this](const HttpRequest& req) { return handleStaticFiles(req); });
}

void DashboardApp::runUpdateLoop() {
    while (running_) {
        try {
            updateDemoData();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } catch (const std::exception& e) {
            utils::Logger::error("Error in update loop: " + std::string(e.what()));
        }
    }
}

void DashboardApp::updateDemoData() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Update demo data
    std::vector<data::MarketDataPoint> demo_market_data;
    std::vector<core::PricingResult> demo_pricing_results;
    std::vector<core::ArbitrageOpportunity> demo_opportunities;
    
    // Create demo market data
    data::MarketDataPoint btc_data;
    btc_data.symbol = "BTCUSDT";
    btc_data.exchange = "binance";
    btc_data.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    btc_data.bid = 50000.50;
    btc_data.ask = 50000.75;
    btc_data.last = 50000.62;
    btc_data.volume = 1234.56;
    btc_data.funding_rate = 0.0001;
    demo_market_data.push_back(btc_data);
    
    // Create demo pricing result
    core::PricingResult pricing_result;
    pricing_result.instrument_id = "BTCUSDT";
    pricing_result.synthetic_price = 50000.62;
    pricing_result.confidence = 1.0;
    pricing_result.model_name = "Spot";
    pricing_result.components.base_price = 50000.62;
    pricing_result.success = true;
    pricing_result.calculation_time_ms = 0.5;
    pricing_result.timestamp = std::chrono::system_clock::now();
    demo_pricing_results.push_back(pricing_result);
    
    // Store demo data
    latest_market_data_ = demo_market_data;
    latest_pricing_results_ = demo_pricing_results;
    latest_opportunities_ = demo_opportunities;
}

HttpResponse DashboardApp::handleStaticFiles(const HttpRequest& request) {
    // Serve an enhanced HTML dashboard with real-time updates
    std::string html = "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "    <title>Arbitrage Engine Dashboard</title>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <style>\n"
        "        body { font-family: 'Segoe UI', Arial, sans-serif; margin: 0; padding: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; }\n"
        "        .container { max-width: 1200px; margin: 0 auto; }\n"
        "        .header { text-align: center; color: white; margin-bottom: 30px; }\n"
        "        .header h1 { font-size: 2.5rem; margin-bottom: 10px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }\n"
        "        .header p { font-size: 1.2rem; opacity: 0.9; }\n"
        "        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }\n"
        "        .card { background: rgba(255,255,255,0.95); border-radius: 12px; padding: 20px; box-shadow: 0 8px 32px rgba(31, 38, 135, 0.37); backdrop-filter: blur(4px); border: 1px solid rgba(255,255,255,0.18); }\n"
        "        .card h3 { margin-top: 0; color: #333; border-bottom: 2px solid #667eea; padding-bottom: 10px; }\n"
        "        .status { padding: 8px 16px; border-radius: 6px; font-weight: bold; display: inline-block; }\n"
        "        .status.running { background-color: #4CAF50; color: white; }\n"
        "        .data-item { margin: 10px 0; padding: 10px; background: #f8f9fa; border-radius: 6px; }\n"
        "        .price { font-size: 1.2rem; font-weight: bold; color: #2196F3; }\n"
        "        .timestamp { font-size: 0.8rem; color: #666; }\n"
        "        .loading { text-align: center; color: #666; font-style: italic; }\n"
        "        .error { color: #f44336; background: #ffebee; padding: 10px; border-radius: 6px; }\n"
        "        .refresh-indicator { position: fixed; top: 20px; right: 20px; background: rgba(76, 175, 80, 0.9); color: white; padding: 10px 15px; border-radius: 20px; font-size: 0.9rem; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <div class=\"container\">\n"
        "        <div class=\"header\">\n"
        "            <h1>🚀 Synthetic Arbitrage Detection Engine</h1>\n"
        "            <p>Phase 9 Implementation - Real-time Dashboard</p>\n"
        "        </div>\n"
        "        \n"
        "        <div class=\"refresh-indicator\" id=\"refreshIndicator\" style=\"display: none;\">🔄 Updating...</div>\n"
        "        \n"
        "        <div class=\"grid\">\n"
        "            <div class=\"card\">\n"
        "                <h3>📊 System Status</h3>\n"
        "                <div id=\"systemStatus\" class=\"loading\">Loading...</div>\n"
        "            </div>\n"
        "            \n"
        "            <div class=\"card\">\n"
        "                <h3>💹 Market Data</h3>\n"
        "                <div id=\"marketData\" class=\"loading\">Loading market data...</div>\n"
        "            </div>\n"
        "            \n"
        "            <div class=\"card\">\n"
        "                <h3>⚡ Pricing Results</h3>\n"
        "                <div id=\"pricingResults\" class=\"loading\">Loading pricing data...</div>\n"
        "            </div>\n"
        "            \n"
        "            <div class=\"card\">\n"
        "                <h3>🎯 Arbitrage Opportunities</h3>\n"
        "                <div id=\"opportunities\" class=\"loading\">Scanning for opportunities...</div>\n"
        "            </div>\n"
        "            \n"
        "            <div class=\"card\">\n"
        "                <h3>📈 Performance Metrics</h3>\n"
        "                <div id=\"performance\" class=\"loading\">Loading performance data...</div>\n"
        "            </div>\n"
        "            \n"
        "            <div class=\"card\">\n"
        "                <h3>⚠️ Risk Metrics</h3>\n"
        "                <div id=\"riskMetrics\" class=\"loading\">Loading risk data...</div>\n"
        "            </div>\n"
        "        </div>\n"
        "        \n"
        "        <div class=\"card\" style=\"margin-top: 20px; text-align: center; color: #666;\">\n"
        "            <p>🔗 API Endpoints: \n"
        "                <a href=\"/api/status\">Status</a> | \n"
        "                <a href=\"/api/market-data\">Market Data</a> | \n"
        "                <a href=\"/api/pricing-results\">Pricing</a> | \n"
        "                <a href=\"/api/opportunities\">Opportunities</a>\n"
        "            </p>\n"
        "            <p>Last Updated: <span id=\"lastUpdate\">-</span></p>\n"
        "        </div>\n"
        "    </div>\n"
        "    \n"
        "    <script>\n"
        "        // Dashboard JavaScript for real-time updates\n"
        "        let isUpdating = false;\n"
        "        \n"
        "        async function fetchJson(url) {\n"
        "            try {\n"
        "                const response = await fetch(url);\n"
        "                if (!response.ok) throw new Error(`HTTP ${response.status}`);\n"
        "                return await response.json();\n"
        "            } catch (error) {\n"
        "                console.error(`Error fetching ${url}:`, error);\n"
        "                return null;\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        function formatTimestamp(timestamp) {\n"
        "            return new Date(timestamp).toLocaleTimeString();\n"
        "        }\n"
        "        \n"
        "        function showError(elementId, message) {\n"
        "            document.getElementById(elementId).innerHTML = `<div class=\"error\">${message}</div>`;\n"
        "        }\n"
        "        \n"
        "        async function updateSystemStatus() {\n"
        "            const data = await fetchJson('/api/status');\n"
        "            const element = document.getElementById('systemStatus');\n"
        "            \n"
        "            if (data) {\n"
        "                element.innerHTML = `\n"
        "                    <div class=\"status ${data.status}\">${data.status.toUpperCase()}</div>\n"
        "                    <div class=\"data-item\">\n"
        "                        <strong>Version:</strong> ${data.version}<br>\n"
        "                        <strong>Uptime:</strong> ${data.uptime}s<br>\n"
        "                        <strong>Components:</strong><br>\n"
        "                        • Pricing Engine: ${data.components.pricing_engine}<br>\n"
        "                        • Market Data: ${data.components.market_data}<br>\n"
        "                        • Arbitrage Detector: ${data.components.arbitrage_detector}\n"
        "                    </div>\n"
        "                `;\n"
        "            } else {\n"
        "                showError('systemStatus', 'Failed to load system status');\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        async function updateMarketData() {\n"
        "            const data = await fetchJson('/api/market-data');\n"
        "            const element = document.getElementById('marketData');\n"
        "            \n"
        "            if (data && Array.isArray(data)) {\n"
        "                if (data.length === 0) {\n"
        "                    element.innerHTML = '<p>No market data available</p>';\n"
        "                    return;\n"
        "                }\n"
        "                \n"
        "                let html = '';\n"
        "                data.forEach(item => {\n"
        "                    html += `\n"
        "                        <div class=\"data-item\">\n"
        "                            <strong>${item.symbol}</strong> (${item.exchange})<br>\n"
        "                            <div class=\"price\">Bid: $${item.bid} | Ask: $${item.ask}</div>\n"
        "                            <div>Last: $${item.last} | Volume: ${item.volume}</div>\n"
        "                            <div class=\"timestamp\">${formatTimestamp(item.timestamp)}</div>\n"
        "                        </div>\n"
        "                    `;\n"
        "                });\n"
        "                element.innerHTML = html;\n"
        "            } else {\n"
        "                showError('marketData', 'Failed to load market data');\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        async function updatePricingResults() {\n"
        "            const data = await fetchJson('/api/pricing-results');\n"
        "            const element = document.getElementById('pricingResults');\n"
        "            \n"
        "            if (data && Array.isArray(data)) {\n"
        "                if (data.length === 0) {\n"
        "                    element.innerHTML = '<p>No pricing results available</p>';\n"
        "                    return;\n"
        "                }\n"
        "                \n"
        "                let html = '';\n"
        "                data.forEach(item => {\n"
        "                    html += `\n"
        "                        <div class=\"data-item\">\n"
        "                            <strong>${item.instrument_id}</strong><br>\n"
        "                            <div class=\"price\">Synthetic Price: $${item.synthetic_price}</div>\n"
        "                            <div>Model: ${item.model_name} | Confidence: ${(item.confidence * 100).toFixed(1)}%</div>\n"
        "                            <div>Calc Time: ${item.calculation_time_ms}ms</div>\n"
        "                            <div class=\"timestamp\">${formatTimestamp(item.timestamp)}</div>\n"
        "                        </div>\n"
        "                    `;\n"
        "                });\n"
        "                element.innerHTML = html;\n"
        "            } else {\n"
        "                showError('pricingResults', 'Failed to load pricing results');\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        async function updateOpportunities() {\n"
        "            const data = await fetchJson('/api/opportunities');\n"
        "            const element = document.getElementById('opportunities');\n"
        "            \n"
        "            if (data && Array.isArray(data)) {\n"
        "                if (data.length === 0) {\n"
        "                    element.innerHTML = '<p>No arbitrage opportunities detected</p>';\n"
        "                } else {\n"
        "                    let html = '';\n"
        "                    data.forEach(item => {\n"
        "                        html += `\n"
        "                            <div class=\"data-item\">\n"
        "                                <strong>${item.instrument}</strong><br>\n"
        "                                <div class=\"price\">Expected Return: ${item.expected_return}%</div>\n"
        "                                <div>Risk Score: ${item.risk_score}</div>\n"
        "                                <div class=\"timestamp\">${formatTimestamp(item.timestamp)}</div>\n"
        "                            </div>\n"
        "                        `;\n"
        "                    });\n"
        "                    element.innerHTML = html;\n"
        "                }\n"
        "            } else {\n"
        "                showError('opportunities', 'Failed to load opportunities');\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        async function updatePerformance() {\n"
        "            const data = await fetchJson('/api/performance');\n"
        "            const element = document.getElementById('performance');\n"
        "            \n"
        "            if (data) {\n"
        "                element.innerHTML = `\n"
        "                    <div class=\"data-item\">\n"
        "                        <strong>Total Return:</strong> ${data.total_return}%<br>\n"
        "                        <strong>Sharpe Ratio:</strong> ${data.sharpe_ratio}<br>\n"
        "                        <strong>Max Drawdown:</strong> ${data.max_drawdown}%<br>\n"
        "                        <strong>Win Rate:</strong> ${data.win_rate}%\n"
        "                    </div>\n"
        "                `;\n"
        "            } else {\n"
        "                showError('performance', 'Failed to load performance data');\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        async function updateRiskMetrics() {\n"
        "            const data = await fetchJson('/api/risk');\n"
        "            const element = document.getElementById('riskMetrics');\n"
        "            \n"
        "            if (data) {\n"
        "                element.innerHTML = `\n"
        "                    <div class=\"data-item\">\n"
        "                        <strong>VaR (95%):</strong> ${data.var_95}%<br>\n"
        "                        <strong>Expected Shortfall:</strong> ${data.expected_shortfall}%<br>\n"
        "                        <strong>Portfolio Beta:</strong> ${data.portfolio_beta}<br>\n"
        "                        <strong>Risk Score:</strong> ${data.risk_score}/100\n"
        "                    </div>\n"
        "                `;\n"
        "            } else {\n"
        "                showError('riskMetrics', 'Failed to load risk metrics');\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        async function updateAll() {\n"
        "            if (isUpdating) return;\n"
        "            \n"
        "            isUpdating = true;\n"
        "            document.getElementById('refreshIndicator').style.display = 'block';\n"
        "            \n"
        "            try {\n"
        "                await Promise.all([\n"
        "                    updateSystemStatus(),\n"
        "                    updateMarketData(),\n"
        "                    updatePricingResults(),\n"
        "                    updateOpportunities(),\n"
        "                    updatePerformance(),\n"
        "                    updateRiskMetrics()\n"
        "                ]);\n"
        "                \n"
        "                document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();\n"
        "            } catch (error) {\n"
        "                console.error('Error updating dashboard:', error);\n"
        "            } finally {\n"
        "                isUpdating = false;\n"
        "                document.getElementById('refreshIndicator').style.display = 'none';\n"
        "            }\n"
        "        }\n"
        "        \n"
        "        // Initialize dashboard\n"
        "        document.addEventListener('DOMContentLoaded', function() {\n"
        "            updateAll();\n"
        "            \n"
        "            // Auto-refresh every 5 seconds\n"
        "            setInterval(updateAll, 5000);\n"
        "        });\n"
        "    </script>\n"
        "</body>\n"
        "</html>";
    
    HttpResponse response;
    response.status_code = 200;
    response.content_type = "text/html";
    response.body = html;
    return response;
}

HttpResponse DashboardApp::handleApiStatus(const HttpRequest& request) {
    nlohmann::json status = {
        {"status", "running"},
        {"uptime", 1.5},
        {"version", "1.0.0"},
        {"components", {
            {"pricing_engine", "active"},
            {"market_data", "streaming"},
            {"arbitrage_detector", "scanning"}
        }}
    };
    return createJsonResponse(status);
}

HttpResponse DashboardApp::handleApiMarketData(const HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    nlohmann::json market_data = nlohmann::json::array();
    
    for (const auto& point : latest_market_data_) {
        market_data.push_back(data_exporter_->serializeMarketDataPoint(point));
    }
    
    return createJsonResponse(market_data);
}

HttpResponse DashboardApp::handleApiPricingResults(const HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    nlohmann::json pricing_results = nlohmann::json::array();
    
    for (const auto& result : latest_pricing_results_) {
        pricing_results.push_back(data_exporter_->serializePricingResult(result));
    }
    
    return createJsonResponse(pricing_results);
}

HttpResponse DashboardApp::handleApiOpportunities(const HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    nlohmann::json opportunities = nlohmann::json::array();
    
    for (const auto& opportunity : latest_opportunities_) {
        opportunities.push_back(data_exporter_->serializeArbitrageOpportunity(opportunity));
    }
    
    return createJsonResponse(opportunities);
}

HttpResponse DashboardApp::handleApiPerformance(const HttpRequest& request) {
    nlohmann::json performance = {
        {"latency", 2.5},
        {"throughput", 1500},
        {"memory_usage", 256},
        {"cpu_usage", 15.3}
    };
    return createJsonResponse(performance);
}

HttpResponse DashboardApp::handleApiRisk(const HttpRequest& request) {
    nlohmann::json risk = {
        {"var", 2.5},
        {"sharpe", 1.8},
        {"max_drawdown", 5.2},
        {"volatility", 18.5}
    };
    return createJsonResponse(risk);
}

HttpResponse DashboardApp::createJsonResponse(const nlohmann::json& data) const {
    HttpResponse response;
    response.status_code = 200;
    response.content_type = "application/json";
    response.body = data.dump(2);
    return response;
}

HttpResponse DashboardApp::createErrorResponse(int status_code, const std::string& message) const {
    HttpResponse response;
    response.status_code = status_code;
    response.content_type = "application/json";
    response.body = nlohmann::json{{"error", message}}.dump();
    return response;
}

bool DashboardApp::isRunning() const {
    return running_ && http_server_ && http_server_->isRunning();
}

std::string DashboardApp::getDashboardUrl() const {
    return "http://localhost:" + std::to_string(port_);
}

void DashboardApp::updateMarketData(const std::vector<data::MarketDataPoint>& data) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_market_data_ = data;
    utils::Logger::debug("Updated market data with " + std::to_string(data.size()) + " points");
}

void DashboardApp::updatePricingResults(const std::vector<core::PricingResult>& results) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_pricing_results_ = results;
    utils::Logger::debug("Updated pricing results with " + std::to_string(results.size()) + " results");
}

void DashboardApp::updateArbitrageOpportunities(const std::vector<core::ArbitrageOpportunity>& opportunities) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_opportunities_ = opportunities;
    utils::Logger::debug("Updated arbitrage opportunities with " + std::to_string(opportunities.size()) + " opportunities");
}

} // namespace ui
} // namespace arbitrage
