#include "BLMR.h"
#include "Func.h"
using namespace std;
#define X first
#define Y second
#define f(i,s,e) for(int i=s; i<=e; i++)
BLMR::BLMR(){}

void BLMR::merge_group() {
    printf("merge_group()\n");
    vector<int> mg(static_cast<int>(group_list.size()));
    for (int i=0; i<static_cast<int>(mg.size()); i++) mg.at(i)=i;
    // printf("mg size %d\n", mg.size());
    for (auto n=net_list.begin(); n!=net_list.end(); n++) {
        if (n->belong_group.size()==0)
            continue;
        int first_gid=group_NameID.at(n->belong_group.at(0));
        for (auto bg=n->belong_group.begin(); bg!=n->belong_group.end(); bg++) {

            int gid=group_NameID.at(*bg);
            // printf("gid%d\n", gid);
            if (find_root(mg, gid)!=find_root(mg, first_gid)) {
                if (find_root(mg, gid)<find_root(mg, first_gid)) 
                    mg.at(find_root(mg, first_gid)) = find_root(mg, gid);
                else
                    mg.at(find_root(mg, gid)) = find_root(mg, first_gid);
            }
        }
    }
    map<int,vector<int>> temp_merge_group;
    for (int i=0; i<mg.size(); i++) {
        temp_merge_group[find_root(mg,mg.at(i))].push_back(i);
    }
    for (auto g=temp_merge_group.begin(); g!=temp_merge_group.end(); g++)
        merged_groupID.push_back(g->second);
    int idx(0);
    for (auto g=merged_groupID.begin(); g!=merged_groupID.end(); g++) {
        printf("group %d : ",idx);
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            printf("%d ",*sub_g);
        }
        printf("\n");
        idx++;
    }    
    std::vector<std::set<int>> merged_group_temp;
    merged_group_temp.resize(merged_groupID.size());
    merged_group.resize(merged_groupID.size());
    idx=0;
    for (auto g=merged_groupID.begin(); g!=merged_groupID.end(); g++) {
        for (auto real_g=g->begin(); real_g!=g->end(); real_g++) {
            for (auto nid=group_list.at(*real_g).begin(); nid!=group_list.at(*real_g).end(); nid++) {
                // for (auto pid=net_list.at(*nid).net_pinID.begin(); pid!=net_list.at(*nid).net_pinID.end(); pid++) 
                //     merged_group.at(idx).insert(*pid);
                merged_group_temp.at(idx).insert(*nid);
                net_list.at(*nid).merge_group_ID = idx;
            }
        }
        idx++;
    }
    idx=0;
    for (auto g=merged_group_temp.begin(); g!=merged_group_temp.end(); g++) {
        printf("merged group  %d: ",idx);
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            printf("%d ", *nid);
            merged_group.at(idx).push_back(*nid);
        }
            printf("\n");
        idx++;
    }
}

/*void BLMR::partition() {
    //find same CPU size
    for (auto g=merged_group.begin(); g!=merged_group.end(); g++) {
        std::map<int,std::vector<int>> coarse_cpu_pin;//coarse cell id of cpu side pin : netID 
        std::map<int,std::vector<int>> fine_cpu_pin;//fine cell id of cpu side pin : netID 
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            for (auto pid=net_list.at(*nid).net_pinID.begin(); pid!=net_list.at(*nid).net_pinID.end(); pid++) {
                // printf("pin comp: %s   CPU name:%s\n", pin_list.at(*pid).comp_name.c_str(), CPU_name.c_str());
                if (pin_list.at(*pid).comp_name == CPU_name) {
                    coarse_cpu_pin[pin_list.at(*pid).coarse_coor.X+pin_list.at(*pid).coarse_coor.Y*coarse_x_size].push_back(*nid);
                    fine_cpu_pin[pin_list.at(*pid).fine_coor.X+pin_list.at(*pid).fine_coor.Y*fine_x_size].push_back(*nid);
                }
            }
        }
        for (auto sub_g=coarse_cpu_pin.begin(); sub_g!=coarse_cpu_pin.end(); sub_g++) { 
            std::map<int,std::vector<int>> coarse_ddr_pin;//coarse cell id of cpu side pin : netID 
            std::map<int,std::vector<int>> fine_ddr_pin;//fine cell id of cpu side pin : netID 
            for (auto nid = sub_g->second.begin(); nid!=sub_g->second.end(); nid++) {
                // cout<<"#pin :"<<net_list.at(*nid).net_pinID.size()<<endl;
                // cout<<"net_list.at(*nid).is2pin_net: "<<net_list.at(*nid).is2pin_net<<endl;
                if (net_list.at(*nid).is2pin_net) {
                    for (auto pid=net_list.at(*nid).net_pinID.begin(); pid!=net_list.at(*nid).net_pinID.end(); pid++) {
                        if (pin_list.at(*pid).comp_name != CPU_name) {
                            coarse_ddr_pin[pin_list.at(*pid).coarse_coor.X+pin_list.at(*pid).coarse_coor.Y*coarse_x_size].push_back(*nid);
                            fine_ddr_pin[pin_list.at(*pid).fine_coor.X+pin_list.at(*pid).fine_coor.Y*fine_x_size].push_back(*nid);
                        }
                    }
                }
            }
            for (auto it=coarse_ddr_pin.begin(); it!=coarse_ddr_pin.end(); it++) {
                coarse_group.push_back(it->second);
            }
            for (auto it=fine_ddr_pin.begin(); it!=fine_ddr_pin.end(); it++) {
                fine_group.push_back(it->second);
            }

        }
    }
    // int idx(0);
    // for (auto g=coarse_group.begin(); g!=coarse_group.end(); g++) {
    //     printf("group %d : ",idx);
    //     for (auto nid=g->begin(); nid!=g->end(); nid++) {
    //         printf("%d ",*nid);
    //     }
    //     printf("\n");
    //     idx++;
    // }
}*/

