# Order Matching Engine

A high-performance order matching engine in C++, implementing the core logic behind real stock exchanges like NSE and NYSE.

## Performance
- **5.8M+ orders/second** throughput on a single core
- **~172ns** average latency per order
- Benchmarked with realistic mixed limit/market/IOC order flow

## Features
- Limit, Market, and IOC (Immediate or Cancel) order types
- Price-time priority (FIFO) matching
- Partial fills
- O(1) order cancellation via iterator-based indexing
- Multiple symbols — separate order book per instrument
- Single-threaded event loop — no locks needed
- Complete trade log

## Build & Run
```bash
# Build engine
g++ -std=c++17 src/main.cpp src/OrderBook.cpp -I include -o engine
./engine

# Run benchmark
g++ -std=c++17 -O2 src/benchmark.cpp src/OrderBook.cpp -I include -o benchmark
./benchmark
```

## Usage

submit BUY 100 RELIANCE 2500 LIMIT
submit SELL 100 RELIANCE 2500 LIMIT
TRADE: 100 shares of RELIANCE @ 2500
submit BUY 150 RELIANCE 2501 IOC
Order #3 — 50 shares unfilled, cancelled
cancel RELIANCE 2
book RELIANCE
trades


## Roadmap
- [ ] Zerodha Kite API integration for live NSE market data
- [ ] Backtesting framework
- [ ] Trading strategy layer (momentum, mean reversion, pairs trading)
- [ ] Live paper trading
