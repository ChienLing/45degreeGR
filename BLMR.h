#ifndef _BLMR_H_
#define _BLMR_H_
#include "Structure.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern int cell_coe;
extern int level_coe;
extern int split_coe;
extern int sub_group_size;
extern int tolerance;
extern int coarse_x_length;
extern int coarse_y_length;
extern int fine_x_length;
extern int fine_y_length;
extern double pitch_extend_coe;
class BLMR
{
private:
public:
    int layer_num;
    int can_use_layer_num;
    int coarse_x_size, coarse_y_size;
    int fine_x_size, fine_y_size;
    Boundary total_boundary;
    std::string CPU_name;
    std::vector<std::string> DDR_name;
    std::map<std::string,Boundary> comp_boundary;
    std::map<std::string,int> ddr_escape;
    std::map<std::string,int> cpu_escape;
    std::vector<Net> net_list;
    std::vector<Pin> pin_list;
    std::vector<Drc> drc_list;
    std::vector<Obs> obs_list;
    std::vector<Diff> diff_list;
    std::vector<std::string> layer_list;
    std::vector<std::vector<int>> group_list;

    std::vector<std::vector<int>> merged_groupID;//big group:small group ID
    std::vector<std::vector<int>> merged_group;//new group:netID
    std::vector<std::vector<int>> merged_group_by_ddr;//new group:netID
    std::vector<std::vector<int>> merged_group_by_ddr_and_cpu_region;//new group:netID
    std::vector<std::vector<int>> all_group_net;//0:2pin 1:else
    //part merged_group into non-crossing group  group:netID
    std::vector<std::vector<std::vector<int>>> parted_group;//merged group : sub group : net
    std::vector<std::vector<int>> coarse_group;
    std::vector<std::vector<int>> fine_group;
    std::vector<int> layer_can_use;
    std::map<std::string,int> net_NameID;
    std::map<std::string,int> pin_NameID;
    std::map<std::string,int> diff_NameID;
    std::map<std::string,int> layer_NameID;
    std::map<std::string,int> group_NameID;
    

    
    std::vector<std::vector<std::vector<std::pair<int,int>>>> CPU_LCS_group;
    std::vector<std::vector<std::vector<std::pair<int,int>>>> DDR_LCS_group;
    std::vector<std::vector<std::map<int,Line>>> LCS_group_net_line_align_bdy;



    std::vector<std::vector<std::vector<Cell>>> coarse_GRcell;
    std::vector<std::vector<std::vector<Cell>>> fine_GRcell;
    BLMR();
    void update_cell_demand(int, Cell&, std::pair<int,int> , Boundary);
    void merge_group();
    void planning();
    void partition();
    void partition_by_position(std::vector<std::vector<int>>& group);
    void partition_by_size(std::vector<std::vector<int>>& group);
    std::vector<std::vector<std::pair<std::vector<std::pair<int,int>>,std::vector<std::pair<int,int>>>>> partition_by_LCS(std::vector<std::vector<int>>& group, std::vector<std::vector<int>>& rest_of_net);
    void construct_3DGCells();
    std::vector<std::string> sort_ddr(const std::map<std::string, std::vector<Pin*>>& ddr_ddrpin);
    void Astar(std::vector<std::vector<std::vector<Cell>>>& GRmap, std::pair<int,int> star, int net_num);
    void sort_LCS_input( std::map<std::string, std::vector<Pin*>>& ddr_ddrpin,  std::map<std::string, std::vector<Pin*>>& ddr_cpupin, std::map<std::string, std::vector<std::vector<Pin*>>>& ddr_sort_ddrpin, std::map<std::string, std::vector<std::vector<Pin*>>>& ddr_sort_cpupin);
    void sort_LCS_input( std::map<std::string, std::vector<Pin*>>& ddr_ddrpin,  std::map<std::string, std::vector<Pin*>>& ddr_cpupin, std::map<std::string, std::vector<std::vector<Pin*>>>& ddr_sort_ddrpin, std::vector<std::vector<Pin*>>& sort_cpupin);
    void update_escape_routing_length();
    void insert_rest_of_net(const std::vector<std::vector<int>>& rest_of_net);
    void rest_net_detour(Detour_info& di, bool is_CPU, bool is_coarse, const Boundary& bdy_coor, Pin* p, int layer);
    void set_line (std::vector<std::vector<std::vector<std::pair<int,int>>>>& CPU_LCS_group, const std::vector<std::vector<std::vector<std::pair<int,int>>>>& DDR_LCS_group);
};





#endif