void BLMR::planning() {
    //deal with CPU pin
    for (auto g=merged_group.begin(); g!=merged_group.end(); g++) {
        map<string, vector<int>> comp_cpu_pin;
        map<string, vector<int>> comp_ddr_pin;
        Boundary b=comp_boundary.at(CPU_name);
        printf("CPU boundary:(%d,%d)~(%d,%d)\n",b.left,b.bot,b.right,b.top);
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            if (net_list.at(*nid).is2pin_net) {
                for (auto pid=net_list.at(*nid).net_pinID.begin(); pid!=net_list.at(*nid).net_pinID.end(); pid++) {
                    auto p=&pin_list.at(*pid);
                    if (p->comp_name==CPU_name) {
                        comp_cpu_pin[p->ddr_name].push_back(p->pin_ID);
                    }
                } 
            }        
        }
        for (auto n=net_list.begin(); n!=net_list.end(); n++) {
            if (n->net_pinID.size()==DDR_name.size()+1 || n->is2pin_net) {
                for (auto pid=n->net_pinID.begin(); pid!=n->net_pinID.end(); pid++) {
                    auto p=&pin_list.at(*pid);
                    if (p->comp_name!=CPU_name) {
                        comp_ddr_pin[p->comp_name].push_back(p->pin_ID);
                        // printf("adr net:%d pin:%s comp:%s\n",p->net_ID, p->pin_name.c_str(), p->comp_name.c_str());
                    }
                }
            }
        }
        //deal with CPU
        // int idx(0);
        for (auto ddr=comp_cpu_pin.begin(); ddr!=comp_cpu_pin.end(); ddr++) {
            // printf("idx: %d\n",idx);
            // int idy(0);
            for (auto pid=ddr->second.begin(); pid!=ddr->second.end(); pid++) {
                // printf("idy: %d\n",idy);
                int dist(1e8);
                if (abs (pin_list.at(*pid).real_pos.X - b.left) < dist) {
                    dist = abs (pin_list.at(*pid).real_pos.X - b.left);
                    pin_list.at(*pid).escape_dir = LEFT;
                }
                if (abs (pin_list.at(*pid).real_pos.X - b.right) < dist) {
                    dist = abs (pin_list.at(*pid).real_pos.X - b.right);
                    pin_list.at(*pid).escape_dir = RIGHT;
                }
                if (abs (pin_list.at(*pid).real_pos.Y - b.bot) < dist) {
                    dist = abs (pin_list.at(*pid).real_pos.Y - b.bot);
                    pin_list.at(*pid).escape_dir = BOT;
                }
                if (abs (pin_list.at(*pid).real_pos.Y - b.top) < dist) {
                    dist = abs (pin_list.at(*pid).real_pos.Y - b.top);
                    pin_list.at(*pid).escape_dir = TOP;
                }
                // idy++;
            }
            map<int,int> count;
            for (auto pid=ddr->second.begin(); pid!=ddr->second.end(); pid++) {
                count[pin_list.at(*pid).escape_dir]++;
            }
            int max(0);
            int e_d;
            for (auto dir=count.begin(); dir!=count.end(); dir++) {
                if (dir->second>max) {
                    max = dir->second;
                    e_d = dir->first;
                }
            }
            for (auto pid=ddr->second.begin(); pid!=ddr->second.end(); pid++) {
                pin_list.at(*pid).escape_dir=e_d;
                // printf("net %d in cpu pin escape direction:%d\n",pin_list.at(*pid).net_ID, e_d);
            }
            printf("CPU(%s) escape direction : %d\n",ddr->first.c_str(), e_d);
            // idx++;
        }
            
        //deal with DDR pin
        for (auto comp=comp_ddr_pin.begin(); comp!=comp_ddr_pin.end(); comp++){
            Boundary DATA_B, ADR_b;
            //if only 1 ddr, the code must be wrong
            for(auto pid=comp->second.begin(); pid!=comp->second.end(); pid++) {
                if (net_list.at(pin_list.at(*pid).net_ID).is2pin_net) {
                    if (pin_list.at(*pid).real_pos.X>DATA_B.right)
                        DATA_B.right = pin_list.at(*pid).real_pos.X;
                    if (pin_list.at(*pid).real_pos.X<DATA_B.left)
                        DATA_B.left = pin_list.at(*pid).real_pos.X;
                    if (pin_list.at(*pid).real_pos.Y>DATA_B.top)
                        DATA_B.top = pin_list.at(*pid).real_pos.Y;
                    if (pin_list.at(*pid).real_pos.Y<DATA_B.bot)
                        DATA_B.bot = pin_list.at(*pid).real_pos.Y;
                }
                else {
                    if (pin_list.at(*pid).real_pos.X>ADR_b.right)
                        ADR_b.right = pin_list.at(*pid).real_pos.X;
                    if (pin_list.at(*pid).real_pos.X<ADR_b.left)
                        ADR_b.left = pin_list.at(*pid).real_pos.X;
                    if (pin_list.at(*pid).real_pos.Y>ADR_b.top)
                        ADR_b.top = pin_list.at(*pid).real_pos.Y;
                    if (pin_list.at(*pid).real_pos.Y<ADR_b.bot)
                        ADR_b.bot = pin_list.at(*pid).real_pos.Y;
                }
            }
            printf("DATA Boundary:(%d,%d)~(%d,%d)\nADDR Boundary:(%d,%d)~(%d,%d)\n",DATA_B.left,DATA_B.bot,DATA_B.right,DATA_B.top,
                                                                                    ADR_b.left ,ADR_b.bot ,ADR_b.right ,ADR_b.top);
            if (DATA_B.top < ADR_b.bot) 
                ddr_escape[comp->first]=BOT;
            if (DATA_B.bot > ADR_b.top) 
                ddr_escape[comp->first]=TOP;
            if (DATA_B.right < ADR_b.left) 
                ddr_escape[comp->first]=LEFT;
            if (DATA_B.left > ADR_b.right) 
                ddr_escape[comp->first]=RIGHT;
        }
        for (auto comp=ddr_escape.begin(); comp!=ddr_escape.end(); comp++) {
            printf("comp:%s, escape direct: %d\n",comp->first.c_str(),comp->second);
        }
    }


}

void BLMR::partition() {
    planning();
    // partition_by_position(); 
    // partition_by_size();
    vector<int> temp;
    // for (auto mg=merged_group.begin(); mg!=merged_group.end(); mg++) {
    //     LCS_group=partition_by_LCS(mg);
    // }
    vector<vector<int>> rest_of_net(merged_group.size());
    vector<vector<pair<vector<pair<int,int>>,vector<pair<int,int>>>>> LCS_group=partition_by_LCS(merged_group,rest_of_net);
    for (auto it1=LCS_group.begin(); it1!=LCS_group.end(); it1++) {
        CPU_LCS_group.push_back(vector<vector<pair<int,int>>>());
        DDR_LCS_group.push_back(vector<vector<pair<int,int>>>());
        for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
            CPU_LCS_group.back().push_back(it2->first);
            DDR_LCS_group.back().push_back(it2->second);
        }
    }
    for (auto g=CPU_LCS_group.begin(); g!=CPU_LCS_group.end(); g++) {
        printf("group\n");
        int idx_g(0);
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            printf("sub group %d\n",idx_g);
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end();nid_shift++) {
                printf("%d ",nid_shift->first);
            }
            printf("\n");
            idx_g++;
        }
    }
    update_escape_routing_length();
    insert_rest_of_net(rest_of_net);
}

