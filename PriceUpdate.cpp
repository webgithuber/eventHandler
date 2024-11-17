#include "PriceUpdate.hpp"
#include<vector>
#include <typeinfo>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <thread>
#include<mutex>
#include <queue>
#include<condition_variable>
#include<future>
#include<cstdlib>
#include <time.h>
#include <memory>
#include<functional>
#include<mutex>
#include<semaphore>

#include "single_include/nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;


json Order:: to_json()const {
    return {{"price", price}, {"size", size}};
}


json PriceUpdate::to_json() const
    {
        json bids_json = json::array();
        for (const auto& bid : bids) {
            bids_json.push_back(bid.to_json());
        }

        json asks_json = json::array();
        for (const auto& ask : asks) {
            asks_json.push_back(ask.to_json());
        }
        return {
            {"symbol",symbol},
            {"timestamp",timestamp},
            {"best_bid",{{"price",best_bid.price},{"size",best_bid.size}}},
            {"best_ask",{{"price",best_ask.price},{"size",best_ask.size}}},
            {"last_trade",{{"price",last_trade.price},{"size",last_trade.size}}},
            {"order_book",{{"bids",bids_json},{"asks",asks_json}}},
            {"volume",volume},
            {"high",high},
            {"low",low},
            {"change",change},

        };
    }
PriceUpdate PriceUpdate::from_json(json & obj)
    {PriceUpdate p;
        p.symbol=obj.at("symbol").get<std::string>();
        p.timestamp=obj.at("timestamp").get<std::string>();
        p.best_bid={obj.at("best_bid").at("price").get<double>(),obj.at("best_bid").at("size").get<int>()};
        p.best_bid={obj.at("best_ask").at("price").get<double>(),obj.at("best_ask").at("size").get<int>()};
        p.best_bid={obj.at("last_trade").at("price").get<double>(),obj.at("last_trade").at("size").get<int>()};
        p.volume=obj.at("volume").get<int>();
        p.high=obj.at("high").get<double>();
        p.low=obj.at("low").get<double>();
        p.change=obj.at("change").get<double>();
        
        for (const auto& bid : obj.at("order_book").at("bids")) {
            p.bids.push_back({bid.at("price").get<double>(), bid.at("size").get<int>()});
        }
        for (const auto& ask : obj.at("order_book").at("asks")) {
            p.asks.push_back({ask.at("price").get<double>(), ask.at("size").get<int>()});
        }
        return p;
    }


double generate_random_price(double min)
{
    unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::srand(seed);
    return 1.0*(rand()%100) + 1.0*(rand()%10)/10.0 + min;
}  

// Function to generate a random price update object for testing with dynamic bids and asks
PriceUpdate create_dynamic_test_price_update() {
unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::srand(seed);

    PriceUpdate price_update;
    price_update.symbol = "AAPL";

    auto now = std::chrono::system_clock::now();
	price_update.timestamp=to_string(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
    //price_update.timestamp = string(std::chrono::system_clock::now()    );
    
    // Generate best bid and ask with random values
    price_update.best_bid = {generate_random_price(200), std::rand() % 500 + 1}; // Random price between 150.00 and 155.00, random size between 1 and 500
    price_update.best_ask = {generate_random_price(190), std::rand() % 500 + 1}; // Random price between 155.00 and 160.00, random size between 1 and 500
    price_update.last_trade = {generate_random_price(150), std::rand() % 1000 + 1}; // Random trade price and size

    price_update.volume = std::rand() % 1000000 + 1; // Random volume
    price_update.high = generate_random_price(250);
    price_update.low = generate_random_price(90);
    price_update.change = 5 - std::rand()%10; // Random change

    // Generate 10 random bids and asks
    for (int i = 0; i < 10; ++i) {
        price_update.bids.push_back({generate_random_price(150), std::rand() % 500 + 1});
        price_update.asks.push_back({generate_random_price(150), std::rand() % 500 + 1});
    }

    return price_update;
}

