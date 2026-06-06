# Electricity Price Fetcher (15-minute market prices)

This project fetches electricity prices from the SahkoHinta API and provides utilities to query:

- Today’s full 15-minute price curve (96 values)
- Historical day prices
- Current price (“price now”)
- Specific quarter-hour price lookup

## API Source

This project uses:

https://www.sahkohinta-api.fi/documentation.htm#aloitus

---

## Build requirements

### Dependencies

- C++20 compiler (GCC / Clang)
- libcurl
- nlohmann/json

### Install dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install g++ libcurl4-openssl-dev nlohmann-json3-dev
