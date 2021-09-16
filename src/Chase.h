#include <string>

namespace chase{

void say_hello(std::string name);

class Player{
public:
    Player(double knows);
    
    Player() = default;
    Player(Player const&) = default;
    Player& operator=(Player const&) = default;
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

private:
    double knows_ = 0;
};

class Chase{
public:
    Chase(double candidateChance);
    Chase() = default;
    Chase(Chase const&) = default;
    Chase& operator=(Chase const&) = default;
    Chase(Chase&&) = default;
    Chase& operator=(Chase&&) = default;
private:
    std::vector<Player> candidates_;
    Player chaser_;
};

}