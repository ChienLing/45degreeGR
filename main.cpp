// #include <bits/stdc++.h>
#include "Parser.h"
#include "IO_interface.h"
using namespace std;
double cell_coe;//cell width = pitch*cell coe
int level_coe;//coe between coarse and fine
int split_coe;
int sub_group_size;
int tolerance;//decide LCS pin list, the pins have same x(or y) coor means same list' 
int coarse_x_length;
int coarse_y_length;
int fine_x_length;
int fine_y_length;
int l_dia = 400;
int MSL = 400;
int slack_tol = 400;//tolerant slack val
bool cycle_ddr;
double pitch_extend_coe;
int main(int argc, char** argv) {
    cycle_ddr = false;
    MIN_SPACING = 400;
    WIRE_WIDTH = 400;
    l_dia = 400;
    MSL = 400;
    cell_coe = 6;
    level_coe = 2;
    split_coe = 8;
    obs_ext = 2000;
    tolerance = 100;
    sub_group_size = 6;
    pitch_extend_coe = 2;
    coarse_x_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe;
    coarse_y_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe;
    fine_x_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe/level_coe;
    fine_y_length = (WIRE_WIDTH+MIN_SPACING)*cell_coe/level_coe;
    GR* router = new GR;
    string filename(argv[1]);
    Parser parser(router,argv[1]);
    parser.read_inputfile();
    router->file_name = filename;
    router->merge_group();
    router->construct_3DGCells();
    router->partition();
    // router->Global_routing();
    
    router->Global_routing(router->all_net.at(0));

    for (int layer=0; layer<router->can_use_layer_num; layer++) {
        // printf("layer: %d  fail cluster: ",layer);
        int total_slack(0);
        int total_remain_slack(0);
        for (auto cluster=router->GR_unit.at(layer).begin(); cluster!=router->GR_unit.at(layer).end(); cluster++) {
            total_slack+=cluster->max_slack;
            if (cluster->path.size()==0)
                total_remain_slack += cluster->max_slack;
            else if (cluster->remain_slack > slack_tol)
                total_remain_slack += cluster->remain_slack;
        }
        printf("layer:%d  total slack:%d  total remain slack:%d\n",layer,total_slack,total_remain_slack);
    }
    printf("fail cluster:\n");
    for (int layer=0; layer<router->can_use_layer_num; layer++) {
        printf("layer:%d: ",layer);
        for (auto cluster=router->GR_unit.at(layer).begin(); cluster!=router->GR_unit.at(layer).end(); cluster++) {
            if(cluster->path.size()==0)
                printf("%d(%d) ",cluster->cluster_relative_idx,cluster->path.size());
        }
        printf("\n");
    }
    for (int layer=0; layer<router->can_use_layer_num; layer++) {
        printf("layer:%d  #cluster:%d\n",layer, router->GR_unit.at(layer).size());
    }


    // for (int layer=0; layer<router->can_use_layer_num; layer++) {
    //     printf("layer: %d  fail cluster: ",layer);
    //     for (auto cluster=router->GR_unit.at(layer).begin(); cluster!=router->GR_unit.at(layer).end(); cluster++) {
    //         if (cluster->path.empty()) {
    //             printf("%d ",cluster->cluster_relative_idx);
    //         }
    //     }
    //     printf("\n",layer);
    // }

    // printf("After Global_routing (0,6,7) edge_r=%f",router->coarse_GRcell.at(0).at(6).at(7).edge_r[LEFT]);
    printf("\n");
    output_gds(argv[1], router);
    // output_SER(router, filename);
    string command;
    string path = argv[1];
    size_t pos=path.rfind("/");
    string case_name; case_name.assign(path, pos+1, path.size()-pos);

    for (int i=0; i<router->layer_can_use.size(); i++) {
        string l_name = router->layer_list.at(router->layer_can_use.at(i));
        command = "./gdt2gds "+path+"_"+l_name+".gdt " + case_name +"_"+l_name+".gds";
        printf("commend line: %s\n",command.c_str());
        system(command.c_str());
    }

    // command = "./gdt2gds "+path+"_BOTTOM.gdt " + case_name +"_BOT.gds";
    // printf("commend line: %s\n",command.c_str());
    // system(command.c_str());
    // command = "./gdt2gds "+path+"_S3.gdt " + case_name +"_S3.gds";
    // printf("commend line: %s\n",command.c_str());
    // system(command.c_str());
}