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

Agent::Agent(std::string name, double capital, Archetype archetype)
    : name {name}
    , capital {capital}
    , archetype {archetype}
    , gen {std::random_device{}()} {

    switch (archetype) {
        case Intimidator:
            mean_raise_percent = 0.05;
            sd_raise_percent = 0.01;

            mean_response_time_ms = 6000;
            sd_response_time_ms = 4000;
            return;
        case Hedger:
            mean_raise_percent = 0.01;
            sd_raise_percent = 0.01;

            mean_response_time_ms = 12000;
            sd_response_time_ms = 4000;
            return;
        case Nibbler:
            mean_raise_percent = 0.001;
            sd_raise_percent = 0.001;

            mean_response_time_ms = 6000;
            sd_response_time_ms = 4000;
            return;
        case Hammer:
            mean_raise_percent = 0.2;
            sd_raise_percent = 0.01;

            mean_response_time_ms = 12000;
            sd_response_time_ms = 4000;
            return;
    }
};

bool Agent::want_to_make_bid(State& state) {
    if (state.current_item.name != current_item_name) {
        current_item_name = state.current_item.name;
        current_max_bid_threshold = get_max_bid_threshold(state.current_item);
    }

    int expected_bid {static_cast<int>(state.current_bid + state.current_item.value * mean_raise_percent)
};
    return expected_bid <= capital && expected_bid <= current_max_bid_threshold;
}

void Agent::bid_msg(State& state) {
    std::string msg_left_part {std::string("Bid: ") + "{" + this->name + ", "};
    std::string msg_right_part {std::to_string(Config::CURRENCY) + std::to_string(state.current_bid) + "}"};

    int num_spaces {35};
    num_spaces -= msg_left_part.length() + msg_right_part.length();
    std::string space {};

    for (int i = 0; i < num_spaces; i++) {
        space += " ";
    }

    std::cout << "Bid: " << "{" << this->name << "," << space << Config::CURRENCY << state.current_bid << "}\n";
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
            auction.state.current_bid += get_raise_amount(auction.state.current_item);
            auction.state.highest_bidder = this;

            bid_msg(auction.state);

            auction.state_cv.notify_all();
        }
    }
}

int Agent::get_response_time() {
    std::normal_distribution<double> distrib(mean_response_time_ms, sd_response_time_ms);
    int response_time = static_cast<int>(distrib(gen));
    response_time = response_time < 1500 ? 1500 : response_time;
    return response_time;
}

int Agent::get_raise_amount(Item& item) {
    std::normal_distribution<double> distrib(mean_raise_percent, sd_raise_percent);
    int raise_amount = static_cast<int>(distrib(gen) * item.value);
    raise_amount = raise_amount < 1 ? 1 : raise_amount;
    return raise_amount;
}

int Agent::get_max_bid_threshold(Item& item) {
    std::normal_distribution<double> distrib(1, 0.1);
    int max_bid_threshold = static_cast<int>(distrib(gen) * item.value);
    max_bid_threshold = max_bid_threshold < 1 ? 1 : max_bid_threshold;
    return max_bid_threshold;
}

