#ifndef RBSS_HPP
#define RBSS_HPP 1

#include "Strategy.hpp"

class RandomBuySellStrategy : public Strategy {
public:
    Trade processTick(Tick t);
};

#endif