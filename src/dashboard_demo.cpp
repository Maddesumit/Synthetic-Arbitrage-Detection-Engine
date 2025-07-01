/**
 * @file dashboard_demo.cpp
 * @brief Phase 9 Dashboard Demo - Real-time Web UI
 */

#include "ui/DashboardApp.hpp"
#include "core/PricingEngine.hpp"
#include "core/ArbitrageEngine.hpp"
#include "core/MathUtils.hpp"
#include "data/MarketData.hpp"
#include "utils/ConfigManager.hpp"
#include "utils/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <csignal>
#include <atomic>

using namespace arbitrage;

// Global flag for graceful shutdown
std::atomic<bool> shutdown_requested(false);

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\n🛑 Shutdown signal received. Gracefully shutting down...\n";
    shutdown_requested = true;
}

/**
 * @brief Generate realistic demo market data for all exchanges
 */
std::vector<data::MarketDataPoint> generateDemoMarketData() {
    std::vector<data::MarketDataPoint> data;
    
    // Random number generators for realistic price movements
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<double> price_noise(0.0, 0.5);
    static std::uniform_real_distribution<double> volume_dist(100.0, 2000.0);
    static std::normal_distribution<double> funding_noise(0.0, 0.00005);
    static std::uniform_real_distribution<double> exchange_spread(-2.0, 2.0);
    
    // Base prices (shared across exchanges with small variations)
    static double btc_base = 50000.0;
    static double eth_base = 3000.0;
    
    // Update base prices with small random walk
    btc_base += price_noise(gen);
    eth_base += price_noise(gen) * 0.1;
    
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    
    // List of exchanges to generate data for
    std::vector<std::pair<data::Exchange, std::string>> exchanges = {
        {data::Exchange::BINANCE, "binance"},
        {data::Exchange::OKX, "okx"},
        {data::Exchange::BYBIT, "bybit"}
    };
    
    // Generate data for each exchange
    for (const auto& [exchange_enum, exchange_str] : exchanges) {
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
        data.push_back(btc_spot);
        
        // BTC perpetual data
        data::MarketDataPoint btc_perp = btc_spot;
        btc_perp.symbol = "BTCUSDT-PERP";
        btc_perp.bid += 1.5;
        btc_perp.ask += 1.5;
        btc_perp.last += 1.5;
        data.push_back(btc_perp);
        
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
        data.push_back(eth_spot);

        // ETH perpetual data
        data::MarketDataPoint eth_perp = eth_spot;
        eth_perp.symbol = "ETHUSDT-PERP";
        eth_perp.bid += 0.8;
        eth_perp.ask += 0.8;
        eth_perp.last += 0.8;
        data.push_back(eth_perp);
    }
    
    return data;
}

/**
 * @brief Generate demo arbitrage opportunities across multiple exchanges
 */
std::vector<core::ArbitrageOpportunity> generateDemoOpportunities() {
    std::vector<core::ArbitrageOpportunity> opportunities;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> profit_dist(0.001, 0.005);
    static std::uniform_real_distribution<double> capital_dist(1000.0, 50000.0);
    static std::uniform_real_distribution<double> risk_dist(0.1, 0.4);
    static std::uniform_real_distribution<double> confidence_dist(0.7, 0.95);
    static std::uniform_int_distribution<int> exchange_dist(0, 2);
    
    // List of exchanges for generating opportunities
    std::vector<data::Exchange> exchanges = {
        data::Exchange::BINANCE,
        data::Exchange::OKX,
        data::Exchange::BYBIT
    };
    
    // Occasionally generate opportunities (higher chance for demo purposes)
    if (gen() % 5 == 0) {  // 20% chance
        core::ArbitrageOpportunity opp;
        opp.underlying_symbol = (gen() % 2 == 0) ? "BTCUSDT" : "ETHUSDT";
        
        // Select two different exchanges for the arbitrage
        int exchange1_idx = exchange_dist(gen);
        int exchange2_idx = exchange_dist(gen);
        while (exchange2_idx == exchange1_idx) {
            exchange2_idx = exchange_dist(gen);
        }
        
        data::Exchange exchange1 = exchanges[exchange1_idx];
        data::Exchange exchange2 = exchanges[exchange2_idx];
        
        // Create legs for the cross-exchange arbitrage opportunity
        core::ArbitrageOpportunity::Leg leg1;
        leg1.symbol = opp.underlying_symbol;
        leg1.type = core::InstrumentType::SPOT;
        leg1.exchange = exchange1;
        
        double base_price = (opp.underlying_symbol == "BTCUSDT") ? 
            50000.0 + (gen() % 1000 - 500) : 
            3000.0 + (gen() % 100 - 50);
            
        leg1.price = base_price;
        leg1.synthetic_price = base_price * (1.0 + profit_dist(gen));
        leg1.deviation = (leg1.price - leg1.synthetic_price) / leg1.synthetic_price;
        leg1.action = "BUY";
        
        core::ArbitrageOpportunity::Leg leg2;
        leg2.symbol = opp.underlying_symbol;
        leg2.type = core::InstrumentType::SPOT;
        leg2.exchange = exchange2;
        leg2.price = base_price * (1.0 + profit_dist(gen));
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
        
        opportunities.push_back(opp);
    }
    
    return opportunities;
}

