#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>

struct Agent;
struct Item;
struct Auctioneer;
struct State;

struct Item {
    std::string name {};
    double value {};
};

struct State {
    Item current_item {};
    double current_bid {};
    Agent* highest_bidder {};
};

struct Agent {
    std::string name {};
    double capital {};
    double raise_amount {};
    double response_time {};

    Agent(std::string name, double capital, double raise_amount) {
        this->name = name;
        this->capital = capital;
        this->raise_amount = raise_amount;
    }

    bool want_to_make_bid(State state) {
        return (state.current_bid + this->raise_amount) <= capital * 0.8;
    }
};

struct Auctioneer {
public:
    State state {};

    void begin_auction(Item& item, std::vector<Agent>& agents) {

        // Starting price below market value
        state.current_item = item;
        state.current_bid = item.value * 0.7;

        std::cout << "Starting price " << state.current_bid << "\n";

        bool new_round {true};
        while (new_round) {
            new_round = false;

            for (Agent& agent : agents) {
                if (&agent == state.highest_bidder) {
                    continue;
                }

                if (agent.want_to_make_bid(state)) {
                    state.current_bid += agent.raise_amount;
                    state.highest_bidder = &agent;

                    new_round = true;
                    std::cout << "Bid: " << (*state.highest_bidder).name << ", " << state.current_bid << "\n";
                }
            }
        }
        std::cout << "Resulting bid: " << state.current_bid << "\n";
        std::cout << "Highest bidder " << (*state.highest_bidder).name;
    }
};

int main() {
    Item item {"vase", 200};
    Auctioneer auctioneer {};
    Agent tom {"tom", 1800, 20};
    Agent joe {"joe", 2000, 20};
    Agent ben {"ben", 1600, 20};

    std::vector agents {tom, joe, ben};
    auctioneer.begin_auction(item, agents);

    return 0;
}