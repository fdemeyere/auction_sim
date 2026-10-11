#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>
#include <condition_variable>

constexpr double MAX_BID_THRESHOLD = 0.8;
constexpr double STARTING_PRICE = 0.7;

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
    bool is_active {false};
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

    bool want_to_make_bid(State& state) {
        return (state.current_bid + this->raise_amount) <= capital * MAX_BID_THRESHOLD;
    }

    void run(Auctioneer& auction);

    int get_response_time() {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<int> distrib(1000, 4000);
        return distrib(gen);
    }
};

struct Auctioneer {
public:
    State state {};

    std::mutex state_mutex {};
    std::condition_variable state_cv {};

    void begin_auction(Item& item, std::vector<Agent>& agents) {

        // Starting price below market value
        state.current_item = item;
        state.current_bid = item.value * STARTING_PRICE;
        state.is_active = true;

        std::cout << "Starting price " << state.current_bid << "\n";

        std::vector<std::thread> threads {};
        for (Agent& agent : agents) {
            threads.emplace_back(&Agent::run, &agent, std::ref(*this));
        }

        double last_bid {STARTING_PRICE};
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

        std::cout << "Going once..." << "\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::cout << "Going twice..." << "\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(4));

        std::cout << "Sold!" << "\n\n" << "----------------" << "\n";
        std::cout << "Resulting bid: " << state.current_bid << "\n";
        std::cout << "Highest bidder " << (*state.highest_bidder).name;
    }
};

inline void Agent::run(Auctioneer& auction) {
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
            // Someone placed a bid while agents was thinking or auction is over
            continue;
        }

        if (auction.state.is_active && this->want_to_make_bid(auction.state)) {
            auction.state.current_bid += this->raise_amount;
            auction.state.highest_bidder = this;

            std::cout << "Bid: " << this->name << ": " << auction.state.current_bid << "\n";

            auction.state_cv.notify_all();
        }
    }
}

int main() {
    Item item {"vase", 200};
    Auctioneer auctioneer {};
    Agent tom {"tom", 1800, 200};
    Agent joe {"joe", 2000, 200};
    Agent ben {"ben", 1600, 200};

    std::vector agents {tom, joe, ben};
    auctioneer.begin_auction(item, agents);

    return 0;
};