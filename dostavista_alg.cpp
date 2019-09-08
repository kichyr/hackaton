#include"data_classes.cpp"
#include <algorithm>
#include<vector>
#include<map>

#define START_TIME 360
#define INFTY 999999

class Dostavister {
public:
    vector<order> orders;
    vector<courier> couriers;
    //key is order id
    map<int, order*> get_ptr_order_by_order_id;
    //key is Point id
    map<int, depot> depots;
    int time;

    Dostavister();
    void make_start_state();
    void make_decision(courier c);
    depot& closest_depot(Point p);
    int closest_dest(Point p);
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
    //сопоставляем orders_id
    for(int i = 0; i < orders.size(); i++) {
        //orders[i].current_pos = orders[i].pickup_from;
        get_ptr_order_by_order_id[orders[i].order_id] = &orders[i];
    }
    for(int i = 0; i < couriers.size(); i++) {
        couriers[i].task_list.push_back({Action::pickup, 
                                        orders[i].order_id,
                                        orders[i].pickup_from,
                                        START_TIME
                                        });
        orders[i].availability = false;
    }
        
}


void Dostavister::make_decision(courier c) {
    //берем заказ и ищем куда его доставить
    order* taken_order;
    if(c.task_list.back().action == Action::pickup) {
        //значит берем заказ у получателя
        if(depots.find(c.task_list.back().point_id) == depots.end()) {
            taken_order = get_ptr_order_by_order_id[c.task_list.back().order_id];
            get_ptr_order_by_order_id[c.task_list.back().order_id]->availability = false;
        //значит берем заказ на складе
        } else {
            auto orders_in_depot = depots[c.task_list.back().point_id].orders_here;
            if(orders_in_depot.empty()) {
                c.task_list.push_back(task {
                Action::wait,
                0,
                c.task_list.back().point_id,
                time,
                time + 5
                });
                return;
            }
            else {
                //берем самый дорогой заказ
                int expensive_index = orders_in_depot[0];
                for(int x : orders_in_depot)
                    if(orders[x].payment > orders[expensive_index].payment)
                        expensive_index = x;
                taken_order = &orders[expensive_index];
            }
        }
        //если получилось взять заказ с точки ищем наилучшее место - 
        //либо dest либо ближайший к dest depot

        Point p_dest = (*taken_order).dropoff_location;
        depot dep =  closest_depot(p_dest);
        Point p_depot_dest = dep.location;
        int ro_to_dest = ro(c.curr_location, p_dest);
        int ro_to_depot = ro(c.curr_location, p_depot_dest);
        int rest = ro(p_dest, p_depot_dest);
        int t_to_dest = ro_to_dest + 10;
        int t_to_depot = ro_to_depot + 10;

        int wait_time = ((*taken_order).dropoff_from - (time + t_to_dest));
        //!!!!пока считаем,что всегда доставит до завершения окна, просто дропнем в противном случае
        if(wait_time < 0)
            wait_time = 0;
        double eff_to_dest = double(ro_to_dest) / (t_to_dest + wait_time);
        // (ro_to_depot + 10) - что-то вроде нижней оценки на среднее время ПОИСКА
        double eff_to_depot = double(ro_to_dest - rest) / (t_to_depot + (ro_to_depot + 10));
        //задаем новую задачу и переводим алгоритм в следующую итерацию
        if(eff_to_dest < eff_to_depot) {
            c.task_list.push_back(task {
                Action::dropoff,
                (*taken_order).order_id,
                dep.point_id,
                time,
                time + t_to_depot
            });
        } else {
            c.task_list.push_back(task {
                Action::dropoff,
                (*taken_order).order_id,
                (*taken_order).dropoff_point_id,
                time,
                time + t_to_dest + wait
            });
        }
    }
    //
}

depot& Dostavister::closest_depot(Point p) {
    depot closest_dep;
    closest_dep.location = make_pair(-INFTY, -INFTY);
    int closest_key;
    for(auto depot : depots)
        if(ro(p, depot.second.location) < ro(p, closest_dep.locagittion)) {
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