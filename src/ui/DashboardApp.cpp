#include "DashboardApp.hpp"
#include "../utils/Logger.hpp"
#include <sstream>
#include <random>
#include <chrono>

namespace arbitrage {
namespace ui {

DashboardApp::DashboardApp(int port) 
    : port_(port), running_(false), arbitrage_running_(false) {
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

void DashboardApp::initializeArbitrageEngine(std::shared_ptr<core::ArbitrageEngine> arbitrage_engine) {
    arbitrage_engine_ = arbitrage_engine;
    utils::Logger::info("Dashboard initialized with arbitrage engine");
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
    stopArbitrageDetection();
    
    if (update_thread_.joinable()) {
        update_thread_.join();
    }
    if (arbitrage_thread_.joinable()) {
        arbitrage_thread_.join();
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
    http_server_->addRoute("GET", "/api/market-data/filtered", 
        [this](const HttpRequest& req) { return handleApiFilteredMarketData(req); });
    http_server_->addRoute("GET", "/api/pricing-results", 
        [this](const HttpRequest& req) { return handleApiPricingResults(req); });
    http_server_->addRoute("GET", "/api/pricing-results/filtered", 
        [this](const HttpRequest& req) { return handleApiFilteredPricingResults(req); });
    http_server_->addRoute("GET", "/api/opportunities", 
        [this](const HttpRequest& req) { return handleApiOpportunities(req); });
    http_server_->addRoute("GET", "/api/opportunities/extended", 
        [this](const HttpRequest& req) { return handleApiExtendedOpportunities(req); });
    http_server_->addRoute("GET", "/api/arbitrage/metrics", 
        [this](const HttpRequest& req) { return handleApiArbitrageMetrics(req); });
    http_server_->addRoute("POST", "/api/arbitrage/control", 
        [this](const HttpRequest& req) { return handleApiArbitrageControl(req); });
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
}

void ui::DashboardApp::runUpdateLoop() {
    while (running_) {
        try {
            updateDemoData();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } catch (const std::exception& e) {
            utils::Logger::error("Error in update loop: " + std::string(e.what()));
        }
    }
}

void ui::DashboardApp::updateDemoData() {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Random number generators for realistic price movements
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<double> price_noise(0.0, 0.5);
    static std::uniform_real_distribution<double> volume_dist(100.0, 2000.0);
    static std::normal_distribution<double> funding_noise(0.0, 0.00005);
    static std::uniform_real_distribution<double> exchange_spread(-50.0, 50.0);
    static std::uniform_real_distribution<double> profit_dist(0.001, 0.005);
    static std::uniform_real_distribution<double> capital_dist(1000.0, 50000.0);
    static std::uniform_real_distribution<double> risk_dist(0.1, 0.4);
    static std::uniform_real_distribution<double> confidence_dist(0.7, 0.95);
    static std::uniform_int_distribution<int> exchange_dist(0, 2);
    
    // Base prices (shared across exchanges with small variations)
    static double btc_base = 50000.0;
    static double eth_base = 3000.0;
    
    // Update base prices with small random walk
    btc_base += price_noise(gen);
    eth_base += price_noise(gen) * 0.1;
    
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    
    // List of exchanges to generate data for
    std::vector<std::pair<std::string, std::string>> exchanges = {
        {"binance", "Binance"},
        {"okx", "OKX"},
        {"bybit", "Bybit"}
    };
    
    // Clear previous data
    std::vector<data::MarketDataPoint> demo_market_data;
    std::vector<core::PricingResult> demo_pricing_results;
    std::vector<core::ArbitrageOpportunity> demo_opportunities;
    
    // Generate data for each exchange
    for (const auto& [exchange_str, exchange_name] : exchanges) {
        // Small price variations between exchanges to simulate real market conditions
        double btc_variation = exchange_spread(gen);
        double eth_variation = exchange_spread(gen) * 0.1;
        
        // BTC spot data
        data::MarketDataPoint btc_spot;
        btc_spot.symbol = "BTCUSDT";
        btc_spot.exchange = exchange_str;
        btc_spot.timestamp = timestamp;
        btc_spot.bid = btc_base + btc_variation - 0.25;
        btc_spot.ask = btc_base + btc_variation + 0.25;
        btc_spot.last = btc_base + btc_variation;
        btc_spot.volume = volume_dist(gen);
        btc_spot.funding_rate = 0.0001 + funding_noise(gen);
        demo_market_data.push_back(btc_spot);
        
        // BTC perpetual data
        data::MarketDataPoint btc_perp = btc_spot;
        btc_perp.symbol = "BTCUSDT-PERP";
        btc_perp.bid += 1.5;
        btc_perp.ask += 1.5;
        btc_perp.last += 1.5;
        demo_market_data.push_back(btc_perp);
        
        // ETH spot data
        data::MarketDataPoint eth_spot;
        eth_spot.symbol = "ETHUSDT";
        eth_spot.exchange = exchange_str;
        eth_spot.timestamp = timestamp;
        eth_spot.bid = eth_base + eth_variation - 0.15;
        eth_spot.ask = eth_base + eth_variation + 0.15;
        eth_spot.last = eth_base + eth_variation;
        eth_spot.volume = volume_dist(gen);
        eth_spot.funding_rate = 0.00008 + funding_noise(gen);
        demo_market_data.push_back(eth_spot);

        // ETH perpetual data
        data::MarketDataPoint eth_perp = eth_spot;
        eth_perp.symbol = "ETHUSDT-PERP";
        eth_perp.bid += 0.8;
        eth_perp.ask += 0.8;
        eth_perp.last += 0.8;
        demo_market_data.push_back(eth_perp);
        
        // Generate pricing results for this exchange
        for (const auto& md : {btc_spot, btc_perp, eth_spot, eth_perp}) {
            core::PricingResult result;
            result.instrument_id = md.symbol + "@" + exchange_str;
            result.synthetic_price = md.last;
            result.confidence = confidence_dist(gen);
            result.model_name = (md.symbol.find("PERP") != std::string::npos) ? 
                "Perpetual Swap" : "Spot";
            result.components.base_price = md.last;
            if (md.symbol.find("PERP") != std::string::npos) {
                result.components.funding_adjustment = md.funding_rate * md.last * 8.0;
            }
            result.success = true;
            result.calculation_time_ms = 0.1 + (gen() % 10) * 0.05;
            result.timestamp = std::chrono::system_clock::time_point(md.timestamp);
            demo_pricing_results.push_back(result);
        }
    }
    
    // Generate cross-exchange arbitrage opportunities
    if (gen() % 3 == 0) {  // 33% chance
        core::ArbitrageOpportunity opp;
        opp.underlying_symbol = (gen() % 2 == 0) ? "BTCUSDT" : "ETHUSDT";
        
        // Select two different exchanges for the arbitrage
        int exchange1_idx = exchange_dist(gen);
        int exchange2_idx = exchange_dist(gen);
        while (exchange2_idx == exchange1_idx) {
            exchange2_idx = exchange_dist(gen);
        }
        
        std::string exchange1_str = exchanges[exchange1_idx].first;
        std::string exchange2_str = exchanges[exchange2_idx].first;
        
        // Create cross-exchange arbitrage opportunity
        core::ArbitrageOpportunity::Leg leg1;
        leg1.symbol = opp.underlying_symbol;
        leg1.type = core::InstrumentType::SPOT;
        // Convert string to Exchange enum - simplified approach
        leg1.exchange = (exchange1_str == "binance") ? data::Exchange::BINANCE :
                       (exchange1_str == "okx") ? data::Exchange::OKX : data::Exchange::BYBIT;
        
        double base_price = (opp.underlying_symbol == "BTCUSDT") ? btc_base : eth_base;
        leg1.price = base_price + exchange_spread(gen);
        leg1.synthetic_price = base_price;
        leg1.deviation = (leg1.price - leg1.synthetic_price) / leg1.synthetic_price;
        leg1.action = "BUY";
        
        core::ArbitrageOpportunity::Leg leg2;
        leg2.symbol = opp.underlying_symbol;
        leg2.type = core::InstrumentType::SPOT;
        leg2.exchange = (exchange2_str == "binance") ? data::Exchange::BINANCE :
                       (exchange2_str == "okx") ? data::Exchange::OKX : data::Exchange::BYBIT;
        leg2.price = base_price + exchange_spread(gen);
        leg2.synthetic_price = base_price;
        leg2.deviation = (leg2.price - leg2.synthetic_price) / leg2.synthetic_price;
        leg2.action = "SELL";
        
        opp.legs.push_back(leg1);
        opp.legs.push_back(leg2);
        
        opp.expected_profit_pct = profit_dist(gen);
        opp.required_capital = capital_dist(gen);
        opp.risk_score = risk_dist(gen);
        opp.confidence = confidence_dist(gen);
        opp.detected_at = std::chrono::system_clock::now();
        
        demo_opportunities.push_back(opp);
    }
    
    // Store demo data
    latest_market_data_ = demo_market_data;
    latest_pricing_results_ = demo_pricing_results;
    latest_opportunities_ = demo_opportunities;
}

ui::HttpResponse ui::DashboardApp::handleStaticFiles(const ui::HttpRequest& request) {
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
        "        .exchange-selector { text-align: center; margin-bottom: 30px; }\n"
        "        .exchange-selector label { color: white; font-size: 1.1rem; margin-right: 15px; }\n"
        "        .exchange-selector select { padding: 10px 15px; border-radius: 8px; border: none; background: rgba(255,255,255,0.95); font-size: 1rem; min-width: 150px; cursor: pointer; }\n"
        "        .exchange-selector select:focus { outline: none; box-shadow: 0 0 0 3px rgba(103, 126, 234, 0.3); }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <div class=\"container\">\n"
        "        <div class=\"header\">\n"
        "            <h1>🚀 Synthetic Arbitrage Detection Engine</h1>\n"
        "            <p>Phase 9 Implementation - Real-time Dashboard</p>\n"
        "        </div>\n"
        "        \n"
        "        <div class=\"exchange-selector\">\n"
        "            <label for=\"exchangeSelect\">📈 Select Exchange:</label>\n"
        "            <select id=\"exchangeSelect\" onchange=\"onExchangeChange()\">\n"
        "                <option value=\"all\">All Exchanges</option>\n"
        "                <option value=\"binance\">🟡 Binance</option>\n"
        "                <option value=\"okx\">🔵 OKX</option>\n"
        "                <option value=\"bybit\">🟠 Bybit</option>\n"
        "            </select>\n"
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
        "                <h3 id=\"marketDataTitle\">💹 Market Data</h3>\n"
        "                <div id=\"marketData\" class=\"loading\">Loading market data...</div>\n"
        "            </div>\n"
        "            \n"
        "            <div class=\"card\">\n"
        "                <h3 id=\"pricingResultsTitle\">⚡ Pricing Results</h3>\n"
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
        "        let selectedExchange = 'all';\n"
        "        \n"
        "        function onExchangeChange() {\n"
        "            const select = document.getElementById('exchangeSelect');\n"
        "            selectedExchange = select.value;\n"
        "            console.log('Exchange changed to:', selectedExchange);\n"
        "            \n"
        "            // Update titles based on selected exchange\n"
        "            const exchangeText = selectedExchange === 'all' ? 'All Exchanges' : selectedExchange.toUpperCase();\n"
        "            const exchangeIcon = selectedExchange === 'binance' ? '🟡' : \n"
        "                               selectedExchange === 'okx' ? '🔵' : \n"
        "                               selectedExchange === 'bybit' ? '🟠' : '🌐';\n"
        "            \n"
        "            document.getElementById('marketDataTitle').textContent = `💹 Market Data - ${exchangeIcon} ${exchangeText}`;\n"
        "            document.getElementById('pricingResultsTitle').textContent = `⚡ Pricing Results - ${exchangeIcon} ${exchangeText}`;\n"
        "            \n"
        "            // Immediately update all data with new filter\n"
        "            updateAllData();\n"
        "        }\n"
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
        "            const url = selectedExchange === 'all' ? '/api/market-data' : `/api/market-data?exchange=${selectedExchange}`;\n"
        "            const data = await fetchJson(url);\n"
        "            const element = document.getElementById('marketData');\n"
        "            \n"
        "            if (data && Array.isArray(data)) {\n"
        "                if (data.length === 0) {\n"
        "                    const exchangeText = selectedExchange === 'all' ? 'any exchange' : selectedExchange.toUpperCase();\n"
        "                    element.innerHTML = `<p>No market data available for ${exchangeText}</p>`;\n"
        "                    return;\n"
        "                }\n"
        "                \n"
        "                let html = '';\n"
        "                data.forEach(item => {\n"
        "                    const exchangeIcon = item.exchange === 'binance' ? '🟡' : \n"
        "                                        item.exchange === 'okx' ? '🔵' : \n"
        "                                        item.exchange === 'bybit' ? '🟠' : '⚪';\n"
        "                    html += `\n"
        "                        <div class=\"data-item\">\n"
        "                            <strong>${item.symbol}</strong> ${exchangeIcon} (${item.exchange.toUpperCase()})<br>\n"
        "                            <div class=\"price\">Bid: $${parseFloat(item.bid).toFixed(2)} | Ask: $${parseFloat(item.ask).toFixed(2)}</div>\n"
        "                            <div>Last: $${parseFloat(item.last).toFixed(2)} | Volume: ${parseFloat(item.volume).toFixed(2)}</div>\n"
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
        "            const url = selectedExchange === 'all' ? '/api/pricing-results' : `/api/pricing-results?exchange=${selectedExchange}`;\n"
        "            const data = await fetchJson(url);\n"
        "            const element = document.getElementById('pricingResults');\n"
        "            \n"
        "            if (data && Array.isArray(data)) {\n"
        "                if (data.length === 0) {\n"
        "                    const exchangeText = selectedExchange === 'all' ? 'any exchange' : selectedExchange.toUpperCase();\n"
        "                    element.innerHTML = `<p>No pricing results available for ${exchangeText}</p>`;\n"
        "                    return;\n"
        "                }\n"
        "                \n"
        "                let html = '';\n"
        "                data.forEach(item => {\n"
        "                    // Extract exchange from instrument_id (format: SYMBOL@exchange)\n"
        "                    const parts = item.instrument_id.split('@');\n"
        "                    const symbol = parts[0] || item.instrument_id;\n"
        "                    const exchange = parts[1] || 'unknown';\n"
        "                    const exchangeIcon = exchange === 'binance' ? '🟡' : \n"
        "                                        exchange === 'okx' ? '🔵' : \n"
        "                                        exchange === 'bybit' ? '🟠' : '⚪';\n"
        "                    html += `\n"
        "                        <div class=\"data-item\">\n"
        "                            <strong>${symbol}</strong> ${exchangeIcon} (${exchange.toUpperCase()})<br>\n"
        "                            <div class=\"price\">Synthetic Price: $${parseFloat(item.synthetic_price).toFixed(2)}</div>\n"
        "                            <div>Model: ${item.model_name} | Confidence: ${(item.confidence * 100).toFixed(1)}%</div>\n"
        "                            <div>Calc Time: ${parseFloat(item.calculation_time_ms).toFixed(2)}ms</div>\n"
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
        "        async function updateAllData() {\n"
        "            // Force immediate update of all exchange-dependent data\n"
        "            await Promise.all([\n"
        "                updateMarketData(),\n"
        "                updatePricingResults()\n"
        "            ]);\n"
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

ui::HttpResponse ui::DashboardApp::handleApiStatus(const ui::HttpRequest& request) {
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

ui::HttpResponse ui::DashboardApp::handleApiMarketData(const ui::HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Get exchange filter from query parameters
    std::string exchange_filter = "all";
    auto it = request.query_params.find("exchange");
    if (it != request.query_params.end()) {
        exchange_filter = it->second;
    }
    
    nlohmann::json market_data = nlohmann::json::array();
    
    for (const auto& point : latest_market_data_) {
        // Filter by exchange if specified
        if (exchange_filter == "all" || point.exchange == exchange_filter) {
            market_data.push_back(data_exporter_->serializeMarketDataPoint(point));
        }
    }
    
    return createJsonResponse(market_data);
}

ui::HttpResponse ui::DashboardApp::handleApiFilteredMarketData(const ui::HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Get exchange filter from query parameters
    std::string exchange_filter = "all";
    auto it = request.query_params.find("exchange");
    if (it != request.query_params.end()) {
        exchange_filter = it->second;
    }
    
    nlohmann::json market_data = nlohmann::json::array();
    
    for (const auto& point : latest_market_data_) {
        // Filter by exchange if specified
        if (exchange_filter == "all" || point.exchange == exchange_filter) {
            market_data.push_back(data_exporter_->serializeMarketDataPoint(point));
        }
    }
    
    return createJsonResponse(market_data);
}

ui::HttpResponse ui::DashboardApp::handleApiPricingResults(const ui::HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Get exchange filter from query parameters
    std::string exchange_filter = "all";
    auto it = request.query_params.find("exchange");
    if (it != request.query_params.end()) {
        exchange_filter = it->second;
    }
    
    nlohmann::json pricing_results = nlohmann::json::array();
    
    for (const auto& result : latest_pricing_results_) {
        // Filter by exchange - extract exchange from instrument_id (format: "SYMBOL@exchange")
        std::string instrument_exchange = "";
        size_t at_pos = result.instrument_id.find("@");
        if (at_pos != std::string::npos) {
            instrument_exchange = result.instrument_id.substr(at_pos + 1);
        }
        
        if (exchange_filter == "all" || instrument_exchange == exchange_filter) {
            pricing_results.push_back(data_exporter_->serializePricingResult(result));
        }
    }
    
    return createJsonResponse(pricing_results);
}

ui::HttpResponse ui::DashboardApp::handleApiFilteredPricingResults(const ui::HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    
    // Get exchange filter from query parameters
    std::string exchange_filter = "all";
    auto it = request.query_params.find("exchange");
    if (it != request.query_params.end()) {
        exchange_filter = it->second;
    }
    
    nlohmann::json pricing_results = nlohmann::json::array();
    
    for (const auto& result : latest_pricing_results_) {
        // Filter by exchange - extract exchange from instrument_id (format: "SYMBOL@exchange")
        std::string instrument_exchange = "";
        size_t at_pos = result.instrument_id.find("@");
        if (at_pos != std::string::npos) {
            instrument_exchange = result.instrument_id.substr(at_pos + 1);
        }
        
        if (exchange_filter == "all" || instrument_exchange == exchange_filter) {
            pricing_results.push_back(data_exporter_->serializePricingResult(result));
        }
    }
    
    return createJsonResponse(pricing_results);
}

ui::HttpResponse ui::DashboardApp::handleApiOpportunities(const ui::HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    nlohmann::json opportunities = nlohmann::json::array();
    
    for (const auto& opportunity : latest_opportunities_) {
        opportunities.push_back(data_exporter_->serializeArbitrageOpportunity(opportunity));
    }
    
    return createJsonResponse(opportunities);
}

ui::HttpResponse ui::DashboardApp::handleApiExtendedOpportunities(const ui::HttpRequest& request) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    nlohmann::json opportunities = nlohmann::json::array();
    
    for (const auto& opportunity : latest_extended_opportunities_) {
        nlohmann::json opp_json;
        opp_json["id"] = opportunity.id;
        opp_json["type"] = static_cast<int>(opportunity.strategy_type);
        opp_json["strategy_name"] = [&]() {
            switch(opportunity.strategy_type) {
                case core::ArbitrageOpportunityExtended::StrategyType::SPOT_PERP_ARBITRAGE: return "Spot-Perpetual";
                case core::ArbitrageOpportunityExtended::StrategyType::FUNDING_RATE_ARBITRAGE: return "Funding Rate";
                case core::ArbitrageOpportunityExtended::StrategyType::CROSS_EXCHANGE_ARBITRAGE: return "Cross-Exchange";
                case core::ArbitrageOpportunityExtended::StrategyType::BASIS_ARBITRAGE: return "Basis";
                case core::ArbitrageOpportunityExtended::StrategyType::VOLATILITY_ARBITRAGE: return "Volatility";
                case core::ArbitrageOpportunityExtended::StrategyType::STATISTICAL_ARBITRAGE: return "Statistical";
                default: return "Unknown";
            }
        }();
        opp_json["expected_profit_usd"] = opportunity.expected_profit_usd;
        opp_json["expected_profit_percent"] = opportunity.expected_profit_percent;
        opp_json["confidence_score"] = opportunity.confidence_score;
        opp_json["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        nlohmann::json legs = nlohmann::json::array();
        for (const auto& leg : opportunity.legs) {
            nlohmann::json leg_json;
            leg_json["exchange"] = leg.exchange;
            leg_json["instrument"] = leg.instrument;
            leg_json["action"] = leg.action;
            leg_json["quantity"] = leg.quantity;
            leg_json["price"] = leg.price;
            leg_json["weight"] = leg.weight;
            legs.push_back(leg_json);
        }
        opp_json["legs"] = legs;
        
        opportunities.push_back(opp_json);
    }
    
    return createJsonResponse(opportunities);
}

ui::HttpResponse ui::DashboardApp::handleApiArbitrageMetrics(const ui::HttpRequest& request) {
    auto metrics = getArbitrageMetrics();
    return createJsonResponse(metrics);
}

ui::HttpResponse ui::DashboardApp::handleApiArbitrageControl(const ui::HttpRequest& request) {
    try {
        if (request.method != "POST") {
            return createErrorResponse(405, "Method not allowed");
        }
        
        // Parse JSON body
        auto body = nlohmann::json::parse(request.body);
        std::string action = body.value("action", "");
        
        if (action == "start") {
            startArbitrageDetection();
            return createJsonResponse({{"status", "started"}, {"message", "Arbitrage detection started"}});
        } else if (action == "stop") {
            stopArbitrageDetection();
            return createJsonResponse({{"status", "stopped"}, {"message", "Arbitrage detection stopped"}});
        } else {
            return createErrorResponse(400, "Invalid action. Use 'start' or 'stop'");
        }
    } catch (const std::exception& e) {
        return createErrorResponse(400, "Invalid request: " + std::string(e.what()));
    }
}

ui::HttpResponse ui::DashboardApp::handleApiPerformance(const ui::HttpRequest& request) {
    nlohmann::json performance = {
        {"latency", 2.5},
        {"throughput", 1500},
        {"memory_usage", 256},
        {"cpu_usage", 15.3}
    };
    return createJsonResponse(performance);
}

ui::HttpResponse ui::DashboardApp::handleApiRisk(const ui::HttpRequest& request) {
    nlohmann::json risk = {
        {"var", 2.5},
        {"sharpe", 1.8},
        {"max_drawdown", 5.2},
        {"volatility", 18.5}
    };
    return createJsonResponse(risk);
}

ui::HttpResponse ui::DashboardApp::createJsonResponse(const nlohmann::json& data) const {
    HttpResponse response;
    response.status_code = 200;
    response.content_type = "application/json";
    response.body = data.dump(2);
    return response;
}

ui::HttpResponse ui::DashboardApp::createErrorResponse(int status_code, const std::string& message) const {
    HttpResponse response;
    response.status_code = status_code;
    response.content_type = "application/json";
    response.body = nlohmann::json{{"error", message}}.dump();
    return response;
}

bool ui::DashboardApp::isRunning() const {
    return running_ && http_server_ && http_server_->isRunning();
}

std::string ui::DashboardApp::getDashboardUrl() const {
    return "http://localhost:" + std::to_string(port_);
}

void ui::DashboardApp::updateMarketData(const std::vector<data::MarketDataPoint>& data) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_market_data_ = data;
    utils::Logger::debug("Updated market data with " + std::to_string(data.size()) + " points");
}

void ui::DashboardApp::updatePricingResults(const std::vector<core::PricingResult>& results) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_pricing_results_ = results;
    utils::Logger::debug("Updated pricing results with " + std::to_string(results.size()) + " results");
}

void ui::DashboardApp::updateArbitrageOpportunities(const std::vector<core::ArbitrageOpportunity>& opportunities) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_opportunities_ = opportunities;
    utils::Logger::debug("Updated arbitrage opportunities with " + std::to_string(opportunities.size()) + " opportunities");
}

void ui::DashboardApp::updateExtendedArbitrageOpportunities(const std::vector<core::ArbitrageOpportunityExtended>& opportunities) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    latest_extended_opportunities_ = opportunities;
    utils::Logger::debug("Updated extended arbitrage opportunities with " + std::to_string(opportunities.size()) + " opportunities");
}

void ui::DashboardApp::startArbitrageDetection() {
    if (!arbitrage_engine_) {
        utils::Logger::error("Cannot start arbitrage detection: ArbitrageEngine not initialized");
        return;
    }
    
    if (arbitrage_running_) {
        utils::Logger::warn("Arbitrage detection already running");
        return;
    }
    
    arbitrage_running_ = true;
    arbitrage_thread_ = std::thread([this]() { runArbitrageLoop(); });
    utils::Logger::info("Started arbitrage detection");
}

void ui::DashboardApp::stopArbitrageDetection() {
    if (arbitrage_running_) {
        arbitrage_running_ = false;
        if (arbitrage_thread_.joinable()) {
            arbitrage_thread_.join();
        }
        utils::Logger::info("Stopped arbitrage detection");
    }
}

void ui::DashboardApp::runArbitrageLoop() {
    if (!arbitrage_engine_) {
        utils::Logger::error("ArbitrageEngine not initialized for detection loop");
        return;
    }
    
    arbitrage_engine_->start();
    
    // Simple demo market data generator
    auto generateDemoMarketData = []() -> std::vector<data::MarketDataPoint> {
        std::vector<data::MarketDataPoint> data;
        
        // Create market data for all exchanges
        std::vector<std::pair<data::Exchange, std::string>> exchanges = {
            {data::Exchange::BINANCE, "binance"},
            {data::Exchange::OKX, "okx"},
            {data::Exchange::BYBIT, "bybit"}
        };
        
        std::vector<std::string> symbols = {"BTC-USDT", "ETH-USDT", "BTC-USDT-PERP", "ETH-USDT-PERP"};
        
        // Generate realistic price variations
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<double> price_variation(-10.0, 10.0);
        static std::uniform_real_distribution<double> volume_variation(500.0, 2000.0);
        
        // Base prices for different symbols
        std::map<std::string, double> base_prices = {
            {"BTC-USDT", 43500.0},
            {"ETH-USDT", 2300.0},
            {"BTC-USDT-PERP", 43502.0},
            {"ETH-USDT-PERP", 2301.0}
        };
        
        for (const auto& [exchange_enum, exchange_str] : exchanges) {
            for (const auto& symbol : symbols) {
                data::MarketDataPoint point;
                point.symbol = symbol;
                point.exchange = exchange_str;  // Use lowercase string for filtering
                
                double base_price = base_prices[symbol];
                double variation = price_variation(gen);
                
                point.bid = base_price + variation - 0.5;
                point.ask = base_price + variation + 0.5;
                point.last = base_price + variation;
                point.volume = volume_variation(gen);
                point.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                );
                
                data.push_back(point);
            }
        }
        
        return data;
    };
    
