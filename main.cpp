#include "Agent.h"
#include "Auctioneer.h"
#include "Item.h"
#include <vector>
#include <chrono>

int main() {
    Item item1 {"vase", 2000};
    Item item2 {"painting", 1500};
    Item item3 {"chandelier", 2200};
    std::vector<Item> items {item1, item2, item3};

    Agent tom {"tom", 1800, 200};
    Agent joe {"joe", 2000, 200};
    Agent ben {"ben", 1600, 200};
    std::vector agents {tom, joe, ben};

    Auctioneer auctioneer {};
    auctioneer.begin_auction(item1, agents);

    return 0;
};