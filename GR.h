#ifndef _BLMR_H_
#define _BLMR_H_
#include "Structure.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern double cell_coe;
extern int level_coe;
extern int split_coe;
extern int sub_group_size;
extern int tolerance;
extern int coarse_x_length;
extern int coarse_y_length;
extern int fine_x_length;
extern int fine_y_length;
extern bool cycle_ddr;
extern double pitch_extend_coe;
class GR
{
private:
public:
    int layer_num;
    int can_use_layer_num;
    int coarse_x_size, coarse_y_size;
    int fine_x_size, fine_y_size;
    int fail_data_net_num;
    std::string file_name;
    Boundary total_boundary;
    std::string CPU_name;
    std::vector<std::string> DDR_name;
    std::map<std::string,Boundary> comp_boundary;
    std::map<std::string,Boundary> unrotate_comp_boundary;
    std::map<std::string,int> ddr_escape;
    std::map<std::string,int> cpu_escape;
    std::map<std::string,int> CPU_AR_shift;
    std::map<std::string,int> DDR_AR_shift;
//              ddr       shift
    std::vector<Net> net_list;
    std::vector<Pin> pin_list;
    std::vector<Drc> drc_list;
    std::vector<Obs> obs_list;
    std::vector<Diff> diff_list;
    std::vector<std::string> layer_list;
    std::vector<std::vector<int>> group_list;
    std::vector<std::map<int,std::vector<Segment>>> seg_list;
    std::map<std::string,DDR> DDR_list;
    std::vector<std::vector<int>> ripuped_cluster;

    std::vector<std::vector<int>> all_net;
    std::vector<int> rest_DATA_net;
    std::vector<std::vector<int>> merged_groupID;//big group:small group ID
    std::vector<std::vector<int>> merged_group;//new group:netID
    std::vector<std::vector<int>> merged_group_by_ddr;//new group:netID
    std::vector<std::vector<int>> merged_group_by_ddr_and_cpu_region;//new group:netID
    std::vector<Group_net> merged_group_net;
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
    

        //group     //layer      //nets          //nid, shift
    std::vector<std::vector<std::vector<std::pair<int,int>>>> CPU_LCS_group;
    std::vector<std::vector<std::vector<std::pair<int,int>>>> DDR_LCS_group;
    std::vector<std::vector<std::map<int,Line>>> LCS_group_net_line_align_bdy;
    std::vector<std::vector<Line>> l_restnet_line;
    std::map<std::pair<int,int>,int> direction;
    std::vector<std::vector<std::vector<Cell>>> coarse_GRcell;
    std::vector<std::vector<std::vector<Cell>>> fine_GRcell;
    std::vector<std::vector<std::vector<Edge>>> coarse_edge_table;
    std::vector<std::vector<std::vector<Edge>>> fine_edge_table;

    std::vector<std::vector<std::vector<int>>>cluster_relative_position;//layer : nets order
    std::vector<std::vector<Cluster>> GR_unit;
    std::vector<std::map<int,Edge>> edge_table;

    GR();
    Pin* get_cpupin(const Net& n);
    Pin* get_ddrpin(const Net& n);
    void rotate_ddr();
    void birotate_ddr();
    void Global_routing();
    void Global_routing(std::vector<int>& GR_net);
    void update_cell_demand(int, Cell&, Coor , Boundary);
    void merge_group();
    void planning();
    void partition();
    void partition_by_position(std::vector<std::vector<int>>& group);
    void partition_by_size(std::vector<std::vector<int>>& group);
    std::vector<std::vector<std::pair<std::vector<std::pair<int,int>>,std::vector<std::pair<int,int>>>>> partition_by_LCS(std::vector<std::vector<int>>& group, std::vector<std::vector<int>>& rest_of_net);
    void construct_3DGCells();
    std::vector<std::string> sort_ddr(const std::map<std::string, std::vector<Pin*>>& ddr_ddrpin);
    // void Astar(std::vector<std::vector<std::vector<Cell>>>& GRmap,  Net& net, bool coarse);
    // Path_node Astar(std::vector<std::vector<std::vector<Cell>>>& GRmap,  Sub_gn& sg_net, bool coarse);
    Path_node Astar(std::vector<std::vector<std::vector<Cell>>>& GRmap, std::vector<Cluster>& GR_unit,  Cluster& cluster, bool coarse);
    Path_node LMaware_Astar(std::vector<std::vector<std::vector<Cell>>>& GRmap, std::vector<Cluster>& GR_unit,  Cluster& cluster, bool coarse);
    void sort_LCS_input( std::map<std::string, std::vector<Pin*>>& ddr_ddrpin,  std::map<std::string, std::vector<Pin*>>& ddr_cpupin, std::map<std::string, std::vector<std::vector<Pin*>>>& ddr_sort_ddrpin, std::map<std::string, std::vector<std::vector<Pin*>>>& ddr_sort_cpupin);
    void sort_LCS_input( std::map<std::string, std::vector<Pin*>>& ddr_ddrpin,  std::map<std::string, std::vector<Pin*>>& ddr_cpupin, std::map<std::string, std::vector<std::vector<Pin*>>>& ddr_sort_ddrpin, std::vector<std::vector<Pin*>>& sort_cpupin);
    void update_escape_routing_length();
    void insert_rest_of_net(const std::vector<std::vector<int>>& rest_of_net);
    void rest_net_detour(Detour_info& di, bool is_CPU, bool is_coarse, const Boundary& bdy_coor, const Boundary& bdy_region, Pin* p, int layer);
    void set_line (std::vector<std::vector<std::vector<std::pair<int,int>>>>& CPU_LCS_group, const std::vector<std::vector<std::vector<std::pair<int,int>>>>& DDR_LCS_group);
    // void update_Cell(std::vector<std::vector<std::vector<Cell>>>& map, int subg_idx);
    void update_Cell(std::vector<std::vector<std::vector<Cell>>>& map);
    void classfy_cluster_relative_position(std::vector<int>& nets, std::vector<std::vector<Cluster>>& GR_unit);
    void update_forbidden_region(std::vector<std::vector<std::vector<Cell>>>& GRmap, Cluster& c, bool coarse);
    void add_forbidden_region(int c_idx, std::vector<Cluster>& _GR_unit, bool coarse);
    void remove_forbidden_region(int c_idx, std::vector<Cluster>& _GR_unit, bool coarse);
    void AR_RSRC_allocate(std::string filename);
    bool RSRC_allocate_fail();
    void ripup_cluster(bool AstarFail, int c_idx, std::vector<Cluster>& _GR_unit, bool coarse);
    void update_total_wl();
    void update_total_wl(int layer);
    void update_GR_unit();
    void print_GR(std::vector<std::vector<Cluster>> new_GR_unit);
    void add_history_cost(std::vector<Cluster>& _GR_unit, int c_idx, int layer);
    void remove_history_cost(std::vector<Cluster>& _GR_unit, int c_idx, int layer);
    void print_map_cluster(int layer);
    void update_forbidden_edge(Cluster & cluster);
    void rotate_pin(const std::map<std::string, std::vector<int>>& comp_ddr_pin);
    void reply_pin();
};





#endif