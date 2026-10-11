//
// Created by legen on 2026-10-11.
//

#ifndef AUCTION_AGENT_H
#define AUCTION_AGENT_H


#include <string>

class State;
class Auctioneer;

class Agent {
public:
    std::string name {};

    void run(Auctioneer& auction);

    Agent(std::string name, double capital, double raise_amount);

    void bid_msg(State& state);

private:
    double capital {};
    double raise_amount {};
    double response_time {};

    bool want_to_make_bid(State& state);

    int get_response_time();
};


#endif //AUCTION_AGENT_H