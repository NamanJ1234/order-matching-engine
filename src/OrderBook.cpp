#include "OrderBook.h"
#include <iostream>

void OrderBook::submitOrder(Order order) {
    incomingOrders.push(order);
}

void OrderBook::processNext() {
    if (incomingOrders.empty()) return;
    Order order = incomingOrders.front();
    incomingOrders.pop();
    addOrder(order);
}

void OrderBook::processAll() {
    while (!incomingOrders.empty()) {
        processNext();
    }
}

void OrderBook::addOrder(Order order) {
    if (order.type == OrderType::MARKET || 
        order.type == OrderType::IOC) {
        
        // IOC with price means — try to match at 
        // this price or better, cancel remainder
        // MARKET means — match at any price
        bool isMarket = (order.type == OrderType::MARKET);

        if (order.side == Side::BUY) {
            while (order.quantity > 0 && !asks.empty()) {
                double bestAsk = asks.begin()->first;
                
                // IOC respects price limit, MARKET doesn't
                if (!isMarket && bestAsk > order.price) break;

                Order& bestSell = asks.begin()->second.front();
                int matchedQty  = std::min(order.quantity,
                                           bestSell.quantity);

                Trade trade;
                trade.buyOrderId  = order.id;
                trade.sellOrderId = bestSell.id;
                trade.symbol      = order.symbol;
                trade.quantity    = matchedQty;
                trade.price       = bestAsk;
                trades.push_back(trade);

                order.quantity    -= matchedQty;
                bestSell.quantity -= matchedQty;

                if (bestSell.quantity == 0) {
                    orderLocation.erase(bestSell.id);
                    orderSide.erase(bestSell.id);
                    asks.begin()->second.pop_front();
                    if (asks.begin()->second.empty())
                        asks.erase(asks.begin());
                }
            }
            if (order.quantity > 0)
                std::cout << "Order #" << order.id 
                          << " — " << order.quantity 
                          << " shares unfilled, cancelled\n";
        } else {
            while (order.quantity > 0 && !bids.empty()) {
                double bestBid = bids.begin()->first;

                if (!isMarket && bestBid < order.price) break;

                Order& bestBuy  = bids.begin()->second.front();
                int matchedQty  = std::min(order.quantity,
                                           bestBuy.quantity);

                Trade trade;
                trade.buyOrderId  = bestBuy.id;
                trade.sellOrderId = order.id;
                trade.symbol      = order.symbol;
                trade.quantity    = matchedQty;
                trade.price       = bestBid;
                trades.push_back(trade);

                order.quantity   -= matchedQty;
                bestBuy.quantity -= matchedQty;

                if (bestBuy.quantity == 0) {
                    orderLocation.erase(bestBuy.id);
                    orderSide.erase(bestBuy.id);
                    bids.begin()->second.pop_front();
                    if (bids.begin()->second.empty())
                        bids.erase(bids.begin());
                }
            }
            if (order.quantity > 0)
                std::cout << "Order #" << order.id
                          << " — " << order.quantity
                          << " shares unfilled, cancelled\n";
        }
        return;
    }

    // limit order
    if (order.side == Side::BUY) {
        bids[order.price].push_back(order);
        orderLocation[order.id] = std::prev(bids[order.price].end());
        orderSide[order.id] = Side::BUY;
    } else {
        asks[order.price].push_back(order);
        orderLocation[order.id] = std::prev(asks[order.price].end());
        orderSide[order.id] = Side::SELL;
    }
    matchOrders();
}

void OrderBook::matchOrders() {
    while (!bids.empty() && !asks.empty()) {
        double bestBid = bids.begin()->first;
        double bestAsk = asks.begin()->first;

        if (bestBid < bestAsk) break;

        Order& buyOrder  = bids.begin()->second.front();
        Order& sellOrder = asks.begin()->second.front();

        int matchedQty = std::min(buyOrder.quantity,
                                  sellOrder.quantity);

        Trade trade;
        trade.buyOrderId  = buyOrder.id;
        trade.sellOrderId = sellOrder.id;
        trade.symbol      = buyOrder.symbol;
        trade.quantity    = matchedQty;
        trade.price       = bestAsk;
        trades.push_back(trade);

        // std::cout << "TRADE: " << matchedQty << " shares of "
        //           << trade.symbol << " @ " << trade.price << "\n";

        buyOrder.quantity  -= matchedQty;
        sellOrder.quantity -= matchedQty;

        if (buyOrder.quantity == 0) {
            orderLocation.erase(buyOrder.id);
            orderSide.erase(buyOrder.id);
            bids.begin()->second.pop_front();
            if (bids.begin()->second.empty())
                bids.erase(bids.begin());
        }
        if (sellOrder.quantity == 0) {
            orderLocation.erase(sellOrder.id);
            orderSide.erase(sellOrder.id);
            asks.begin()->second.pop_front();
            if (asks.begin()->second.empty())
                asks.erase(asks.begin());
        }
    }
}

void OrderBook::cancelOrder(int orderId) {
    if (orderLocation.find(orderId) == orderLocation.end()) {
        std::cout << "Order #" << orderId << " not found\n";
        return;
    }

    auto it    = orderLocation[orderId];
    Side side  = orderSide[orderId];
    double price = it->price;

    if (side == Side::BUY) {
        bids[price].erase(it);
        if (bids[price].empty())
            bids.erase(price);
    } else {
        asks[price].erase(it);
        if (asks[price].empty())
            asks.erase(price);
    }

    orderLocation.erase(orderId);
    orderSide.erase(orderId);
    std::cout << "Order #" << orderId << " cancelled\n";
}

void OrderBook::printBook() {
    std::cout << "\n--- ORDER BOOK ---\n";
    std::cout << "ASK side:\n";
    for (auto& [price, list] : asks)
        std::cout << "  " << price << " : "
                  << list.size() << " orders\n";
    std::cout << "BID side:\n";
    for (auto& [price, list] : bids)
        std::cout << "  " << price << " : "
                  << list.size() << " orders\n";
    std::cout << "------------------\n";
}