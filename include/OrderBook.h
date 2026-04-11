#pragma once
#include <map>
#include <list>
#include <queue>
#include <vector>
#include <unordered_map>
#include "Order.h"

struct Trade {
    int buyOrderId;
    int sellOrderId;
    std::string symbol;
    int quantity;
    double price;
};

class OrderBook {
public:
    // changed queue → list for O(1) removal
    std::map<double, std::list<Order>, 
             std::greater<double>> bids;
    std::map<double, std::list<Order>> asks;
    
    std::vector<Trade> trades;
    std::queue<Order> incomingOrders;

    // NEW — order ID → price level
    std::unordered_map<int, std::list<Order>::iterator> orderLocation;
    
    // NEW — track which side each order is on
    std::unordered_map<int, Side> orderSide;

    void submitOrder(Order order);
    void cancelOrder(int orderId);    // NEW
    void processNext();
    void processAll();
    void addOrder(Order order);
    void matchOrders();
    void printBook();
};