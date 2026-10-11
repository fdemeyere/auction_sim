//
// Created by legen on 2026-10-11.
//

#include "Auctioneer.h"
#include "Agent.h"
#include "Constants.h"
#include "State.h"
#include <chrono>
#include <mutex>
#include <iostream>
#include <random>

Agent::Agent(std::string name, double capital, double raise_amount)
    : name {name}
    , capital {capital}
    , raise_amount {raise_amount} {}

bool Agent::want_to_make_bid(State& state) {
    return (state.current_bid + this->raise_amount) <= capital * Config::MAX_BID_THRESHOLD;
}

void Agent::run(Auctioneer& auction) {
    std::unique_lock<std::mutex> lock(auction.state_mutex);

    while (auction.state.is_active) {


        auction.state_cv.wait(lock, [&auction, this]() {
            return !auction.state.is_active || auction.state.highest_bidder != this;
        });

        if (!this->want_to_make_bid(auction.state)) {
            auction.state_cv.wait(lock, [&auction]() {
                return !auction.state.is_active;
            });
            break;
        }

        double bid_being_evaluated = auction.state.current_bid;
        auto think_duration = std::chrono::milliseconds(this->get_response_time());

        bool interrupted = auction.state_cv.wait_for(lock, think_duration,
            [&auction, bid_being_evaluated]() {
                return !auction.state.is_active || auction.state.current_bid != bid_being_evaluated;
            });

        if (interrupted) {
            // Someone placed a bid while agent was thinking or auction is over
            continue;
        }

        if (auction.state.is_active && this->want_to_make_bid(auction.state)) {
            auction.state.current_bid += this->raise_amount;
            auction.state.highest_bidder = this;

            std::cout << "Bid: " << "(" << this->name << ", " << Config::CURRENCY << auction.state.current_bid << ")\n";

            auction.state_cv.notify_all();
        }
    }
}

int Agent::get_response_time() {
 std::random_device rd;
 std::mt19937 gen(rd());

 std::uniform_int_distribution<int> distrib(1000, 4000);
 return distrib(gen);
}
