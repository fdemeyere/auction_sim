//
// Created by legen on 2026-10-11.
//

#ifndef AUCTION_CONSTANTS_H
#define AUCTION_CONSTANTS_H

#include <string>

namespace Config {
    inline constexpr double MAX_BID_THRESHOLD = 0.8;
    inline constexpr double STARTING_PRICE = 0.7;
    inline constexpr char CURRENCY = '$';
    inline constexpr int SECONDS_UNTIL_START = 10;
    inline constexpr int SECONDS_BETW_AUCTIONS = 15;
    inline constexpr std::string TIME_ZONE = "UTC+1";
}
#endif //AUCTION_CONSTANTS_H