//                  ans,shift
vector<vector<pair<vector<pair<int,int>>,vector<pair<int,int>>>>> BLMR::partition_by_LCS(vector<vector<int>>& group, vector<vector<int>>& rest_of_net) {
    //2 pin net
    vector<vector<pair<vector<pair<int,int>>,vector<pair<int,int>>>>> ans(group.size());//merged gourp/sub_group/net
    int idx_g(0);
    for (auto g=group.begin(); g!=group.end(); g++) {
        vector<int> location1, location2;
        vector<int> ED_table1, ED_table2;
        map<string, vector<Pin*>> ddr_ddrpin, ddr_cpupin;
        map<string, vector<vector<Pin*>>> ddr_sort_ddrpin, ddr_sort_cpupin;
        vector<string> ddr_order;
        vector<vector<int>> list1, list2;        
        vector<pair<int,int>> dp_list;
        for (auto dp=diff_list.begin(); dp!=diff_list.end(); dp++) {
            pair<int,int> dp_net = {dp->netID_list.at(0), dp->netID_list.at(1)};
            dp_list.push_back(dp_net);
        }
        //splite 2 pin net and classify by ddr name
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            if (!net_list.at(*nid).is2pin_net)
                continue;
            for (auto pid=net_list.at(*nid).net_pinID.begin(); pid!=net_list.at(*nid).net_pinID.end(); pid++) {
                if (pin_list.at(*pid).CPU_side)
                    ddr_cpupin[pin_list.at(*pid).ddr_name].push_back(&pin_list.at(*pid));
                else
                    ddr_ddrpin[pin_list.at(*pid).ddr_name].push_back(&pin_list.at(*pid));
            }
        }
        //deal with ddr position sort
        ddr_order = sort_ddr(ddr_ddrpin);
        //sort pin in CPU/DDR by their escape direction
        sort_LCS_input(ddr_ddrpin,ddr_cpupin,ddr_sort_ddrpin,ddr_sort_cpupin);
        
        for (auto ddr=ddr_order.begin(); ddr!=ddr_order.end(); ddr++) {
            for (auto it1=ddr_sort_cpupin.at(*ddr).begin(); it1!=ddr_sort_cpupin.at(*ddr).end(); it1++) {
                list1.push_back(vector<int>());
                // printf("\n");
                if (it1->size()==0) {
                    printf("cpu pin sort wrong!!!\n");
                }
                ED_table1.push_back((it1->at(0))->escape_dir);
                if ((it1->at(0))->escape_dir == TOP || (it1->at(0))->escape_dir == BOT) {
                    location1.push_back((it1->at(0))->real_pos.X);
                }
                else {
                    location1.push_back((it1->at(0))->real_pos.Y);
                }
                for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
                    list1.back().push_back((*it2)->net_ID);
                    printf("%d ",(*it2)->net_ID);
                }
            }
                printf("\n");
            for (auto it1=ddr_sort_ddrpin.at(*ddr).begin(); it1!=ddr_sort_ddrpin.at(*ddr).end(); it1++) {
                list2.push_back(vector<int>());
                if (it1->size()==0) {
                    printf("ddr pin sort wrong!!!\n");
                }
                ED_table2.push_back((it1->at(0))->escape_dir);
                // if ((it1->at(0))->escape_dir == TOP || (it1->at(0))->escape_dir == BOT) {
                //     location2.push_back((it1->at(0))->real_pos.X);
                //     printf("location2 %d\n",(it1->at(0))->real_pos.X);
                // }
                // else {
                //     location2.push_back((it1->at(0))->real_pos.Y);
                // }
                if (ddr_escape.at(*ddr)==TOP ||ddr_escape.at(*ddr)==BOT) {
                    location2.push_back((it1->at(0))->real_pos.X);
                    // printf("location2 %d\n",(it1->at(0))->real_pos.X);
                }
                else {
                    location2.push_back((it1->at(0))->real_pos.Y);
                }
                for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
                    list2.back().push_back((*it2)->net_ID);
                    printf("%d ",(*it2)->net_ID);
                }
            }
        }
                printf("\n");

        set<int> used_net;
        ans.at(idx_g).push_back(LCS(list1, list2, dp_list, ED_table1, ED_table2, location1, location2));
        for (int i=0; i<can_use_layer_num-1; i++) {
            set<int> del_num;
            for (auto nid_shift=ans.at(idx_g).back().first.begin(); nid_shift!=ans.at(idx_g).back().first.end(); nid_shift++) {
                del_num.insert(nid_shift->first);
                used_net.insert(nid_shift->first);
            }
            vector<vector<int>> temp_list1, temp_list2;
            for (auto it1=list1.begin(); it1!=list1.end(); it1++) {
                bool tag(false);
                temp_list1.push_back(vector<int>());
                for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
                    if (del_num.find(*it2)==del_num.end()) {
                        if (tag == false) {
                            // temp_list1.push_back(vector<int>());
                            tag=true;
                        }
                        temp_list1.back().push_back(*it2);
                    }
                }
            }
            for (auto it1=list2.begin(); it1!=list2.end(); it1++) {
                bool tag(false);
                temp_list2.push_back(vector<int>());
                for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
                    if (del_num.find(*it2)==del_num.end()) {
                        if (tag == false) {
                            tag=true;
                            // temp_list2.push_back(vector<int>());
                        }
                        temp_list2.back().push_back(*it2);
                    }
                }
            }
            list1.clear(); list2.clear();
            list1 = temp_list1;
            list2 = temp_list2;
            ans.at(idx_g).push_back(LCS(list1, list2, dp_list,ED_table1, ED_table2, location1, location2));
        }
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            if (used_net.find(*nid)==used_net.end()) {
                rest_of_net.at(idx_g).push_back(*nid);
            }
        }
        idx_g++;
    }
    return ans;
}

void BLMR::update_escape_routing_length() {
    // for (auto g=LCS_group.begin(); g!=LCS_group.end()) {
    //     for (auto nid_shift=g->begin(); nid!=g->end(); nid_shift++) {
    //         if (!net_list.at(nid->first).is2pin_net)
    //             continue;
    //         for (auto pid=net_list.at(nid->first).net_pinID.begin();pid!=net_list.at(*nid->first).net_pinID.end();pid++) {
    //             auto p=pin_list.at(*pid);
    //             if (p.escape_dir==RIGHT) {
    //                 double length = comp_boundary.at(p.ddr).right-p.real_pos.X;
    //                 p.esti_escape_routing_length = lentgh;
    //             }
    //             else if (p.escape_dir==LEFT) {
    //                 double length = p.real_pos.X-comp_boundary.at(p.ddr).left;
    //                 p.esti_escape_routing_length = lentgh;
    //             }
    //             else if (p.escape_dir==TOP) {
    //                 double length = comp_boundary.at(p.ddr).top-p.real_pos.Y;
    //                 p.esti_escape_routing_length = lentgh;
    //             }
    //             else if (p.escape_dir==BOT) {
    //                 double length = p.real_pos.Y-comp_boundary.at(p.ddr).bot;
    //                 p.esti_escape_routing_length = lentgh;
    //             }
    //         }
    //     }
    // }
}
void BLMR::partition_by_size(std::vector<std::vector<int>>& group) {
}

