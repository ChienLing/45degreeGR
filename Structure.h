#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
#define X first
#define Y second
#define TOP 0
#define BOT 2
#define RIGHT 1
#define LEFT 3

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
    double cost;
    std::tuple<int,int,int> next;
    std::tuple<int,int,int> last;
    std::vector<int> net;
    std::vector<Boundary> obs;
    std::vector<double> edge_r;
    std::vector<double> edge_cap;
    std::vector<double> edge_demand;
    std::vector<double> edge_ini_demand;
    Cell(/* args */);
    Cell(int w, double c);
    void setup(int w, double cap);
    void update_cost();

};

class Pin{
    public:

    int pin_ID;
    int net_ID;
    int layer;
    int shift;
    int esti_escape_routing_length;
    bool CPU_side;
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
    // std::vector<Pin> pinlist;
    int net_ID;
    int merge_group_ID;
    bool isdiff;
    bool is2pin_net;
    std::string net_name;
    std::vector<int> net_pinID;
    Net():net_ID(-1),merge_group_ID(-1),isdiff(false),is2pin_net(true){};
    Net(int, std::string);
    std::string get_netname(){return net_name;};
    std::vector<std::tuple<int,int,int>> GR_path;
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
#endif