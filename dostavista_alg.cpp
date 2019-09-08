#include"data_classes.cpp"
#include <algorithm>
#include<vector>
#include<map>
#include<tuple>

#define START_TIME 360
#define END_TIME 1440
#define INFTY 9999


class Dostavister {
public:
    vector<order> orders;
    vector<courier> couriers;
    //
    int profit = 0;
    //key is order id
    map<int, order*> get_ptr_order_by_order_id;
    //key is Point id
    map<int, depot> depots;
    int time;

    Dostavister();
    void make_start_state();
    void make_decision(courier& c);
    depot& closest_depot(Point p);
    int closest_dest(Point p);
    void searching_optimal_new_order(courier& c);
    auto efficiency(courier& c, order& ord);
};

void Emulation(Dostavister& d) {
    //возвращаем элемент с минимальным временем окончания последней задачи
    //составляем решение о дальнейших действиях
    d.make_decision( *min_element(d.couriers.begin(), d.couriers.end(), 
    [](const courier & a, const courier & b) {
        if(a.task_list.back().time_end < b.task_list.back().time_end)               //??????? хуй знает в какую сторону знак ставить
            return true;
        else return false;
    }
    ));
}


Dostavister::Dostavister() {
    read_data(orders, couriers, depots);
    make_start_state();
    Emulation(*this);
}


void Dostavister::make_start_state() {
    time = START_TIME;
    sort(orders.begin(), orders.end(),
    //сортируем заказы по цене
    [](const order & a, const order & b)
    {
        return a.payment > b.payment;                                                //??????? хуй знает в какую сторону знак ставить
    });
    //сопоставляем orders_id
    for(int i = 0; i < orders.size(); i++) {
        //orders[i].current_pos = orders[i].pickup_from;
        get_ptr_order_by_order_id[orders[i].order_id] = &orders[i];
        orders[i].curr_location = orders[i].pickup_location;
        orders[i].time_of_avaliability = START_TIME;
        orders[i].curr_location_id = orders[i].pickup_point_id;
    }
    for(int i = 0; i < couriers.size(); i++) {
        couriers[i].task_list.push_back({Action::burn, 
                                        -1
                                        -1,
                                        START_TIME,
                                        START_TIME
                                        });
        couriers[i].curr_location = orders[rand() % orders.size()].curr_location;
    }
        
}


void Dostavister::make_decision(courier& c) {
    
    if(c.task_list.back().action == Action::burn) {
        searching_optimal_new_order(c);
        Emulation(*this);
        return;
    }

    if(c.task_list.back().action == Action::die)
        return;
    //----------------кладем заказ-------------------
    time = c.task_list.back().time_end;
    if(c.task_list.back().action == Action::dropoff) {
        order* taken_order = get_ptr_order_by_order_id[c.task_list.back().order_id];
        //доставили клиенту
        if(taken_order->curr_location == taken_order->dropoff_location) {
            profit += taken_order->payment;
            taken_order->closed = true;
        }
        //выбираем новый оптимальный для себя заказ
        searching_optimal_new_order(c);
        Emulation(*this);
    }
}