void BLMR::update_cell_demand(int coarse, Cell& cell, std::pair<int,int> coor, Boundary obs_bd){
    int wire_unit = MIN_SPACING+WIRE_WIDTH;
    int coarse_cell_size = (cell_coe*wire_unit);
    int fine_cell_size = (cell_coe*wire_unit/level_coe);
    int cell_size = coarse?coarse_cell_size:fine_cell_size;
    // printf("update_cell_demand- cell size:%d\n",cell_size);
    Boundary cell_bd(total_boundary.left+coor.X*cell_size,\
                     total_boundary.bot+coor.Y*cell_size,\
                     total_boundary.left+(coor.X+1)*cell_size,\
                     total_boundary.bot+(coor.Y+1)*cell_size);
                    //  printf("coor(%d,%d)  pos:(%d,%d)~(%d,%d)\n",coor.X, coor.Y,cell_bd.left,cell_bd.bot, cell_bd.right, cell_bd.top);
    cell.cell_ini_demand = overlap_area(cell_bd, obs_bd);
    cell.obs.push_back(obs_bd);
    if (obs_bd.top <= cell_bd.top) {
        int dist = cell_bd.top-obs_bd.top;
        if (dist < overlap_w(cell_bd, obs_bd))
            cell.edge_ini_demand[TOP] += (double)(overlap_w(cell_bd, obs_bd)-dist)/wire_unit;
        if (obs_bd.bot < cell_bd.bot)
            cell.edge_ini_demand[BOT] += (double)overlap_w(cell_bd, obs_bd)/wire_unit;
    }
    if (obs_bd.bot >= cell_bd.bot) {
        int dist = obs_bd.bot - cell_bd.bot;
        if (dist < overlap_w(cell_bd, obs_bd))
            cell.edge_ini_demand[BOT] += (double)(overlap_w(cell_bd, obs_bd)-dist)/wire_unit;
        if (obs_bd.top > cell_bd.top)
            cell.edge_ini_demand[TOP] += (double)overlap_w(cell_bd, obs_bd)/wire_unit;
    }
    if (obs_bd.right <= cell_bd.right) {
        int dist = cell_bd.right-obs_bd.right;
        if (dist < overlap_h(cell_bd, obs_bd))
            cell.edge_ini_demand[RIGHT] += (double)(overlap_h(cell_bd, obs_bd)-dist)/wire_unit;
        if (obs_bd.left < cell_bd.left)
            cell.edge_ini_demand[LEFT] += (double)overlap_h(cell_bd, obs_bd)/wire_unit;
    }
    if (obs_bd.left >= cell_bd.left) {
        int dist = obs_bd.left - cell_bd.left;
        if (dist < overlap_h(cell_bd, obs_bd))
            cell.edge_ini_demand[LEFT] += (double)(overlap_h(cell_bd, obs_bd)-dist)/wire_unit;
        if (obs_bd.right > cell_bd.right)
            cell.edge_ini_demand[RIGHT] += (double)overlap_h(cell_bd, obs_bd)/wire_unit;
    }
    // printf("overlap h %d w %d\n",overlap_h(cell_bd, obs_bd), overlap_w(cell_bd, obs_bd));
    // printf("1.ini demand:\nleft:%f right:%f bot:%f top:%f\n",cell.edge_ini_demand[LEFT], cell.edge_ini_demand[RIGHT], cell.edge_ini_demand[BOT],cell.edge_ini_demand[TOP]);
    cell.edge_ini_demand[RIGHT] = min(cell.edge_ini_demand[RIGHT], cell.edge_cap[RIGHT]);
    cell.edge_ini_demand[LEFT] = min(cell.edge_ini_demand[LEFT], cell.edge_cap[LEFT]);
    cell.edge_ini_demand[TOP] = min(cell.edge_ini_demand[TOP], cell.edge_cap[TOP]);
    cell.edge_ini_demand[BOT] = min(cell.edge_ini_demand[BOT], cell.edge_cap[BOT]);
    // printf("cap:\nleft:%f right:%f bot:%f top:%f\n",cell.edge_cap[LEFT], cell.edge_cap[RIGHT], cell.edge_cap[BOT],cell.edge_cap[TOP]);
    // printf("2.ini demand:\nleft:%f right:%f bot:%f top:%f\n",cell.edge_ini_demand[LEFT], cell.edge_ini_demand[RIGHT], cell.edge_ini_demand[BOT],cell.edge_ini_demand[TOP]);
}

