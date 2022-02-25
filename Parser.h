#ifndef _PARSER_H_
#define _PARSER_H_
#include <bits/stdc++.h>
#include "BLMR.h"
class Parser{
    public:
    BLMR* router;
    std::string iFile_name;
    Parser();
    Parser(BLMR* r, std::string ifile):router(r), iFile_name(ifile){}
    // void read_obs();
    // void read_diff();
    void read_group();
    void read_netlist();
    void read_inputfile();
    void read_inputfile(const std::string filename);
    void print_netlist();
};






#endif