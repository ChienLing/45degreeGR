#include "Structure.h"
Pin::Pin():pin_ID(-1), net_ID(-1) {}
Pin::Pin(int pID, int nID, std::pair<int,int> p, std::string pName, std::string nName):        pin_ID(pID), net_ID(nID), pin_name(pName), net_name(nName) {
    real_pos.x=p.x;
    real_pos.y=p.y;
}
void Pin::change_pos(int x, int y){
    real_pos.first = x;
    real_pos.second = y;
}


Net::Net(int nID, std::string nName){
    net_ID = nID;
    net_name = nName;
}

