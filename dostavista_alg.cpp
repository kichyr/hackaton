#include"data_classes.cpp"
#include <algorithm>
#include<vector>


#define START_TIME 360

class Dostavister {
public:
    vector<order> orders;
    vector<courier> couriers;
    vector<depot> depots;
    int time;

    Dostavister();
    void make_start_state();
    void make_decision(courier c);
    depot& closest_depot(Point p);
    order& closest_dest(Point p);
};


Dostavister::Dostavister() {
    read_data(orders, couriers, depots);
}



void Dostavister::make_start_state() {
    time = START_TIME;
    sort(orders.begin(), orders.end(),
    //сортируем заказы по цене
    [](const order & a, const order & b)
    {
        return a.payment > b.payment;                                                //??????? хуй знает в какую сторону знак ставить
    });
    for(int i = 0; i < couriers.size(); i++)
        couriers[i].task_list.push_back({Action::pickup, 
                                        orders[i].order_id,
                                        orders[i].pickup_from,
                                        START_TIME
                                        });
}


void Dostavister::make_decision(courier c) {
    //берем заказ и ищем куда его доставить
    if(c.task_list.back().action == Action::pickup) {

    }
}

depot& Dostavister::closest_depot(Point p) {
    depot closest_dep;
    for(auto depot : depots)
        if(ro(p, depot.location) < ro(p, closest_dep.location))
            closest_dep = depot;
    return closest_dep;
}

depot& Dostavister::closest_depot(Point p) {
    depot closest_dep;
    for(auto depot : depots)
        if(ro(p, depot.location) < ro(p, closest_dep.location))
            closest_dep = depot;
    return closest_dep;
}

void Emulation(Dostavister& d) {
    //возвращаем элемент с минимальным временем окончания последней задачи
    //составляем решение о дальнейших действиях
    d.make_decision( *min_element(d.couriers.begin(), d.couriers.end(), 
    [](const courier & a, const courier & b) {
        if(a.task_list.back().time_end > b.task_list.back().time_end)               //??????? хуй знает в какую сторону знак ставить
            return true;                              
        else return false;
    }
    ));
}