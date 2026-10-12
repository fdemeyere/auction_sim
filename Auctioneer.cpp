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
    std::vector<std::thread> threads {};
    for (Agent& agent : agents) {
        threads.emplace_back(&Agent::run, &agent, std::ref(*this));
    }

    double last_bid {state.current_bid};
    int warning_level {0};

    std::unique_lock<std::mutex> lock(this->state_mutex);

    while (warning_level < 3) {
        bool bid_changed = state_cv.wait_for(lock, std::chrono::seconds(15),
            [this, last_bid] {
                return this->state.current_bid != last_bid;
            });

        if (bid_changed) {
            warning_level = 0;
            last_bid = state.current_bid;
        } else {
            warning_level++;
            if (warning_level == 1) {
                std::cout << "\nGoing once...\n";
            } else if (warning_level == 2) {
                std::cout << "Going twice...\n\n";
            }
        }
        // std::this_thread::sleep_for(std::chrono::seconds(20));
        //
        // std::lock_guard<std::mutex> lock(this->state_mutex);
        // if (state.current_bid == last_bid) {
        //     this->state.is_active = false;
        //     this->state_cv.notify_all();
        //     break;
        // }
        last_bid = this->state.current_bid;
    }

    this->state.is_active = false;
    this->state_cv.notify_all();
    lock.unlock();

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
    std::cout << "Today's auctions will commence at " << get_time_string(Config::SECONDS_UNTIL_START)
        << " (" << Config::TIME_ZONE << ")";

    std::cout << "\n";

    std::this_thread::sleep_for(std::chrono::seconds(Config::SECONDS_UNTIL_START));
}

void Auctioneer::auction_ending_msg() {
    if (state.highest_bidder) {
        std::cout << "Sold!" << "\n";
        std::cout << "Resulting bid: " << Config::CURRENCY << state.current_bid << "\n";
        std::cout << "Highest bidder: " << state.highest_bidder->name;
    }
    else {
        std::cout << "\nAuction ended with no bids";
    }

    std::cout << "\n";

    std::cout << "-------------------------------\n";

    std::cout << "Next auction will commence at " << get_time_string(Config::SECONDS_BETW_AUCTIONS)
        << " (" << Config::TIME_ZONE << ")";

    std::this_thread::sleep_for(std::chrono::seconds(Config::SECONDS_BETW_AUCTIONS));
}

std::string Auctioneer::get_time_string(int forw_offset_sec) {
    auto now = std::chrono::system_clock::now() + std::chrono::hours(2);

    auto current_day = std::chrono::floor<std::chrono::days>(now);
    std::chrono::hh_mm_ss begin_auction_at{now + std::chrono::seconds(forw_offset_sec) - current_day};

    int hours   = begin_auction_at.hours().count();
    int minutes = begin_auction_at.minutes().count();
    int seconds = begin_auction_at.seconds().count();
    std::string seconds_str {std::to_string(seconds)};
    std::string minutes_str {std::to_string(minutes)};
    std::string hours_str {std::to_string(hours)};

    if (seconds / 10 < 1) {
        seconds_str = "0" + std::to_string(seconds);
    }
    if (minutes / 10 < 1) {
        minutes_str = "0" + std::to_string(minutes);
    }
    if (hours / 10 < 1) {
        hours_str = "0" + std::to_string(hours);
    }

    return hours_str + ":" + minutes_str + ":" + seconds_str;
}