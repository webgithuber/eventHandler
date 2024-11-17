# EventHandler and Exchange System

This project simulates a simplified financial trading system using components such as an exchange, price updates, a strategy component, and an event handler. The system demonstrates concepts like socket programming, multithreading, observer pattern, and thread pools.

---

## Features

- **Exchange Component**:
  - Sends dynamic `PriceUpdate` objects serialized as JSON strings to clients via sockets.
  - Generates random market data for testing purposes.

- **EventHandler**:
  - Processes price updates received from the exchange.
  - Uses a queue and thread pool for efficient event handling.
  - Notifies subscribed strategies asynchronously.

- **Strategy Component**:
  - Implements different trading strategies such as Spatial Arbitrage, Statistical Arbitrage, and Delta Neutral.
  - Analyzes received price updates in a multithreaded environment.

- **Thread Pool**:
  - Efficiently handles task execution in a pool of worker threads.
  - Dynamically manages tasks to avoid overloading the system.

---

## Project Structure

- **exchange.cpp**: Simulates the exchange that generates and sends random market price updates.
- **eventhandler.cpp**: Contains the main event handler logic, thread pool, and strategy components.
- **PriceUpdate.hpp/PriceUpdate.cpp**: Implements the `PriceUpdate` and `Order` classes for generating and serializing price updates.

---

## How It Works

1. The **Exchange** continuously generates `PriceUpdate` objects and sends them to clients using sockets.
2. The **EventHandler** receives and parses the updates, enqueues them for processing, and notifies all subscribed strategies asynchronously.
3. The **Strategy Component** processes these updates according to its defined algorithm.

---

## Setup

### Prerequisites
- Windows OS (using Winsock2 for socket programming)
- A C++ compiler supporting C++17 or later (e.g., GCC, MSVC)
- [nlohmann/json](https://github.com/nlohmann/json) for JSON serialization and parsing

### Compilation and Execution

#### Windows (using MinGW or MSVC)

1. Open the terminal or developer command prompt.

2. Compile and execution:
   ```bash
   g++ exchange.cpp PriceUpdate.cpp -o exchange.exe -lwsock32
   g++ eventHandler.cpp PriceUpdate.cpp -o eventHandler.exe -lwsock32
   ./exchange.exe
   ./eventHandler.exe

