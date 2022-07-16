#include "Structure.h"
using namespace std;
Pin::Pin():pin_ID(-1), net_ID(-1), CPU_side(false),ignore(false){}
Pin::Pin(int pID, int nID, std::pair<int,int> p, std::string pName, std::string nName, std::string cName):  \
                       pin_ID(pID), net_ID(nID), pin_name(pName), net_name(nName), comp_name(cName), CPU_side(false),ignore(false) {
    real_pos.X=p.X;
    real_pos.Y=p.Y;
    escape_dir=-1;
}
void Pin::change_pos(int x, int y){
    real_pos.first = x;
    real_pos.second = y;
}


Net::Net(int nID, std::string nName):merged_group_ID(-1),gn_ID(-1),sub_g_ID(-1),isdiff(false),cluster_relative_idx(-1),is2pin_net(true),ER_routed_wirelength(0), AR_routed_wirelength(0), ignore(false){
    net_ID = nID;
    net_name = nName;
    slack_wirelength = 0;
    WCS = true;
    order = -1;
}
void Net::update_wirelength(vector<Pin>& pin_list) {
    // printf("update_wirelength nid %d ini rwl: %d\n",net_ID,ER_routed_wirelength);
    int rwl = ER_routed_wirelength;
    for (auto pid=net_pinID.begin(); pid!=net_pinID.end(); pid++) {
        int _pid = *pid;
        int temp = pin_list.at(*pid).esti_escape_routing_length;
        // printf("update_wirelength pid %d pin ERL: %d\n",*pid,temp);
        ER_routed_wirelength += pin_list.at(*pid).esti_escape_routing_length;
    }
    // printf("update_wirelength nid %d rwl: %d\n",net_ID,ER_routed_wirelength);
}


Drc::Drc(std::string _name, int s, int w):drc_name(_name), spacing(s), wire_width(w) {}
Drc::Drc() {}


void Obs::setup(Boundary _bd,int l,int n) {
    bd = _bd;
    layer = l;
    net = n;
}


Cell::Cell() {
    cluster = -1;
    investigated = -1;
    forbidden_region = false;
    history_cost = 0;
    edge_r.resize(8,0);
    edge_cap.resize(8,0);
    edge_demand.resize(8,0);
    edge_ini_demand.resize(8,0);
    edge_temp_demand.resize(8,0);
    }
Cell::Cell(int w, double cap){
    width = w;
    cluster = -1;
    forbidden_region = false;
    investigated = -1;
    history_cost = 0;
    edge_r.resize(8,0);
    edge_cap.resize(8,cap);
    edge_demand.resize(8,0);
    edge_ini_demand.resize(8,0);
    edge_temp_demand.resize(8,0);
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
void Cell::update_recourse() {
    for (int i=0; i<8; i++) {
        // auto temp1 = edge_r.at(i);
        // auto temp2 = edge_cap.at(i);
        // auto temp3 = edge_demand.at(i);
        edge_r.at(i) = edge_cap.at(i)-edge_demand.at(i)-edge_ini_demand.at(i);
    }
}

void Sub_gn::update_demand_val(const std::vector<Net>& net_list, int mwl) {
    int temp(0);
    for (auto nid=net.begin(); nid!=net.end(); nid++) {
        if (temp < net_list.at(*nid).ER_routed_wirelength) {
            temp = net_list.at(*nid).ER_routed_wirelength;
        }
        lack += mwl-net_list.at(*nid).ER_routed_wirelength;
    }
    demand_val=net.size();
    max_wl = temp;
}

void Group_net::initialize(vector<Net>& net_list) {
    int mwl(max_wl);
    for (auto pos_g=sgn.begin(); pos_g!=sgn.end(); pos_g++) {
        for (auto nid=pos_g->net.begin(); nid!=pos_g->net.end();nid++) {
            if (net_list.at(*nid).ER_routed_wirelength > max_wl)
                max_wl = net_list.at(*nid).ER_routed_wirelength;            
        }
    }
    int total_lack(0);
    for (auto pos_g=sgn.begin(); pos_g!=sgn.end(); pos_g++) {
        pos_g->lack = 0;
        for (auto nid=pos_g->net.begin(); nid!=pos_g->net.end(); nid++) {
            if (net_list.at(*nid).ER_routed_wirelength > pos_g->max_wl)
                pos_g->max_wl = net_list.at(*nid).ER_routed_wirelength;
            total_lack+=max_wl-net_list.at(*nid).ER_routed_wirelength;
            pos_g->lack+=max_wl-net_list.at(*nid).ER_routed_wirelength;
        }
    }
}


Cluster::Cluster():ripup_num(0),cluster_relative_idx(-1),
                   CW_idx(-1), CCW_idx(-1),last_AR_routed_WL(-1),cost(0),remain_slack(0),route_order(0){}
Cluster::Cluster(int oci, int cri, Coor s, Coor t, double d_v):
                ori_c_idx(oci),cluster_relative_idx(cri), start(s), 
                end(t), demand_val(d_v),CW_idx(-1), CCW_idx(-1),cost(0),remain_slack(0),route_order(0){}
Cluster::Cluster(int oci, int cri, int r_n, int m_s, double d_v):
                ori_c_idx(oci),cluster_relative_idx(cri),ripup_num(r_n),
                max_slack(m_s),demand_val(d_v), CW_idx(-1), CCW_idx(-1),cost(0),remain_slack(0),route_order(0){}