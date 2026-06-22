#!/usr/bin/env bash
cd "$(dirname "$0")"
mkdir -p dist
g++ -std=c++17 c1.cpp -o dist/asset_scraper && ./dist/asset_scraper
