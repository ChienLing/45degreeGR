// #include <bits/stdc++.h>
#include "Parser.h"
#include "IO_interface.h"
using namespace std;
int cell_coe;//cell width = pitch*cell coe
int level_coe;//coe between coarse and fine
int split_coe;
int sub_group_size;
int tolerance;
int main(int argc, char** argv) {
    MIN_SPACING = 400;
    WIRE_WIDTH = 400;
    cell_coe = 10;
    level_coe = 2;
    split_coe = 8;
    obs_ext = 2000;
    tolerance = 100;
    sub_group_size = 6;
    BLMR* router = new BLMR;
    string input_file = "./input2/2/2";
    Parser parser(router,input_file);
    parser.read_inputfile();
    router->merge_group();
    router->construct_3DGCells();
    router->partition();
    output_gds(input_file, router);
    // parser.read_netlist();
    // parser.print_netlist();
    // parser.read_group();
    // parser.read_drc();
    // parser.read_diff();
    // parser.read_layer();
    // parser.read_obs();
    // parser.print_obs();
}