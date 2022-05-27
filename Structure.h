#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
#define X first
#define Y second
#define TOP 0
#define BOT 2
#define RIGHT 1
#define LEFT 3
#define TR 4
#define BR 5
#define BL 6
#define TL 7

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
struct Coor
{
    int x;
    int y;
    int z;
    Coor(){}
    Coor(int _z, int _x, int _y):x(_x),y(_y),z(_z){}
    void setup(int _z, int _x, int _y) {
        x=_x;
        y=_y;
        z=_z;
    }
    bool operator== (const Coor &coor) const {
        return (z==coor.z && x==coor.x && y==coor.y);
    }
    bool operator!= (const Coor &coor) const {
        return !(z==coor.z && x==coor.x && y==coor.y);
    }
    bool operator< (const Coor &coor) const {
        if (z<coor.z)
            return true;
        else if (z==coor.z && x<coor.x)
            return true;
        else if (z==coor.z && x==coor.x && y<coor.y)
            return true;
        return false;

    }
    static bool less_x(const Coor c1, const Coor c2) {
        return c1.x > c2.x;
    }    
    static bool greater_x(const Coor c1, const Coor c2) {
        return c1.x < c2.x;
    } 
    static bool less_y(const Coor c1, const Coor c2) {
        return c1.y > c2.y;
    }
    static bool greater_y(const Coor c1, const Coor c2) {
        return c1.y < c2.y;
    }
    // static bool less_x_greater_y(const Coor c1, const Coor c2) {
    //     if (c1.x < c2.x)
    //         return true;
    //     else if (c1.x == c2.x && c1.y>c2.y)
    //         return true;
    //     return false;
    // }
};

struct Boundary
{
    int top;
    int bot;
    int right;
    int left;
    std::string name;// if boundary is used for ddr, it means ddr name .else it is empty
    Boundary():top(0), right(0), left(1e8), bot(1e8){}
    Boundary(int lbx,int lby,int rtx, int rty):left(lbx), bot(lby), right(rtx), top(rty){}
    void setup(int lbx,int lby,int rtx, int rty) {
        left = lbx; 
        bot = lby;
        right = rtx;
        top = rty;
    }
};

class Cell{
    public:
    int cell_r;
    int cell_cap;
    int cell_ini_demand;
    int cell_demand;
    int width;
    int cluster;
    int investigated;//record the region which visited by cluster finding forbidden region
    bool forbidden_region;
    double cost;
    std::tuple<int,int,int> next;
    std::tuple<int,int,int> last;
    std::vector<bool> dir;//0 TOP 1 RIGHT 2 BOT 3 LEFT 4 TR 5 BR 6 BL 7 TL
    std::vector<int> net;
    std::vector<Boundary> obs;
    std::vector<double> edge_r;//0 TOP 1 RIGHT 2 BOT 3 LEFT 4 TR 5 BR 6 BL 7 TL
    std::vector<double> edge_cap;//0 TOP 1 RIGHT 2 BOT 3 LEFT 4 TR 5 BR 6 BL 7 TL
    std::vector<double> edge_demand;//0 TOP 1 RIGHT 2 BOT 3 LEFT 4 TR 5 BR 6 BL 7 TL
    std::vector<double> edge_ini_demand;//0 TOP 1 RIGHT 2 BOT 3 LEFT 4 TR 5 BR 6 BL 7 TL
    std::vector<double> edge_temp_demand;
    Cell(/* args */);
    Cell(int w, double c);
    void setup(int w, double cap);
    void update_cost();
    void update_recourse();
    bool operator<(const Cell*b)const {
        return this->cost < b->cost;
    }
};

class Pin{
    public:

    int pin_ID;
    int net_ID;
    int layer;
    int shift;
    int esti_escape_routing_length;
    bool CPU_side;
    bool ignore;
    int escape_dir;
    std::pair<int,int> real_pos;
    std::pair<int,int> coarse_coor;
    std::pair<int,int> fine_coor;
    std::string pin_name;
    std::string net_name;
    std::string comp_name;
    std::string ddr_name;
    std::vector<Coor> ER_coarse_cell;
    std::vector<Coor> ER_fine_cell;
    Pin();
    Pin(int pID, int nID, std::pair<int,int> p, std::string pName, std::string nName, std::string cName);
    bool dec_compare_x(Pin _p);
    std::string get_pinname(){return pin_name;};
  
    void change_pos(int,int);
};

