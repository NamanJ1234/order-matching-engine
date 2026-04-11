#pragma once
#include <unordered_map>
#include <string>
#include "OrderBook.h"
#include <iostream>

class Exchange {
public:
    std::unordered_map<std::string, OrderBook> books;

    void submitOrder(Order order) {
        books[order.symbol].submitOrder(order);
        books[order.symbol].processNext();
    }

    void cancelOrder(const std::string& symbol, int orderId) {
        if (books.find(symbol) == books.end()) {
            std::cout << "Symbol " << symbol << " not found\n";
            return;
        }
        books[symbol].cancelOrder(orderId);
    }

    void printBook(const std::string& symbol) {
        if (books.find(symbol) == books.end()) {
            std::cout << "Symbol " << symbol << " not found\n";
            return;
        }
        std::cout << "=== " << symbol << " ===\n";
        books[symbol].printBook();
    }

    void printAllBooks() {
        for (auto& [symbol, book] : books) {
            std::cout << "=== " << symbol << " ===\n";
            book.printBook();
        }
    }

    void printTrades(const std::string& symbol) {
        if (books.find(symbol) == books.end()) {
            std::cout << "Symbol " << symbol << " not found\n";
            return;
        }
        auto& trades = books[symbol].trades;
        if (trades.empty()) {
            std::cout << "No trades for " << symbol << "\n";
            return;
        }
        for (auto& t : trades) {
            std::cout << t.symbol << " — " << t.quantity
                      << " shares @ " << t.price
                      << " (Buy #" << t.buyOrderId
                      << " x Sell #" << t.sellOrderId
                      << ")\n";
        }
    }

    void printAllTrades() {
        for (auto& [symbol, book] : books)
            printTrades(symbol);
    }
};