void BLMR::construct_3DGCells(){
    for (auto b=comp_boundary.begin(); b!=comp_boundary.end(); b++) {
        if (b->second.left < total_boundary.left)
            total_boundary.left = b->second.left;
        if (b->second.bot < total_boundary.bot)
            total_boundary.bot = b->second.bot;
        if (b->second.right > total_boundary.right)
            total_boundary.right = b->second.right;
        if (b->second.top > total_boundary.top)
            total_boundary.top = b->second.top;
    }
    printf("Total boundary (%d,%d)~(%d,%d)\n",\
    total_boundary.left,total_boundary.bot,total_boundary.right,total_boundary.top);
    int wire_unit = MIN_SPACING+WIRE_WIDTH;
    int coarse_cell_size = (cell_coe*wire_unit);
    int fine_cell_size = (cell_coe/level_coe*wire_unit);
    //extend boundary until it %cell size = 0
    if ((total_boundary.right-total_boundary.left)%coarse_cell_size !=0) {
        int lack = coarse_cell_size - (total_boundary.right-total_boundary.left)%coarse_cell_size;
        total_boundary.left-= lack/2;
        total_boundary.right+= (lack-lack/2);
    }
    if ((total_boundary.top-total_boundary.bot)%coarse_cell_size !=0) {
        int lack = coarse_cell_size - (total_boundary.top-total_boundary.bot)%coarse_cell_size;
        total_boundary.bot-= lack/2;
        total_boundary.top+= (lack-lack/2);
    }
    printf("resize Total boundary (%d,%d)~(%d,%d)\n",\
    total_boundary.left,total_boundary.bot,total_boundary.right,total_boundary.top);
    coarse_x_size = (total_boundary.right-total_boundary.left)/coarse_cell_size ;
    coarse_y_size = (total_boundary.top-total_boundary.bot)/coarse_cell_size ;
    fine_x_size = (total_boundary.right-total_boundary.left)/coarse_cell_size*level_coe ;
    fine_y_size = (total_boundary.top-total_boundary.bot)/coarse_cell_size*level_coe ;
    printf("x size %d, y size %d\n", coarse_x_size, coarse_y_size);
    coarse_GRcell.resize(can_use_layer_num, vector<vector<Cell>>(coarse_x_size,vector<Cell>(coarse_y_size,Cell(coarse_cell_size, (double)cell_coe))));
    fine_GRcell.resize(can_use_layer_num, vector<vector<Cell>>(fine_x_size,vector<Cell>(fine_y_size,Cell(fine_cell_size, (double)cell_coe/level_coe))));
    
// assign coor of pin in coarse/fine GR cell
    for (auto p=pin_list.begin(); p!=pin_list.end(); p++) {
        p->coarse_coor.X = (p->real_pos.X-total_boundary.left)/coarse_cell_size;
        p->coarse_coor.Y = (p->real_pos.Y-total_boundary.bot)/coarse_cell_size;
        p->fine_coor.X = (p->real_pos.X-total_boundary.left)/(coarse_cell_size/level_coe);
        p->fine_coor.Y = (p->real_pos.Y-total_boundary.bot)/(coarse_cell_size/level_coe);
        // printf("pin pos(%d,%d) C coor(%d,%d) fcoor(%d,%d)\n",p->real_pos.X,p->real_pos.Y,\
                                                             p->coarse_coor.X,p->coarse_coor.Y,\
                                                             p->fine_coor.X,p->fine_coor.Y);
    }

    for (auto o=obs_list.begin(); o!=obs_list.end(); o++) {
        if (!occupy(o->bd, total_boundary)) 
            continue;
        int o_layer_inGR;
        o->in_routing_region = true;
        pair<int,int> obs_coarse_coor, obs_fine_coor;
        obs_coarse_coor = {(o->center.X-total_boundary.left)/coarse_cell_size, (o->center.Y-total_boundary.bot)/coarse_cell_size};
        obs_fine_coor = {(o->center.X-total_boundary.left)/(coarse_cell_size/level_coe), (o->center.Y-total_boundary.bot)/(coarse_cell_size/level_coe)};
        if (o->center.X < total_boundary.left) {
            obs_coarse_coor.X = 0;
            obs_fine_coor.X = 0;
        }
        if (o->center.Y < total_boundary.bot){
            obs_coarse_coor.Y = 0;
            obs_fine_coor.Y = 0;
        }
        if (o->center.X > total_boundary.right) {
            obs_coarse_coor.X = coarse_GRcell.at(0).size()-1;
            obs_fine_coor.X = fine_GRcell.at(0).size()-1;
        }
        if (o->center.Y > total_boundary.top){
            obs_coarse_coor.Y = coarse_GRcell.at(0).at(0).size()-1;
            obs_fine_coor.Y = fine_GRcell.at(0).at(0).size()-1;
        }
        for (int l=0; l<layer_can_use.size(); l++){
            if (o->layer == layer_can_use.at(l)) {
                o_layer_inGR = l;
                break;
            } 
        }
            
        f(i,-1,1) {
            if (obs_coarse_coor.X+i <0 || obs_coarse_coor.X+i>=coarse_GRcell.at(o_layer_inGR).size())   continue;
            f(j,-1,1){
                if (obs_coarse_coor.Y+j <0 || obs_coarse_coor.Y+j>=coarse_GRcell.at(o_layer_inGR).at(obs_coarse_coor.X).size())   continue;
                Boundary bd(total_boundary.left+(obs_coarse_coor.X+i)*coarse_cell_size, total_boundary.bot+(obs_coarse_coor.Y+j)*coarse_cell_size,\
                            total_boundary.left+(obs_coarse_coor.X+i+1)*coarse_cell_size, total_boundary.bot+(obs_coarse_coor.Y+j+1)*coarse_cell_size);
                if (!occupy(bd, o->bd))
                    continue;
                    // printf("coor (%d,%d)\n",obs_coarse_coor.X+i, obs_coarse_coor.Y+j);
                Cell tempc = coarse_GRcell.at(o_layer_inGR).at(obs_coarse_coor.X+i).at(obs_coarse_coor.Y+j);
                // if (i!=0 || j!=0)
                //     cout<<"break point\n";
                update_cell_demand(1, coarse_GRcell.at(o_layer_inGR).at(obs_coarse_coor.X+i).at(obs_coarse_coor.Y+j),{obs_coarse_coor.X+i,obs_coarse_coor.Y+j} , o->bd);
            }
        }

        f(i,-1,1) {
            if (obs_fine_coor.X+i <0 || obs_fine_coor.X+i>=fine_GRcell.at(o_layer_inGR).size())   continue;
            f(j,-1,1){
                if (obs_fine_coor.Y+j <0 || obs_fine_coor.Y+j>=fine_GRcell.at(o_layer_inGR).at(obs_fine_coor.X).size())   continue;
                Boundary bd(total_boundary.left+(obs_fine_coor.X+i)*fine_cell_size, total_boundary.bot+(obs_fine_coor.Y+j)*fine_cell_size,\
                            total_boundary.left+(obs_fine_coor.X+i+1)*fine_cell_size, total_boundary.bot+(obs_fine_coor.Y+j+1)*fine_cell_size);
                if (!occupy(bd, o->bd));
                    continue;
                // printf("coor (%d,%d)\n",obs_fine_coor.X+i, obs_fine_coor.Y+j);
                update_cell_demand(0,fine_GRcell.at(o_layer_inGR).at(obs_fine_coor.X+i).at(obs_fine_coor.Y+j), \
                                {obs_fine_coor.X+i, obs_fine_coor.Y+j}, o->bd);
            }
        }


    }        
    for (int l=0; l<coarse_GRcell.size(); l++) {
        for (int x=0; x<coarse_GRcell.at(l).size(); x++) {//no -1
            for (int y=0; y<coarse_GRcell.at(l).at(x).size(); y++) {
                // if (l==0) {
                //     printf("TOP layer (%d,%d)\n",x,y);
                // }
                // printf("%f\n",coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(idx));
                if (y+1<coarse_GRcell.at(l).at(x).size())
                    coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(TOP) = 
                            max(coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(TOP),
                                coarse_GRcell.at(l).at(x).at(y+1).edge_ini_demand.at(BOT)); 
                if (y-1 >= 0)
                    coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(BOT) = 
                            max(coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(BOT),
                                coarse_GRcell.at(l).at(x).at(y-1).edge_ini_demand.at(TOP));  
                if (x+1<coarse_GRcell.at(l).size())  
                    coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(RIGHT) = 
                            max(coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(RIGHT),
                            coarse_GRcell.at(l).at(x+1).at(y).edge_ini_demand.at(LEFT)); 
                if (x-1 >= 0)
                    coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(LEFT) = 
                            max(coarse_GRcell.at(l).at(x).at(y).edge_ini_demand.at(LEFT),
                            coarse_GRcell.at(l).at(x-1).at(y).edge_ini_demand.at(RIGHT)); 

            }
        }
    }
}

