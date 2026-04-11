#pragma once
#include <string>

enum class Side { BUY, SELL };

enum class OrderType { 
    LIMIT, 
    MARKET,
    IOC      // Immediate or Cancel
};

struct Order {
    int id;
    Side side;
    std::string symbol;
    int quantity;
    double price;
    OrderType type;
    long timestamp;
};