auto Dostavister::efficiency(courier& c, order& ord) {
        //если получилось взять заказ с точки ищем наилучшее место - 
        //либо dest либо ближайший к dest depot

        Point p_dest = ord.dropoff_location;
        depot dep =  closest_depot(p_dest);
        Point p_depot_dest = dep.location;
        int ro_to_dest = ro(ord.curr_location, p_dest);
        int ro_to_depot = ro(ord.curr_location, p_depot_dest);
        int rest = ro(p_dest, p_depot_dest);
        int t_to_dest = ro_to_dest + 10;
        int t_to_depot = ro_to_depot + 10;

        if(ro_to_dest == 0) 
            ro_to_dest = 100;
        int t_to_source = ro(c.curr_location, ord.curr_location) + 10;
        
        int wait_t_source;
        if(ord.curr_location != ord.pickup_location)
            int wait_t_source = ord.time_of_avaliability - (time + t_to_source);
        else {
            if((wait_t_source = ord.pickup_from - (time + t_to_source)) < 0)
                wait_t_source = 0;
            if(ord.pickup_to - (time + t_to_source) < 0)
                wait_t_source = INFTY;
        }

        int wait_t_dest;
        if((wait_t_dest = ord.dropoff_from - (time + t_to_source + t_to_dest + wait_t_source)) < 0)
                wait_t_dest = 0;
        if(ord.dropoff_to - (time + t_to_source + t_to_dest + wait_t_source) < 0)
                wait_t_dest = INFTY;


        if(ord.pickup_location == c.curr_location) {
            t_to_source = 0;
            wait_t_source = 0;
            t_to_source = 0;
        }

        double eff_to_dest = double(ro_to_dest) / (t_to_source + wait_t_source + t_to_dest + wait_t_dest);
        // (ro_to_depot + 10) - что-то вроде нижней оценки на среднее время ПОИСКА
        double eff_to_depot = double(ro_to_dest - rest) / (t_to_source + wait_t_source + t_to_depot + (ro_to_depot + 10));

        Point next_point_of_order, next_hop_point;
        int next_point_of_order_id, next_hop_id;
        
        int time_task1 = t_to_source + wait_t_source;
        int time_task2 = time_task1;
        if(eff_to_dest > eff_to_depot) {
            next_hop_point = ord.curr_location;
            next_hop_id = ord.curr_location_id;
            next_point_of_order_id = ord.dropoff_point_id;
            next_point_of_order = ord.dropoff_location;
            time_task2 += t_to_dest + wait_t_dest;
        }
        else {
            next_hop_point = ord.curr_location;
            next_hop_id = ord.curr_location_id;
            next_point_of_order_id = dep.point_id;
            next_point_of_order = dep.location;
            time_task2 += t_to_depot;
        }
        return make_tuple(std::max(eff_to_dest, eff_to_depot), next_point_of_order, next_hop_point, next_point_of_order_id, next_hop_id, 
            time_task1, time_task2);
}

void Dostavister::searching_optimal_new_order(courier& c) {
    Point next_point_of_order = make_pair(0,0), next_hop_point;
    int next_point_of_order_id, next_hop_id;
    int most_eff_order_index = -1;
    int time_task1, time_task2;
    double temp_eff, max_eff = - INFTY;
    for(int i = 0; i < orders.size(); i++) {
        if(!orders[i].closed) {
            tie(temp_eff, next_point_of_order, next_hop_point, next_point_of_order_id, next_hop_id, time_task1, time_task2) = 
            efficiency(c, orders[i]);
            cout << time_task1 << " " << time_task2;
            if(temp_eff > max_eff && time + time_task1 + time_task2 < END_TIME) {
                max_eff = temp_eff;
                most_eff_order_index = i;
            }
        }
    }
    if(most_eff_order_index == -1) {
        c.task_list.push_back(task{
        Action::die,
        -1,
        -1,
        time,
        time + END_TIME,
        });
        profit -= (time - START_TIME) * 2;
        return;
    }
    cout << "ok";
    c.task_list.push_back(task{
        Action::pickup,
        orders[most_eff_order_index].order_id,
        next_hop_id,
        time,
        time + time_task1,
    });

    c.task_list.push_back(task{
        Action::dropoff,
        orders[most_eff_order_index].order_id,
        next_point_of_order_id,
        time + time_task1,
        time + time_task1 + time_task2,
    });

    orders[most_eff_order_index].time_of_avaliability = time + time_task1 + time_task2;
    orders[most_eff_order_index].curr_location = next_point_of_order;
    orders[most_eff_order_index].curr_location_id = next_point_of_order_id;
    c.curr_location = next_point_of_order;
}



depot& Dostavister::closest_depot(Point p) {
    depot closest_dep;
    closest_dep.location = make_pair(-INFTY, -INFTY);
    int closest_key;
    for(auto depot : depots)
        if(ro(p, depot.second.location) < ro(p, closest_dep.location)) {
            closest_dep = depot.second;
            closest_key = depot.first;
        }
    return depots[closest_key];
}

//возвращает индекс на ижайший dest иначе -1
int Dostavister::closest_dest(Point p) {
    order closest_dest;
    int i = 0, index = -1;
    for(auto order : orders) {
        i++;
        if(10 + ro(p, order.dropoff_location) + time > order.dropoff_from 
            && 10 + ro(p, order.dropoff_location) + time < order.dropoff_to
            && (index == -1 
            || ro(p, order.dropoff_location) < ro(p, closest_dest.dropoff_location))) {
                closest_dest = order;
                index = i;
            }
    }
    return index;
}