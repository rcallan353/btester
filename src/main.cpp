#include <fstream>
#include <iostream>
#include <random>

#include "cxxopts.hpp"
#include "staticmsgs.hpp"

#include "YahooFinanceFileTickManager.hpp"
#include "RandomBuySellStrategy.hpp"
#include "SmaStrategy.hpp"
#include "VwmaStrategy.hpp"
#include "VirtualBank.hpp"
#include "PaperTradeManager.hpp"

int main(int argc, char **argv) {
    cxxopts::Options options("btester v0.0.1", "btester - mini backtester/trading engine");

    // command line option related things

    options.add_options()("i,input", "Input csv file, will read ticks from here",
                        cxxopts::value<std::string>())(
      "o,output", "Output csv file, will output trade / results here",
      cxxopts::value<std::string>()->default_value("output.csv"))(
      "d,debug", "Enable debugging",
      cxxopts::value<bool>()->default_value("false"))("h,help", "Print usage");

    std::string input_file;
    std::string output_file;
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    bool debug = result["debug"].as<bool>();

    // check if input file was specified - exit if it was not
    if (result.count("input")) {
        input_file = result["input"].as<std::string>();
    } else {
        std::cout << staticmsgs::noinputfile << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    output_file = result["output"].as<std::string>();

    if (DEBUG_FLAG) {
        std::cout << staticmsgs::printargs << std::endl;
        std::cout << "Path to input file " << input_file << std::endl;
        std::cout << "Path to output file " << output_file << std::endl;
    }

    // command line related things are now done

    // initialize tick manager
    std::unique_ptr<TickManager> tick_manager(new YahooFinanceFileTickManager(input_file));

    // initialize strategy
    // std::unique_ptr<Strategy> strategy(new RandomBuySellStrategy());
    // std::unique_ptr<Strategy> strategy(new SmaStrategy());
    std::unique_ptr<Strategy> strategy(new VwmaStrategy());

    // initialize a virtual bank
    long double starting_balance = 100000;
    std::shared_ptr<VirtualBank> vb(new VirtualBank(starting_balance));

    // initialize trade manager
    std::unique_ptr<TradeManager> trade_manager(new PaperTradeManager(vb));

    // start the core event loop
    Tick last_traded_tick;
    while (tick_manager->hasNextTick()) {
        Tick t = tick_manager->getNextTick();
        last_traded_tick = t;

        Trade tr = strategy->processTick(t);
        trade_manager->performTrade(tr);
    }

    trade_manager->squareOff(last_traded_tick);
    trade_manager->dumpTrades();

    // show the final balance and profit/loss value

    std::string final_bank_balance = std::to_string(vb->getBankBalance());
    std::string profit_or_loss = std::to_string(vb->getBankBalance() - starting_balance);

    std::cout << "total bank balance: " << final_bank_balance << std::endl;
    std::cout << "profit or loss: " << profit_or_loss << std::endl;
    
    return 0;
}