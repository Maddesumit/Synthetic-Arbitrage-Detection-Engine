/**
 * @file dashboard_demo.cpp
 * @brief Phase 9 Dashboard Demo - Real-time Web UI
 */

#include "ui/DashboardApp.hpp"
#include "core/PricingEngine.hpp"
#include "core/MathUtils.hpp"
#include "data/MarketData.hpp"
#include "utils/ConfigManager.hpp"
#include "utils/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace arbitrage;

/**
 * @brief Generate realistic demo market data
 */
std::vector<data::MarketDataPoint> generateDemoMarketData() {
    std::vector<data::MarketDataPoint> data;
    
    // Random number generators for realistic price movements
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<double> price_noise(0.0, 0.5);
    static std::uniform_real_distribution<double> volume_dist(100.0, 2000.0);
    static std::normal_distribution<double> funding_noise(0.0, 0.00005);
    
    // Base prices
    static double btc_base = 50000.0;
    static double eth_base = 3000.0;
    
    // Update base prices with small random walk
    btc_base += price_noise(gen);
    eth_base += price_noise(gen) * 0.1;
    
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    
    // BTC spot data
    data::MarketDataPoint btc_spot;
    btc_spot.symbol = "BTCUSDT";
    btc_spot.exchange = "binance";
    btc_spot.timestamp = timestamp;
    btc_spot.bid = btc_base - 0.25;
    btc_spot.ask = btc_base + 0.25;
    btc_spot.last = btc_base;
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
    eth_spot.exchange = "binance";
    eth_spot.timestamp = timestamp;
    eth_spot.bid = eth_base - 0.15;
    eth_spot.ask = eth_base + 0.15;
    eth_spot.last = eth_base;
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
    
    return data;
}

/**
 * @brief Generate demo arbitrage opportunities
 */
std::vector<core::ArbitrageOpportunity> generateDemoOpportunities() {
    std::vector<core::ArbitrageOpportunity> opportunities;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> profit_dist(0.001, 0.005);
    static std::uniform_real_distribution<double> capital_dist(1000.0, 50000.0);
    static std::uniform_real_distribution<double> risk_dist(0.1, 0.4);
    static std::uniform_real_distribution<double> confidence_dist(0.7, 0.95);
    
    // Occasionally generate opportunities
    if (gen() % 10 == 0) {  // 10% chance
        core::ArbitrageOpportunity opp;
        opp.underlying_symbol = "BTCUSDT";
        
        // Create legs for the arbitrage opportunity
        core::ArbitrageOpportunity::Leg leg1;
        leg1.symbol = "BTCUSDT";
        leg1.type = core::InstrumentType::SPOT;
        leg1.exchange = data::Exchange::BINANCE;
        leg1.price = 50000.0 + (gen() % 1000 - 500);
        leg1.synthetic_price = leg1.price * 1.002;
        leg1.deviation = (leg1.price - leg1.synthetic_price) / leg1.synthetic_price;
        leg1.action = "BUY";
        
        core::ArbitrageOpportunity::Leg leg2;
        leg2.symbol = "BTCUSDT-PERP";
        leg2.type = core::InstrumentType::PERPETUAL_SWAP;
        leg2.exchange = data::Exchange::BINANCE;
        leg2.price = leg1.price * 1.002;
        leg2.synthetic_price = leg1.price;
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
        std::cout << "\n🚀 Phase 9: Dashboard Demo - Real-time Web UI\n";
        std::cout << "===============================================\n\n";

        // Initialize logging
        utils::Logger::initialize();
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
        
        // Register demo instruments
        core::InstrumentSpec btc_spot("BTCUSDT", core::InstrumentType::SPOT, data::Exchange::BINANCE);
        pricing_engine->registerInstrument(btc_spot);
        
        core::InstrumentSpec btc_perp("BTCUSDT-PERP", core::InstrumentType::PERPETUAL_SWAP, data::Exchange::BINANCE);
        pricing_engine->registerInstrument(btc_perp);
        
        core::InstrumentSpec eth_spot("ETHUSDT", core::InstrumentType::SPOT, data::Exchange::BINANCE);
        pricing_engine->registerInstrument(eth_spot);

        // Create and initialize dashboard
        ui::DashboardApp dashboard(8080);
        dashboard.initialize(pricing_engine);
        
        std::cout << "Starting dashboard server...\n";
        std::cout << "Dashboard URL: " << dashboard.getDashboardUrl() << "\n\n";
        std::cout << "📊 Dashboard Features:\n";
        std::cout << "   • Real-time system status monitoring\n";
        std::cout << "   • Live market data visualization\n";
        std::cout << "   • Pricing results display\n";
        std::cout << "   • Arbitrage opportunity tracking\n";
        std::cout << "   • Performance metrics dashboard\n";
        std::cout << "   • Risk metrics monitoring\n";
        std::cout << "   • Auto-refresh every 5 seconds\n\n";
        
        // Start dashboard in background
        dashboard.startAsync();
        
        // Wait for server to start
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        if (dashboard.isRunning()) {
            std::cout << "✅ Dashboard server started successfully!\n";
            std::cout << "   Open your browser and navigate to: " << dashboard.getDashboardUrl() << "\n\n";
            
            std::cout << "🔄 Starting demo data simulation...\n";
            std::cout << "   Press Ctrl+C to stop the demo\n\n";
            
            // Simulation loop
            int iteration = 0;
            while (true) {
                iteration++;
                
                // Generate and update demo data
                auto market_data = generateDemoMarketData();
                dashboard.updateMarketData(market_data);
                
                // Generate pricing results
                std::vector<core::PricingResult> pricing_results;
                for (const auto& md : market_data) {
                    core::PricingResult result;
                    result.instrument_id = md.symbol;
                    result.synthetic_price = md.last;
                    result.confidence = 0.95;
                    result.model_name = (md.symbol.find("PERP") != std::string::npos) ? 
                        "Perpetual Swap" : "Spot";
                    result.components.base_price = md.last;
                    if (md.symbol.find("PERP") != std::string::npos) {
                        result.components.funding_adjustment = md.funding_rate * md.last * 8.0;
                    }
                    result.success = true;
                    result.calculation_time_ms = 0.1 + (iteration % 10) * 0.05;
                    result.timestamp = std::chrono::system_clock::time_point(md.timestamp);
                    pricing_results.push_back(result);
                }
                dashboard.updatePricingResults(pricing_results);
                
                // Generate arbitrage opportunities
                auto opportunities = generateDemoOpportunities();
                dashboard.updateArbitrageOpportunities(opportunities);
                
                // Log progress
                if (iteration % 10 == 0) {
                    std::cout << "📈 Demo iteration " << iteration 
                              << " - " << market_data.size() << " market data points, "
                              << pricing_results.size() << " pricing results, "
                              << opportunities.size() << " opportunities\n";
                }
                
                // Wait before next update
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
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