void BLMR::Astar(vector<vector<vector<Cell>>>& GRmap, pair<int,int> star, int net_num) {
    vector<vector<vector<Cell>>>* tmp;
    // if ()
    //     tmp = coarse_GRcell;
    // else    
}

vector<string> BLMR::sort_ddr(const map<string, vector<Pin*>>& ddr_ddrpin) {
        //deal with ddr position sort

        vector<string> ddr_yx_bdy;
        vector<string> ddr_xy_bdy;
        
        {
            vector<Boundary> ddr_bdy;
            for (auto ddr=ddr_ddrpin.begin(); ddr!=ddr_ddrpin.end(); ddr++) {
                ddr_bdy.push_back(comp_boundary.at(ddr->first));
            }
            sort(ddr_bdy.begin(),ddr_bdy.end(), Bdy_greater_x);
            vector<vector<Boundary>> ddr_x_bdy;
            int left(-100);
            for (auto ddr_b=ddr_bdy.begin(); ddr_b!=ddr_bdy.end(); ddr_b++) {
                if (ddr_b->left-left >= tolerance) {
                    ddr_x_bdy.push_back(vector<Boundary>());
                }
                left = ddr_b->left;
                ddr_x_bdy.back().push_back(*ddr_b);
            }
            for (auto ddr=ddr_x_bdy.begin(); ddr!=ddr_x_bdy.end(); ddr++) {
                sort(ddr->begin(),ddr->end(), Bdy_greater_y);
            }
            for (auto ddr_x=ddr_x_bdy.begin(); ddr_x!=ddr_x_bdy.end(); ddr_x++) {
                for (auto ddr_y=ddr_x->begin(); ddr_y!=ddr_x->end(); ddr_y++) {
                    ddr_xy_bdy.push_back(ddr_y->name);
                }
            }
        }
        {
            vector<Boundary> ddr_bdy;
            for (auto ddr=ddr_ddrpin.begin(); ddr!=ddr_ddrpin.end(); ddr++) {
                ddr_bdy.push_back(comp_boundary.at(ddr->first));
            }
            sort(ddr_bdy.begin(),ddr_bdy.end(), Bdy_greater_y);
            vector<vector<Boundary>> ddr_y_bdy;
            int bot(-100);
            for (auto ddr_b=ddr_bdy.begin(); ddr_b!=ddr_bdy.end(); ddr_b++) {
                if (ddr_b->bot-bot >= tolerance) {
                    ddr_y_bdy.push_back(vector<Boundary>());
                }
                bot = ddr_b->bot;
                ddr_y_bdy.back().push_back(*ddr_b);
            }
            for (auto ddr=ddr_y_bdy.begin(); ddr!=ddr_y_bdy.end(); ddr++) {
                sort(ddr->begin(),ddr->end(), Bdy_greater_x);
            }
            for (auto ddr_y=ddr_y_bdy.begin(); ddr_y!=ddr_y_bdy.end(); ddr_y++) {
                for (auto ddr_x=ddr_y->begin(); ddr_x!=ddr_y->end(); ddr_x++) {
                    ddr_yx_bdy.push_back(ddr_x->name);
                }
            }
        }
        for (int i=0; i<ddr_yx_bdy.size(); i++) {
            printf("%s ",ddr_yx_bdy.at(i).c_str());
            if (ddr_yx_bdy.at(i)!=ddr_xy_bdy.at(i))
                printf("ddr order need chech!!!\n");
        }
        printf("\n");
        return ddr_yx_bdy;
}

