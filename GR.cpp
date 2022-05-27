#include "GR.h"
#include "Func.h"
using namespace std;
#define X first
#define Y second
#define f(i,s,e) for(int i=s; i<=e; i++)
GR::GR(){}

void GR::merge_group() {
    //if the net in different length matching groups simutaneously, then merge group
    //if the region of pins of ddr1 and ddr2 have overlap, then merge group (for weigtedLCS)
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
                // net_list.at(*nid).merged_groupID = idx;
            }
        }
        idx++;
    }
    idx=0;
    for (auto g=merged_group_temp.begin(); g!=merged_group_temp.end(); g++) {
        printf("merged group  %d: ",idx);
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            printf("%d ", *nid);
            if (!net_list.at(*nid).is2pin_net)
                continue;
            merged_group.at(idx).push_back(*nid);
            net_list.at(*nid).merged_group_ID=idx;
        }
            printf("\n");
        idx++;
    }
    all_group_net.resize(2);
    for(auto g=merged_group.begin(); g!=merged_group.end(); g++) {
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            if (net_list.at(*nid).is2pin_net)
                all_group_net.at(0).push_back(*nid);
            else
                all_group_net.at(1).push_back(*nid);
        }
    }



    /*map<string,int> ddr_nameID;
    map<int,string> ddr_IDname;
    map<string,vector<Pin*>> net_pin_in_cpu;
    map<string,Boundary> net_pin_in_cpu_region;
    vector<int> overlap_cpu_pin(ddr_nameID.size());
    // f(i,0,ddr_nameID.size()-1) {
    //     overlap_cpu_pin[i]=i;
    // }

    // //classify the group net by its ddr name
    map<string,vector<int>> temp_merged_group_by_ddr;//new group:netID
    vector<vector<int>> merged_group_by_ddr;
    // for (auto n=net_list.begin(); n!=net_list.end(); n++) {
    //     if (n->belong_group.size()==0 || !n->is2pin_net)
    //         continue;
    //     temp_merged_group_by_ddr[pin_list.at(n->net_pinID.at(0)).ddr_name].push_back(n->net_ID);
    // }
    //find the region of each ddr pin of cpu
    for (auto g=merged_group.begin(); g!=merged_group.end(); g++){
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            for (auto pid=net_list.at(*nid).net_pinID.begin();pid!=net_list.at(*nid).net_pinID.end(); pid++) {
                auto p=pin_list.at(*pid);
                if (p.comp_name==CPU_name && p.ddr_name!="ADR") {
                    net_pin_in_cpu[p.ddr_name].push_back(&pin_list.at(*pid));
                    if (p.real_pos.X > net_pin_in_cpu_region[p.ddr_name].right)
                        net_pin_in_cpu_region[p.ddr_name].right = p.real_pos.X;
                    if (p.real_pos.X < net_pin_in_cpu_region[p.ddr_name].left)
                        net_pin_in_cpu_region[p.ddr_name].left = p.real_pos.X;
                    if (p.real_pos.X > net_pin_in_cpu_region[p.ddr_name].top)
                        net_pin_in_cpu_region[p.ddr_name].top = p.real_pos.Y;
                    if (p.real_pos.X < net_pin_in_cpu_region[p.ddr_name].bot)
                        net_pin_in_cpu_region[p.ddr_name].bot = p.real_pos.Y;
                }
            }
        }
    }
    //transfer map<string,vector<int>> to  vector<vector<int>> and use ddr_nameID record;
    for (auto g=temp_merged_group_by_ddr.begin(); g!=temp_merged_group_by_ddr.end(); g++) {
        merged_group_by_ddr.push_back(g->second);
        int i=ddr_nameID.size();
        ddr_nameID[g->first] = i;
        ddr_IDname[i] = g->first;
        printf("ddr: %s ID:%d\n",g->first.c_str(),i);
    }
    //if region overlap, then union;
    for (auto nameID=ddr_nameID.begin(); nameID!=ddr_nameID.end(); nameID++) {
        auto next_nameID = nameID;  next_nameID++;
        int firstID = ddr_nameID[nameID->first];
        while (next_nameID!=ddr_nameID.end()){
            int ddrID=ddr_nameID[next_nameID->first];
            if (occupy(net_pin_in_cpu_region[nameID->first],net_pin_in_cpu_region[next_nameID->first])) {
                if (find_root(overlap_cpu_pin,firstID) != find_root(overlap_cpu_pin,ddrID)) {
                    if (find_root(overlap_cpu_pin,firstID) < find_root(overlap_cpu_pin,ddrID))
                        overlap_cpu_pin.at(find_root(overlap_cpu_pin,ddrID)) = find_root(overlap_cpu_pin,firstID);
                    else
                        overlap_cpu_pin.at(find_root(overlap_cpu_pin,firstID)) = find_root(overlap_cpu_pin,ddrID);
                }
            }
            next_nameID++;
        }
    }
    
    map<int,vector<int>> temp_merge_ddr_region;
    vector<vector<int>> temp_merge_ddrID_region;
    for (int i=0; i<overlap_cpu_pin.size(); i++) {
        temp_merge_ddr_region[find_root(overlap_cpu_pin, overlap_cpu_pin.at(i))].push_back(i);
    }
    idx=0;
    for (auto it=temp_merge_ddr_region.begin(); it!=temp_merge_ddr_region.end(); it++) {
        printf("ddr region group %d:",idx);
        for (auto it2=it->second.begin(); it2!=it->second.end(); it2++) {
            printf(" %d",*it2);
        }
        printf(" \n");
        idx++;
    }
    for (auto g=temp_merge_ddr_region.begin(); g!=temp_merge_ddr_region.end(); g++) {
        temp_merge_ddrID_region.push_back(g->second);
    }
    idx=0;
    merged_group_by_ddr_and_cpu_region.resize(temp_merge_ddrID_region.size());
    for (auto ddrID_group=temp_merge_ddrID_region.begin(); ddrID_group!=temp_merge_ddrID_region.end(); ddrID_group++) {
        for (auto ddr_ID=ddrID_group->begin(); ddr_ID!=ddrID_group->end(); ddr_ID++) {
            string ddr_name=ddr_IDname[*ddr_ID];
            for (auto double_pin_it=net_pin_in_cpu[ddr_name].begin(); double_pin_it!=net_pin_in_cpu[ddr_name].end(); double_pin_it++) {
                merged_group_by_ddr_and_cpu_region.at(idx).push_back((*(double_pin_it))->net_ID);
            }
        }
        idx++;
    }*/
}

void GR::planning() {
    //deal with CPU pin
    // if (g!=merged_group.begin())
    //     break;
    map<string, vector<int>> comp_cpu_pin;
    map<string, vector<int>> comp_ddr_pin;
    Boundary b=comp_boundary.at(CPU_name);
    printf("CPU boundary:(%d,%d)~(%d,%d)\n",b.left,b.bot,b.right,b.top);
    for (auto n=net_list.begin(); n!=net_list.end(); n++) {
        if (n->is2pin_net) {
            for (auto pid=n->net_pinID.begin(); pid!=n->net_pinID.end(); pid++) {
                auto p=&pin_list.at(*pid);
                if (p->comp_name==CPU_name && p->ddr_name!="ADR" && p->ddr_name!="IGNORE") {
                    // printf("pid: %d  ddr: %s\n",p->pin_ID, p->ddr_name.c_str());
                    // if (p->net_ID==89)
                    //     printf("%d\n",p->pin_ID);
                    if (p->ddr_name=="U14") {
                        printf("%d", n->net_ID);
                        printf("break point\n");
                    }
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
        // printf("ddr: %s\n",ddr->first.c_str());
        // int idy(0);
        for (auto pid=ddr->second.begin(); pid!=ddr->second.end(); pid++) {
            // printf("idy: %d\n",idy);
            int dist(1e8);
            Pin P=pin_list.at(*pid);
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
        int e_d;
        while (true) {
            int max(0);
            for (auto dir=count.begin(); dir!=count.end(); dir++) {
                printf("dir: %d count:%d\n",dir->first,dir->second);
                if (dir->second>max) {
                    max = dir->second;
                    e_d = dir->first;
                }
            }
            if (e_d==TOP) {
                if (abs(comp_boundary.at(CPU_name).top-total_boundary.top)<3*coarse_y_length/2) {
                    count.at(e_d)=0;
                }
                else
                    break;
            }   
            else if (e_d==BOT) {
                if (abs(comp_boundary.at(CPU_name).bot-total_boundary.bot)<3*coarse_y_length/2) {
                    count.at(e_d)=0;
                }
                else
                    break;
            }   
            else if (e_d==RIGHT) {
                if (abs(comp_boundary.at(CPU_name).right-total_boundary.right)<3*coarse_x_length/2) {
                    count.at(e_d)=0;
                }
                else
                    break;
            }   
            else if(e_d==LEFT){
                if (abs(comp_boundary.at(CPU_name).left-total_boundary.left)<3*coarse_x_length/2) {
                    count.at(e_d)=0;
                }
                else
                    break;
            }      
        }

        for (auto pid=ddr->second.begin(); pid!=ddr->second.end(); pid++) {
            pin_list.at(*pid).escape_dir=e_d;
            // printf("net %d in cpu pin escape direction:%d\n",pin_list.at(*pid).net_ID, e_d);
        }
        printf("CPU(%s) escape direction : %d\n",ddr->first.c_str(), e_d);
        cpu_escape[ddr->first] = e_d;
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
        printf("(%s)DATA Boundary:(%d,%d)~(%d,%d)\n     ADDR Boundary:(%d,%d)~(%d,%d)\n\n",comp->first.c_str(), DATA_B.left,DATA_B.bot,DATA_B.right,DATA_B.top,
                                                                                ADR_b.left ,ADR_b.bot ,ADR_b.right ,ADR_b.top);
        // if (DATA_B.top < ADR_b.bot) 
        //     ddr_escape[comp->first]=BOT;
        // if (DATA_B.bot > ADR_b.top) 
        //     ddr_escape[comp->first]=TOP;
        // if (DATA_B.right < ADR_b.left) 
        //     ddr_escape[comp->first]=LEFT;
        // if (DATA_B.left > ADR_b.right) 
        //     ddr_escape[comp->first]=RIGHT;
        Boundary ddr_region(DATA_B.left<ADR_b.left  ?DATA_B.left :ADR_b.left,
                            DATA_B.bot<ADR_b.bot    ?DATA_B.bot  :ADR_b.bot,
                            DATA_B.right>ADR_b.right?DATA_B.right:ADR_b.right,
                            DATA_B.top>ADR_b.top    ?DATA_B.top  :ADR_b.top);
        int h=ddr_region.top  -ddr_region.bot;
        int w=ddr_region.right-ddr_region.left;
        if (DATA_B.top - ADR_b.top > h*0.3) 
            ddr_escape[comp->first]=TOP;
        if (ADR_b.top - DATA_B.top > h*0.3) 
            ddr_escape[comp->first]=BOT;
        if (DATA_B.right - ADR_b.right> w*0.3) 
            ddr_escape[comp->first]=RIGHT;
        if (ADR_b.right - DATA_B.right> w*0.3) 
            ddr_escape[comp->first]=LEFT;
    }

    for (auto comp=ddr_escape.begin(); comp!=ddr_escape.end(); comp++) {
            printf("ddr:%s, escape direct: %d\n",comp->first.c_str(),comp->second);
        }
    for (auto n=net_list.begin(); n!=net_list.end(); n++) {
        if (!n->is2pin_net)
            continue;
        Pin* cpu_p=get_cpupin(*n);
        Pin* ddr_p=get_ddrpin(*n);
        if (cpu_p->pin_ID==-1){
            printf("skip 2 pin net(not data signal)\n");
            continue;
        }
        int d=ddr_escape.at(ddr_p->ddr_name);
        ddr_p->escape_dir = ddr_escape.at(ddr_p->ddr_name);
        printf("net %d ddr pin escape dir:%d\n", n->net_ID, ddr_p->escape_dir);
    }

    for (auto n=net_list.begin(); n!=net_list.end(); n++) {
        if (!n->is2pin_net || n->ignore)
            continue;
        for (auto pid=n->net_pinID.begin();pid!=n->net_pinID.end();pid++) {
            
            auto p=&pin_list.at(*pid);
            if (!p->CPU_side) {
                p->escape_dir=ddr_escape.at(p->comp_name);
            }
        }
    }
}        

void GR::partition() {
    planning();
    // partition_by_position(); 
    // partition_by_size();
    vector<int> temp;
    // for (auto mg=merged_group.begin(); mg!=merged_group.end(); mg++) {
    //     LCS_group=partition_by_LCS(mg);
    // }
    all_net.resize(2);
    for(auto n=net_list.begin(); n!=net_list.end(); n++) {
        // printf("nid : %d  ignore:%d\n", n->net_ID,n->ignore);
        if (n->ignore)
            continue;
        if (n->is2pin_net)
            all_net.at(0).push_back(n->net_ID);
        else if (n->net_pinID.size()<=DDR_name.size()+1 && n->net_pinID.size()>1)
            all_net.at(1).push_back(n->net_ID);
    }
    vector<vector<int>> rest_of_net(2);
    // vector<vector<int>> rest_of_net(merged_group.size());
    // vector<vector<pair<vector<pair<int,int>>,vector<pair<int,int>>>>> LCS_group=partition_by_LCS(all_group_net,rest_of_net);
    vector<vector<pair<vector<pair<int,int>>,vector<pair<int,int>>>>> LCS_group=partition_by_LCS(all_net,rest_of_net);
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
                Pin* p=get_cpupin(net_list.at(nid_shift->first));
                if (p->escape_dir==RIGHT || p->escape_dir==BOT)
                    nid_shift->second *=-1;
                printf("%d/%d ",nid_shift->first,nid_shift->second);
                p->layer = idx_g;
            }
            printf("\n");
            idx_g++;
        }
    }
    for (auto g=DDR_LCS_group.begin(); g!=DDR_LCS_group.end(); g++) {
        printf("group\n");
        int idx_g(0);
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            printf("sub group %d\n",idx_g);
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end();nid_shift++) {
                Pin* p=get_ddrpin(net_list.at(nid_shift->first));
                if (p->escape_dir==LEFT || p->escape_dir==TOP)
                    nid_shift->second *=-1;
                printf("%d/%d ",nid_shift->first,nid_shift->second);
                p->layer = idx_g;
            }
            printf("\n");
            idx_g++;
        }
    }
    update_escape_routing_length();
    insert_rest_of_net(rest_of_net);
}   
//                  ans,shift
vector<vector<pair<vector<pair<int,int>>,vector<pair<int,int>>>>> GR::partition_by_LCS(vector<vector<int>>& group, vector<vector<int>>& rest_of_net) {
    //2 pin net
    vector<vector<pair<vector<pair<int,int>>,vector<pair<int,int>>>>> ans(group.size());//merged gourp/sub_group/net
    int idx_g(0);
    for (auto g=group.begin(); g!=group.end(); g++) {
        vector<int> location1, location2;
        vector<int> ED_table1, ED_table2;
        map<string, vector<Pin*>> ddr_ddrpin, ddr_cpupin;
        map<string, vector<vector<Pin*>>> ddr_sort_ddrpin;//, ddr_sort_cpupin
        vector<vector<Pin*>> sort_cpupin;
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
                auto p=pin_list.at(*pid);
                if (pin_list.at(*pid).CPU_side)
                    ddr_cpupin[pin_list.at(*pid).ddr_name].push_back(&pin_list.at(*pid));
                else
                    ddr_ddrpin[pin_list.at(*pid).ddr_name].push_back(&pin_list.at(*pid));
            }
        }
        //deal with ddr position sort
        ddr_order = sort_ddr(ddr_ddrpin);
        //sort pin in CPU/DDR by their escape direction
        sort_LCS_input(ddr_ddrpin,ddr_cpupin,ddr_sort_ddrpin,sort_cpupin);
        printf("list1: ");
        for (auto sameline_pin=sort_cpupin.begin(); sameline_pin!=sort_cpupin.end(); sameline_pin++) {
            list1.push_back(vector<int>());
            ED_table1.push_back((sameline_pin->at(0))->escape_dir);
            if ((sameline_pin->at(0))->escape_dir == TOP || (sameline_pin->at(0))->escape_dir == BOT) {
                location1.push_back((sameline_pin->at(0))->real_pos.X);
            }
            else {
                location1.push_back((sameline_pin->at(0))->real_pos.Y);
            }
            for (auto double_pin_it=sameline_pin->begin(); double_pin_it!=sameline_pin->end(); double_pin_it++) {
                list1.back().push_back((*double_pin_it)->net_ID);
                printf("%d ",(*double_pin_it)->net_ID);
            }

        }  printf("\n");
        int start(0);
        for (auto ddr=ddr_order.begin(); ddr!=ddr_order.end(); ddr++) {
            if (cpu_escape.at(*ddr) ==BOT) {
                start++;
            }
            else
                break;
        }
        int temp=ddr_order.size();
        if (temp!=0)
            start%=temp;
        if (temp!=0 && start!=0) {
            vector<string> new_ddr_order;
            // if (start%temp==0) {
            //     for (auto it=ddr_order.rbegin(); it!=ddr_order.rend(); it++)
            //         new_ddr_order.push_back(*it);
                
            // }
            // else {
                for (int s=start; s!=start-1;) {
                    new_ddr_order.push_back(ddr_order.at(s)); 
                    s++;
                    s%=temp;
                }
                new_ddr_order.push_back(ddr_order.at(start-1));
            // }
            ddr_order.clear();             
            ddr_order=new_ddr_order;   
        }
        for (auto ddr=ddr_order.begin(); ddr!=ddr_order.end(); ddr++) {
            
            /*printf("list1: ");
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
            printf("\n");*/
                printf("list2: ");
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
            if (list1.size()==0 && list2.size()==0)
                break;
            ans.at(idx_g).push_back(LCS(list1, list2, dp_list,ED_table1, ED_table2, location1, location2));
        }
        for (auto nid_shift=ans.at(idx_g).back().first.begin(); nid_shift!=ans.at(idx_g).back().first.end(); nid_shift++) {
            used_net.insert(nid_shift->first);
        }
        printf("used_net:\n");
        for (auto un=used_net.begin(); un!=used_net.end(); un++) {
            printf("%d ",*un);
        }printf("\n");
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            if (used_net.find(*nid)==used_net.end()) {
                rest_of_net.at(idx_g).push_back(*nid);
            }
        }
        idx_g++;
    }
    return ans;
}

