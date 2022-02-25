#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
#define x first
#define y second
#include <bits/stdc++.h>

class Pin
{
private:
    /* data */
public:

    int pin_ID;
    int net_ID;
    std::pair<int,int> real_pos;
    std::string pin_name;
    std::string net_name;
    Pin();
    Pin(int pID, int nID, std::pair<int,int> p, std::string pName, std::string nName);
    std::string get_pinname(){return pin_name;};
  
    void change_pos(int,int);
};

class Net
{
private:

public:
    // std::vector<Pin> pinlist;
    int net_ID;
    std::string net_name;
    std::vector<int> net_pinID;
    Net():net_ID(-1){};
    Net(int, std::string);
    std::string get_netname(){return net_name;};
    
};




#endif