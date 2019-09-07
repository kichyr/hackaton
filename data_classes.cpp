#include<iostream>
#include<utility>
#include<vector>
#include<fstream>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

typedef pair<int, int> Point;

//метрика Манхэттена
int ro(Point p1, Point p2) {
    return abs(p1.first - p2.first) + abs(p1.second - p2.second);
}
enum Action {
    pickup,
    dropoff,
    wait
};

typedef struct task {
    Action action;
    int order_id;
    int point_id;
    int time_start = -1;
    int time_end = -1;
};
 
class courier {
public:
    int courier_id;
    pair<int, int> curr_location;
    vector<task> task_list;
};

class order {
public:
    int order_id;
    int pickup_point_id, dropoff_point_id;
    pair<int, int> pickup_location, dropoff_location;
    int pickup_from; // время начала окна
    int pickup_to; // время конца окна
    int dropoff_from;
    int dropoff_to;
    int payment;
};


class depot {
public:
    int point_id;
    pair<int, int> location;
};

void read_data(vector<order>& orders, vector<courier>& couriers, vector<depot>& depots){
    std::ifstream ifs("input.json");
    json j = json::parse(ifs);
    for (size_t i = 0; i < j["orders"].size(); i++) {
        order ord;
        ord.order_id = j["orders"][i]["order_id"];
        ord.pickup_point_id = j["orders"][i]["pickup_point_id"];
        ord.dropoff_point_id = j["orders"][i]["dropoff_point_id"];
        ord.pickup_location.first = j["orders"][i]["pickup_location_x"];
        ord.pickup_location.second = j["orders"][i]["pickup_location_y"];
        ord.dropoff_location.first = j["orders"][i]["dropoff_location_x"];
        ord.dropoff_location.second = j["orders"][i]["dropoff_location_y"];
        ord.pickup_from = j["orders"][i]["pickup_from"];
        ord.pickup_to = j["orders"][i]["pickup_to"];
        ord.dropoff_from = j["orders"][i]["dropoff_from"];
        ord.dropoff_to = j["orders"][i]["dropoff_to"];
        ord.payment = j["orders"][i]["payment"];
        orders.push_back(ord);
    }
    for (size_t i = 0; i < j["couriers"].size(); i++) {
        courier cour;
        cour.courier_id = j["couriers"][i]["courier_id"];
        cour.curr_location.first = j["couriers"][i]["location_x"];
        cour.curr_location.second = j["couriers"][i]["location_y"];
        couriers.push_back(cour);
    }
    for (size_t i = 0; i < j["depots"].size(); i++) {
        depot dep;
        dep.point_id = j["depots"][i]["point_id"];
        dep.location.first = j["depots"][i]["location_x"];
        dep.location.second = j["depots"][i]["location_y"];
        depots.push_back(dep);
    }
}

json make_json(vector<courier>& couriers) {
    json output = json::array();
    for (size_t i = 0; i < couriers.size(); i++) {
        for (size_t j = 0; j < couriers[i].task_list.size(); j++) {
            json event;
            event["order_id"] = couriers[i].task_list[j].order_id;
            event["point_id"] = couriers[i].task_list[j].point_id;
            event["courier_id"] = couriers[i].courier_id;
            if (couriers[i].task_list[j].action == pickup) {
                event["action"] = "pickup";
            }
            else if (couriers[i].task_list[j].action == dropoff) {
                event["action"] = "dropoff";
            }
            output.push_back(event);
        }
    }
    return output;
}
