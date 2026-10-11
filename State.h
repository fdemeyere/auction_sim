//
// Created by legen on 2026-10-11.
//

#ifndef AUCTION_STATE_H
#define AUCTION_STATE_H

#include "Item.h"

class Agent;

struct State {
    Item current_item {};
    double current_bid {};
    Agent* highest_bidder {};
    bool is_active {false};
};


#endif //AUCTION_STATE_H