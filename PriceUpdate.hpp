#pragma once

#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "single_include/nlohmann/json.hpp"

using json = nlohmann::json;

struct Order {
    double price;
    int size;
    json to_json() const;
};

struct PriceUpdate {
    std::string symbol;
    std::string timestamp;
    Order best_bid;
    Order best_ask;
    Order last_trade;
    std::vector<Order> bids;
    std::vector<Order> asks;
    int volume;
    double high;
    double low;
    double change;

    json to_json() const;
    static PriceUpdate from_json(json& obj);
};

// // Function prototypes
// double generate_random_price(double min);
PriceUpdate create_dynamic_test_price_update();
