#include "Structure.h"
using namespace std;
Pin::Pin():pin_ID(-1), net_ID(-1) {}
Pin::Pin(int pID, int nID, std::pair<int,int> p, std::string pName, std::string nName, std::string cName):  \
                       pin_ID(pID), net_ID(nID), pin_name(pName), net_name(nName), comp_name(cName), CPU_side(false) {
    real_pos.x=p.x;
    real_pos.y=p.y;
    escape_dir=-1;
}
void Pin::change_pos(int x, int y){
    real_pos.first = x;
    real_pos.second = y;
}


Net::Net(int nID, std::string nName):merge_group_ID(-1),isdiff(false),is2pin_net(true){
    net_ID = nID;
    net_name = nName;
}


Drc::Drc(std::string _name, int s, int w):drc_name(_name), spacing(s), wire_width(w) {}
Drc::Drc() {}

void Obs::setup(Boundary _bd,int l,int n) {
    bd = _bd;
    layer = l;
    net = n;
}

Cell::Cell() {
    edge_cap.resize(8,0);
    edge_demand.resize(8,0);
    edge_ini_demand.resize(8,0);
    }

Cell::Cell(int w, double cap){
    width = w;
    edge_cap.resize(8,cap);
    edge_demand.resize(8,0);
    edge_ini_demand.resize(8,0);
}

void Cell::setup(int w, double cap) {
    // printf("Cap %f  ini_demand %f  demand %f\n",edge_cap[1],edge_ini_demand[1], edge_demand[1]);
    cout << cap << endl;
    for (int i=0; i<8; i++) {
        edge_cap[i]=cap;
        cout << i << " " << edge_cap[i] << endl;
        edge_demand[i]=0;
        edge_ini_demand[i]=0;
    }
    // printf("Cap %f  ini_demand %f  demand %f\n",edge_cap[1],edge_ini_demand[1], edge_demand[1]);
    }
void Cell::update_cost(){}