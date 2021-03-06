#ifndef _FUNC_H_
#define _FUNC_H_

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include "Structure.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern double pitch_extend_coe;
// namespace my
// {
    struct DPnode {
    int length;
    double num;
    bool upward;
    bool backward;
    DPnode():length(0), num(0), upward(false), backward(false){}
};
struct decode_node {
    int ID;
    int escape_dir;
    int last_or_next;
    std::pair<int,int> list_coor;
    decode_node(){}
    void setup(int id, int e_d, int l_or_n, std::pair<int,int> coor) {
        ID = id;
        last_or_next = l_or_n ; 
        escape_dir = e_d;
        list_coor.first = coor.first; 
        list_coor.second = coor.second;
    }
    bool operator==(const decode_node& _dn) const {
        return (this->ID==_dn.ID && this->last_or_next==_dn.last_or_next);
    }
};

struct vertex
{
public:
    int ID;
    int comp;
    vertex():ID(-1), comp(-1){}
    vertex(int id):ID(id), comp(-1){}
};

void SplitString(const std::string& s, const std::string& c, std::vector<std::string>& v);
bool out_of_bdy(const Coor& coor,const Coor& diagonal_coor);
int find_root(std::vector<int>& root_table, int idx);
bool occupy (Boundary b1, Boundary b2);
int overlap_area(Boundary b1, Boundary b2);
int overlap_h(Boundary b1, Boundary b2);
int overlap_w(Boundary b1, Boundary b2);
std::vector<int> LCS(const std::vector<std::vector<int>>& v1, const std::vector<std::vector<int>>& v2);
std::vector<int> LCS(const std::vector<int>& v1, const std::vector<int>& v2);
// std::vector<int> LCS(const std::vector<std::vector<int>>& v1, const std::vector<std::vector<int>>& v2, 
//                      const std::vector<std::pair<int,int>>& dp_list, std::vector<std::pair<int,int>>& decode_table);
bool coor_less_x_less_y(const Coor _c1, const Coor _c2)  ;         
bool pin_less_x (const Pin* _p1, const Pin* _p2) ;
bool pin_greater_x (const Pin* _p1, const Pin* _p2) ;
bool pin_less_y (const Pin* _p1, const Pin* _p2) ;
bool pin_greater_y (const Pin* _p1, const Pin* _p2) ;
// }
bool Bdy_less_x (const Boundary _b1, const Boundary _b2);
bool Bdy_greater_x (const Boundary _b1, const Boundary _b2);
bool Bdy_less_y (const Boundary _b1, const Boundary _b2);
bool Bdy_greater_y (Boundary const & _b1, Boundary const & _b2);
std::pair<std::vector<std::pair<int,int>>,std::vector<std::pair<int,int>>> 
                    LCS(const std::vector<std::vector<int>>& v1, const std::vector<std::vector<int>>& v2, 
                        const std::vector<std::pair<int,int>>& dp_list, std::vector<int>& ED_table1,std::vector<int>& ED_table2, std::vector<int>& location1, std::vector<int>& location2);

std::pair<std::vector<std::pair<int,int>>,std::vector<std::pair<int,int>>> 
                    LCS( std::vector<decode_node> v1,  std::vector<decode_node> v2,
                        std::map<int,double>& weight, const std::vector<std::pair<int,int>>& diff_pair, 
                        std::vector<int>& location1, std::vector<int>& location2);
bool line_crossing(const Line& line1, const Line& line2);

std::set<int> test_crossing(const std::map<int,Line>& line_list, const Line& line);
std::set<int> test_crossing(const std::vector<Line>& line_list, const Line& line);
std::vector<Line> test_crossing_Line(const std::vector<Line>& line_list, const Line& line);
#endif