void GR::update_escape_routing_length() {
    int pitch=MIN_SPACING+WIRE_WIDTH;
    for (auto g=CPU_LCS_group.begin(); g!=CPU_LCS_group.end();g++) {
        int layer(0);
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                if (!net_list.at(nid_shift->first).is2pin_net)
                    continue;
                // Pin *p = get_cpupin(net_list.at(nid_shift->first));
                Pin *p;
                if (pin_list.at(net_list.at(nid_shift->first).net_pinID.at(0)).CPU_side) 
                    p=&pin_list.at(net_list.at(nid_shift->first).net_pinID.at(0));
                else 
                    p=&pin_list.at(net_list.at(nid_shift->first).net_pinID.at(1));
                
                int nid=nid_shift->first;
                if (p->escape_dir==RIGHT) {
                    int l = comp_boundary.at(p->comp_name).right-p->real_pos.X;
                    p->esti_escape_routing_length = l;
                    int new_y=p->real_pos.Y+nid_shift->second*pitch;//*pitch_extend_coe
                    int top_bdy=(p->coarse_coor.Y+1)*coarse_y_length+total_boundary.bot;
                    int bot_bdy=(p->coarse_coor.Y)*coarse_y_length+total_boundary.bot;
                    int right_coor = (comp_boundary.at(CPU_name).right-total_boundary.left)/coarse_x_length;
                    if (new_y > top_bdy) {
                        int top_coor = ((p->real_pos.Y+nid_shift->second*pitch)-total_boundary.bot)/coarse_y_length;//*pitch_extend_coe
                        for (int y=p->coarse_coor.Y; y<top_coor; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x<=right_coor+1; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,top_coor));
                        }
                    }
                    else if (new_y < bot_bdy) {
                        int bot_coor = ((p->real_pos.Y+nid_shift->second*pitch)-total_boundary.bot)/coarse_y_length;//*pitch_extend_coe
                        for (int y=p->coarse_coor.Y; y>bot_coor; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x<=right_coor+1; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,bot_coor));
                        }
                    }
                    else {
                        for (int x=p->coarse_coor.X; x<=right_coor+1; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                    }
                }
                else if (p->escape_dir==LEFT) {
                    int l = p->real_pos.X-comp_boundary.at(p->comp_name).left;
                    p->esti_escape_routing_length = l;
                    int new_y=p->real_pos.Y+nid_shift->second*pitch;//*pitch_extend_coe
                    int top_bdy=(p->coarse_coor.Y+1)*coarse_y_length+total_boundary.bot;
                    int bot_bdy=(p->coarse_coor.Y)*coarse_y_length+total_boundary.bot;
                    int left_coor = (comp_boundary.at(CPU_name).left-total_boundary.left)/coarse_x_length;
                    if (new_y > top_bdy) {
                        int top_coor = (new_y-total_boundary.bot)/coarse_y_length;
                        for (int y=p->coarse_coor.Y; y<top_coor; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x>=left_coor-1; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,top_coor));
                        }
                    }
                    else if (new_y < bot_bdy) {
                        int bot_coor = (new_y-total_boundary.bot)/coarse_y_length;
                        for (int y=p->coarse_coor.Y; y>bot_coor; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x>=left_coor-1; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,bot_coor));
                        }
                    }
                    else {
                        for (int x=p->coarse_coor.X; x>=left_coor-1; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                    }
                }
                else if (p->escape_dir==TOP) {
                    int l = comp_boundary.at(p->comp_name).top-p->real_pos.Y;
                    p->esti_escape_routing_length = l;
                    int new_x=p->real_pos.X+nid_shift->second*pitch;//*pitch_extend_coe
                    int right_bdy=(p->coarse_coor.X+1)*coarse_x_length+total_boundary.left;
                    int left_bdy=(p->coarse_coor.X)*coarse_x_length+total_boundary.left;
                    int top_coor = (comp_boundary.at(CPU_name).top-total_boundary.bot)/coarse_y_length;
                    if (new_x > right_bdy) {
                        int right_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x<right_coor; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y<=top_coor+1; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,right_coor,y));
                        }
                    }
                    else if (new_x < left_bdy) {
                        int left_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x>left_coor; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y<=top_coor+1; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,left_coor,y));
                        }
                    }
                    else {
                        for (int y=p->coarse_coor.Y; y<=top_coor+1; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                    }
                }
                else if (p->escape_dir==BOT) {
                    int cpu_bot_coor=(comp_boundary.at(CPU_name).bot-total_boundary.bot)/coarse_y_length;
                    int bot = total_boundary.bot+cpu_bot_coor*coarse_y_length;
                    int l = p->real_pos.Y-bot;
                    p->esti_escape_routing_length = l;
                    int new_x=p->real_pos.X+nid_shift->second*pitch;//*pitch_extend_coe
                    int right_bdy=(p->coarse_coor.X+1)*coarse_x_length+total_boundary.left;
                    int left_bdy=(p->coarse_coor.X)*coarse_x_length+total_boundary.left;
                    int bot_coor = (comp_boundary.at(CPU_name).bot-total_boundary.bot)/coarse_y_length;
                    if (new_x > right_bdy) {
                        int right_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x<right_coor; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,right_coor,y));
                        }
                    }
                    else if (new_x < left_bdy) {
                        int left_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x>left_coor; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,left_coor,y));
                        }
                    }
                    else {
                        for (int y=p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                    }
                }
                p->esti_escape_routing_length+=abs(nid_shift->second*pitch);
                p->shift = nid_shift->second;

            }
            layer++;
        }

    }    
    for (auto g=DDR_LCS_group.begin(); g!=DDR_LCS_group.end();g++) {
        int layer(0);
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                if (!net_list.at(nid_shift->first).is2pin_net)
                    continue;           
                Pin *p;
                if (!pin_list.at(net_list.at(nid_shift->first).net_pinID.at(0)).CPU_side) 
                    p=&pin_list.at(net_list.at(nid_shift->first).net_pinID.at(0));
                else 
                    p=&pin_list.at(net_list.at(nid_shift->first).net_pinID.at(1));
                if (p->pin_ID==113 || p->pin_ID==112)
                    printf("check point\n");
                p->escape_dir = ddr_escape.at(p->comp_name);
                if (p->escape_dir==RIGHT) {
                    int l = comp_boundary.at(p->comp_name).right-p->real_pos.X;
                    p->esti_escape_routing_length = l;
                    int new_y=p->real_pos.Y+nid_shift->second*pitch;//*pitch_extend_coe
                    int top_bdy=(p->coarse_coor.Y+1)*coarse_y_length+total_boundary.bot;
                    int bot_bdy=(p->coarse_coor.Y)*coarse_y_length+total_boundary.bot;
                    int right_coor = (comp_boundary.at(p->comp_name).right-total_boundary.left)/coarse_x_length;
                    if (new_y > top_bdy) {
                        int top_coor = ((p->real_pos.Y+nid_shift->second*pitch)-total_boundary.bot)/coarse_y_length;//*pitch_extend_coe
                        for (int y=p->coarse_coor.Y; y<top_coor; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x<=right_coor+1; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,top_coor));
                        }
                    }
                    else if (new_y < bot_bdy) {
                        int bot_coor = ((p->real_pos.Y+nid_shift->second*pitch)-total_boundary.bot)/coarse_y_length;//*pitch_extend_coe
                        for (int y=p->coarse_coor.Y; y>bot_coor; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x<=right_coor+1; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,bot_coor));
                        }
                    }
                    else {
                        for (int x=p->coarse_coor.X; x<=right_coor+1; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                    }
                
                }
                else if (p->escape_dir==LEFT) {
                    int l = p->real_pos.X-comp_boundary.at(p->comp_name).left;
                    p->esti_escape_routing_length = l;
                    int new_y=p->real_pos.Y+nid_shift->second*pitch;//*pitch_extend_coe
                    int top_bdy=(p->coarse_coor.Y+1)*coarse_y_length+total_boundary.bot;
                    int bot_bdy=(p->coarse_coor.Y)*coarse_y_length+total_boundary.bot;
                    int left_coor = (comp_boundary.at(p->comp_name).left-total_boundary.left)/coarse_x_length;
                    if (new_y > top_bdy) {
                        int top_coor = (new_y-total_boundary.bot)/coarse_y_length;
                        for (int y=p->coarse_coor.Y; y<top_coor; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x>=left_coor-1; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,top_coor));
                        }
                    }
                    else if (new_y < bot_bdy) {
                        int bot_coor = (new_y-total_boundary.bot)/coarse_y_length;
                        for (int y=p->coarse_coor.Y; y>bot_coor; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                        for (int x=p->coarse_coor.X; x>=left_coor-1; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,bot_coor));
                        }
                    }
                    else {
                        for (int x=p->coarse_coor.X; x>=left_coor-1; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                    }
                
                }
                else if (p->escape_dir==TOP) {
                    int t(comp_boundary.at(p->comp_name).top); 
                    int ddr_top_coor=(comp_boundary.at(p->comp_name).top-total_boundary.bot)/coarse_y_length;
                    int top = total_boundary.bot+(ddr_top_coor+1)*coarse_y_length;
                    int l = top-p->real_pos.Y;
                    p->esti_escape_routing_length = l;
                    int new_x=p->real_pos.X+nid_shift->second*pitch;//*pitch_extend_coe
                    int right_bdy=(p->coarse_coor.X+1)*coarse_x_length+total_boundary.left;
                    int left_bdy=(p->coarse_coor.X)*coarse_x_length+total_boundary.left;
                    int top_coor = (comp_boundary.at(p->comp_name).top-total_boundary.bot)/coarse_y_length;
                    if (new_x > right_bdy) {
                        int right_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x<right_coor; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y<=top_coor+1; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,right_coor,y));
                        }
                    }
                    else if (new_x < left_bdy) {
                        int left_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x>left_coor; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y<=top_coor+1; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,left_coor,y));
                        }
                    }
                    else {
                        for (int y=p->coarse_coor.Y; y<=top_coor+1; y++) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                    }
                
                }
                else if (p->escape_dir==BOT) {
                    int l = p->real_pos.Y-comp_boundary.at(p->comp_name).bot;
                    p->esti_escape_routing_length = l;
                    int new_x=p->real_pos.X+nid_shift->second*pitch;//*pitch_extend_coe
                    int right_bdy=(p->coarse_coor.X+1)*coarse_x_length+total_boundary.left;
                    int left_bdy=(p->coarse_coor.X)*coarse_x_length+total_boundary.left;
                    int bot_coor = (comp_boundary.at(p->comp_name).bot-total_boundary.bot)/coarse_y_length;
                    if (new_x > right_bdy) {
                        int right_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x<right_coor; x++) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,right_coor,y));
                        }
                    }
                    else if (new_x < left_bdy) {
                        int left_coor = (new_x-total_boundary.left)/coarse_x_length;
                        for (int x=p->coarse_coor.X; x>left_coor; x--) {
                            p->ER_coarse_cell.push_back(Coor(layer,x,p->coarse_coor.Y));
                        }
                        for (int y=p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,left_coor,y));
                        }
                    }
                    else {
                        for (int y=p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            p->ER_coarse_cell.push_back(Coor(layer,p->coarse_coor.X,y));
                        }
                    }
                
                }
                p->shift = nid_shift->second;
                p->esti_escape_routing_length+=abs(nid_shift->second*pitch);
                
            }
            layer++;
        }

    }
    // for (auto g=CPU_LCS_group.begin(); g!=CPU_LCS_group.end(); g++) {
    //     for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
    //         for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
    //             printf("net ID %d\n",nid_shift->first);
    //             for (auto pid=net_list.at(nid_shift->first).net_pinID.begin(); pid!=net_list.at(nid_shift->first).net_pinID.end(); pid++) {
    //                 auto p=pin_list.at(*pid);
    //                 printf ("pin %s(%d) shift %d esti length: %d \n", p.pin_name.c_str(),*pid, p.shift,p.esti_escape_routing_length);
    //             }
    //             printf("\n");
    //         }
    //     }
    // }
    for (auto g=CPU_LCS_group.begin(); g!=CPU_LCS_group.end();g++) {
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                Pin *p=get_cpupin(net_list.at(nid_shift->first));
                // printf("nid %d cpu pin coarse ER coor:",nid_shift->first);
                // for (auto coor=p->ER_coarse_cell.begin(); coor!=p->ER_coarse_cell.end(); coor++) {
                //     printf("(%d,%d,%d)->",coor->z,coor->x,coor->y);
                // }
                // printf("end\n");
            }
        }
    }
    for (auto g=DDR_LCS_group.begin(); g!=DDR_LCS_group.end();g++) {
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                Pin *p=get_ddrpin(net_list.at(nid_shift->first));
                // printf("nid %d ddr pin coarse ER coor:",nid_shift->first);
                // for (auto coor=p->ER_coarse_cell.begin(); coor!=p->ER_coarse_cell.end(); coor++) {
                //     printf("(%d,%d,%d)->",coor->z,coor->x,coor->y);
                // }
                // printf("end\n");
            }
        }
    }
}

void GR::partition_by_size(std::vector<std::vector<int>>& group) {
}

void GR::construct_3DGCells(){
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
    total_boundary.left-=coarse_cell_size;
    total_boundary.right+=coarse_cell_size;
    total_boundary.bot-=coarse_cell_size;
    total_boundary.top+=coarse_cell_size;
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
        // if (o_layer_inGR != 0)//only reserve TOP layer obstacle
        //     continue;
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
                Coor coor(o_layer_inGR,obs_coarse_coor.X+i,obs_coarse_coor.Y+j);
                update_cell_demand(1, coarse_GRcell.at(o_layer_inGR).at(obs_coarse_coor.X+i).at(obs_coarse_coor.Y+j), coor, o->bd);
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
                Coor coor(o_layer_inGR,obs_fine_coor.X+i, obs_fine_coor.Y+j);
                update_cell_demand(0,fine_GRcell.at(o_layer_inGR).at(obs_fine_coor.X+i).at(obs_fine_coor.Y+j), coor, o->bd);
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
    
    for (int l=0; l<coarse_GRcell.size(); l++) {
        for (int x=0; x<coarse_GRcell.at(l).size(); x++) {//no -1
            for (int y=0; y<coarse_GRcell.at(l).at(x).size(); y++) {
                coarse_GRcell.at(l).at(x).at(y).update_recourse();
            }
        }
    }
}

void GR::update_cell_demand(int coarse, Cell& cell, Coor coor, Boundary obs_bd){
    int wire_unit = MIN_SPACING+WIRE_WIDTH;
    int coarse_cell_size = (cell_coe*wire_unit);
    int fine_cell_size = (cell_coe*wire_unit/level_coe);
    int cell_size = coarse?coarse_cell_size:fine_cell_size;
    std::vector<std::vector<std::vector<Edge>>>* map=coarse?&coarse_edge_table:&fine_edge_table;
    // printf("update_cell_demand- cell size:%d\n",cell_size);
    Boundary cell_bd(total_boundary.left+coor.x*cell_size,\
                     total_boundary.bot+coor.y*cell_size,\
                     total_boundary.left+(coor.x+1)*cell_size,\
                     total_boundary.bot+(coor.y+1)*cell_size);
                    //  printf("coor(%d,%d,%d)  pos:(%d,%d)~(%d,%d)\n",coor.z,coor.x, coor.y,cell_bd.left,cell_bd.bot, cell_bd.right, cell_bd.top);
    cell.cell_ini_demand = overlap_area(cell_bd, obs_bd);
    cell.obs.push_back(obs_bd);
    if (obs_bd.top <= cell_bd.top) {
        int dist = cell_bd.top-obs_bd.top;
        if (dist < overlap_w(cell_bd, obs_bd)) {
            cell.edge_ini_demand[TOP] += (double)(overlap_w(cell_bd, obs_bd)-dist)/wire_unit;
            // map->at(coor.z).at(coor.x).at(coor.y+1).ini_demand += (double)(overlap_w(cell_bd, obs_bd)-dist)/wire_unit;
        }
        if (obs_bd.bot < cell_bd.bot){
            cell.edge_ini_demand[BOT] += (double)overlap_w(cell_bd, obs_bd)/wire_unit;
            // map->at(coor.z).at(coor.x).at(coor.y).ini_demand += (double)overlap_w(cell_bd, obs_bd)/wire_unit;
        }
    }
    if (obs_bd.bot >= cell_bd.bot) {
        int dist = obs_bd.bot - cell_bd.bot;
        if (dist < overlap_w(cell_bd, obs_bd)){
            cell.edge_ini_demand[BOT] += (double)(overlap_w(cell_bd, obs_bd)-dist)/wire_unit;
            // map->at(coor.z).at(coor.x).at(coor.y).ini_demand += (double)(overlap_w(cell_bd, obs_bd)-dist)/wire_unit;
        }
        if (obs_bd.top > cell_bd.top) {
            cell.edge_ini_demand[TOP] += (double)overlap_w(cell_bd, obs_bd)/wire_unit;
            // map->at(coor.z).at(coor.x).at(coor.y+1).ini_demand += (double)overlap_w(cell_bd, obs_bd)/wire_unit;
        }
    }
    if (obs_bd.right <= cell_bd.right) {
        int dist = cell_bd.right-obs_bd.right;
        if (dist < overlap_h(cell_bd, obs_bd)) {
            cell.edge_ini_demand[RIGHT] += (double)(overlap_h(cell_bd, obs_bd)-dist)/wire_unit;
            // map->at(coor.z).at(coor.x+1).at(coor.y).ini_demand += (double)(overlap_h(cell_bd, obs_bd)-dist)/wire_unit;
        }
        if (obs_bd.left < cell_bd.left) {
            cell.edge_ini_demand[LEFT] += (double)overlap_h(cell_bd, obs_bd)/wire_unit;
            // map->at(coor.z).at(coor.x).at(coor.y).ini_demand += (double)overlap_h(cell_bd, obs_bd)/wire_unit;
        }
    }
    if (obs_bd.left >= cell_bd.left) {
        int dist = obs_bd.left - cell_bd.left;
        if (dist < overlap_h(cell_bd, obs_bd)){
            cell.edge_ini_demand[LEFT] += (double)(overlap_h(cell_bd, obs_bd)-dist)/wire_unit;
            // map->at(coor.z).at(coor.x).at(coor.y).ini_demand += (double)(overlap_h(cell_bd, obs_bd)-dist)/wire_unit;
        }
        if (obs_bd.right > cell_bd.right){
            cell.edge_ini_demand[RIGHT] += (double)overlap_h(cell_bd, obs_bd)/wire_unit;
            // map->at(coor.z).at(coor.x+1).at(coor.y).ini_demand += (double)overlap_h(cell_bd, obs_bd)/wire_unit;
        }
    }
    // printf("overlap h %d w %d\n",overlap_h(cell_bd, obs_bd), overlap_w(cell_bd, obs_bd));
    // printf("1.ini demand:\nleft:%f right:%f bot:%f top:%f\n",cell.edge_ini_demand[LEFT], cell.edge_ini_demand[RIGHT], cell.edge_ini_demand[BOT],cell.edge_ini_demand[TOP]);
    cell.edge_ini_demand[RIGHT] = min(cell.edge_ini_demand[RIGHT], cell.edge_cap[RIGHT]);
    cell.edge_ini_demand[LEFT] = min(cell.edge_ini_demand[LEFT], cell.edge_cap[LEFT]);
    cell.edge_ini_demand[TOP] = min(cell.edge_ini_demand[TOP], cell.edge_cap[TOP]);
    cell.edge_ini_demand[BOT] = min(cell.edge_ini_demand[BOT], cell.edge_cap[BOT]);
    cell.edge_r[RIGHT] = cell.edge_cap[RIGHT]-cell.edge_ini_demand[RIGHT];
    cell.edge_r[LEFT] = cell.edge_cap[LEFT]-cell.edge_ini_demand[LEFT];
    cell.edge_r[TOP] = cell.edge_cap[TOP]-cell.edge_ini_demand[TOP];
    cell.edge_r[BOT] = cell.edge_cap[BOT]-cell.edge_ini_demand[BOT];
    // map->at(coor.z).at(coor.x+1).at(coor.y).ini_demand = min(cell.edge_ini_demand[RIGHT], cell.edge_cap[RIGHT]);
    // map->at(coor.z).at(coor.x).at(coor.y).ini_demand = min(cell.edge_ini_demand[LEFT], cell.edge_cap[LEFT]);
    // map->at(coor.z).at(coor.x).at(coor.y+1).ini_demand = min(cell.edge_ini_demand[TOP], cell.edge_cap[TOP]);
    // map->at(coor.z).at(coor.x).at(coor.y).ini_demand = min(cell.edge_ini_demand[BOT], cell.edge_cap[BOT]);
    // printf("cap:\nleft:%f right:%f bot:%f top:%f\n",cell.edge_cap[LEFT], cell.edge_cap[RIGHT], cell.edge_cap[BOT],cell.edge_cap[TOP]);
    // printf("2.ini demand:\nleft:%f right:%f bot:%f top:%f\n",cell.edge_ini_demand[LEFT], cell.edge_ini_demand[RIGHT], cell.edge_ini_demand[BOT],cell.edge_ini_demand[TOP]);
}

