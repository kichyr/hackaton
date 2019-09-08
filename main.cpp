#include"dostavista_alg.cpp"
#include<vector>
using namespace std;


ostream& operator<< (ostream& os, const order& ord) {
    return os << "{" << ord.order_id << "}";
}

int main() {
    Dostavister D;
    std::ofstream file("output.json");
    file << make_json(D.couriers);
    cout << D.orders;
}