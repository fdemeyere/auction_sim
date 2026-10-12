//
// Created by legen on 2026-10-11.
//

#ifndef AUCTION_ITEM_H
#define AUCTION_ITEM_H

#include <string>

struct Item {
    std::string name {};
    double value {};

    enum Categories {
        Painting,
        Furniture,
        NFT,
    };
};


#endif //AUCTION_ITEM_H