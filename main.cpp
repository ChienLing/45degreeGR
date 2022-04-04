// #include <bits/stdc++.h>
#include "Parser.h"
#include "IO_interface.h"
using namespace std;
int cell_coe;//coarse cell width = pitch*cell coe
int level_coe;//coe between coarse and fine
int split_coe;
int sub_group_size;
int tolerance;
int coarse_x_length;
int coarse_y_length;
int fine_x_length;
int fine_y_length;
double pitch_extend_coe;
int main(int argc, char** argv) {
    MIN_SPACING = 400;
    WIRE_WIDTH = 400;
    cell_coe = 10;
    level_coe = 2;
    split_coe = 8;
    obs_ext = 2000;
    tolerance = 100;
    sub_group_size = 6;
    pitch_extend_coe = 1.6;
    coarse_x_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe;
    coarse_y_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe;
    fine_x_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe/level_coe;
    fine_y_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe/level_coe;
    BLMR* router = new BLMR;
    string input_file = "./input2/outb_14/b_14";
    Parser parser(router,input_file);
    parser.read_inputfile();
    router->merge_group();
    router->construct_3DGCells();
    router->partition();
    output_gds(input_file, router);
}