    while (arbitrage_running_) {
        try {
            // Generate sample market data and pricing results for demo
            auto sample_market_data = generateDemoMarketData();
            
            // Generate demo pricing results based on market data
            std::vector<core::PricingResult> sample_pricing_results;
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<double> confidence_dist(0.7, 0.95);
            
            for (const auto& market_point : sample_market_data) {
                core::PricingResult result;
                result.instrument_id = market_point.symbol + "@" + market_point.exchange;
                result.synthetic_price = market_point.last;
                result.confidence = confidence_dist(gen);
                result.model_name = (market_point.symbol.find("PERP") != std::string::npos) ? 
                    "Perpetual Swap" : "Spot";
                result.components.base_price = market_point.last;
                sample_pricing_results.push_back(result);
            }
            
            // Update the main dashboard data
            updateMarketData(sample_market_data);
            updatePricingResults(sample_pricing_results);
            
            // Trigger arbitrage detection cycle
            auto opportunities = arbitrage_engine_->detectOpportunities(sample_market_data, sample_pricing_results);
            updateExtendedArbitrageOpportunities(opportunities);
            
            // Sleep for a reasonable interval (1 second instead of 100ms to reduce spam)
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } catch (const std::exception& e) {
            utils::Logger::error("Error in arbitrage detection loop: " + std::string(e.what()));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    arbitrage_engine_->stop();
}

nlohmann::json ui::DashboardApp::getArbitrageMetrics() const {
    if (!arbitrage_engine_) {
        return nlohmann::json::object();
    }
    
    const auto& metrics = arbitrage_engine_->getPerformanceMetrics();
    nlohmann::json json_metrics;
    
    json_metrics["detection_cycles"] = metrics.detection_cycles.load();
    json_metrics["opportunities_detected"] = metrics.opportunities_detected.load();
    json_metrics["opportunities_validated"] = metrics.opportunities_validated.load();
    json_metrics["total_expected_profit_usd"] = metrics.total_expected_profit.load();
    json_metrics["average_detection_latency_ms"] = metrics.avg_detection_latency_ms.load();
    json_metrics["is_running"] = arbitrage_running_.load();
    
    return json_metrics;
}

} // namespace ui
