#include <iostream>
#include <chrono>
#include <random>
#include "OrderBook.h"

int main() {
    OrderBook book;
    const int NUM_ORDERS = 1000000;

    std::mt19937 rng(42);
    // prices between 2490-2510
    std::uniform_real_distribution<double> 
        priceDist(2490.0, 2510.0);
    std::uniform_int_distribution<int> 
        qtyDist(1, 1000);
    std::uniform_int_distribution<int> 
        typeDist(0, 9); // 0-6=LIMIT, 7-9=MARKET

    std::vector<Order> orders;
    orders.reserve(NUM_ORDERS);

    for (int i = 0; i < NUM_ORDERS; i++) {
        Order o;
        o.id       = i + 1;
        o.symbol   = "RELIANCE";
        o.quantity = qtyDist(rng);
        o.timestamp = i;
        o.side     = (i % 2 == 0) ? Side::BUY : Side::SELL;

        int t = typeDist(rng);
        if (t <= 6) {
            o.type  = OrderType::LIMIT;
            o.price = (double)(int)priceDist(rng);
        } else {
            o.type  = OrderType::MARKET;
            o.price = 0;
        }
        orders.push_back(o);
    }

    std::cout << "Benchmarking " << NUM_ORDERS
              << " realistic orders...\n";

    auto start = std::chrono::high_resolution_clock::now();

    for (auto& order : orders) {
        book.submitOrder(order);
        book.processNext();
    }

    auto end = std::chrono::high_resolution_clock::now();

    double seconds = std::chrono::duration<double>(
                         end - start).count();

    std::cout << "Time taken  : " << seconds << "s\n";
    std::cout << "Throughput  : "
              << (long)(NUM_ORDERS / seconds)
              << " orders/second\n";
    std::cout << "Latency     : "
              << (seconds * 1e9 / NUM_ORDERS)
              << " ns/order\n";
    std::cout << "Trades executed: " 
              << book.trades.size() << "\n";
    std::cout << "Orders in book : "
              << [&]() {
                     int count = 0;
                     for (auto& [p,l] : book.bids) 
                         count += l.size();
                     for (auto& [p,l] : book.asks) 
                         count += l.size();
                     return count;
                 }() << "\n";

    return 0;
}