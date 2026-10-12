//
// Created by legen on 2026-10-11.
//

#ifndef AUCTION_AGENT_H
#define AUCTION_AGENT_H


#include <string>
#include <random>

class State;
class Auctioneer;
class Item;

class Agent {
public:
    enum Archetype {
        Intimidator,
        Hammer,
        Nibbler,
        Hedger,
    };

    std::string name {};

    void run(Auctioneer& auction);

    Agent(std::string name, double capital, Archetype archetype);

    void bid_msg(State& state);

private:
    std::mt19937 gen;

    double capital {};
    int current_max_bid_threshold {0};
    std::string current_item_name {};

    double mean_raise_percent {};
    double sd_raise_percent {};

    double mean_response_time_ms {};
    double sd_response_time_ms {};

    Archetype archetype {};

    bool want_to_make_bid(State& state);
    int get_response_time();
    int get_raise_amount(Item& item);
    int get_max_bid_threshold(Item& item);
};


#endif //AUCTION_AGENT_H