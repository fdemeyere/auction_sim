//
// Created by legen on 2026-10-11.
//

#ifndef AUCTION_AUCTIONEER_H
#define AUCTION_AUCTIONEER_H

#include "State.h"
#include "Item.h"
#include <mutex>
#include <condition_variable>
#include <vector>

class Agent;

class Auctioneer {
public:
    State state {};

    std::mutex state_mutex {};
    std::condition_variable state_cv {};

    void begin_auction(Item& item, std::vector<Agent>& agents);

    void start(std::vector<Item> items, std::vector<Agent>& agents);

    void pre_brief_msg();

    void auction_ending_msg();

    void auction_beginning_msg();
};


#endif //AUCTION_AUCTIONEER_H