class Net{
    public:
    std::vector<std::string> belong_group;
    int merged_group_ID;
    int gn_ID;
    int sub_g_ID;
    // std::vector<Pin> pinlist;
    int net_ID;
    int cluster_relative_idx;
    bool isdiff;
    bool is2pin_net;
    bool ignore;
    double demand_val;
    int routed_wirelength;
    std::string net_name;
    std::vector<int> net_pinID;
    Net():net_ID(-1),gn_ID(-1),cluster_relative_idx(-1),sub_g_ID(-1),merged_group_ID(-1),isdiff(false),ignore(false), is2pin_net(true),routed_wirelength(0){};
    Net(int, std::string);
    std::string get_netname(){return net_name;};
    std::vector<std::tuple<int,int,int>> coarse_GR_path;
    void update_wirelength(std::vector<Pin>& pin_list);
};

class Path_node {
    public:
    int bounded_length;
    double routed_wirelength;
    double esti_routing_length;
    double cost;
    std::vector<Coor> path;
    Path_node():cost(1e8),esti_routing_length(0),routed_wirelength(0),bounded_length(0){}
    // int next_coor;
};

class Sub_gn {
    public:
    int gn_ID;
    int sub_g_ID;
    int lack;
    int max_wl;
    double demand_val;
    std::vector<int> net;
    Coor cpu_coor;
    Coor ddr_coor;
    Path_node path_node;
    Sub_gn():gn_ID(-1),sub_g_ID(-1),lack(0),max_wl(0){}
    void update_demand_val(const std::vector<Net>&, int mwl);
    bool operator<(const Sub_gn& _sgn)const {
        return max_wl<_sgn.max_wl;
    }
};

class Group_net{
    public:
    // std::vector<Pin> pinlist;
    int max_wl;
    std::vector<Sub_gn> sgn;//part by fanout pos
    int max_ER_length;
    int min_ER_length;
    int merge_group_ID;
    // std::vector<std::vector<int>> net_pinID;
    Group_net():merge_group_ID(-1),max_wl(0){};
    Group_net(int mg_ID):merge_group_ID(mg_ID){}
    void initialize(std::vector<Net>& net_list);
};

class Cluster {
    public:
    std::vector<int> net;
    int cluster_relative_idx;
    double demand_val;
    Coor start, end;
    std::vector<Coor> path;
    std::vector<Coor> CW_path;
    std::vector<Coor> CCW_path;

    Cluster():cluster_relative_idx(-1){}
};

class Drc{
    public:
    int spacing;
    int wire_width;
    std::string drc_name;
    Drc();
    Drc(std::string, int, int);
};

class Diff{
    public:
    std::string diff_name;
    std::vector<std::string> netname_list;
    std::vector<int> netID_list;
    Diff(){}
};

class Obs {
    public:
    Boundary bd;
    // std::pair<int,int> LB;
    // std::pair<int,int> RT;
    std::pair<int,int> center;
    int layer;
    int net;
    bool in_routing_region;
    Obs():layer(-1),net(-1), in_routing_region(false){}
    void setup(Boundary _bd, int l, int n);
};

class Line {
    public:
    int net;
    std::pair<int,int> ep1,ep2;
    Line(){}
    Line(int n, std::pair<int,int> ep1, std::pair<int,int>ep2){
        net = n;
        this->ep1 = ep1;
        this->ep2 = ep2;
    }
    void setup(int n, std::pair<int,int> ep1, std::pair<int,int>ep2) {
        
        net = n;
        this->ep1 = ep1;
        this->ep2 = ep2;
    }

};

class Detour_info {
    public:
    int layer;
    int rest_net;
    int detour_dist;
    bool CPU_side;
    std::vector<Coor> path_coor;
    Detour_info():detour_dist(1e8){}
    Detour_info(int nid, int l):detour_dist(1e8), layer(l), rest_net(nid){}
    void print() {
        printf("rest net:%d  detour_dist:%d layer:%d\n",rest_net,detour_dist,layer);
        for (auto it=path_coor.begin(); it!=path_coor.end(); it++) {
            printf("(%d,%d,%d)->",it->z,it->x,it->y);
        }
        printf("end\n");
    }
};

class Edge {
    public:
    int length;
    double cost;
    double capacity;
    double resource;
    double demand;
    double ini_demand;
    // std::vector<bool> dir;//0 TOP 2 RIGHT 4 BOT 6 LEFT
    std::vector<int> net;
    Edge(){
        length=0;
        cost=0;
        capacity=0;
        resource=0;
        demand=0;
        ini_demand=0;
    }
};
struct compare {
    bool operator()(const Path_node & a, const Path_node & b)
    {
        // if(a.salary==b.salary)
        // {
        //     return a.age>b.age;
        // } else {
        //     return a.salary>b.salary;
        // }
        // return (a.routed_wirelength+a.cost) < (b.routed_wirelength+b.cost);
        return (a.cost+a.esti_routing_length+a.routed_wirelength) > (b.cost+b.esti_routing_length+b.routed_wirelength);
    }
};
#endif