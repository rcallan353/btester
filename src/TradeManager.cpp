#include <iostream>

#include "TradeManager.hpp"

int TradeManager::performTrade(Trade trade) {
    if (trade.mv == Trade::BUY) {
        this->performBuyInternal(trade);
    } else if (trade.mv == Trade::SELL) {
        this->performSellInternal(trade);
    }

    return 0;
}

int TradeManager::performBuyInternal(Trade trade) {
    if (this->canBuy(trade)) {
        if (DEBUG_FLAG) {
            std::string trade_log = "bought - " + std::to_string(trade.quantity) +
                              " at " + std::to_string(trade.tick.close) +
                              " on " + std::to_string(trade.tick.time);
        }
        this->trades.push_back(trade);
        this->quantity += trade.quantity;
        this->performBuy(trade);
        return 1;
    }

    return 0;
}

int TradeManager::performSellInternal(Trade trade) {
    if (this->canSell(trade)) {
        if (DEBUG_FLAG) {
            std::string trade_log = "sold - " + std::to_string(trade.quantity) +
                              " at " + std::to_string(trade.tick.close) +
                              " on " + std::to_string(trade.tick.time);
        }
        this->trades.push_back(trade);
        this->quantity -= trade.quantity;
        this->performSell(trade);
        return 1;
    }

    return 0;
}

int TradeManager::dumpTrades() {
    std::cout << "printing all trades so far" << std::endl;
    std::cout << std::endl;
    for (auto trade : this->trades) {
        trade.dump();
        std::cout << std::endl;
    }
    std::cout << std::endl;
}