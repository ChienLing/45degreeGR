#ifndef _IO_INTERFACE_H_
#define _IO_INTERFACE_H_
#include "GR.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern int MSL;
extern int l_dia;
extern double cell_coe;
extern int coarse_x_length;
extern int coarse_y_length;
extern int fine_x_length;
extern int fine_y_length;
extern int fine_y_length;
extern int slack_tol;
void output_gds(std::string filename, GR* router);
void output_CNF(std::vector<std::string>& s_vec,const std::vector<std::map<int,std::pair<Line,Line>>>& restnet_line );
bool Satisfiable(std::string &sat_out);
std::map<int,Detour_info> Load_MAXSAT_Output(const std::map<int,std::vector<std::pair<Detour_info,Detour_info>>>& coarse_detour_info, const std::map<int,int>& ID_restnet);
void output_ideal_lp(int layer, std::string output_name, std::map<int,Edge> edge_table, std::vector<Cluster>& GR_unit, std::map<int, std::vector<Segment>>& seg_list, std::vector<Net>& net_list);
void output_passable_lp(int layer, std::string output_name, std::map<int,Edge> edge_table, std::vector<Cluster>& GR_unit, std::map<int,std::vector<Segment>>& seg_list, std::vector<Net>& net_list);
bool Load_ideal_LP_Output(std::string sol_file_name, std::map<int,std::vector<Segment>>& seg_list, std::vector<Cluster>& _GR_unit);
std::vector<int> Load_passable_LP_Output(int layer, std::string sol_file_name, std::map<int,std::vector<Segment>>& seg_list, std::vector<Cluster>& _GR_unit);
void output_SER(GR* router, std::string file_name);
#endif