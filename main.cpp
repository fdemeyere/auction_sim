#include <iostream>
#include <vector>

struct Agent;
struct Item;
struct Auctioneer;

struct Item {
    std::string name {};
    double value {};
};

struct Agent {
    std::string name {};
    double capital {};

    bool want_to_make_bid(Item& item, double next_bid) {
        return next_bid <= capital * 0.8;
    }
};

struct Auctioneer {
public:
    double raise_amount {50};

    void begin_auction(Item& item, std::vector<Agent>& agents) {

        // Starting price below market value
        double current_bid {item.value * 0.7};
        Agent* highest_bidder {};

        std::cout << "Starting price " << current_bid << "\n";

        bool new_round {true};
        while (new_round) {
            new_round = false;

            for (Agent& agent : agents) {
                if (&agent == highest_bidder) {
                    continue;
                }

                if (agent.want_to_make_bid(item, current_bid + raise_amount)) {
                    current_bid += raise_amount;
                    highest_bidder = &agent;

                    new_round = true;
                    std::cout << "Bid: " << (*highest_bidder).name << ", " << current_bid << "\n";
                }
            }
        }
        std::cout << "Resulting bid: " << current_bid << "\n";
        std::cout << "Highest bidder " << (*highest_bidder).name;
    }
private:
};


// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    Item item {"vase", 200};
    Auctioneer auctioneer {};
    Agent frank {"frank", 1000};
    Agent joe {"joe", 2000};

    std::vector agents {frank, joe};
    auctioneer.begin_auction(item, agents);

    return EXIT_SUCCESS;
}