//single net
void GR::Astar(vector<vector<vector<Cell>>>& GRmap, Net& net, bool coarse) {
    vector<vector<vector<Cell>>>* tmp;
    Pin *cpu_p=get_cpupin(net);
    Pin *ddr_p=get_ddrpin(net);
    Coor start, end;
    Coor diagonal_coor;
    int x_length, y_length;
    Path_node pn; 
    std::vector<std::vector<std::vector<Cell>>>* map=&GRmap;
    priority_queue<Path_node,vector<Path_node>,compare> PQ;
    if (coarse) {
        start = cpu_p->ER_coarse_cell.back();
        end = ddr_p->ER_coarse_cell.back();
        diagonal_coor.setup(can_use_layer_num-1,coarse_x_size-1,coarse_y_size-1);
        x_length = coarse_x_length;
        y_length = coarse_y_length;
    }
    else {
        start = cpu_p->ER_fine_cell.back();
        end = ddr_p->ER_fine_cell.back();
        diagonal_coor.setup(can_use_layer_num-1,fine_x_size-1,fine_y_size-1);
        x_length = fine_x_length;
        y_length = fine_y_length;
    }
    pn.path.push_back(start);
    PQ.push(pn);
    while (true) {
        Path_node top_pn=PQ.top();  PQ.pop();
        printf("%f",top_pn.cost);
        f (i,-1,1) {
            f (j,-1,1) {
                Path_node temp_pn;
                double cost;
                int wl;
                temp_pn=top_pn;
                Coor coor=top_pn.path.back();
                coor.setup(coor.z,coor.x+i,coor.y+j);
                if (out_of_bdy(coor,diagonal_coor))
                    continue;           
                temp_pn.path.push_back(coor);
                if (i==0) {
                    wl=y_length;
                    if (j==0)   
                        continue;
                    else if (j==1) {
                        double r=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                        if (r<1)
                            continue;   
                        cost=1.0/(1+exp(r));
                    }
                    else {
                        double r=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                        if (r<1)
                            continue;   
                        cost=1.0/(1+exp(r));
                    }

                }
                else if (j==0) {
                    wl=x_length;
                    if (i==1) {
                        double r = map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                        if (r<1)
                            continue;
                        cost=1.0/(1+exp(r));
                    }
                    else {
                        double r = map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                        if (r<1)
                            continue;
                        cost=1.0/(1+exp(r));
                    }
                }
                else {
                    wl=sqrt(pow(y_length,2)+pow(x_length,2));
                    if (i*j==-1) {//BR or TL
                        if (i==-1) {
                            double xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                            double x_cost = 1.0/(1+exp(xr));
                            double y_cost = 1.0/(1+exp(yr));
                            if (xr<1 || yr<1)
                                continue;
                            cost = x_cost+y_cost;
                        }
                        else {
                            double xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                            double yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                            double x_cost = 1.0/(1+exp(xr));
                            double y_cost = 1.0/(1+exp(yr));
                            if (xr<1 || yr<1)
                                continue;
                            cost = x_cost+y_cost;
                        }
                    }
                    else {
                        if (i==-1) {
                            double xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                            double x_cost = 1.0/(1+exp(xr));
                            double y_cost = 1.0/(1+exp(yr));
                            if (xr<1 || yr<1)
                                continue;
                            cost = x_cost+y_cost;
                        }
                        else {
                            double xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                            double yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                            double x_cost = 1.0/(1+exp(xr));
                            double y_cost = 1.0/(1+exp(yr));
                            if (xr<1 || yr<1)
                                continue;
                            cost = x_cost+y_cost;
                        }
                    }
                }
                temp_pn.cost += cost;
                temp_pn.routed_wirelength +=wl;
                PQ.push(temp_pn);
            }
        }
        f (k,-1,1) {

        }
    }
}
//group nets
Path_node GR::Astar(vector<vector<vector<Cell>>>& GRmap, Sub_gn& sg_net, bool coarse) {
    printf("Astar: ");
    for (auto it=sg_net.net.begin(); it!=sg_net.net.end(); it++) {
        printf("%d  ",*it);
    }
    printf("\n");
    double c0;
    vector<vector<vector<Cell>>>* tmp;
    Coor start, end;
    Coor diagonal_coor;
    int x_length, y_length;
    double min_cost = 1e8;
    // double esti_demand = g_net.esti_demand;
    double esti_demand = 0;
    Path_node best_pn;
    Path_node pn; 
    std::vector<std::vector<std::vector<Cell>>>* map=&GRmap;
    priority_queue<Path_node,vector<Path_node>,compare> PQ;
    if (coarse) {
        start = sg_net.cpu_coor;
        end   = sg_net.ddr_coor;
        diagonal_coor.setup(can_use_layer_num-1,coarse_x_size-1,coarse_y_size-1);
        x_length = coarse_x_length;
        y_length = coarse_y_length;
    }
    else {
        start = sg_net.cpu_coor;
        end   = sg_net.ddr_coor;
        diagonal_coor.setup(can_use_layer_num-1,fine_x_size-1,fine_y_size-1);
        x_length = fine_x_length;
        y_length = fine_y_length;
    }
    if (start.z == 2 && *sg_net.net.begin()==34)
        printf("BOTTOM layer\n");
    pn.path.push_back(start);
    pn.esti_routing_length = min(abs(end.x-start.x), abs(end.y-start.y))*1.4+(max(abs(end.x-start.x), abs(end.y-start.y))-min(abs(end.x-start.x), abs(end.y-start.y)));
    pn.cost = 0;
    c0 = pn.esti_routing_length*0.5;
    PQ.push(pn);
    int idx(0);
    while (true) {
        Path_node top_pn=PQ.top();  PQ.pop();
        if (start.z == 2)
            printf("TOP coor: (%d,%d,%d)  \n",top_pn.path.back().x,top_pn.path.back().y,top_pn.path.back().z);
        // printf("%f",top_pn.cost);
        // printf("top_pn coor: (%d,%d,%d) routed_WL:%f cost:%f + ERL:%f = %f \n",
        // top_pn.path.back().x,top_pn.path.back().y,top_pn.path.back().z,
        // top_pn.routed_wirelength,top_pn.cost,top_pn.esti_routing_length,
        //                          top_pn.cost+top_pn.esti_routing_length);
        if ( (top_pn.cost+top_pn.esti_routing_length+top_pn.routed_wirelength) > 
             (best_pn.cost+best_pn.routed_wirelength) ) {
            break;
        }
        f (i,-1,1) {
            f (j,-1,1) {
                Path_node temp_pn;
                double cost;
                double wl;
                temp_pn=top_pn;
                Coor coor=top_pn.path.back();
                coor.setup(coor.z,coor.x+i,coor.y+j);
                if (out_of_bdy(coor,diagonal_coor))
                    continue;           
                temp_pn.path.push_back(coor);
                temp_pn.esti_routing_length = min(abs(end.x-coor.x), abs(end.y-coor.y))*1.4+(max(abs(end.x-coor.x), abs(end.y-coor.y))-min(abs(end.x-coor.x), abs(end.y-coor.y)));
                if (i==0) {
                    wl=1.0;
                    if (j==0)   
                        continue;
                    else if (j==1) {
                        double r=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                        if (r<sg_net.demand_val)
                            continue;   
                        cost=1.0/(1+exp(r));
                    }
                    else {
                        double r=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                        if (r<sg_net.demand_val)
                            continue;   
                        cost=1.0/(1+exp(r));
                    }


                }
                else if (j==0) {
                    wl=1.0;
                    if (i==1) {
                        double r = map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                        if (r<sg_net.demand_val)
                            continue;
                        cost=1.0/(1+exp(r));
                    }
                    else {
                        double r = map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                        if (r<sg_net.demand_val)
                            continue;
                        cost=1.0/(1+exp(r));
                    }
                }
                else {
                    // wl=sqrt(pow(y_length,2)+pow(x_length,2));
                    wl=1.4;
                    if (i*j==-1) {//BR or TL
                        if (i==-1) {//TL
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[LEFT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[LEFT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[TOP]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[TOP];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            if (min(xr,next_xr)<sg_net.demand_val || min(yr,next_yr)<sg_net.demand_val)
                                continue;
                            cost = x_cost+y_cost;
                        }
                        else {//BR
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[RIGHT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[RIGHT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[BOT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[BOT];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            if (min(xr,next_xr)<sg_net.demand_val || min(yr,next_yr)<sg_net.demand_val)
                                continue;
                            cost = (x_cost+y_cost)/2.0;
                        }
                    }
                    else {
                        if (i==-1) {
                            // int z=map->size();
                            // int x=map->at(0).size();
                            // int y=map->at(0).at(0).size();
                            auto temp1=map->at(coor.z).at(coor.x).at(coor.y);
                            auto temp2=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT];
                            auto temp3=map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[LEFT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[LEFT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[BOT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[BOT];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            if (min(xr,next_xr)<sg_net.demand_val || min(yr,next_yr)<sg_net.demand_val)
                                continue;
                            cost = (x_cost+y_cost)/2.0;
                        }
                        else {
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[RIGHT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[RIGHT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[TOP]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[TOP];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            if (min(xr,next_xr)<sg_net.demand_val ||  min(yr,next_yr)<sg_net.demand_val)
                                continue;
                            cost = x_cost+y_cost;
                        }
                    }
                }
                temp_pn.cost += cost;
                temp_pn.routed_wirelength +=wl;
                if (temp_pn.path.back()==end && (temp_pn.cost+temp_pn.esti_routing_length+temp_pn.routed_wirelength) < (best_pn.cost+best_pn.routed_wirelength)) {
                    best_pn=temp_pn;
                    // min_cost = temp_pn.cost;
                    continue;
                }
                PQ.push(temp_pn);
            }
        }
        // if (idx==2) {
        //     while (!PQ.empty())
        //     {
        //         top_pn = PQ.top(); PQ.pop();
        //         Path_node temp_pn=top_pn;
        //         printf("test top_pn coor: (%d,%d,%d) cost:%f + ERL:%f = %f \n",top_pn.path.back().x,top_pn.path.back().y,top_pn.path.back().z,top_pn.cost,top_pn.esti_routing_length,top_pn.cost+top_pn.esti_routing_length);
        //     } 
        // }
        // printf("\n");
        f (k,-1,1) {

        }
        idx++;
    }
    printf("best path: ");
    for (auto it=best_pn.path.begin(); it!=best_pn.path.end(); it++) {
        // map->at(it->z).at(it->x).at(it->y).edge_temp_demand;
        printf("(%d,%d,%d)->", it->x,it->y,it->z);
    }
    printf("end\n");
    
    for (auto it=best_pn.path.begin(); it!=best_pn.path.end(); it++) {
        auto next_it=it;    next_it++;
        auto temp0 = *it;
        auto temp1 = *next_it;
        if (next_it==best_pn.path.end())
            break;
        if (next_it->x-it->x==0 || next_it->y-it->y==0) {
            if (next_it->x-it->x==1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[RIGHT]+=sg_net.demand_val;
                map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[LEFT]+=sg_net.demand_val;

            }
            else if (next_it->x-it->x==-1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[LEFT]+=sg_net.demand_val;
                map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[RIGHT]+=sg_net.demand_val;
            }
            if (next_it->y-it->y==1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[TOP]+=sg_net.demand_val;
                map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[BOT]+=sg_net.demand_val;

            }
            else if (next_it->y-it->y==-1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[BOT]+=sg_net.demand_val;
                map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[TOP]+=sg_net.demand_val;
            }
        }
        else {
            if ( (next_it->x-it->x)* (next_it->y-it->y) == -1) {//TL or BR
                if (next_it->x-it->x==-1) {//TL
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[LEFT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[RIGHT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[TOP]+=sg_net.demand_val;
                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[BOT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[LEFT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y+1).edge_temp_demand[RIGHT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[TOP]+=sg_net.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y+1).edge_temp_demand[BOT]+=sg_net.demand_val;
                }
                else {//BR
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[RIGHT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[LEFT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[BOT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[TOP]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[RIGHT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y-1).edge_temp_demand[LEFT]+=sg_net.demand_val;
                    
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[BOT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y-1).edge_temp_demand[TOP]+=sg_net.demand_val;
                }
            }
            if ( (next_it->x-it->x)* (next_it->y-it->y) == 1) {//TR or BL
                if (next_it->x-it->x==-1) {//BL
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[LEFT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[RIGHT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[BOT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[TOP]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[LEFT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y-1).edge_temp_demand[RIGHT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[BOT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y-1).edge_temp_demand[TOP]+=sg_net.demand_val;
                }
                else {//TR
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[RIGHT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[LEFT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[TOP]+=sg_net.demand_val;
                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[BOT]+=sg_net.demand_val;

                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[RIGHT]+=sg_net.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y+1).edge_temp_demand[LEFT]+=sg_net.demand_val;
                    
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[TOP]+=sg_net.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y+1).edge_temp_demand[BOT]+=sg_net.demand_val;
                }
            }
        }
            

    }
    return best_pn;
}

Path_node GR::Astar(vector<vector<vector<Cell>>>& GRmap, vector<Cluster>& GR_unit, Cluster& cluster, bool coarse) {
    printf("Astar: ");
    for (auto it=cluster.net.begin(); it!=cluster.net.end(); it++) {
        printf("%d  ",*it);
        if (*it == 104)
            printf("break point\n");
    }
     printf("\n");
    double c0;
    vector<vector<vector<Cell>>>* tmp;
    Coor start, end;
    Coor diagonal_coor;
    int x_length, y_length;
    double min_cost = 1e8;
    // double esti_demand = g_net.esti_demand;
    double esti_demand = 0;
    Path_node best_pn;
    Path_node pn; 
    std::vector<std::vector<std::vector<Cell>>>* map=&GRmap;
    priority_queue<Path_node,vector<Path_node>,compare> PQ;
    if (coarse) {
        start = cluster.start;
        end   = cluster.end;
        diagonal_coor.setup(can_use_layer_num-1,coarse_x_size-1,coarse_y_size-1);
        x_length = coarse_x_length;
        y_length = coarse_y_length;
    }
    else {
        start = cluster.start;
        end   = cluster.end;
        diagonal_coor.setup(can_use_layer_num-1,fine_x_size-1,fine_y_size-1);
        x_length = fine_x_length;
        y_length = fine_y_length;
    }
    if (start.z == 2 && *cluster.net.begin()==34)
        printf("BOTTOM layer\n");
    pn.path.push_back(start);
    pn.esti_routing_length = min(abs(end.x-start.x), abs(end.y-start.y))*1.4+(max(abs(end.x-start.x), abs(end.y-start.y))-min(abs(end.x-start.x), abs(end.y-start.y)));
    pn.cost = 0;
    c0 = pn.esti_routing_length*0.5;
    PQ.push(pn);
    int idx(0);
    while (true) {
        if (idx%1000==0)
            printf("Astar loop:%d\n",idx);
        Path_node top_pn=PQ.top();  PQ.pop();
        // if (start.z == 2)
        // printf("TOP coor: (%d,%d,%d)  \n",top_pn.path.back().x,top_pn.path.back().y,top_pn.path.back().z);
        // printf("%f",top_pn.cost);
        // printf("top_pn coor: (%d,%d,%d) routed_WL:%f cost:%f + ERL:%f = %f \n",
        // top_pn.path.back().x,top_pn.path.back().y,top_pn.path.back().z,
        // top_pn.routed_wirelength,top_pn.cost,top_pn.esti_routing_length,
        //                          top_pn.cost+top_pn.esti_routing_length);
        if ( (top_pn.cost+top_pn.esti_routing_length+top_pn.routed_wirelength) > 
             (best_pn.cost+best_pn.routed_wirelength) ) {
            break;
        }
        if (idx>150000 && best_pn.esti_routing_length==0) {
            printf("Astar no solution\n");
            break;
        }
        f (i,-1,1) {
            f (j,-1,1) {
                Path_node temp_pn;
                double cost;
                double wl;
                temp_pn=top_pn;
                Coor coor=top_pn.path.back();
                coor.setup(coor.z,coor.x+i,coor.y+j);
                if (out_of_bdy(coor,diagonal_coor))
                    continue;  
                if (map->at(coor.z).at(coor.x).at(coor.y).forbidden_region)
                    continue;         
                temp_pn.path.push_back(coor);
                temp_pn.esti_routing_length = min(abs(end.x-coor.x), abs(end.y-coor.y))*1.4+(max(abs(end.x-coor.x), abs(end.y-coor.y))-min(abs(end.x-coor.x), abs(end.y-coor.y)));
                if (i==0) {
                    wl=1.0;
                    if (j==0)   
                        continue;
                    else if (j==1) {
                        double r=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                        if (r<cluster.demand_val)
                            continue;   
                        cost=1.0/(1+exp(r));
                    }
                    else {
                        double r=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                        if (r<cluster.demand_val)
                            continue;   
                        cost=1.0/(1+exp(r));
                    }


                }
                else if (j==0) {
                    wl=1.0;
                    if (i==1) {
                        double r = map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                        if (r<cluster.demand_val)
                            continue;
                        cost=1.0/(1+exp(r));
                    }
                    else {
                        double r = map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                        if (r<cluster.demand_val)
                            continue;
                        cost=1.0/(1+exp(r));
                    }
                }
                else {
                    // wl=sqrt(pow(y_length,2)+pow(x_length,2));
                    wl=1.4;
                    if (i*j==-1) {//BR or TL
                        if (i==-1) {//TL
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[LEFT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[LEFT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[TOP]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[TOP];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            double min_xr = min(xr,next_xr);
                            double min_yr = min(yr,next_yr);
                            if (min_xr<cluster.demand_val || min_yr<cluster.demand_val)
                                continue;
                            cost = x_cost+y_cost;
                        }
                        else {//BR
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[RIGHT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[RIGHT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[BOT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[BOT];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            double min_xr = min(xr,next_xr);
                            double min_yr = min(yr,next_yr);
                            if (min_xr<cluster.demand_val || min_yr<cluster.demand_val)
                                continue;
                            cost = (x_cost+y_cost)/2.0;
                        }
                    }
                    else {
                        if (i==-1) {
                            // int z=map->size();
                            // int x=map->at(0).size();
                            // int y=map->at(0).at(0).size();
                            auto temp1=map->at(coor.z).at(coor.x).at(coor.y);
                            auto temp2=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT];
                            auto temp3=map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[LEFT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[LEFT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[BOT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[BOT];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[RIGHT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[RIGHT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[TOP]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[TOP];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            double min_xr = min(xr,next_xr);
                            double min_yr = min(yr,next_yr);
                            // printf("min_xr: %f  min_yr: %f  demand value:%f\n",min_xr,min_yr,cluster.demand_val);
                            if (min_xr<cluster.demand_val || min_yr<cluster.demand_val)
                                continue;
                            cost = (x_cost+y_cost)/2.0;
                        }
                        else {
                            double xr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[RIGHT]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[RIGHT];
                            double yr=map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_r[TOP]-map->at(coor.z).at(coor.x-i).at(coor.y-j).edge_temp_demand[TOP];
                            double next_xr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[LEFT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[LEFT];
                            double next_yr=map->at(coor.z).at(coor.x).at(coor.y).edge_r[BOT]-map->at(coor.z).at(coor.x).at(coor.y).edge_temp_demand[BOT];
                            double x_cost = 1.0/(1+exp(xr))+1.0/(1+exp(next_xr));
                            double y_cost = 1.0/(1+exp(yr))+1.0/(1+exp(next_yr));
                            double min_xr = min(xr,next_xr);
                            double min_yr = min(yr,next_yr);
                            if (min_xr<cluster.demand_val || min_yr<cluster.demand_val)
                                continue;
                            cost = x_cost+y_cost;
                        }
                    }
                }
                temp_pn.cost += cost;
                temp_pn.routed_wirelength +=wl;
                if (temp_pn.path.back()==end && (temp_pn.cost+temp_pn.esti_routing_length+temp_pn.routed_wirelength) < (best_pn.cost+best_pn.routed_wirelength)) {
                    best_pn=temp_pn;
                    // min_cost = temp_pn.cost;
                    continue;
                }
                PQ.push(temp_pn);
            }
        }
        // if (idx==2) {
        //     while (!PQ.empty())
        //     {
        //         top_pn = PQ.top(); PQ.pop();
        //         Path_node temp_pn=top_pn;
        //         printf("test top_pn coor: (%d,%d,%d) cost:%f + ERL:%f = %f \n",top_pn.path.back().x,top_pn.path.back().y,top_pn.path.back().z,top_pn.cost,top_pn.esti_routing_length,top_pn.cost+top_pn.esti_routing_length);
        //     } 
        // }
        // printf("\n");
        f (k,-1,1) {

        }
        idx++;
    }
    printf("best path: ");
    for (auto it=best_pn.path.begin(); it!=best_pn.path.end(); it++) {
        // map->at(it->z).at(it->x).at(it->y).edge_temp_demand;
        printf("(%d,%d,%d)->", it->x,it->y,it->z);
    }
    printf("end\n");
    
    for (auto it=best_pn.path.begin(); it!=best_pn.path.end(); it++) {
        auto next_it=it;    next_it++;
        auto temp0 = *it;
        auto temp1 = *next_it;
        if (next_it==best_pn.path.end())
            break;
        if (next_it->x-it->x==0 || next_it->y-it->y==0) {
            if (next_it->x-it->x==1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[RIGHT]+=cluster.demand_val;
                map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[LEFT]+=cluster.demand_val;

            }
            else if (next_it->x-it->x==-1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[LEFT]+=cluster.demand_val;
                map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[RIGHT]+=cluster.demand_val;
            }
            if (next_it->y-it->y==1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[TOP]+=cluster.demand_val;
                map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[BOT]+=cluster.demand_val;

            }
            else if (next_it->y-it->y==-1) {
                map->at(it->z).at(it->x).at(it->y).edge_temp_demand[BOT]+=cluster.demand_val;
                map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[TOP]+=cluster.demand_val;
            }
        }
        else {
            if ( (next_it->x-it->x)* (next_it->y-it->y) == -1) {//TL or BR
                if (next_it->x-it->x==-1) {//TL
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[LEFT]+=cluster.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[RIGHT]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[TOP]+=cluster.demand_val;
                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[BOT]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[LEFT]+=cluster.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y+1).edge_temp_demand[RIGHT]+=cluster.demand_val;

                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[TOP]+=cluster.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y+1).edge_temp_demand[BOT]+=cluster.demand_val;
                }
                else {//BR
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[RIGHT]+=cluster.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[LEFT]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[BOT]+=cluster.demand_val;
                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[TOP]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[RIGHT]+=cluster.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y-1).edge_temp_demand[LEFT]+=cluster.demand_val;
                    
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[BOT]+=cluster.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y-1).edge_temp_demand[TOP]+=cluster.demand_val;
                }
            }
            if ( (next_it->x-it->x)* (next_it->y-it->y) == 1) {//TR or BL
                if (next_it->x-it->x==-1) {//BL
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[LEFT]+=cluster.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[RIGHT]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[BOT]+=cluster.demand_val;
                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[TOP]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y-1).edge_temp_demand[LEFT]+=cluster.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y-1).edge_temp_demand[RIGHT]+=cluster.demand_val;

                    map->at(it->z).at(it->x-1).at(it->y).edge_temp_demand[BOT]+=cluster.demand_val;
                    map->at(it->z).at(it->x-1).at(it->y-1).edge_temp_demand[TOP]+=cluster.demand_val;
                }
                else {//TR
                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[RIGHT]+=cluster.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[LEFT]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y).edge_temp_demand[TOP]+=cluster.demand_val;
                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[BOT]+=cluster.demand_val;

                    map->at(it->z).at(it->x).at(it->y+1).edge_temp_demand[RIGHT]+=cluster.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y+1).edge_temp_demand[LEFT]+=cluster.demand_val;
                    
                    map->at(it->z).at(it->x+1).at(it->y).edge_temp_demand[TOP]+=cluster.demand_val;
                    map->at(it->z).at(it->x+1).at(it->y+1).edge_temp_demand[BOT]+=cluster.demand_val;
                }
            }
        }
            

    }
    return best_pn;
}

/**/void GR::Golbal_routing(vector<int>& GR_net) {
    printf("Global routing \n");
    cluster_relative_position.resize(can_use_layer_num);
    map<Coor,vector<int>> cpuFO_net;
    classfy_cluster_relative_position(GR_net,GR_unit);
    vector<Group_net> group_net;
    for (auto nid=GR_net.begin(); nid!=GR_net.end(); nid++) {
        // if (g!=merged_group.begin())
        //     break;
        Group_net gn;
        // printf("nid : %d\n",*nid);
        // if (*nid==46)
        //     printf("break point\n");
        Pin* cpu_p=get_cpupin(net_list.at(*nid));
        if (cpu_p->ER_coarse_cell.empty())
            printf("ER coarse cell ini fail!!\n");
        Coor coor=cpu_p->ER_coarse_cell.back();
        cpuFO_net[coor].push_back(*nid);

    }        
    // printf("sort cpu coor: \n");
    // for (auto cpu_coor=cpuFO_net.begin(); cpu_coor!=cpuFO_net.end(); cpu_coor++) {
    //     map<Coor,vector<int>> cpu_ddrFO_net;
    //     printf("cpu coor: (%d,%d,%d) \n",cpu_coor->first.x,cpu_coor->first.y,cpu_coor->first.z);
    //     for (auto nid=cpu_coor->second.begin();nid!=cpu_coor->second.end();nid++) {
    //         Pin* ddr_p=get_ddrpin(net_list.at(*nid));
    //         Coor coor=ddr_p->ER_coarse_cell.back();
    //         cpu_ddrFO_net[coor].push_back(*nid);
    //     }
    //     for (auto nets=cpu_ddrFO_net.begin(); nets!=cpu_ddrFO_net.end(); nets++) {
    //         // mg_partite_by_FO.back().push_back(nets->second);
    //         Cluster cluster;
    //         cluster.start = cpu_coor->first;
    //         cluster.end = nets->first;
    //         cluster.net = nets->second;
    //         GR_unit.push_back(cluster);
    //     }
    // }
    for (auto nid=GR_net.begin(); nid!=GR_net.end(); nid++) {
        net_list.at(*nid).update_wirelength(pin_list);
        // net_list.at(*nid).initialize(net_list);
    }
    int layer(0);
    for (auto l=GR_unit.begin(); l!=GR_unit.end(); l++) {
        for(auto cluster=l->begin(); cluster!=l->end(); cluster++) {
            add_forbidden_region(cluster->cluster_relative_idx,GR_unit.at(layer),true);
            Path_node pn=Astar(coarse_GRcell,*l, *cluster, true);
            cluster->path = pn.path;
            remove_forbidden_region(cluster->cluster_relative_idx,GR_unit.at(layer),true);
            if (!pn.path.empty())
                update_forbidden_region(coarse_GRcell,*cluster,true);
        }
        layer++;
    }

}

void GR::classfy_cluster_relative_position(vector<int>& nets, vector<vector<Cluster>>& GR_unit) {
    // map<int,vector<int>> dir_ddr_pin;
    printf("classfy_cluster_relative_position\n");
    vector<vector<int>> layer_nets(can_use_layer_num);
    GR_unit.clear();
    GR_unit.resize(can_use_layer_num);
    for (auto nid=nets.begin(); nid!=nets.end(); nid++) {
        if (!net_list.at(*nid).is2pin_net || net_list.at(*nid).ignore) {
            printf("wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }
        Pin* cpu_p=get_cpupin(net_list.at(*nid));
        Pin* ddr_p=get_ddrpin(net_list.at(*nid));
        // printf("net:%d cpu_p->layer:%d\n",*nid, cpu_p->layer);
        layer_nets.at(cpu_p->layer).push_back(*nid);
    }
    int layer(0);
    for (auto l=layer_nets.begin(); l!=layer_nets.end(); l++) {
        map<int,vector<int>> dir_cpu_pin;
        for (auto nid=l->begin(); nid!=l->end(); nid++) {
            Pin* cpu_p=get_cpupin(net_list.at(*nid));
            Pin* ddr_p=get_ddrpin(net_list.at(*nid));
            dir_cpu_pin[cpu_p->escape_dir].push_back(cpu_p->net_ID);
        }
        map<Coor,vector<int>,bool (*)(Coor, Coor)> TOP_coor_nets( Coor::greater_x);
        map<Coor,vector<int>,bool (*)(Coor, Coor)> BOT_coor_nets( Coor::less_x);
        map<Coor,vector<int>,bool (*)(Coor, Coor)> RIGHT_coor_nets( Coor::less_y);
        map<Coor,vector<int>,bool (*)(Coor, Coor)> LRFT_coor_nets( Coor::greater_y);
        for (auto nid=dir_cpu_pin[LEFT].begin(); nid!=dir_cpu_pin[LEFT].end();nid++) {
            Pin* cpu_p=get_cpupin(net_list.at(*nid));
            Coor coor=cpu_p->ER_coarse_cell.back();
            LRFT_coor_nets[coor].push_back(*nid);
        }
        for (auto nid=dir_cpu_pin[TOP].begin(); nid!=dir_cpu_pin[TOP].end();nid++) {
            Pin* cpu_p=get_cpupin(net_list.at(*nid));
            Coor coor=cpu_p->ER_coarse_cell.back();
            TOP_coor_nets[coor].push_back(*nid);
        }
        for (auto nid=dir_cpu_pin[RIGHT].begin(); nid!=dir_cpu_pin[RIGHT].end();nid++) {
            Pin* cpu_p=get_cpupin(net_list.at(*nid));
            Coor coor=cpu_p->ER_coarse_cell.back();
            RIGHT_coor_nets[coor].push_back(*nid);
        }
        for (auto nid=dir_cpu_pin[BOT].begin(); nid!=dir_cpu_pin[BOT].end();nid++) {
            Pin* cpu_p=get_cpupin(net_list.at(*nid));
            Coor coor=cpu_p->ER_coarse_cell.back();
            BOT_coor_nets[coor].push_back(*nid);
        }

        for (auto coor_nid=LRFT_coor_nets.begin(); coor_nid!=LRFT_coor_nets.end(); coor_nid++) {
            map<Coor,vector<int>,bool (*)(Coor, Coor)> TOP_ddr_coor( Coor::greater_x);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> RIGHT_ddr_coor( Coor::greater_y);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> BOT_ddr_coor( Coor::less_x);
            for (auto nid=coor_nid->second.begin(); nid!=coor_nid->second.end(); nid++) {
                Pin* ddr_p = get_ddrpin(net_list.at(*nid));
                Coor coor = ddr_p->ER_coarse_cell.back();
                if (ddr_escape.at(ddr_p->ddr_name) == TOP) {
                    TOP_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == RIGHT) {
                    RIGHT_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == BOT) {
                    BOT_ddr_coor[coor].push_back(*nid);
                }
            }
            for (auto coor_nets=BOT_ddr_coor.begin(); coor_nets!=BOT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
            for (auto coor_nets=RIGHT_ddr_coor.begin(); coor_nets!=RIGHT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
            for (auto coor_nets=TOP_ddr_coor.begin(); coor_nets!=TOP_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
        }
        for (auto coor_nid=TOP_coor_nets.begin(); coor_nid!=TOP_coor_nets.end(); coor_nid++) {
            map<Coor,vector<int>,bool (*)(Coor, Coor)> RIGHT_ddr_coor(Coor::greater_y);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> BOT_ddr_coor(Coor::greater_x);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> LEFT_ddr_coor(Coor::less_y);
            for (auto nid=coor_nid->second.begin(); nid!=coor_nid->second.end(); nid++) {
                Pin* ddr_p = get_ddrpin(net_list.at(*nid));
                Coor coor = ddr_p->ER_coarse_cell.back();
                if (ddr_escape.at(ddr_p->ddr_name) == RIGHT) {
                    RIGHT_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == BOT) {
                    BOT_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == LEFT) {
                    LEFT_ddr_coor[coor].push_back(*nid);
                }
            }  
            for (auto coor_nets=RIGHT_ddr_coor.begin(); coor_nets!=RIGHT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
            for (auto coor_nets=BOT_ddr_coor.begin(); coor_nets!=BOT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }          
            for (auto coor_nets=LEFT_ddr_coor.begin(); coor_nets!=LEFT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
        }
        for (auto coor_nid=RIGHT_coor_nets.begin(); coor_nid!=RIGHT_coor_nets.end(); coor_nid++) {
            map<Coor,vector<int>,bool (*)(Coor, Coor)> BOT_ddr_coor( Coor::greater_x);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> LEFT_ddr_coor( Coor::less_y);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> TOP_ddr_coor( Coor::less_x);
            for (auto nid=coor_nid->second.begin(); nid!=coor_nid->second.end(); nid++) {
                Pin* ddr_p = get_ddrpin(net_list.at(*nid));
                Coor coor = ddr_p->ER_coarse_cell.back();
                if (ddr_escape.at(ddr_p->ddr_name) == BOT) {
                    BOT_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == LEFT) {
                    LEFT_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == TOP) {
                    TOP_ddr_coor[coor].push_back(*nid);
                }
            }
            for (auto coor_nets=BOT_ddr_coor.begin(); coor_nets!=BOT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
            for (auto coor_nets=LEFT_ddr_coor.begin(); coor_nets!=LEFT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
            for (auto coor_nets=TOP_ddr_coor.begin(); coor_nets!=TOP_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
        }
        for (auto coor_nid=BOT_coor_nets.begin(); coor_nid!=BOT_coor_nets.end(); coor_nid++) {
            map<Coor,vector<int>,bool (*)(Coor, Coor)> LEFT_ddr_coor( Coor::less_y);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> TOP_ddr_coor( Coor::less_x);
            map<Coor,vector<int>,bool (*)(Coor, Coor)> RIGHT_ddr_coor( Coor::greater_y);
            for (auto nid=coor_nid->second.begin(); nid!=coor_nid->second.end(); nid++) {
                Pin* ddr_p = get_ddrpin(net_list.at(*nid));
                Coor coor = ddr_p->ER_coarse_cell.back();
                if (ddr_escape.at(ddr_p->ddr_name) == LEFT) {
                    LEFT_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == TOP) {
                    TOP_ddr_coor[coor].push_back(*nid);
                }
                else if (ddr_escape.at(ddr_p->ddr_name) == RIGHT) {
                    RIGHT_ddr_coor[coor].push_back(*nid);
                }
            }
            for (auto coor_nets=LEFT_ddr_coor.begin(); coor_nets!=LEFT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
            for (auto coor_nets=TOP_ddr_coor.begin(); coor_nets!=TOP_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
            for (auto coor_nets=RIGHT_ddr_coor.begin(); coor_nets!=RIGHT_ddr_coor.end(); coor_nets++) {
                for (auto nid=coor_nets->second.begin(); nid!=coor_nets->second.end(); nid++) {
                    net_list.at(*nid).cluster_relative_idx = cluster_relative_position.at(layer).size();
                }
                Cluster temp_c;
                temp_c.net = coor_nets->second;
                temp_c.cluster_relative_idx = cluster_relative_position.at(layer).size();
                temp_c.start = coor_nid->first;
                temp_c.end = coor_nets->first;
                temp_c.demand_val = coor_nets->second.size();
                GR_unit.at(layer).push_back(temp_c);
                cluster_relative_position.at(layer).push_back(coor_nets->second);
            }
        }
        layer++;
    }
    layer = 0;
    // printf("test point!!\n");
    // for (auto l=cluster_relative_position.begin(); l!=cluster_relative_position.end(); l++) {
    //     printf("layer:%d\n",layer);
    //     int c(0);
    //     for (auto cluster=l->begin(); cluster!=l->end(); cluster++) {
    //         printf("cluster:%d\n",c);
    //         for (auto nid=cluster->begin(); nid!=cluster->end(); nid++) {
    //             printf("net: %d cluster idx:%d\n",*nid,net_list.at(*nid).cluster_relative_idx);
    //         }
    //         c++;
    //     }
    //     printf("\n");
    //     layer++;
    // }
    layer = 0;
    for (auto l=GR_unit.begin(); l!=GR_unit.end(); l++) {
        printf("layer:%d\n",layer);
        for (auto cluster = l->begin(); cluster!=l->end(); cluster++) {
            printf("cluster:%d\n",cluster->cluster_relative_idx);
            for (auto nid=cluster->net.begin(); nid!=cluster->net.end(); nid++){
                printf("%d ",*nid);
            }
            printf("\n");
        }
        printf("\n");

    }
}   

vector<string> GR::sort_ddr(const map<string, vector<Pin*>>& ddr_ddrpin) {
        //deal with ddr position sort
        printf("Function sort_ddr:\n");
        vector<string> ddr_yx_bdy;
        vector<string> ddr_xy_bdy;
        vector<string> ddr_sort;
        map<int,vector<Boundary>> escape_dir_ddr;
        for (auto ddr=ddr_ddrpin.begin(); ddr!=ddr_ddrpin.end(); ddr++) {
            escape_dir_ddr[ddr_escape.at(ddr->first)].push_back(comp_boundary.at(ddr->first));
        }
        for (auto dir=escape_dir_ddr.begin(); dir!=escape_dir_ddr.end(); dir++) {
            if (dir->first==TOP) {
                sort(dir->second.begin(), dir->second.end(), Bdy_less_x);
            }
            else if (dir->first==BOT) {
                sort(dir->second.begin(), dir->second.end(), Bdy_greater_x);
            }
            else if (dir->first==RIGHT) {
                sort(dir->second.begin(), dir->second.end(), Bdy_greater_y);
            }
            else if (dir->first==LEFT) {
                sort(dir->second.begin(), dir->second.end(), Bdy_less_y);
            }
            else {
                printf("something wrong\n");
            }
        }
        for (auto dir=escape_dir_ddr.begin(); dir!=escape_dir_ddr.end(); dir++) {
            for (auto ddr=dir->second.begin(); ddr!=dir->second.end(); ddr++) {
                ddr_sort.push_back(ddr->name);
            }
        }
        printf("ddr order: ");
        for (int i=0; i<ddr_sort.size(); i++) {
            printf("%s ",ddr_sort.at(i).c_str());
            // if (ddr_yx_bdy.at(i)!=ddr_xy_bdy.at(i))
            //     printf("ddr order need chech!!!\n");
        }
        printf("\n");
        return ddr_sort;
        // {
        //     vector<Boundary> ddr_bdy;
        //     for (auto ddr=ddr_ddrpin.begin(); ddr!=ddr_ddrpin.end(); ddr++) {
        //         ddr_bdy.push_back(comp_boundary.at(ddr->first));
        //     }
        //     sort(ddr_bdy.begin(),ddr_bdy.end(), Bdy_greater_x);
        //     vector<vector<Boundary>> ddr_x_bdy;
        //     int left(-100);
        //     for (auto ddr_b=ddr_bdy.begin(); ddr_b!=ddr_bdy.end(); ddr_b++) {
        //         if (ddr_b->left-left >= tolerance) {
        //             ddr_x_bdy.push_back(vector<Boundary>());
        //         }
        //         left = ddr_b->left;
        //         ddr_x_bdy.back().push_back(*ddr_b);
        //     }
        //     for (auto ddr=ddr_x_bdy.begin(); ddr!=ddr_x_bdy.end(); ddr++) {
        //         sort(ddr->begin(),ddr->end(), Bdy_greater_y);
        //     }
        //     for (auto ddr_x=ddr_x_bdy.begin(); ddr_x!=ddr_x_bdy.end(); ddr_x++) {
        //         for (auto ddr_y=ddr_x->begin(); ddr_y!=ddr_x->end(); ddr_y++) {
        //             ddr_xy_bdy.push_back(ddr_y->name);
        //         }
        //     }
        // }
        // {
        //     vector<Boundary> ddr_bdy;
        //     for (auto ddr=ddr_ddrpin.begin(); ddr!=ddr_ddrpin.end(); ddr++) {
        //         ddr_bdy.push_back(comp_boundary.at(ddr->first));
        //     }
        //     sort(ddr_bdy.begin(),ddr_bdy.end(), Bdy_greater_y);
        //     // for (auto it=ddr_bdy.begin(); it!=ddr_bdy.end(); it++) {
        //     //     printf("%s ",it->name.c_str());
        //     // }printf("\n");
        //     vector<vector<Boundary>> ddr_y_bdy;
        //     int bot(-100000);
        //     for (auto ddr_b=ddr_bdy.begin(); ddr_b!=ddr_bdy.end(); ddr_b++) {
        //         if (ddr_b->bot-bot >= tolerance*50) {
        //             ddr_y_bdy.push_back(vector<Boundary>());
        //         }
        //         bot = ddr_b->bot;
        //         ddr_y_bdy.back().push_back(*ddr_b);
        //     }
        //     for (auto ddr=ddr_y_bdy.begin(); ddr!=ddr_y_bdy.end(); ddr++) {
        //             sort(ddr->begin(),ddr->end(), Bdy_greater_x);
        //     }
        //     for (auto ddr_y=ddr_y_bdy.begin(); ddr_y!=ddr_y_bdy.end(); ddr_y++) {
        //         for (auto ddr_x=ddr_y->begin(); ddr_x!=ddr_y->end(); ddr_x++) {
        //             ddr_yx_bdy.push_back(ddr_x->name);
        //         }
        //     }
        // }
}

void GR::sort_LCS_input( map<string, vector<Pin*>>& ddr_ddrpin,  map<string, vector<Pin*>>& ddr_cpupin, map<string, vector<vector<Pin*>>>& ddr_sort_ddrpin, map<string, vector<vector<Pin*>>>& ddr_sort_cpupin){
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
                Pin P=*(*double_it_p);
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
            if (((*(ddr->second.begin()))->escape_dir == BOT && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==RIGHT) ||
                ((*(ddr->second.begin()))->escape_dir == TOP && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==LEFT))
                sort(ddr->second.begin(),ddr->second.end(), pin_less_x);
            else
                sort(ddr->second.begin(),ddr->second.end(), pin_greater_x);
            if ((*(ddr->second.begin()))->escape_dir != TOP && (*(ddr->second.begin()))->escape_dir != BOT) {
                printf("pin escape not identical!\n");
            }
            int left(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                Pin P = *(*double_it_p);
                if (abs((*double_it_p)->real_pos.X-left) >= tolerance) {
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
            if (((*(ddr->second.begin()))->escape_dir == RIGHT && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==BOT) ||
                ((*(ddr->second.begin()))->escape_dir == LEFT && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==TOP))
                sort(ddr->second.begin(),ddr->second.end(), pin_less_y);
            else
                sort(ddr->second.begin(),ddr->second.end(), pin_greater_y);
            if ((*(ddr->second.begin()))->escape_dir != RIGHT && (*(ddr->second.begin()))->escape_dir != LEFT) {
                printf("pin escape not identical!\n");
            }
            int bot(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                if (abs((*double_it_p)->real_pos.Y-bot) >= tolerance) {
                    ddr_sort_cpupin[ddr->first].push_back(vector<Pin*>());
                }
                Pin P = *(*double_it_p);
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

void GR::sort_LCS_input( map<string, vector<Pin*>>& ddr_ddrpin,  map<string, vector<Pin*>>& ddr_cpupin, map<string, vector<vector<Pin*>>>& ddr_sort_ddrpin, vector<vector<Pin*>>& sort_cpupin){
    //sort ddr position by escape direction
    for (auto ddr=ddr_ddrpin.begin(); ddr!=ddr_ddrpin.end(); ddr++) {
        printf("sort_LCS_input ddr:%s\n",ddr->first.c_str());
        if (ddr_escape.at(ddr->first)==TOP || ddr_escape.at(ddr->first)==BOT) {
            if (ddr_escape.at(ddr->first) == TOP){//(ddr_escape.at(ddr->first) == TOP && cpu_escape.at((*(ddr->second.begin()))->ddr_name)==LEFT)
                sort(ddr->second.begin(),ddr->second.end(), pin_less_x);
                int right(1e8);
                for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                    if (abs((*double_it_p)->real_pos.X-right) >= tolerance) {
                        ddr_sort_ddrpin[ddr->first].push_back(vector<Pin*>());
                    }
                    right = (*double_it_p)->real_pos.X;
                    ddr_sort_ddrpin[ddr->first].back().push_back(*double_it_p);
                }   
                for (auto x_pin_group=ddr_sort_ddrpin[ddr->first].begin(); x_pin_group!=ddr_sort_ddrpin[ddr->first].end(); x_pin_group++) {
                    sort(x_pin_group->begin(), x_pin_group->end(), pin_less_y);
                }
            }
            else {
                sort(ddr->second.begin(),ddr->second.end(), pin_greater_x);
                int left(-100);
                for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                    if (abs((*double_it_p)->real_pos.X-left) >= tolerance) {
                        ddr_sort_ddrpin[ddr->first].push_back(vector<Pin*>());
                    }
                    left = (*double_it_p)->real_pos.X;
                    ddr_sort_ddrpin[ddr->first].back().push_back(*double_it_p);
                } 
  
                for (auto x_pin_group=ddr_sort_ddrpin[ddr->first].begin(); x_pin_group!=ddr_sort_ddrpin[ddr->first].end(); x_pin_group++) {
                    sort(x_pin_group->begin(), x_pin_group->end(), pin_greater_y);
                }
            }           
        }
        else {            
            if (ddr_escape.at(ddr->first)== LEFT ) {
                sort(ddr->second.begin(),ddr->second.end(), pin_less_y);
                int top(1000000);
                for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                    if (abs((*double_it_p)->real_pos.Y-top) >= tolerance) {
                        ddr_sort_ddrpin[ddr->first].push_back(vector<Pin*>());
                    }
                    Pin P=*(*double_it_p);
                    top = (*double_it_p)->real_pos.Y;
                    ddr_sort_ddrpin[ddr->first].back().push_back(*double_it_p);
                }
                for (auto y_pin_group=ddr_sort_ddrpin[ddr->first].begin(); y_pin_group!=ddr_sort_ddrpin[ddr->first].end(); y_pin_group++) {
                    sort(y_pin_group->begin(), y_pin_group->end(), pin_greater_x);
                }                
            }
            else {
                sort(ddr->second.begin(),ddr->second.end(), pin_greater_y);
                int bot(-100);
                for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                    if (abs((*double_it_p)->real_pos.Y-bot) >= tolerance) {
                        ddr_sort_ddrpin[ddr->first].push_back(vector<Pin*>());
                    }
                    Pin P=*(*double_it_p);
                    bot = (*double_it_p)->real_pos.Y;
                    ddr_sort_ddrpin[ddr->first].back().push_back(*double_it_p);
                }
                for (auto y_pin_group=ddr_sort_ddrpin[ddr->first].begin(); y_pin_group!=ddr_sort_ddrpin[ddr->first].end(); y_pin_group++) {
                    sort(y_pin_group->begin(), y_pin_group->end(), pin_less_x);
                }
            }
        }
    }
    //sort cpu position by escape direction
    map<int, vector<Pin*>> dir_cpupin;
    for (auto ddr=ddr_cpupin.begin(); ddr!=ddr_cpupin.end(); ddr++) {
        if((*(ddr->second.begin()))->escape_dir==TOP) {
            dir_cpupin[TOP].insert(dir_cpupin[TOP].begin(),ddr->second.begin(),ddr->second.end());
        }
        else if ((*(ddr->second.begin()))->escape_dir==BOT) {
            dir_cpupin[BOT].insert(dir_cpupin[BOT].begin(),ddr->second.begin(),ddr->second.end());
        }
        else if ((*(ddr->second.begin()))->escape_dir==RIGHT) {
            dir_cpupin[RIGHT].insert(dir_cpupin[RIGHT].begin(),ddr->second.begin(),ddr->second.end());
        }
        else if ((*(ddr->second.begin()))->escape_dir==LEFT) {
            dir_cpupin[LEFT].insert(dir_cpupin[LEFT].begin(),ddr->second.begin(),ddr->second.end());
        }
    }
    sort(dir_cpupin[TOP].begin(),dir_cpupin[TOP].end(), pin_greater_x);
    sort(dir_cpupin[RIGHT].begin(),dir_cpupin[RIGHT].end(), pin_less_y);
    sort(dir_cpupin[BOT].begin(),dir_cpupin[BOT].end(), pin_less_x);
    sort(dir_cpupin[LEFT].begin(),dir_cpupin[LEFT].end(), pin_greater_y);
    int bot(-100), top(1e8),left(-100), right(1e8),last_idx;
    for (auto double_pin_it=dir_cpupin[LEFT].begin(); double_pin_it!=dir_cpupin[LEFT].end(); double_pin_it++) {
        auto p=*double_pin_it;
        if (abs(p->real_pos.Y-bot)>=tolerance) {
            if (double_pin_it!=dir_cpupin[LEFT].begin())
                sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_greater_x);
            sort_cpupin.push_back(vector<Pin*>());
        }
        sort_cpupin.back().push_back(*double_pin_it);
        bot=p->real_pos.Y;

    }
    if (sort_cpupin.size()!=0) {
        sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_greater_x);
        last_idx=sort_cpupin.size();
    }
    for (auto double_pin_it=dir_cpupin[TOP].begin(); double_pin_it!=dir_cpupin[TOP].end(); double_pin_it++) {
        auto p=*double_pin_it;
        if (abs(p->real_pos.X-left)>=tolerance) {
            if (double_pin_it!=dir_cpupin[TOP].begin())
                sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_less_y);
            sort_cpupin.push_back(vector<Pin*>());
        }
        sort_cpupin.back().push_back(*double_pin_it);
        left=p->real_pos.X;

    }
    
    if (sort_cpupin.size()!=0 && sort_cpupin.size()!=last_idx) {
        sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_less_y);
        last_idx=sort_cpupin.size();
    }
    for (auto double_pin_it=dir_cpupin[RIGHT].begin(); double_pin_it!=dir_cpupin[RIGHT].end(); double_pin_it++) {
        auto p=*double_pin_it;
        if (abs(p->real_pos.Y-top)>=tolerance) {
            if (double_pin_it!=dir_cpupin[RIGHT].begin())
                sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_less_x);
            sort_cpupin.push_back(vector<Pin*>());
        }
        sort_cpupin.back().push_back(*double_pin_it);
        top=p->real_pos.Y;

    }
    
    if (sort_cpupin.size()!=0 && sort_cpupin.size()!=last_idx) {
        sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_less_x);
        last_idx=sort_cpupin.size();
    }
    for (auto double_pin_it=dir_cpupin[BOT].begin(); double_pin_it!=dir_cpupin[BOT].end(); double_pin_it++) {
        auto p=*double_pin_it;
        if (abs(p->real_pos.X-right)>=tolerance) {
            if (double_pin_it!=dir_cpupin[BOT].begin())
                sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_greater_y);
            sort_cpupin.push_back(vector<Pin*>());
        }
        sort_cpupin.back().push_back(*double_pin_it);
        right=p->real_pos.X;

    }
    if (sort_cpupin.size()!=0 && sort_cpupin.size()!=last_idx) {
        sort(sort_cpupin.back().begin(),sort_cpupin.back().end(),pin_greater_y);
        last_idx=sort_cpupin.size();
    }
    
    /*for (auto ddr=ddr_cpupin.begin(); ddr!=ddr_cpupin.end(); ddr++) {
        // printf("sort CPU pin\n");
        if ((*(ddr->second.begin()))->escape_dir == TOP || (*(ddr->second.begin()))->escape_dir == BOT) {
            if (((*(ddr->second.begin()))->escape_dir == BOT && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==RIGHT) ||
                ((*(ddr->second.begin()))->escape_dir == TOP && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==LEFT))
                sort(ddr->second.begin(),ddr->second.end(), pin_less_x);
            else
                sort(ddr->second.begin(),ddr->second.end(), pin_greater_x);
            if ((*(ddr->second.begin()))->escape_dir != TOP && (*(ddr->second.begin()))->escape_dir != BOT) {
                printf("pin escape not identical!\n");
            }
            int left(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                Pin P = *(*double_it_p);
                if (abs((*double_it_p)->real_pos.X-left) >= tolerance) {
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
            if (((*(ddr->second.begin()))->escape_dir == RIGHT && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==BOT) ||
                ((*(ddr->second.begin()))->escape_dir == LEFT && ddr_escape.at((*(ddr->second.begin()))->ddr_name)==TOP))
                sort(ddr->second.begin(),ddr->second.end(), pin_less_y);
            else
                sort(ddr->second.begin(),ddr->second.end(), pin_greater_y);
            if ((*(ddr->second.begin()))->escape_dir != RIGHT && (*(ddr->second.begin()))->escape_dir != LEFT) {
                printf("pin escape not identical!\n");
            }
            int bot(-100);
            for (auto double_it_p=ddr->second.begin(); double_it_p!=ddr->second.end(); double_it_p++) {
                if (abs((*double_it_p)->real_pos.Y-bot) >= tolerance) {
                    ddr_sort_cpupin[ddr->first].push_back(vector<Pin*>());
                }
                Pin P = *(*double_it_p);
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
    }*/
}

void GR::insert_rest_of_net(const vector<vector<int>>& rest_of_net) {
    // vector<vector<vector<Line>>> LCS_group_net_line;
    // vector<vector<vector<Line>>> LCS_group_net_line_align_bdy;
    vector<Line> restnet_line; //CPU         DDR
    vector<map<int,vector<pair<Detour_info,Detour_info>>>> coarse_detour_info(CPU_LCS_group.size());
    vector<map<int,vector<pair<Detour_info,Detour_info>>>> fine_detour_info(CPU_LCS_group.size());
    //merged_g nid layer  CPU              DDR
    //escape line setting
    set_line(CPU_LCS_group,DDR_LCS_group);

    // for (auto idx_g=0; idx_g<CPU_LCS_group.size();idx_g++) {
    for (auto idx_g=0; idx_g<1;idx_g++) {//only 2 pin nets
        map<int,vector<set<int>>> restnet_crossline;//rest net id : layer :cross net
        map<int,set<int>> restnet_cross_each; //rest line cross each other
        map<int,vertex> restnet_list;
        for (auto nid=rest_of_net.at(idx_g).begin(); nid!=rest_of_net.at(idx_g).end(); nid++) {
            auto n=net_list.at(*nid);
            pair<int,int> ep0 = {pin_list.at(n.net_pinID.at(0)).real_pos.X , pin_list.at(n.net_pinID.at(0)).real_pos.Y};
            pair<int,int> ep1 = {pin_list.at(n.net_pinID.at(1)).real_pos.X , pin_list.at(n.net_pinID.at(1)).real_pos.Y};
            Line l(*nid,ep0,ep1);
            restnet_line.push_back(l);
        }

        //test line of rest net crossing other 'unrouted' line in each layer
        for (auto nid=rest_of_net.at(idx_g).begin(); nid!=rest_of_net.at(idx_g).end(); nid++) {
            auto n=net_list.at(*nid);
            pair<int,int> ep0 = {pin_list.at(n.net_pinID.at(0)).real_pos.X , pin_list.at(n.net_pinID.at(0)).real_pos.Y};
            pair<int,int> ep1 = {pin_list.at(n.net_pinID.at(1)).real_pos.X , pin_list.at(n.net_pinID.at(1)).real_pos.Y};
            Line l(*nid,ep0,ep1);
            restnet_cross_each[*nid] = test_crossing(restnet_line,l);
            // printf("rest net %d corss: ",*nid);
            // for (auto it=restnet_cross_each[*nid].begin();it!=restnet_cross_each[*nid].end();it++) {
            //     printf("%d ",*it);
            // }printf("\n");
        }  
        for (auto nid=restnet_cross_each.begin();)
        restnet_list[]
        BFS(restnet_cross_each, restnet_list)

        //line of unrouted net cross the net in each layer       
        for (auto nid=rest_of_net.at(idx_g).begin(); nid!=rest_of_net.at(idx_g).end(); nid++) {
            vector<pair<int,int>> coarse_path, fine_path;
            int best_idx_g;
            int detour(1e8);
            printf("rest net %d\n",*nid);
            Net n=net_list.at(*nid);
            pair<int,int> ep0,ep1;
            auto p0 = &pin_list.at(n.net_pinID.at(0));
            auto p1 = &pin_list.at(n.net_pinID.at(1));
            ep0 = {pin_list.at(n.net_pinID.at(0)).real_pos.X , pin_list.at(n.net_pinID.at(0)).real_pos.Y};
            ep1 = {pin_list.at(n.net_pinID.at(1)).real_pos.X , pin_list.at(n.net_pinID.at(1)).real_pos.Y};
            Line n_line1(*nid,ep0,ep1);// no usage
            Line n_line2;
            {
                if (p0->escape_dir == TOP) {
                    ep0 = {p0->real_pos.X , comp_boundary.at(p0->comp_name).top};
                }
                else if (p0->escape_dir == BOT) {
                    ep0 = {p0->real_pos.X , comp_boundary.at(p0->comp_name).bot};
                }
                else if (p0->escape_dir == RIGHT) {
                    ep0 = {comp_boundary.at(p0->comp_name).right , p0->real_pos.Y};
                }
                else if (p0->escape_dir == LEFT) {
                    ep0 = {comp_boundary.at(p0->comp_name).left , p0->real_pos.Y};
                }
                if (p1->escape_dir == TOP) {
                    ep1 = {p1->real_pos.X , comp_boundary.at(p1->comp_name).top};
                }
                else if (p1->escape_dir == BOT) {
                    ep1 = {p1->real_pos.X , comp_boundary.at(p1->comp_name).bot};
                }
                else if (p1->escape_dir == RIGHT) {
                    ep1 = {comp_boundary.at(p1->comp_name).right , p1->real_pos.Y};
                }
                else if (p1->escape_dir == LEFT) {
                    ep1 = {comp_boundary.at(p1->comp_name).left , p1->real_pos.Y};
                }
                n_line2.setup(*nid,ep0,ep1);
            }
            for (int sub_idx_g=0; sub_idx_g<CPU_LCS_group.at(idx_g).size(); sub_idx_g++){
                // set<int> crossing_net1 = test_crossing(LCS_group_net_line.at(idx_g).at(sub_idx_g),n_line1);
                set<int> crossing_net2 = test_crossing(LCS_group_net_line_align_bdy.at(idx_g).at(sub_idx_g),n_line2);
                // printf("crossing_net1: ");
                // for (auto cn=crossing_net1.begin(); cn!=crossing_net1.end(); cn++) {
                //     printf("%d ",*cn);
                // }
                // printf("\n");
                // restnet_crossline[*nid].insert(restnet_crossline[*nid].begin(),crossing_net1);
                printf("crossing_net2: ");
                for (auto cn=crossing_net2.begin(); cn!=crossing_net2.end(); cn++) {
                    printf("%d ",*cn);
                }
                printf("\n");
                restnet_crossline[*nid].push_back(crossing_net2);
            }
            printf("\n");
        }
        for (auto rest_net=restnet_crossline.begin(); rest_net!=restnet_crossline.end(); rest_net++) {
            // fine_detour_info.at(idx_g)[rest_net->first].resize(rest_net->second.size());
            // coarse_detour_info.at(idx_g)[rest_net->first].resize(rest_net->second.size());
            Pin* cpu_p,*ddr_p;
            if (pin_list.at(net_list.at(rest_net->first).net_pinID.at(0)).CPU_side) {
                cpu_p=&pin_list.at(net_list.at(rest_net->first).net_pinID.at(0));
                ddr_p=&pin_list.at(net_list.at(rest_net->first).net_pinID.at(1));
            }
            else {
                cpu_p=&pin_list.at(net_list.at(rest_net->first).net_pinID.at(1));
                ddr_p=&pin_list.at(net_list.at(rest_net->first).net_pinID.at(0));
            }
            int detour_dist(1e8);
            int layer(0);
            for (auto sub_g=rest_net->second.begin(); sub_g!=rest_net->second.end(); sub_g++) {//layer
                printf ("layer %d\n",layer);
                Boundary cpu_detour_region(cpu_p->real_pos.X,cpu_p->real_pos.Y,cpu_p->real_pos.X,cpu_p->real_pos.Y),ddr_detour_region(ddr_p->real_pos.X,ddr_p->real_pos.Y,ddr_p->real_pos.X,ddr_p->real_pos.Y);
                for (auto cn=sub_g->begin(); cn!=sub_g->end(); cn++) {
                    Pin* cn_cpu_p,*cn_ddr_p;
                    if (pin_list.at(net_list.at(*cn).net_pinID.at(0)).CPU_side) {
                        cn_cpu_p=&pin_list.at(net_list.at(*cn).net_pinID.at(0));
                        cn_ddr_p=&pin_list.at(net_list.at(*cn).net_pinID.at(1));
                    }
                    else {
                        cn_cpu_p=&pin_list.at(net_list.at(*cn).net_pinID.at(1));
                        cn_ddr_p=&pin_list.at(net_list.at(*cn).net_pinID.at(0));
                    }
                    //CPU detour
                    if (cn_cpu_p->real_pos.X > cpu_detour_region.right) {
                        cpu_detour_region.right = cn_cpu_p->real_pos.X;
                    }
                    if (cn_cpu_p->real_pos.X < cpu_detour_region.left) {
                        cpu_detour_region.left = cn_cpu_p->real_pos.X;
                    }
                    if (cn_cpu_p->real_pos.Y > cpu_detour_region.top) {
                        cpu_detour_region.top = cn_cpu_p->real_pos.Y;
                    }
                    if (cn_cpu_p->real_pos.Y < cpu_detour_region.bot) {
                        cpu_detour_region.bot = cn_cpu_p->real_pos.Y;
                    }
                    //DDR detour
                    if (cn_ddr_p->real_pos.X > ddr_detour_region.right) {
                        ddr_detour_region.right = cn_ddr_p->real_pos.X;
                    }
                    if (cn_ddr_p->real_pos.X < ddr_detour_region.left) {
                        ddr_detour_region.left = cn_ddr_p->real_pos.X;
                    }
                    if (cn_ddr_p->real_pos.Y > ddr_detour_region.top) {
                        ddr_detour_region.top = cn_ddr_p->real_pos.Y;
                    }
                    if (cn_ddr_p->real_pos.Y < ddr_detour_region.bot) {
                        ddr_detour_region.bot = cn_ddr_p->real_pos.Y;
                    }   

                }    
                Boundary cpu_coarse_bdy_coor, ddr_coarse_bdy_coor;
                Boundary cpu_fine_bdy_coor, ddr_fine_bdy_coor;
                {//assign detour boundary coor
                // printf("coarse_x_length:%d  coarse_y_length:%d\n",coarse_x_length,coarse_y_length);
                    int left_coor = (cpu_detour_region.left-total_boundary.left)/coarse_x_length;
                    int right_coor =(cpu_detour_region.right-total_boundary.left)/coarse_x_length;
                    int top_coor = (cpu_detour_region.top-total_boundary.bot)/coarse_y_length;
                    int bot_coor =(cpu_detour_region.bot-total_boundary.bot)/coarse_y_length;
                    cpu_coarse_bdy_coor.setup(left_coor,bot_coor,right_coor,top_coor);

                    left_coor = (cpu_detour_region.left-total_boundary.left)/fine_x_length;
                    right_coor =(cpu_detour_region.right-total_boundary.left)/fine_x_length;
                    top_coor = (cpu_detour_region.top-total_boundary.bot)/fine_y_length;
                    bot_coor =(cpu_detour_region.bot-total_boundary.bot)/fine_y_length;
                    cpu_fine_bdy_coor.setup(left_coor,bot_coor,right_coor,top_coor);

                    left_coor = (ddr_detour_region.left-total_boundary.left)/coarse_x_length;
                    right_coor =(ddr_detour_region.right-total_boundary.left)/coarse_x_length;
                    top_coor = (ddr_detour_region.top-total_boundary.bot)/coarse_y_length;
                    bot_coor =(ddr_detour_region.bot-total_boundary.bot)/coarse_y_length;
                    ddr_coarse_bdy_coor.setup(left_coor,bot_coor,right_coor,top_coor);

                    left_coor = (ddr_detour_region.left-total_boundary.left)/fine_x_length;
                    right_coor =(ddr_detour_region.right-total_boundary.left)/fine_x_length;
                    top_coor = (ddr_detour_region.top-total_boundary.bot)/fine_y_length;
                    bot_coor =(ddr_detour_region.bot-total_boundary.bot)/fine_y_length;
                    ddr_fine_bdy_coor.setup(left_coor,bot_coor,right_coor,top_coor);
                }
                coarse_detour_info.at(idx_g)[rest_net->first].push_back(pair<Detour_info,Detour_info>({Detour_info(rest_net->first,layer),Detour_info(rest_net->first,layer)}));
                rest_net_detour(coarse_detour_info.at(idx_g)[rest_net->first].back().first ,1,1,cpu_coarse_bdy_coor,cpu_p,layer);
                rest_net_detour(coarse_detour_info.at(idx_g)[rest_net->first].back().second,0,1,ddr_coarse_bdy_coor,ddr_p,layer);
                // coarse_detour_info.at(idx_g)[rest_net->first].back().first.print();
                // coarse_detour_info.at(idx_g)[rest_net->first].back().second.print();
                // fine_detour_info.at(idx_g)[rest_net->first].push_back(pair<Detour_info,Detour_info>({Detour_info(rest_net->first,layer),Detour_info(rest_net->first,layer)}));
                // rest_net_detour(fine_detour_info.at(idx_g)[rest_net->first].back().first ,1,0,cpu_fine_bdy_coor,cpu_p);
                // rest_net_detour(fine_detour_info.at(idx_g)[rest_net->first].back().second,0,0,ddr_fine_bdy_coor,ddr_p);
                layer++;
            }printf ("\n");

        }
        for (auto rest_net=coarse_detour_info.at(idx_g).begin();rest_net!=coarse_detour_info.at(idx_g).end(); rest_net++) {
            Detour_info best_detour;
            Pin* cpu_p, *ddr_p;
            int temp1=rest_net->first;
            if (pin_list.at(net_list.at(rest_net->first).net_pinID.at(0)).CPU_side) {
                cpu_p = &pin_list.at(net_list.at(rest_net->first).net_pinID.at(0));
                ddr_p = &pin_list.at(net_list.at(rest_net->first).net_pinID.at(1));
            }
            else {
                cpu_p = &pin_list.at(net_list.at(rest_net->first).net_pinID.at(1));
                ddr_p = &pin_list.at(net_list.at(rest_net->first).net_pinID.at(0));
            }
            // if (ddr_p->pin_ID == 379)
            //     printf("break point\n");
            for (auto layer_detour=rest_net->second.begin(); layer_detour!=rest_net->second.end(); layer_detour++) {
                if (layer_detour->first.layer==0)
                    continue;
                // layer_detour->first.print();
                // layer_detour->second.print();
                if (layer_detour->first.detour_dist < layer_detour->second.detour_dist) {
                    if (layer_detour->first.detour_dist <= best_detour.detour_dist)
                        best_detour = layer_detour->first;
                }
                else {
                    if (layer_detour->second.detour_dist <= best_detour.detour_dist)
                        best_detour = layer_detour->second;
                }
            }
            best_detour.print();
            CPU_LCS_group.at(idx_g).at(best_detour.layer).push_back({best_detour.rest_net,1000});
            DDR_LCS_group.at(idx_g).at(best_detour.layer).push_back({best_detour.rest_net,1000});
            cpu_p->layer = best_detour.layer;
            ddr_p->layer = best_detour.layer;
            if (best_detour.CPU_side) {
                cpu_p->ER_coarse_cell = best_detour.path_coor;
                cpu_p->esti_escape_routing_length = best_detour.detour_dist;
                Coor temp_coor=best_detour.path_coor.back();
                if (cpu_escape.at(cpu_p->ddr_name)==TOP) 
                    temp_coor.y++;
                else if (cpu_escape.at(cpu_p->ddr_name)==BOT)
                    temp_coor.y--;
                else if (cpu_escape.at(cpu_p->ddr_name)==RIGHT)
                    temp_coor.x++;
                else if (cpu_escape.at(cpu_p->ddr_name)==LEFT)
                    temp_coor.x--;
                cpu_p->ER_coarse_cell.push_back(temp_coor);
                if (ddr_p->escape_dir == TOP || ddr_p->escape_dir == BOT) {
                    if (ddr_p->escape_dir == TOP) {
                        int top_coor = (comp_boundary.at(ddr_p->comp_name).top-total_boundary.bot)/coarse_y_length;
                        for (int y=ddr_p->coarse_coor.Y; y<=top_coor+1; y++) {
                            ddr_p->ER_coarse_cell.push_back(Coor(best_detour.layer,ddr_p->coarse_coor.X,y));
                        }
                        ddr_p->esti_escape_routing_length = comp_boundary.at(ddr_p->comp_name).top-ddr_p->real_pos.Y;
                    }
                    else if (ddr_p->escape_dir == BOT){
                        int bot_coor = (comp_boundary.at(ddr_p->comp_name).bot-total_boundary.bot)/coarse_y_length;
                        for (int y=ddr_p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            ddr_p->ER_coarse_cell.push_back(Coor(best_detour.layer,ddr_p->coarse_coor.X,y));
                        }
                        ddr_p->esti_escape_routing_length = ddr_p->real_pos.Y-comp_boundary.at(ddr_p->comp_name).bot;
                    }
                }
                else {
                    if (ddr_p->escape_dir == RIGHT) {
                        int right_coor = (comp_boundary.at(ddr_p->comp_name).right-total_boundary.left)/coarse_x_length;
                        for (int x=ddr_p->coarse_coor.X; x<=right_coor+1; x++) {
                            ddr_p->ER_coarse_cell.push_back(Coor(best_detour.layer,x,ddr_p->coarse_coor.Y));
                        }
                        ddr_p->esti_escape_routing_length = comp_boundary.at(ddr_p->comp_name).right-ddr_p->real_pos.X;
                    }
                    else if (ddr_p->escape_dir == LEFT){
                        int left_coor = (comp_boundary.at(ddr_p->comp_name).left-total_boundary.left)/coarse_x_length;
                        for (int x=ddr_p->coarse_coor.X; x>=left_coor-1; x--) {
                            ddr_p->ER_coarse_cell.push_back(Coor(best_detour.layer,x,ddr_p->coarse_coor.Y));
                        }
                        ddr_p->esti_escape_routing_length = ddr_p->real_pos.X-comp_boundary.at(ddr_p->comp_name).left;
                    }
                }
            }
            else {                
                ddr_p->ER_coarse_cell = best_detour.path_coor;
                ddr_p->esti_escape_routing_length = best_detour.detour_dist;
                Coor temp_coor=best_detour.path_coor.back();
                if (ddr_escape.at(ddr_p->ddr_name)==TOP) 
                    temp_coor.y++;
                else if (ddr_escape.at(ddr_p->ddr_name)==BOT)
                    temp_coor.y--;
                else if (ddr_escape.at(ddr_p->ddr_name)==RIGHT)
                    temp_coor.x++;
                else if (ddr_escape.at(ddr_p->ddr_name)==LEFT)
                    temp_coor.x--;
                cpu_p->ER_coarse_cell.push_back(temp_coor);
                if (cpu_p->escape_dir == TOP || cpu_p->escape_dir == BOT) {
                    if (cpu_p->escape_dir == TOP) {
                        int top_coor = (comp_boundary.at(cpu_p->comp_name).top-total_boundary.bot)/coarse_y_length;
                        for (int y=cpu_p->coarse_coor.Y; y<=top_coor+1; y++) {
                            cpu_p->ER_coarse_cell.push_back(Coor(best_detour.layer,cpu_p->coarse_coor.X,y));
                        }
                        cpu_p->esti_escape_routing_length = comp_boundary.at(cpu_p->comp_name).top-cpu_p->real_pos.Y;
                    }
                    else if (cpu_p->escape_dir == BOT){
                        int bot_coor = (comp_boundary.at(cpu_p->comp_name).bot-total_boundary.bot)/coarse_y_length;
                        for (int y=cpu_p->coarse_coor.Y; y>=bot_coor-1; y--) {
                            cpu_p->ER_coarse_cell.push_back(Coor(best_detour.layer,cpu_p->coarse_coor.X,y));
                        }
                        cpu_p->esti_escape_routing_length = cpu_p->real_pos.Y-comp_boundary.at(cpu_p->comp_name).bot;
                    }
                }
                else {
                    if (cpu_p->escape_dir == RIGHT) {
                        int right_coor = (comp_boundary.at(cpu_p->comp_name).right-total_boundary.left)/coarse_x_length;
                        for (int x=cpu_p->coarse_coor.X; x<=right_coor+1; x++) {
                            cpu_p->ER_coarse_cell.push_back(Coor(best_detour.layer,x,cpu_p->coarse_coor.Y));
                        }
                        cpu_p->esti_escape_routing_length = comp_boundary.at(cpu_p->comp_name).right-cpu_p->real_pos.X;
                    }
                    else if (cpu_p->escape_dir == LEFT){
                        int left_coor = (comp_boundary.at(cpu_p->comp_name).left-total_boundary.left)/coarse_x_length;
                        for (int x=cpu_p->coarse_coor.X; x>=left_coor-1; x--) {
                            cpu_p->ER_coarse_cell.push_back(Coor(best_detour.layer,x,cpu_p->coarse_coor.Y));
                        }
                        cpu_p->esti_escape_routing_length = cpu_p->real_pos.X-comp_boundary.at(cpu_p->comp_name).left;
                    }
                }
            }
        }
    }
}

void GR::rest_net_detour(Detour_info& di, bool is_CPU, bool is_coarse, const Boundary& bdy_coor, Pin* p, int layer) {
    di.layer = layer;
    Boundary comp_bdy_coor;
    di.CPU_side = is_CPU;
    pair<int,int> p_coor;
    int cell_x_length,cell_y_length;
    if (is_coarse) {
        cell_x_length = coarse_x_length;
        cell_y_length = coarse_y_length;
        p_coor=p->coarse_coor;
    }
    else {
        cell_x_length = fine_x_length;
        cell_y_length = fine_y_length;
        p_coor=p->fine_coor;
    }
    if (is_CPU) {
        comp_bdy_coor.setup((comp_boundary.at(CPU_name).left-total_boundary.left)/cell_x_length,
                            (comp_boundary.at(CPU_name).bot-total_boundary.bot)/cell_y_length,
                            (comp_boundary.at(CPU_name).right-total_boundary.left)/cell_x_length,
                            (comp_boundary.at(CPU_name).top-total_boundary.bot)/cell_y_length);
    }
    else {
        comp_bdy_coor.setup((comp_boundary.at(p->comp_name).left-total_boundary.left)/cell_x_length,
                            (comp_boundary.at(p->comp_name).bot-total_boundary.bot)/cell_y_length,
                            (comp_boundary.at(p->comp_name).right-total_boundary.left)/cell_x_length,
                            (comp_boundary.at(p->comp_name).top-total_boundary.bot)/cell_y_length);
    }

    if (p->escape_dir == TOP) {
        for (int y=p_coor.Y; y>=bdy_coor.bot; y--) {
            di.path_coor.push_back(Coor(di.layer,p_coor.X, y));
        }
        if (p_coor.X == bdy_coor.left ) {//right
            for (int x=bdy_coor.left+1; x<=bdy_coor.right; x++) {
                di.path_coor.push_back(Coor(di.layer,x,bdy_coor.bot));
            }
            for (int y=bdy_coor.bot+1; y<=comp_bdy_coor.top; y++) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.right,y));
            }
        }
        else if (p_coor.X == bdy_coor.right) {
            for (int x=bdy_coor.right-1; x>=bdy_coor.left; x--) {
                di.path_coor.push_back(Coor(di.layer,x,bdy_coor.bot));
            }
            for (int y=bdy_coor.bot+1; y<=comp_bdy_coor.top; y++) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.left,y));
            }
        }
        di.detour_dist = ((p_coor.Y-bdy_coor.bot)+(comp_bdy_coor.top-bdy_coor.bot))*cell_y_length+
                          (bdy_coor.right-bdy_coor.left)*cell_x_length;
        
    }
    else if (p->escape_dir == BOT) {
        for (int y=p_coor.Y; y<=bdy_coor.top; y++) {
            di.path_coor.push_back(Coor(di.layer,p_coor.X, y));
        }
        if (p_coor.X == bdy_coor.left ) {//right
            for (int x=bdy_coor.left+1; x<=bdy_coor.right; x++) {
                di.path_coor.push_back(Coor(di.layer,x,bdy_coor.top));
            }
            for (int y=bdy_coor.top-1; y>=comp_bdy_coor.bot; y--) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.right,y));
            }
        }
        else if (p_coor.X == bdy_coor.right) {
            for (int x=bdy_coor.right-1; x>=bdy_coor.left; x--) {
                di.path_coor.push_back(Coor(di.layer,x,bdy_coor.top));
            }
            for (int y=bdy_coor.top-1; y>=comp_bdy_coor.bot; y--) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.left,y));
            }
        }
        di.detour_dist = ((bdy_coor.top-p_coor.Y)+(bdy_coor.top-comp_bdy_coor.bot))*cell_y_length+
                          (bdy_coor.right-bdy_coor.left)*cell_x_length;
    }
    else if (p->escape_dir == RIGHT){        
        for (int i=p_coor.X; i>=bdy_coor.left; i--) {
            di.path_coor.push_back(Coor(di.layer, i, p_coor.Y));
        }
        if (p_coor.Y == bdy_coor.bot ) {//top
            for (int j=bdy_coor.bot+1; j<=bdy_coor.top; j++) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.left,j));
            }
            for (int i=bdy_coor.left+1; i<=comp_bdy_coor.right; i++) {
                di.path_coor.push_back(Coor(di.layer,i,bdy_coor.top));
            }
        }
        else if (p_coor.Y == bdy_coor.top) {//bot
            for (int j=bdy_coor.top-1; j>=bdy_coor.bot; j--) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.left,j));
            }
            for (int i=bdy_coor.left+1; i<=comp_bdy_coor.right; i++) {
                di.path_coor.push_back(Coor(di.layer,i,bdy_coor.bot));
            }
        }
        di.detour_dist = ((p_coor.X-bdy_coor.left)+(comp_bdy_coor.right-bdy_coor.left))*cell_x_length+
                          (bdy_coor.top-bdy_coor.bot)*cell_y_length;
    }    
    else if (p->escape_dir == LEFT){        
        for (int i=p_coor.X; i<=bdy_coor.right; i++) {
            di.path_coor.push_back(Coor(di.layer,i, p_coor.Y));
        }
        if (p_coor.Y == bdy_coor.bot ) {//top
            for (int j=bdy_coor.bot+1; j<=bdy_coor.top; j++) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.right,j));
            }
            for (int i=bdy_coor.right-1; i>=comp_bdy_coor.left; i--) {
                di.path_coor.push_back(Coor(di.layer,i,bdy_coor.top));
            }
        }
        else if (p_coor.Y == bdy_coor.top) {//bot
            for (int j=bdy_coor.top-1; j>=bdy_coor.bot; j--) {
                di.path_coor.push_back(Coor(di.layer,bdy_coor.right,j));
            }
            for (int i=bdy_coor.right-1; i>=comp_bdy_coor.left; i--) {
                di.path_coor.push_back(Coor(di.layer,i,bdy_coor.bot));
            }
        }
        di.detour_dist = ((bdy_coor.right-p_coor.X)+(bdy_coor.right-comp_bdy_coor.left))*cell_x_length+
                          (bdy_coor.top-bdy_coor.bot)*cell_y_length;
    }
    
    di.print();
}

/*void GR::partition_by_position(std::vector<std::vector<int>>& group) {
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
void GR::set_line(vector<vector<vector<pair<int,int>>>>& CPU_LCS_group, const vector<vector<vector<pair<int,int>>>>& DDR_LCS_group) {
    
    int pitch = MIN_SPACING+WIRE_WIDTH;
    for (auto g=CPU_LCS_group.begin(); g!=CPU_LCS_group.end(); g++) {
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                Pin* cpu_p, *ddr_p;
                if (pin_list.at(net_list.at(nid_shift->first).net_pinID.at(0)).CPU_side) {
                    cpu_p = &pin_list.at(net_list.at(nid_shift->first).net_pinID.at(0));
                    ddr_p = &pin_list.at(net_list.at(nid_shift->first).net_pinID.at(1));
                }
                else {
                    cpu_p = &pin_list.at(net_list.at(nid_shift->first).net_pinID.at(1));
                    ddr_p = &pin_list.at(net_list.at(nid_shift->first).net_pinID.at(0));
                }
            }
        }
    }
    for (auto g=CPU_LCS_group.begin(); g!=CPU_LCS_group.end(); g++) {
        LCS_group_net_line_align_bdy.push_back(vector<map<int,Line>>());
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            LCS_group_net_line_align_bdy.back().push_back(map<int,Line>());
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                Net n=net_list.at(nid_shift->first);
                if (!n.is2pin_net)
                    continue;
                Pin p0;
                if (pin_list.at(n.net_pinID.at(0)).CPU_side) {
                    p0=pin_list.at(n.net_pinID.at(0));
                }
                else 
                    p0=pin_list.at(n.net_pinID.at(1));
                // ep0 = {p0.real_pos.X , p0.real_pos.Y};
                // ep1 = {p1.real_pos.X , p1.real_pos.Y};
                // Line line(n.net_ID,ep0,ep1);
                // LCS_group_net_line.back().back().push_back(line);
                {
                    if (p0.escape_dir==TOP) {
                        LCS_group_net_line_align_bdy.back().back()[n.net_ID].ep1 = {p0.real_pos.X+nid_shift->second*pitch , comp_boundary.at(p0.comp_name).top+1000};
                    }
                    else if (p0.escape_dir==BOT){
                        LCS_group_net_line_align_bdy.back().back()[n.net_ID].ep1 = {p0.real_pos.X+nid_shift->second*pitch , comp_boundary.at(p0.comp_name).bot-1000};
                    }
                    else if (p0.escape_dir==RIGHT){
                        LCS_group_net_line_align_bdy.back().back()[n.net_ID].ep1 = {comp_boundary.at(p0.comp_name).right+1000 , p0.real_pos.Y+nid_shift->second*pitch};
                    }
                    else {
                        LCS_group_net_line_align_bdy.back().back()[n.net_ID].ep1 = {comp_boundary.at(p0.comp_name).left-1000 , p0.real_pos.Y+nid_shift->second*pitch};
                    }
                }
                LCS_group_net_line_align_bdy.back().back()[n.net_ID].net = n.net_ID;
            }
        }
    }
    {int idx_g(0);
    for (auto g=DDR_LCS_group.begin(); g!=DDR_LCS_group.end(); g++) {
        int idx_sub_g(0);
        for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
            for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                Net n=net_list.at(nid_shift->first);
                if (!n.is2pin_net)
                    continue;
                Pin p0;
                if (!pin_list.at(n.net_pinID.at(0)).CPU_side) {
                    p0=pin_list.at(n.net_pinID.at(0));
                }
                else 
                    p0=pin_list.at(n.net_pinID.at(1));
                // ep0 = {p0.real_pos.X , p0.real_pos.Y};
                // ep1 = {p1.real_pos.X , p1.real_pos.Y};
                // Line line(n.net_ID,ep0,ep1);
                // LCS_group_net_line.back().back().push_back(line);
                {
                    if (p0.escape_dir==TOP) {
                        LCS_group_net_line_align_bdy.at(idx_g).at(idx_sub_g)[n.net_ID].ep2 = {p0.real_pos.X+nid_shift->second*pitch , comp_boundary.at(p0.comp_name).top+1000};
                    }
                    else if (p0.escape_dir==BOT){
                        LCS_group_net_line_align_bdy.at(idx_g).at(idx_sub_g)[n.net_ID].ep2 = {p0.real_pos.X+nid_shift->second*pitch , comp_boundary.at(p0.comp_name).bot-1000};
                    }
                    else if (p0.escape_dir==RIGHT){
                        LCS_group_net_line_align_bdy.at(idx_g).at(idx_sub_g)[n.net_ID].ep2 = {comp_boundary.at(p0.comp_name).right+1000 , p0.real_pos.Y+nid_shift->second*pitch};
                    }
                    else {
                        LCS_group_net_line_align_bdy.at(idx_g).at(idx_sub_g)[n.net_ID].ep2 = {comp_boundary.at(p0.comp_name).left-1000 , p0.real_pos.Y+nid_shift->second*pitch};
                    }
                }

            }
        idx_sub_g++;
        }
        idx_g++;
    }}
}

Pin* GR::get_cpupin(const Net& n) {
    Pin* p;
    if (!n.is2pin_net)
        printf("not 2 pin net!!\n");
    if (pin_list.at(n.net_pinID.at(0)).CPU_side) {
        return &pin_list.at(n.net_pinID.at(0));
    }
    else 
        return &pin_list.at(n.net_pinID.at(1));
    
        return p;
}
Pin* GR::get_ddrpin(const Net& n) {
    Pin* p;
    if (!n.is2pin_net)
        printf("not 2 pin net!!\n");
    if (!pin_list.at(n.net_pinID.at(0)).CPU_side) {
        return &pin_list.at(n.net_pinID.at(0));
    }
    else 
        return &pin_list.at(n.net_pinID.at(1));
        return p;
}

// void GR::update_Cell(std::vector<std::vector<std::vector<Cell>>>& map, int subg_idx) {
//     int layer(0);
//     for (auto it1=map.begin(); it1!=map.end(); it1++) {
//         int x(0);
//         for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
//             int y(0);
//             for (auto it3=it2->begin(); it3!=it2->end(); it3++) {
//                 // printf("z x y (%d,%d,%d)\n",layer,x,y);
//                 // if (layer==1 && x==3 && y==9)
//                 //     printf("z x y (%d,%d,%d)\n",layer,x,y);
//                 // if (layer==1 && x==2 && y==8)
//                 //     printf("z x y (%d,%d,%d)\n",layer,x,y);
//                 for (int i=0; i<8; i++) {
//                     auto temp0 = it3->edge_r.at(i);
//                     it3->edge_r.at(i) -= it3->edge_temp_demand.at(i);
//                     it3->edge_demand.at(i) += it3->edge_temp_demand.at(i);
//                     auto temp1 = it3->edge_temp_demand.at(i);
//                     auto temp2 = it3->edge_r.at(i);
//                     it3->edge_temp_demand.at(i) = 0;
//                 }
//                 y++;
//             }
//             x++;
//         }
//         layer++;
//     }
// }
void GR::update_Cell(std::vector<std::vector<std::vector<Cell>>>& map) {
    int layer(0);
    for (auto it1=map.begin(); it1!=map.end(); it1++) {
        int x(0);
        for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
            int y(0);
            for (auto it3=it2->begin(); it3!=it2->end(); it3++) {
                // printf("z x y (%d,%d,%d)\n",layer,x,y);
                // if (layer==1 && x==3 && y==9)
                //     printf("z x y (%d,%d,%d)\n",layer,x,y);
                // if (layer==1 && x==2 && y==8)
                //     printf("z x y (%d,%d,%d)\n",layer,x,y);
                for (int i=0; i<8; i++) {
                    auto temp0 = it3->edge_r.at(i);
                    it3->edge_r.at(i) -= it3->edge_temp_demand.at(i);
                    it3->edge_demand.at(i) += it3->edge_temp_demand.at(i);
                    auto temp1 = it3->edge_temp_demand.at(i);
                    auto temp2 = it3->edge_r.at(i);
                    it3->edge_temp_demand.at(i) = 0;
                }
                y++;
            }
            x++;
        }
        layer++;
    }
}

void GR::update_forbidden_region(vector<vector<vector<Cell>>>& GRmap, Cluster& cluster, bool coarse) {
    auto n=net_list.at(cluster.net.at(0));
    int forbidden_idx = cluster.cluster_relative_idx;
    Coor diagonal_coor;
    diagonal_coor.setup(can_use_layer_num-1,coarse_x_size-1,coarse_y_size-1);
    queue<Coor> search_region;
    Pin* cpu_p=get_cpupin(n);
    Pin* ddr_p=get_ddrpin(n);
    int fbd_l(-1),fbd_r(-1),fbd_b(-1),fbd_t(-1);
    Coor CW_start_coor, CCW_start_coor;//clockwise/counterclockwise
    Coor CW_end_coor, CCW_end_coor;//clockwise/counterclockwise
    Coor CW_coor, CCW_coor;//clockwise/counterclockwise
    if (coarse) {
        CW_start_coor=cpu_p->ER_coarse_cell.back();
        CCW_start_coor=cpu_p->ER_coarse_cell.back();
        CW_end_coor=ddr_p->ER_coarse_cell.back();
        CCW_end_coor=ddr_p->ER_coarse_cell.back();
    }
    else {
        CW_start_coor=cpu_p->ER_fine_cell.back();
        CCW_start_coor=cpu_p->ER_fine_cell.back();
        CW_end_coor=ddr_p->ER_fine_cell.back();
        CCW_end_coor=ddr_p->ER_fine_cell.back();
    }
    //mark cluster path in map
    for (auto p=cluster.path.begin(); p!=cluster.path.end(); p++) {
        GRmap.at(p->z).at(p->x).at(p->y).cluster = cluster.cluster_relative_idx;
    }

    //endpoint decide
    {
        if (cpu_p->escape_dir == LEFT) {
            while (GRmap.at(CW_start_coor.z).at(CW_start_coor.x).at(CW_start_coor.y).cluster == forbidden_idx)
                CW_start_coor.y  += 1;
            while (GRmap.at(CCW_start_coor.z).at(CCW_start_coor.x).at(CCW_start_coor.y).cluster == forbidden_idx)
                CCW_start_coor.y -= 1;
            fbd_r = CW_start_coor.x+1;
        }
        else if (cpu_p->escape_dir == TOP) {
            while (GRmap.at(CW_start_coor.z).at(CW_start_coor.x).at(CW_start_coor.y).cluster == forbidden_idx)
                CW_start_coor.x  += 1;
            while (GRmap.at(CCW_start_coor.z).at(CCW_start_coor.x).at(CCW_start_coor.y).cluster == forbidden_idx)
                CCW_start_coor.x -= 1;
            fbd_b = CW_start_coor.y-1;
        }
        else if (cpu_p->escape_dir == RIGHT) {
            while (GRmap.at(CW_start_coor.z).at(CW_start_coor.x).at(CW_start_coor.y).cluster == forbidden_idx)
                CW_start_coor.y  -= 1;
            while (GRmap.at(CCW_start_coor.z).at(CCW_start_coor.x).at(CCW_start_coor.y).cluster == forbidden_idx)
                CCW_start_coor.y += 1;
            fbd_l = CW_start_coor.x-1;
        }
        else if (cpu_p->escape_dir == BOT) {
            while (GRmap.at(CW_start_coor.z).at(CW_start_coor.x).at(CW_start_coor.y).cluster == forbidden_idx) 
                CW_start_coor.x  -= 1;
            while (GRmap.at(CCW_start_coor.z).at(CCW_start_coor.x).at(CCW_start_coor.y).cluster == forbidden_idx) 
                CCW_start_coor.x += 1;
            fbd_t = CW_start_coor.y+1;
        }

        if (ddr_p->escape_dir == LEFT) {
            while (GRmap.at(CW_end_coor.z).at(CW_end_coor.x).at(CW_end_coor.y).cluster == forbidden_idx) 
                CW_end_coor.y--;
            while (GRmap.at(CCW_end_coor.z).at(CCW_end_coor.x).at(CCW_end_coor.y).cluster == forbidden_idx) 
                CCW_end_coor.y++;
            fbd_r = CW_end_coor.x+1;
        }
        else if (ddr_p->escape_dir == TOP) {
            while (GRmap.at(CW_end_coor.z).at(CW_end_coor.x).at(CW_end_coor.y).cluster == forbidden_idx) 
                CW_end_coor.x  -= 1;
            while (GRmap.at(CCW_end_coor.z).at(CCW_end_coor.x).at(CCW_end_coor.y).cluster == forbidden_idx) 
                CCW_end_coor.x += 1;
            fbd_b = CW_end_coor.y-1;
        }
        else if (ddr_p->escape_dir == RIGHT) {
            while (GRmap.at(CW_end_coor.z).at(CW_end_coor.x).at(CW_end_coor.y).cluster == forbidden_idx) 
                CW_end_coor.y  += 1;
            while (GRmap.at(CCW_end_coor.z).at(CCW_end_coor.x).at(CCW_end_coor.y).cluster == forbidden_idx) 
                CCW_end_coor.y -= 1;
            fbd_l = CW_end_coor.x-1;
        }
        else if (ddr_p->escape_dir == BOT) {
            while (GRmap.at(CW_end_coor.z).at(CW_end_coor.x).at(CW_end_coor.y).cluster == forbidden_idx) 
                CW_end_coor.x  += 1;
            while (GRmap.at(CCW_end_coor.z).at(CCW_end_coor.x).at(CCW_end_coor.y).cluster == forbidden_idx) 
                CCW_end_coor.x -= 1;
            fbd_t = CW_end_coor.y+1;
        }        
    }

    queue<Coor> CW_coor_q,CCW_coor_q;
    CW_coor_q.push(CW_start_coor);
    CCW_coor_q.push(CCW_start_coor);
    GRmap.at(CW_start_coor.z).at(CW_start_coor.x).at(CW_start_coor.y).investigated = forbidden_idx;
    GRmap.at(CCW_start_coor.z).at(CCW_start_coor.x).at(CCW_start_coor.y).investigated = forbidden_idx;

    CW_coor = CW_start_coor;
    while (CW_coor != CW_end_coor) {
        if (CW_coor_q.empty())
            printf("break point\n");
        CW_coor = CW_coor_q.front();
        CW_coor_q.pop();
        printf("CW coor:(%d,%d,%d) \n",CW_coor.z, CW_coor.x, CW_coor.y);
        
        if (CW_coor == CW_end_coor) {
            cluster.CW_path.push_back(CW_coor);
            break;
        }
        bool insert(false);
        stack<Coor> temp_stack;
        f(i,-1,1) {
            f(j,-1,1) {
                Coor temp_coor(CW_coor.z,CW_coor.x+i,CW_coor.y+j);
                
                if (out_of_bdy(temp_coor,diagonal_coor))
                    continue;   
                if (temp_coor.x == fbd_l || temp_coor.x == fbd_r)
                    continue;
                if (temp_coor.y == fbd_b || temp_coor.y == fbd_t)
                    continue;
                if (GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).cluster == forbidden_idx) {
                    insert = true;
                }
                else if (GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).investigated != forbidden_idx){
                    if (i*j==0) 
                        temp_stack.push(temp_coor);
                }
            }
        }

        if (insert) {
            printf("insert ");
            cluster.CW_path.push_back(CW_coor);
            while (!temp_stack.empty()){
                Coor temp_coor = temp_stack.top();
                temp_stack.pop();
                printf("(%d,%d,%d) ",temp_coor.z, temp_coor.x, temp_coor.y);
                CW_coor_q.push(temp_coor);
                GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).investigated = forbidden_idx;
                search_region.push(temp_coor);
            }
            printf("\n\n");
        }

    }      
    // printf("serach region  ");
    while(!search_region.empty()) {
        Coor temp_coor=search_region.front();
        // printf("(%d,%d,%d) ",temp_coor.z, temp_coor.x, temp_coor.y);
        search_region.pop();
        GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).investigated = -1;
    }  
    printf("\n");
    printf("CCW start:(%d,%d,%d)   end(%d,%d,%d)\n",CCW_start_coor.z, CCW_start_coor.x, CCW_start_coor.y,
                                                    CCW_end_coor.z, CCW_end_coor.x, CCW_end_coor.y);
    CCW_coor = CCW_start_coor;
    while (CCW_coor != CCW_end_coor) {
        if (CCW_coor_q.empty()) {
            printf("break point\n");
            break;
        }
        CCW_coor = CCW_coor_q.front();
        CCW_coor_q.pop();
        printf("CCW coor:(%d,%d,%d) \n",CCW_coor.z, CCW_coor.x, CCW_coor.y);
        if (CCW_coor == CCW_end_coor) {
            cluster.CCW_path.push_back(CCW_coor);
            break;
        }
        bool insert(false);
        stack<Coor> temp_stack;
        f(i,-1,1) {
            f(j,-1,1) {
                Coor temp_coor(CCW_coor.z,CCW_coor.x+i,CCW_coor.y+j); 
                if (out_of_bdy(temp_coor,diagonal_coor))
                    continue;   
                if (temp_coor.x == fbd_l || temp_coor.x == fbd_r)
                    continue;
                if (temp_coor.y == fbd_b || temp_coor.y == fbd_t)
                    continue;
                int a=GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).investigated;
                if (GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).cluster == forbidden_idx) {
                    insert = true;
                }
                else if (GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).investigated != forbidden_idx){
                    if (i*j ==0)
                        temp_stack.push(temp_coor);
                }
            }
        }
        if (insert) {
            printf("insert ");
            cluster.CCW_path.push_back(CCW_coor);
            while (!temp_stack.empty())
            {
                Coor temp_coor = temp_stack.top();
                temp_stack.pop();
                printf("(%d,%d,%d) ",temp_coor.z, temp_coor.x, temp_coor.y);
                CCW_coor_q.push(temp_coor);
                GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).investigated = forbidden_idx;
                search_region.push(temp_coor);
            }
            printf("\n\n");
            
        }

    }
    while(!search_region.empty()) {
        Coor temp_coor=search_region.front();
        search_region.pop();
        GRmap.at(temp_coor.z).at(temp_coor.x).at(temp_coor.y).investigated = -1;
    } 
    printf("CW(z,x,y):\n");
    for (auto coor=cluster.CW_path.begin(); coor!=cluster.CW_path.end(); coor++) {
        printf("(%d,%d,%d)  ", coor->z,coor->x,coor->y);
    }
    printf("\n");
    printf("CCW(z,x,y):\n");
    for (auto coor=cluster.CCW_path.begin(); coor!=cluster.CCW_path.end(); coor++) {
        printf("(%d,%d,%d)  ", coor->z,coor->x,coor->y);
    }
    printf("\n\n");
}

void GR::add_forbidden_region(int c_idx, vector<Cluster>& _GR_unit, bool coarse) {
    std::vector<std::vector<std::vector<Cell>>>* GR_map;
    if (coarse) {
        GR_map=&coarse_GRcell;
    }
    else
        GR_map=&fine_GRcell;
    
    int CW_idx(c_idx+1), CCW_idx(c_idx-1);
    int round(_GR_unit.size());
    if (CCW_idx<0)
        CCW_idx+=round;
    CW_idx%=round;
    while (CW_idx!=c_idx) {
        if (_GR_unit.at(CW_idx).path.size()==0) {
            CW_idx++;
            CW_idx%=round;
        }
        else 
            break;
    }
    while (CCW_idx!=c_idx) {
        if (_GR_unit.at(CW_idx).path.size()==0) {
            CCW_idx--;
            CCW_idx%=round;
            if (CCW_idx<0) {
                CCW_idx+=round;
                // printf("module wrong   idx<0\n");
            }
        }
        else 
            break;
    }
    if (CCW_idx==c_idx)
        return;
    if (CCW_idx==CW_idx) {
        if (CW_idx > c_idx) {
            if (CW_idx-c_idx < round/2)
                CCW_idx = -1;
            else 
                CW_idx = -1;
        }
        else {
            if (c_idx-CW_idx < round/2)
                CW_idx = -1;
            else 
                CCW_idx = -1;
        }

    }
    int dist(0), CW_dist, CCW_dist;
    if (CCW_idx > CW_idx)
        dist = CCW_idx-CW_idx;
    else 
        dist = CW_idx-CCW_idx;

    //test CW and CCW have any overlap
    if (CCW_idx!=-1 && CW_idx!=-1) {
        bool overlap(false);
        for (auto coor=_GR_unit.at(CW_idx).path.begin(); coor!=_GR_unit.at(CW_idx).path.end(); coor++) {
            GR_map->at(coor->z).at(coor->x).at(coor->y).cluster=CW_idx;
        }
        for (auto coor=_GR_unit.at(CCW_idx).path.begin(); coor!=_GR_unit.at(CCW_idx).path.end(); coor++) {
            if (GR_map->at(coor->z).at(coor->x).at(coor->y).cluster == CCW_idx) {
                printf ("CCW and CW overlap\n");
                overlap = true;
            }
        }
        for (auto coor=_GR_unit.at(CW_idx).path.begin(); coor!=_GR_unit.at(CW_idx).path.end(); coor++) {
            GR_map->at(coor->z).at(coor->x).at(coor->y).cluster=-1;
        }
        if (overlap) {
            CW_dist = CW_idx-c_idx>0?CW_idx-c_idx:round-(c_idx-CW_idx);
            CCW_dist = CCW_idx-c_idx>0?round-(CCW_idx-c_idx):c_idx-CCW_idx;
            if (CW_dist>CCW_dist)
                CW_idx=-1;
            else 
                CCW_idx=-1;
        }        
    }

    printf("\n");
    if (CW_idx != -1) {
        printf("CW cluster: %d\n path(z,x,y): ",CW_idx);
        for (auto coor=_GR_unit.at(CW_idx).CW_path.begin(); coor!=_GR_unit.at(CW_idx).CW_path.end();coor++) {
            printf("(%d,%d,%d)->",coor->z,coor->x,coor->y);
            GR_map->at(coor->z).at(coor->x).at(coor->y).forbidden_region=c_idx;
        }
        printf("end\n");
    }
    if (CCW_idx != -1) {
        printf("CCW cluster: %d\n path(z,x,y): ",CCW_idx);
        for (auto coor=_GR_unit.at(CCW_idx).CCW_path.begin(); coor!=_GR_unit.at(CCW_idx).CCW_path.end();coor++) {
            GR_map->at(coor->z).at(coor->x).at(coor->y).forbidden_region=c_idx;
            printf("(%d,%d,%d)->",coor->z,coor->x,coor->y);
        }
        printf("end\n");
    }
    printf("\n");
}

void GR::remove_forbidden_region(int c_idx, vector<Cluster>& _GR_unit, bool coarse) {
    std::vector<std::vector<std::vector<Cell>>>* GR_map;
    if (coarse) 
        GR_map=&coarse_GRcell;
    else
        GR_map=&fine_GRcell;
    
    int CW_idx(c_idx+1), CCW_idx(c_idx-1);
    int round(_GR_unit.size());
    if (CCW_idx<0)
        CCW_idx+=round;
    CW_idx%=round;
    while (CW_idx!=c_idx) {
        if (_GR_unit.at(CW_idx).path.size()==0) {
            CW_idx++;
            CW_idx%=round;
        }
        else 
            break;
    }
    while (CCW_idx!=c_idx) {
        if (_GR_unit.at(CW_idx).path.size()==0) {
            CCW_idx--;
            CCW_idx%=round;
            if (CCW_idx<0)
                CCW_idx+=round;
                // printf("module wrong   idx<0\n");
        }
        else 
            break;
    }
    if (CCW_idx==c_idx)
        return;
    if (CCW_idx==CW_idx) {
        if (CW_idx > c_idx) {
            if (CW_idx-c_idx < round/2)
                CCW_idx = -1;
            else 
                CW_idx = -1;
        }
        else {
            if (c_idx-CW_idx < round/2)
                CW_idx = -1;
            else 
                CCW_idx = -1;
        }

    }

    int dist(0), CW_dist, CCW_dist;
    if (CCW_idx > CW_idx)
        dist = CCW_idx-CW_idx;
    else 
        dist = CW_idx-CCW_idx;
    //test CW and CCW have any overlap
    if (CCW_idx!=-1 && CW_idx!=-1) {
        bool overlap(false);
        for (auto coor=_GR_unit.at(CW_idx).path.begin(); coor!=_GR_unit.at(CW_idx).path.end(); coor++) {
            GR_map->at(coor->z).at(coor->x).at(coor->y).cluster=CW_idx;
        }
        for (auto coor=_GR_unit.at(CCW_idx).path.begin(); coor!=_GR_unit.at(CCW_idx).path.end(); coor++) {
            if (GR_map->at(coor->z).at(coor->x).at(coor->y).cluster == CCW_idx) {
                printf ("CCW and CW overlap\n");
                overlap = true;
            }
        }
        for (auto coor=_GR_unit.at(CW_idx).path.begin(); coor!=_GR_unit.at(CW_idx).path.end(); coor++) {
            GR_map->at(coor->z).at(coor->x).at(coor->y).cluster=-1;
        }
        if (overlap) {
            CW_dist = CW_idx-c_idx>0?CW_idx-c_idx:round-(c_idx-CW_idx);
            CCW_dist = CCW_idx-c_idx>0?round-(CCW_idx-c_idx):c_idx-CCW_idx;
            if (CW_dist>CCW_dist)
                CW_idx=-1;
            else 
                CCW_idx=-1;
        }        
    }
    if (CW_idx != -1) {
        for (auto coor=_GR_unit.at(CW_idx).CW_path.begin(); coor!=_GR_unit.at(CW_idx).CW_path.end();coor++) {
            GR_map->at(coor->z).at(coor->x).at(coor->y).forbidden_region=-1;
        }
    }
    if (CCW_idx != -1) {
        for (auto coor=_GR_unit.at(CCW_idx).CCW_path.begin(); coor!=_GR_unit.at(CCW_idx).CCW_path.end();coor++) {
            GR_map->at(coor->z).at(coor->x).at(coor->y).forbidden_region=-1;
        }
    }
}