void BLMR::sort_LCS_input( map<string, vector<Pin*>>& ddr_ddrpin,  map<string, vector<Pin*>>& ddr_cpupin, map<string, vector<vector<Pin*>>>& ddr_sort_ddrpin, map<string, vector<vector<Pin*>>>& ddr_sort_cpupin){
    //sort ddr position by escape direction
    for (auto ddr=ddr_ddrpin.begin(); ddr!=ddr_ddrpin.end(); ddr++) {
        if (ddr_escape.at(ddr->first)==TOP || ddr_escape.at(ddr->first)==BOT) {
            sort(ddr->second.begin(), ddr->second.end(), pin_greater_x);
            int left(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                if ((*double_it_p)->real_pos.X-left >= tolerance) {
                    ddr_sort_ddrpin[ddr->first].push_back(vector<Pin*>());
                }
                left = (*double_it_p)->real_pos.X;
                ddr_sort_ddrpin[ddr->first].back().push_back(*double_it_p);
            }   
            if (ddr_escape.at(ddr->first)==TOP) {
                for (auto x_pin_group=ddr_sort_ddrpin[ddr->first].begin(); x_pin_group!=ddr_sort_ddrpin[ddr->first].end(); x_pin_group++) {
                    sort(x_pin_group->begin(), x_pin_group->end(), pin_less_y);
                }
            }  
            else {
                for (auto x_pin_group=ddr_sort_ddrpin[ddr->first].begin(); x_pin_group!=ddr_sort_ddrpin[ddr->first].end(); x_pin_group++) {
                    sort(x_pin_group->begin(), x_pin_group->end(), pin_greater_y);
                }
            }           
        }
        else {
            sort(ddr->second.begin(), ddr->second.end(), pin_greater_y);
            int bot(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                if ((*double_it_p)->real_pos.Y-bot >= tolerance) {
                    ddr_sort_ddrpin[ddr->first].push_back(vector<Pin*>());
                }
                bot = (*double_it_p)->real_pos.Y;
                ddr_sort_ddrpin[ddr->first].back().push_back(*double_it_p);
            }
            if (ddr_escape.at(ddr->first)==RIGHT) {
                for (auto y_pin_group=ddr_sort_ddrpin[ddr->first].begin(); y_pin_group!=ddr_sort_ddrpin[ddr->first].end(); y_pin_group++) {
                    sort(y_pin_group->begin(), y_pin_group->end(), pin_less_x);
                }
            }
            else {
                for (auto y_pin_group=ddr_sort_ddrpin[ddr->first].begin(); y_pin_group!=ddr_sort_ddrpin[ddr->first].end(); y_pin_group++) {
                    sort(y_pin_group->begin(), y_pin_group->end(), pin_greater_x);
                }
            }
        }
    }
    //sort cpu position by escape direction
    for (auto ddr=ddr_cpupin.begin(); ddr!=ddr_cpupin.end(); ddr++) {
        // printf("sort CPU pin\n");
        if ((*(ddr->second.begin()))->escape_dir == TOP || (*(ddr->second.begin()))->escape_dir == BOT) {
            sort(ddr->second.begin(),ddr->second.end(), pin_greater_x);
            if ((*(ddr->second.begin()))->escape_dir != TOP && (*(ddr->second.begin()))->escape_dir != BOT) {
                printf("pin escape not identical!\n");
            }
            int left(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                if ((*double_it_p)->real_pos.X-left >= tolerance) {
                    ddr_sort_cpupin[ddr->first].push_back(vector<Pin*>());
                }
                left=(*double_it_p)->real_pos.X;
                ddr_sort_cpupin[ddr->first].back().push_back(*double_it_p);
            }
            // printf("Only sort by X:\n");
            // for (auto x_pin=ddr_sort_cpupin.begin(); x_pin!=ddr_sort_cpupin.end(); x_pin++) {
            //     printf("DDR: %s\n",x_pin->first.c_str());
            //     for (auto p_v=x_pin->second.begin(); p_v!=x_pin->second.end(); p_v++) {
            //         for (auto p=p_v->begin(); p!=p_v->end(); p++)
            //             printf("%d ", (*p)->net_ID);
            //         printf("\n");
            //     }
            //     printf("\n");
            // }
            if ((*(ddr->second.begin()))->escape_dir == TOP) {
                for (auto x_pin_group=ddr_sort_cpupin.begin(); x_pin_group!=ddr_sort_cpupin.end(); x_pin_group++) {
                    for (auto y_pin_group=x_pin_group->second.begin(); y_pin_group!=x_pin_group->second.end(); y_pin_group++)
                        sort(y_pin_group->begin(), y_pin_group->end(), pin_less_y);
                }
                
                // printf("Then sort by y(TOP):\n");
                // for (auto x_pin=ddr_sort_cpupin.begin(); x_pin!=ddr_sort_cpupin.end(); x_pin++) {
                //     printf("DDR: %s\n",x_pin->first.c_str());
                //     for (auto p_v=x_pin->second.begin(); p_v!=x_pin->second.end(); p_v++) {
                //         for (auto p=p_v->begin(); p!=p_v->end(); p++)
                //             printf("%d ", (*p)->net_ID);
                //         printf("\n");
                //     }
                //     printf("\n");
                // }
            }
            else {
                for (auto x_pin_group=ddr_sort_cpupin.begin(); x_pin_group!=ddr_sort_cpupin.end(); x_pin_group++) {
                    for (auto y_pin_group=x_pin_group->second.begin(); y_pin_group!=x_pin_group->second.end(); y_pin_group++) 
                        sort(y_pin_group->begin(), y_pin_group->end(), pin_greater_y);
                }
                
                // printf("Then sort by y(BOT):\n");
                // for (auto x_pin=ddr_sort_cpupin.begin(); x_pin!=ddr_sort_cpupin.end(); x_pin++) {
                //     printf("DDR: %s\n",x_pin->first.c_str());
                //     for (auto p_v=x_pin->second.begin(); p_v!=x_pin->second.end(); p_v++) {
                //         for (auto p=p_v->begin(); p!=p_v->end(); p++)
                //             printf("%d ", (*p)->net_ID);
                //         printf("\n");
                //     }
                //     printf("\n");
                // }
            }
        }
        else {
            sort(ddr->second.begin(),ddr->second.end(), pin_greater_y);
            if ((*(ddr->second.begin()))->escape_dir != RIGHT && (*(ddr->second.begin()))->escape_dir != LEFT) {
                printf("pin escape not identical!\n");
            }
            int bot(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                if ((*double_it_p)->real_pos.Y-bot >= tolerance) {
                    ddr_sort_cpupin[ddr->first].push_back(vector<Pin*>());
                }
                bot=(*double_it_p)->real_pos.Y;
                ddr_sort_cpupin[ddr->first].back().push_back(*double_it_p);
            }
            if ((*(ddr->second.begin()))->escape_dir == RIGHT) {
                for (auto y_pin_group=ddr_sort_cpupin.begin(); y_pin_group!=ddr_sort_cpupin.end(); y_pin_group++) {
                    for (auto x_pin_group=y_pin_group->second.begin(); x_pin_group!=y_pin_group->second.end(); x_pin_group++) 
                        sort(x_pin_group->begin(), x_pin_group->end(), pin_less_x);
                }
                
            }
            else {
                for (auto y_pin_group=ddr_sort_cpupin.begin(); y_pin_group!=ddr_sort_cpupin.end(); y_pin_group++) {
                    for (auto x_pin_group=y_pin_group->second.begin(); x_pin_group!=y_pin_group->second.end(); x_pin_group++) 
                        sort(x_pin_group->begin(), x_pin_group->end(), pin_greater_x);
                }
            }
        }
    }
}

void BLMR::insert_rest_of_net(const vector<vector<int>>& rest_of_net) {
    for (auto g=CPU_LCS_group.begin(); g!=CPU_LCS_group.end(); g++) {
        for (auto sub_g=g->rbegin(); sub_g!=g->rend(); sub_g++){
            
        }
    }
}


