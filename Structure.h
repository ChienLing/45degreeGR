#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
#define x first
#define y second
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
class Pin
{
public:

    int pin_ID;
    int net_ID;
    double esti_escape_routing_length;
    bool CPU_side;
    int escape_dir;
    std::pair<int,int> real_pos;
    std::pair<int,int> coarse_coor;
    std::pair<int,int> fine_coor;
    std::string pin_name;
    std::string net_name;
    std::string comp_name;
    std::string ddr_name;
    Pin();
    Pin(int pID, int nID, std::pair<int,int> p, std::string pName, std::string nName, std::string cName);
    bool dec_compare_x(Pin _p);
    std::string get_pinname(){return pin_name;};
  
    void change_pos(int,int);
};

class Net
{
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

struct Boundary
{
    int top;
    int bot;
    int right;
    int left;
    std::string name;// if boundary is used for ddr, it means ddr name .else it is empty
    Boundary():top(0), right(0), left(1e8), bot(1e8){}
    Boundary(int lbx,int lby,int rtx, int rty):left(lbx), bot(lby), right(rtx), top(rty){}
};

class Drc
{
private:
    /* data */
public:
    int spacing;
    int wire_width;
    std::string drc_name;
    Drc();
    Drc(std::string, int, int);
};

class Diff
{
private:
    /* data */
public:
    std::string diff_name;
    std::vector<std::string> netname_list;
    std::vector<int> netID_list;
    Diff(){}
};

class Obs
{
private:
    /* data */
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

class Cell
{
private:
    /* data */
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


#endif