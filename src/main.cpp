#include <iostream>
#include <sstream>
#include <string>
#include "Exchange.h"

int main() {
    Exchange exchange;
    int nextId = 1;

    std::cout << "=== Order Matching Engine ===\n";
    std::cout << "Commands:\n";
    std::cout << "  submit BUY/SELL <qty> <symbol> "
              << "<price> LIMIT/MARKET/IOC\n";
    std::cout << "  cancel <symbol> <orderId>\n";
    std::cout << "  book <symbol>\n";
    std::cout << "  books\n";
    std::cout << "  trades <symbol>\n";
    std::cout << "  trades\n";
    std::cout << "  quit\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "quit") {
            break;

        } else if (cmd == "books") {
            exchange.printAllBooks();

        } else if (cmd == "book") {
            std::string symbol;
            if (iss >> symbol)
                exchange.printBook(symbol);
            else
                std::cout << "Usage: book <symbol>\n";

        } else if (cmd == "trades") {
            std::string symbol;
            if (iss >> symbol)
                exchange.printTrades(symbol);
            else
                exchange.printAllTrades();

        } else if (cmd == "cancel") {
            std::string symbol;
            int id;
            if (iss >> symbol >> id)
                exchange.cancelOrder(symbol, id);
            else
                std::cout << "Usage: cancel <symbol> <orderId>\n";

        } else if (cmd == "submit") {
            std::string sideStr, symbol, typeStr;
            int qty;
            double price = 0;

            iss >> sideStr >> qty >> symbol >> price >> typeStr;

            Order order;
            order.id        = nextId++;
            order.symbol    = symbol;
            order.quantity  = qty;
            order.price     = price;
            order.timestamp = order.id;

            if (sideStr == "BUY")
                order.side = Side::BUY;
            else if (sideStr == "SELL")
                order.side = Side::SELL;
            else {
                std::cout << "Invalid side\n";
                continue;
            }

            if (typeStr == "LIMIT")
                order.type = OrderType::LIMIT;
            else if (typeStr == "MARKET") {
                order.type  = OrderType::MARKET;
                order.price = 0;
            } else if (typeStr == "IOC")
                order.type = OrderType::IOC;
            else {
                std::cout << "Invalid type\n";
                continue;
            }

            exchange.submitOrder(order);
            std::cout << "Order #" << order.id 
                      << " submitted\n";

        } else {
            std::cout << "Unknown command\n";
        }
    }
    return 0;
}