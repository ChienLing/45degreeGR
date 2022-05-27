#ifndef _PARSER_H_
#define _PARSER_H_
#include "GR.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern int obs_ext;
class Parser{
    public:
    GR* router;
    std::string iFile_name;
    Parser();
    Parser(GR* r, std::string ifile):router(r), iFile_name(ifile){}
    void read_obs();
    void read_drc();
    void read_diff();
    void read_layer();
    void read_group();
    void read_netlist();
    void read_inputfile();
    void read_inputfile(const std::string filename);
    void print_obs();
    void print_drc();
    void print_diff();
    void print_layer();
    void print_group();
    void print_netlist();
};






#endif