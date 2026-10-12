#include "Agent.h"
#include "Auctioneer.h"
#include "Item.h"
#include <vector>
#include <chrono>
#include <iostream>

int main() {
    Item item1 {"vase", 500};
    Item item2 {"painting", 500};
    Item item3 {"chandelier", 700};
    std::vector<Item> items {item1, item2, item3};

    Agent tom {"Intimidator", 10000, Agent::Archetype::Intimidator};
    Agent joe {"Hammer", 10000, Agent::Archetype::Hammer};
    Agent ben {"Nibbler", 10000, Agent::Archetype::Nibbler};
    Agent roy {"Hedger", 10000, Agent::Archetype::Hedger};
    std::vector agents {tom, joe, ben, roy};

    Auctioneer auctioneer {};
    auctioneer.start(items, agents);

    return 0;
};