#ifndef _IO_INTERFACE_H_
#define _IO_INTERFACE_H_
#include "GR.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern int MSL;
extern int l_dia;
extern int cell_coe;
extern int coarse_x_length;
extern int coarse_y_length;
extern int fine_x_length;
extern int fine_y_length;
void output_gds(std::string filename, GR* router);
void output_CNF(std::vector<std::string>& s_vec,const std::vector<std::map<int,std::pair<Line,Line>>>& restnet_line );
bool Satisfiable(std::string &sat_out);
std::map<int,Detour_info> Load_MAXSAT_Output(const std::map<int,std::vector<std::pair<Detour_info,Detour_info>>>& coarse_detour_info, const std::map<int,int>& ID_restnet);
void output_lp(int layer, std::string output_name, std::map<int,Edge> edge_table, std::vector<Cluster>& GR_unit, std::map<int, std::vector<Segment>>& seg_list, std::vector<Net>& net_list);
void Load_LP_Output(std::string sol_file_name);
#endif