int main() {
    try {
        // Setup signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);  // Ctrl+C
        std::signal(SIGTERM, signalHandler); // Termination request
        
        std::cout << "\n🚀 Phase 9: Dashboard Demo - Real-time Web UI\n";
        std::cout << "===============================================\n\n";

        // Initialize logging system
        utils::Logger::initialize("logs/dashboard_demo.log", utils::Logger::Level::INFO, utils::Logger::Level::DEBUG);
        LOG_INFO("Dashboard Demo Started");
        
        // Load configuration
        utils::ConfigManager config("config/config.json");
        
        // Create market environment
        core::MarketEnvironment market_env;
        market_env.interest_rates["USD"] = 0.05;
        market_env.interest_rates["BTC"] = 0.0;
        market_env.interest_rates["ETH"] = 0.0;
        market_env.default_volatility = 0.3;

        // Initialize pricing engine
        auto pricing_engine = std::make_shared<core::PricingEngine>(market_env);
        
        // Register demo instruments for all exchanges
        std::vector<data::Exchange> exchanges = {
            data::Exchange::BINANCE,
            data::Exchange::OKX,
            data::Exchange::BYBIT
        };
        
        for (const auto& exchange : exchanges) {
            // Register BTC instruments
            core::InstrumentSpec btc_spot("BTCUSDT", core::InstrumentType::SPOT, exchange);
            pricing_engine->registerInstrument(btc_spot);
            
            core::InstrumentSpec btc_perp("BTCUSDT-PERP", core::InstrumentType::PERPETUAL_SWAP, exchange);
            pricing_engine->registerInstrument(btc_perp);
            
            // Register ETH instruments
            core::InstrumentSpec eth_spot("ETHUSDT", core::InstrumentType::SPOT, exchange);
            pricing_engine->registerInstrument(eth_spot);
            
            core::InstrumentSpec eth_perp("ETHUSDT-PERP", core::InstrumentType::PERPETUAL_SWAP, exchange);
            pricing_engine->registerInstrument(eth_perp);
        }

        // Create and initialize ArbitrageEngine
        core::ArbitrageConfig arb_config;
        arb_config.min_profit_threshold_usd = 10.0;        // $10 minimum profit for demo
        arb_config.min_profit_threshold_percent = 0.01;    // 0.01% minimum spread
        arb_config.min_confidence_score = 0.7;             // 70% confidence minimum
        arb_config.max_position_size_usd = 10000.0;        // $10k max position for demo
        
        auto arbitrage_engine = std::make_shared<core::ArbitrageEngine>(arb_config);

        // Create and initialize dashboard
        ui::DashboardApp dashboard(8080);
        dashboard.initialize(pricing_engine);
        dashboard.initializeArbitrageEngine(arbitrage_engine);
        
        std::cout << "Starting dashboard server...\n";
        std::cout << "Dashboard URL: " << dashboard.getDashboardUrl() << "\n\n";
        std::cout << "📊 Dashboard Features:\n";
        std::cout << "   • Real-time system status monitoring\n";
        std::cout << "   • Live market data visualization (Binance, OKX, Bybit)\n";
        std::cout << "   • Cross-exchange pricing results display\n";
        std::cout << "   • Multi-exchange arbitrage opportunity tracking\n";
        std::cout << "   • Integrated Phase 4 Arbitrage Detection Engine\n";
        std::cout << "   • Real-time arbitrage detection and metrics\n";
        std::cout << "   • Performance metrics dashboard\n";
        std::cout << "   • Risk metrics monitoring\n";
        std::cout << "   • Auto-refresh every 2 seconds\n\n";
        
        // Start dashboard in background
        dashboard.startAsync();
        
        // Wait for server to start
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        if (dashboard.isRunning()) {
            std::cout << "✅ Dashboard server started successfully!\n";
            std::cout << "   Open your browser and navigate to: " << dashboard.getDashboardUrl() << "\n\n";
            
            std::cout << "� Starting Phase 4 Arbitrage Detection Engine...\n";
            
            // Generate sample market data for arbitrage engine
            auto sample_data = generateDemoMarketData();
            arbitrage_engine->updateMarketData(sample_data);
            
            // Start arbitrage detection
            dashboard.startArbitrageDetection();
            
            std::cout << "✅ Arbitrage Detection Engine started!\n";
            std::cout << "   Dashboard APIs available at:\n";
            std::cout << "   • /api/arbitrage/metrics - Performance metrics\n";
            std::cout << "   • /api/opportunities/extended - Extended arbitrage opportunities\n";
            std::cout << "   • POST /api/arbitrage/control - Start/stop arbitrage detection\n\n";
            
            std::cout << "�🔄 Dashboard is running with multi-exchange demo data simulation...\n";
            std::cout << "   Generating data for Binance, OKX, and Bybit exchanges\n";
            std::cout << "   Real-time arbitrage detection running in background\n";
            std::cout << "   The dashboard has its own internal data generation system\n";
            std::cout << "   Press Ctrl+C to stop the demo\n\n";
            
            // Keep the demo running until shutdown is requested
            int status_counter = 0;
            while (!shutdown_requested) {
                std::this_thread::sleep_for(std::chrono::seconds(5));
                
                // Periodic status updates
                if (++status_counter % 12 == 0) { // Every 60 seconds
                    std::cout << "📊 Dashboard with Arbitrage Engine is running at " << dashboard.getDashboardUrl() << "\n";
                    auto metrics = dashboard.getArbitrageMetrics();
                    if (!metrics.empty()) {
                        std::cout << "   📈 Arbitrage Metrics: " 
                                  << metrics["opportunities_detected"] << " opportunities detected, "
                                  << metrics["detection_cycles"] << " cycles completed\n";
                    }
                }
                
                // Check if dashboard is still running
                if (!dashboard.isRunning()) {
                    std::cout << "⚠️  Dashboard server stopped unexpectedly. Shutting down...\n";
                    break;
                }
            }
            
            std::cout << "\n🛑 Stopping dashboard server...\n";
            dashboard.stop();
            std::cout << "✅ Dashboard demo stopped successfully.\n";
            
        } else {
            std::cerr << "❌ Failed to start dashboard server!" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