/*void BLMR::partition_by_position(std::vector<std::vector<int>>& group) {
    parted_group.resize(static_cast<int>(group.size()));
    int g_idx(0);
    for (auto g=group.begin(); g!=group.end(); g++) {
        map<string,vector<int>> ddr_net;//DDR : net
        //1.part group net by ddr  
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            // ONLY CONSIDER 2 PIN NET!!!!!!!!!!
            if (!net_list.at(*nid).is2pin_net) continue;
            for (auto pid=net_list.at(*nid).net_pinID.begin(); pid!=net_list.at(*nid).net_pinID.end(); pid++) {
                auto p=pin_list.at(*pid);
                if (p.comp_name == CPU_name)    continue;
                ddr_net[p.comp_name].push_back(*nid);
            }
        }
        //2.then part group net by net position in DDR
        for (auto comp=ddr_net.begin(); comp!=ddr_net.end(); comp++) {
            printf("\ncomponent: %s \n", comp->first.c_str());
            vector<Pin*> ddr_cpupin;//DDR : net
            vector<Pin*> ddr_ddrpin;//DDR : net
            for (auto nid=comp->second.begin(); nid!=comp->second.end(); nid++) {
                // printf("net: %d ", *nid);
                for (auto pid = net_list.at(*nid).net_pinID.begin(); pid!=net_list.at(*nid).net_pinID.end(); pid++) {
                    auto p=pin_list.at(*pid);
                    if (p.comp_name==CPU_name) {
                        ddr_cpupin.push_back(&pin_list.at(*pid));//no use
                        }
                    else {
                        ddr_ddrpin.push_back(&pin_list.at(*pid));
                        }
                }
                
            }
        //     printf("ddr unsort:  ");
        //     for (auto sorted_p=ddr_ddrpin.begin(); sorted_p!=ddr_ddrpin.end(); sorted_p++) {
        //         // printf(" %p", &(*sorted_p));
        //         printf(" %d", (*sorted_p)->real_pos.X);
        //     }
        //     printf("\n");
            sort(ddr_cpupin.begin(), ddr_cpupin.end(), pin_greater_x);
            sort(ddr_ddrpin.begin(), ddr_ddrpin.end(), pin_greater_x);
            // printf("ddr sort:  ");
            // for (auto sorted_p=ddr_ddrpin.begin(); sorted_p!=ddr_ddrpin.end(); sorted_p++) {
            //     // printf(" %p", &(*sorted_p));
            //     printf(" %d", (*sorted_p)->real_pos.X);
            // }
            // printf("\n");
            vector<vector<Pin*>> split_ddr_x_pin;
            vector<vector<int>> split_ddr_xy_pin;
            int left(0);
            int pitch = MIN_SPACING+WIRE_WIDTH;
            for (auto double_it=ddr_ddrpin.begin(); double_it!=ddr_ddrpin.end(); double_it++) {
                if ((*double_it)->real_pos.X-left >= split_coe*pitch) {
                    split_ddr_x_pin.push_back(vector<Pin*>());
                }
                left = (*double_it)->real_pos.X;
                split_ddr_x_pin.back().push_back(*double_it);
            }
            // for (auto x_group=split_ddr_x_pin.begin(); x_group!=split_ddr_x_pin.end(); x_group++)  {
            //     printf("group :\n");
            //     for (auto p=x_group->begin(); p!=x_group->end(); p++) {
            //         printf("pin %d  (%d,%d)\n",(*p)->pin_ID, (*p)->real_pos.X, (*p)->real_pos.Y);
            //     }
            // }
            for (auto x_group=split_ddr_x_pin.begin(); x_group!=split_ddr_x_pin.end(); x_group++)  {
                int bot(0);
                // printf("sort y:\n");
                sort(x_group->begin(), x_group->end(),pin_greater_y);
                for (auto p=x_group->begin(); p!=x_group->end(); p++) {
                    printf("(%d,%d)  ",(*p)->real_pos.X, (*p)->real_pos.Y);
                }
                printf("\n");
                for (auto p=x_group->begin(); p!=x_group->end(); p++) {
                    if ((*p)->real_pos.Y-bot >= split_coe*pitch) {
                        // parted_group.at(g_idx).push_back(vector<int>());
                        split_ddr_xy_pin.push_back(vector<int>());
                    }
                    bot = (*p)->real_pos.Y;
                    int nid = net_NameID.at((*p)->net_name);
                        split_ddr_xy_pin.back().push_back(nid);
                        // parted_group.at(g_idx).back().push_back(nid);
                }
            }
            printf("part by ddr position!!\n");
            for (auto sub_g=split_ddr_xy_pin.begin(); sub_g!=split_ddr_xy_pin.end(); sub_g++) {
                for (auto nid=sub_g->begin(); nid!=sub_g->end(); nid++) {
                    printf("%d  ", *nid);
                }
                printf("\n");
            }
            printf("\n");
            for (auto sub_g=split_ddr_xy_pin.begin(); sub_g!=split_ddr_xy_pin.end(); sub_g++) {
                vector<Pin*> cpu_pin;
                vector<vector<Pin*>> split_cpu_x_pin;
                vector<vector<int>> split_cpu_xy_pin;
                for (auto nid=sub_g->begin(); nid!=sub_g->end(); nid++) {
                    int p0 = net_list.at(*nid).net_pinID.at(0);
                    int p1 = net_list.at(*nid).net_pinID.at(1);
                    if (pin_list.at(p0).CPU_side) {
                        cpu_pin.push_back(&pin_list.at(p0));
                    }
                    else if (pin_list.at(p1).CPU_side)
                        cpu_pin.push_back(&pin_list.at(p1));
                    else {
                        printf("wrong\n");
                    }
                }
                sort(cpu_pin.begin(), cpu_pin.end(), pin_greater_x);
                printf("sort cpu pin x :\n");
                for (auto p=cpu_pin.begin(); p!=cpu_pin.end(); p++) {
                    printf("(%d,%d)  ",(*p)->real_pos.X, (*p)->real_pos.Y);
                }
                printf("\n");
                int left(0);
                for (auto double_it_p=cpu_pin.begin(); double_it_p!=cpu_pin.end(); double_it_p++) {
                    if ((*double_it_p)->real_pos.X-left >=split_coe*pitch) {
                        split_cpu_x_pin.push_back(vector<Pin*>());
                    }
                    left=(*double_it_p)->real_pos.X;
                    split_cpu_x_pin.back().push_back(*double_it_p);
                }
                
                printf("split_cpu_x_pin size: %d\ngroup:\n",split_cpu_x_pin.size());
                for (auto sub_g=split_cpu_x_pin.begin(); sub_g!=split_cpu_x_pin.end(); sub_g++) {
                    for (auto double_it_p=sub_g->begin(); double_it_p!=sub_g->end(); double_it_p++) {
                        int nid=net_NameID.at((*double_it_p)->net_name);
                        printf("%d  ",nid);
                    }
                    printf("\n");
                }
                printf("%\n");
                int bot(0);
                for (auto sub_g=split_cpu_x_pin.begin(); sub_g!=split_cpu_x_pin.end(); sub_g++) {
                    sort(sub_g->begin(), sub_g->end(), pin_greater_y);
                    for (auto double_pin_it=sub_g->begin(); double_pin_it!=sub_g->end(); double_pin_it++) {
                        if ((*double_pin_it)->real_pos.Y-bot >= split_coe*pitch/3*2) {
                            split_cpu_xy_pin.push_back(vector<int>());
                            parted_group.at(g_idx).push_back(vector<int>());
                        }
                        bot = (*double_pin_it)->real_pos.Y;
                        int nid = net_NameID.at((*double_pin_it)->net_name);
                        split_cpu_xy_pin.back().push_back(nid);
                        parted_group.at(g_idx).back().push_back(nid);
                    }
                }
                printf("part by cpu position!!!\n");
                for (auto sub_g=split_cpu_xy_pin.begin(); sub_g!=split_cpu_xy_pin.end(); sub_g++) {
                    for (auto nid=sub_g->begin(); nid!=sub_g->end(); nid++) {
                        printf("%d  ",*nid);
                    }
                    printf("\n");
                }
                printf("\n");
            }
        }
        g_idx++;
    }
        //printf group partite by component
        printf("part by component and distribution\n");
        for (auto comp=parted_group.begin(); comp!=parted_group.end(); comp++) {
            printf("BIG GROUP \n");
            int g=0;
            for (auto sub_g=comp->begin(); sub_g!=comp->end(); sub_g++) {
                printf("sub group(%d): %d ",  sub_g->size(), g);
                for (auto nid = sub_g->begin(); nid!=sub_g->end(); nid++) {
                    printf("%d ",*nid);
                }
                printf("\n");
                g++;
            }
        }

}*/
