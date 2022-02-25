#ifndef _BLMR_H_
#define _BLMR_H_
#include <bits/stdc++.h>
#include "Structure.h"
class BLMR
{
private:
public:
    std::vector<Net> net_list;
    std::vector<Pin> pin_list;
    std::map<std::string,int> net_NameID;
    std::map<std::string,int> pin_NameID;
    std::map<std::string,std::vector<Net>> group;
    std::map<std::string,std::vector<Net>> merged_group;
    BLMR();
};





#endif