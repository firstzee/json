#include <deserialize/from_json.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

#include <map>
#include <iostream>
#include <sstream>
#include <optional>

struct Room {
    int x;
    int y;
};

struct A {
    std::string date;
    std::optional<std::map<std::string, Room>> rooms;
    std::vector<std::string> owners;
};

BOOST_FUSION_ADAPT_STRUCT(Room,
    x,
    y
)

BOOST_FUSION_ADAPT_STRUCT(A,
    date,
    rooms,
    owners
)

int main() {
    A a;
    deserialize::from_json(
        a,
        R"({"date": "26.01.2010", "rooms": {"hall": {"x": 1, "y": 2}, "kitchen": {"x": 3, "y": 4}}, "owners": ["tu", "mu"]})"
    );
    std::cout << a.date << std::endl;
    for(auto& room: *a.rooms) {
        std::cout << room.first << " " << room.second.x << " " << room.second.y << std::endl;
    }
    for(auto& owner: a.owners) {
        std::cout << owner << std::endl;
    }
    return 1;
}
