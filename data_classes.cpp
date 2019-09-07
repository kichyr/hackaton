#include<iostream>
#include<utility>
#include<vector>
using namespace std;

class courier {
public:
    int courier_id;
    pair<int, int> curr_location;
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
    int point_id;
    pair<int, int> location;
};

void read_data(vector<order>& orders, vector<courier>& couriers){

}