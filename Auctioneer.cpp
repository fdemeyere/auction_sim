//
// Created by legen on 2026-10-11.
//

#include "Agent.h"
#include "Auctioneer.h"
#include "Constants.h"
#include <iostream>
#include <chrono>
#include <thread>

void Auctioneer::begin_auction(Item& item, std::vector<Agent>& agents) {

    double last_bid {state.current_bid};

    std::vector<std::thread> threads {};
    for (Agent& agent : agents) {
        threads.emplace_back(&Agent::run, &agent, std::ref(*this));
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        std::lock_guard<std::mutex> lock(this->state_mutex);
        if (state.current_bid == last_bid) {
            this->state.is_active = false;
            this->state_cv.notify_all();
            break;
        }
        last_bid = this->state.current_bid;
    }

    for (std::thread& t: threads) {
        t.join();
    }

    auction_ending_msg();

    std::cout << "\n";
}

void Auctioneer::start(std::vector<Item> items, std::vector<Agent>& agents) {
    pre_brief_msg();

    for (Item& item : items) {

        state.current_item = item;

        // Starting price below market value
        state.current_bid = item.value * Config::STARTING_PRICE;
        state.is_active = true;
        state.highest_bidder = nullptr;

        auction_beginning_msg();

        begin_auction(item, agents);
    }
}
void Auctioneer::auction_beginning_msg() {
    std::cout << "===============================\n" << "       Auction has begun\n";
    std::cout << "Object: " << state.current_item.name << ". Starting bid: " << Config::CURRENCY << state.current_bid << "\n";
    std::cout << "===============================\n";
}

void Auctioneer::pre_brief_msg() {
    auto now = std::chrono::system_clock::now() + std::chrono::hours(2);

    auto current_day = std::chrono::floor<std::chrono::days>(now);
    std::chrono::hh_mm_ss begin_auction_at{now + std::chrono::seconds(Config::SECONDS_UNTIL_START) - current_day};

    int hours   = begin_auction_at.hours().count();
    int minutes = begin_auction_at.minutes().count();
    int seconds = begin_auction_at.seconds().count();
    std::string seconds_str {};

    if (seconds / 10 < 1) {
        seconds_str = "0" + std::to_string(seconds);
        std::cout << "Today's auctions will commence at " << hours << ":" << minutes << ":" << seconds_str <<
            " (" << Config::TIME_ZONE << ")";
    }
    else {
        std::cout << "Today's auctions will commence at " << hours << ":" << minutes << ":" << seconds <<
            " (" << Config::TIME_ZONE << ")";;
    }

    std::cout << "\n";

    std::this_thread::sleep_for(std::chrono::seconds(Config::SECONDS_UNTIL_START));
}

void Auctioneer::auction_ending_msg() {
    if (state.highest_bidder) {
        std::cout << "\nGoing once...\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::cout << "Going twice..." << "\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::cout << "Sold!" << "\n";
        std::cout << "Resulting bid: " << Config::CURRENCY << state.current_bid << "\n";
        std::cout << "Highest bidder: " << state.highest_bidder->name;
    }
    else {
        std::cout << "\nAuction ended with no bids";
    }

    std::cout << "\n";

    auto now = std::chrono::system_clock::now() + std::chrono::hours(2);

    auto current_day = std::chrono::floor<std::chrono::days>(now);
    std::chrono::hh_mm_ss begin_auction_at{now + std::chrono::seconds(Config::SECONDS_UNTIL_START) - current_day};

    int hours   = begin_auction_at.hours().count();
    int minutes = begin_auction_at.minutes().count();
    int seconds = begin_auction_at.seconds().count();
    std::string seconds_str {};

    std::cout << "-------------------------------\n";

    if (seconds / 10 < 1) {
        seconds_str = "0" + std::to_string(seconds);
        std::cout << "Next auction will commence at " << hours << ":" << minutes << ":" << seconds_str;
    }
    else {
        std::cout << "Next auction will commence at " << hours << ":" << minutes << ":" << seconds;
    }

    std::this_thread::sleep_for(std::chrono::seconds(Config::SECONDS_BETW_AUCTIONS));

}