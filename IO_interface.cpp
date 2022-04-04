#include "IO_interface.h"
#define X first
#define Y second
using namespace std;

void output_gds(std::string filename, BLMR* router) {
    int lcu_idx(0);
    int pitch = MIN_SPACING+WIRE_WIDTH;
    for (auto lid=router->layer_can_use.begin(); lid!=router->layer_can_use.end();lid++) {
        std::string layer = router->layer_list.at(*lid);
        ofstream output(filename+"_"+layer+".gdt");
        int left, right, top, bot, x, y, coarse_cell_size, fine_cell_size;
        left = router->total_boundary.left;
        right = router->total_boundary.right;
        bot = router->total_boundary.bot;
        top = router->total_boundary.top;
        coarse_cell_size = (MIN_SPACING+WIRE_WIDTH)*cell_coe;
        fine_cell_size = coarse_cell_size/level_coe;
        printf("output Total boundary (%d,%d)~(%d,%d)\n",left,bot,right,top);
        printf("cell size:%d\n", coarse_cell_size);
        output<<"gds2{600\n"
            <<"m=2018-09-14 14:26:15 a=2018-09-14 14:26:15\n"
            <<"lib 'asap7sc7p5t_24_SL' 0.0025 2.5e-10\n"
            <<"cell{c=2018-09-14 14:26:15 m=2018-09-14 14:26:15 'AND2x2_ASAP7_75t_SL'\n";
            //output GR grid line
        for (size_t i = 0; i < router->coarse_GRcell.at(0).size()+1; i++)
        {
            for (size_t j = 0; j < router->coarse_GRcell.at(0).at(0).size()+1; j++)
            {
                output<<"b{999"<<" dt0 xy("<<left<<" "<<bot+j*coarse_cell_size<<" "<<left<<" "<<bot+j*coarse_cell_size
                    <<" "<<right<<" "<<bot+j*coarse_cell_size<<" "<<right<<" "<<bot+j*coarse_cell_size<<")}\n";
            }
            output<<"b{999"<<" dt0 xy("<<left+i*coarse_cell_size<<" "<<bot<<" "<<left+i*coarse_cell_size<<" "<<bot
                    <<" "<<left+i*coarse_cell_size<<" "<<top<<" "<<left+i*coarse_cell_size<<" "<<top<<")}\n";
            
        }
        for (size_t i = 0; i < router->fine_GRcell.at(0).size()+1; i++)
        {
            for (size_t j = 0; j < router->fine_GRcell.at(0).at(0).size()+1; j++)
            {
                output<<"b{999"<<" dt1 xy("<<left<<" "<<bot+j*fine_cell_size<<" "<<left<<" "<<bot+j*fine_cell_size
                    <<" "<<right<<" "<<bot+j*fine_cell_size<<" "<<right<<" "<<bot+j*fine_cell_size<<")}\n";
            }
            output<<"b{999"<<" dt1 xy("<<left+i*fine_cell_size<<" "<<bot<<" "<<left+i*fine_cell_size<<" "<<bot
                    <<" "<<left+i*fine_cell_size<<" "<<top<<" "<<left+i*fine_cell_size<<" "<<top<<")}\n";
            
        }
        int idx(200);

        // output merged group
        for (auto g=router->merged_groupID.begin(); g!=router->merged_groupID.end(); g++) {
            for (auto real_g=g->begin(); real_g!=g->end(); real_g++) {
                for (auto nid=router->group_list.at(*real_g).begin(); nid!=router->group_list.at(*real_g).end(); nid++) {
                    for (auto pid=router->net_list.at(*nid).net_pinID.begin(); pid!=router->net_list.at(*nid).net_pinID.end(); pid++) {
                        auto p=router->pin_list.at(*pid);
                        output<<"p{"<<idx<<" pt1 w1000.00 xy("<<p.real_pos.X<<" "<<p.real_pos.Y
                            <<" "<<p.real_pos.X<<" "<<p.real_pos.Y<<")}\n";    
                        output<<"t{10 tt0 mc m0.025 xy("<<p.real_pos.X<<" "<<p.real_pos.Y<<") '"<<*nid<<"'}\n";
                }
                
                }
            }
            idx++;
        }
        idx=300;
        for (auto g=router->parted_group.begin(); g!=router->parted_group.end(); g++) {
            for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
                for (auto nid=sub_g->begin(); nid!=sub_g->end(); nid++) {
                    for (auto pid=router->net_list.at(*nid).net_pinID.begin(); pid!=router->net_list.at(*nid).net_pinID.end(); pid++) {
                        auto p=router->pin_list.at(*pid);
                        output<<"p{"<<idx<<" pt1 w2000.00 xy("<<p.real_pos.X<<" "<<p.real_pos.Y
                            <<" "<<p.real_pos.X<<" "<<p.real_pos.Y<<")}\n";   
                    }
                }
                idx++;
            }
        }
        idx = 400;
        for (auto g=router->CPU_LCS_group.begin(); g!=router->CPU_LCS_group.end(); g++) {
            for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
                for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                        for (auto pid=router->net_list.at(nid_shift->first).net_pinID.begin(); pid!=router->net_list.at(nid_shift->first).net_pinID.end(); pid++) {
                            auto p=router->pin_list.at(*pid);
                            output<<"p{"<<idx<<" pt1 w2000.00 xy("<<p.real_pos.X<<" "<<p.real_pos.Y
                            <<" "<<p.real_pos.X<<" "<<p.real_pos.Y<<")}\n"; 
                        }
                    
                }
                idx++;
            }
        }
        idx = 500;
        // printf("#router->CPU_LCS_group:%d\n",router->CPU_LCS_group.size());
        for (auto g=router->CPU_LCS_group.begin(); g!=router->CPU_LCS_group.end(); g++) {
            // printf("#sub_g :%d\n",g->size());
            for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
                vector<int> non_LCS_net;
                for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                        if (nid_shift->second==1000){
                            non_LCS_net.push_back(nid_shift->first);
                            continue;
                        }
                        for (auto pid=router->net_list.at(nid_shift->first).net_pinID.begin(); pid!=router->net_list.at(nid_shift->first).net_pinID.end(); pid++) {
                            auto p=router->pin_list.at(*pid);
                            // printf("{%d,%d} ",nid_shift->first,nid_shift->second);
                            if (!p.CPU_side)
                                continue;
                            if (p.escape_dir==TOP || p.escape_dir==BOT) {
                                string ddr_name=p.ddr_name;
                                // printf("ddr name:%s\n",ddr_name.c_str());
                                if(p.escape_dir==TOP) {
                                    // nid_shift->second*=-1;
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<")}\n";
                                }
                                if(p.escape_dir==BOT) {
                                    // if(router->ddr_escape.at(p.ddr_name)==RIGHT)
                                        nid_shift->second*=-1;
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<")}\n";
                                }
                                output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+100<<" "
                                                             <<p.real_pos.X<<" "<<p.real_pos.Y-100<<" "
                                                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y-100<<" "
                                                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y+100<<")}\n";
                                
                            }
                            if (p.escape_dir==RIGHT || p.escape_dir==LEFT) {
                                string ddr_name=p.ddr_name;
                                // printf("ddr name:%s\n",ddr_name.c_str());
                                if(p.escape_dir==RIGHT) {
                                    // if(router->ddr_escape.at(p.ddr_name)==BOT)
                                    nid_shift->second*=-1;
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                                                 <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                }
                                if(p.escape_dir==LEFT) {
                                    // if(router->ddr_escape.at(p.ddr_name)==TOP)
                                    //     nid_shift->second*=-1;
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                                                 <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                }
                                output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+100<<" "<<p.real_pos.Y<<" "
                                                             <<p.real_pos.X-100<<" "<<p.real_pos.Y<<" "
                                                             <<p.real_pos.X-100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<" "
                                                             <<p.real_pos.X+100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<")}\n";
                                    // cout<<"("<<p.real_pos.X<<","<<p.real_pos.Y+pitch*nid_shift->second+100<<") ("
                                    //          <<p.real_pos.X<<","<<p.real_pos.Y+pitch*nid_shift->second-100<<") ("
                                    //          <<router->comp_boundary.at(p.comp_name).left-1000<<","<<p.real_pos.Y+pitch*nid_shift->second-100<<") ("
                                    //          <<router->comp_boundary.at(p.comp_name).left-1000<<","<<p.real_pos.Y+pitch*nid_shift->second+100<<")\n";
                                
                            }
                        }
                    
                }
                for (auto nid=non_LCS_net.begin(); nid!=non_LCS_net.end(); nid++) {
                    Pin* p;
                    if (router->pin_list.at(router->net_list.at(*nid).net_pinID.at(0)).CPU_side)
                        p=&router->pin_list.at(router->net_list.at(*nid).net_pinID.at(0));
                    else 
                        p=&router->pin_list.at(router->net_list.at(*nid).net_pinID.at(1));
                    for (auto coarse_coor=p->ER_coarse_cell.begin(); coarse_coor!=p->ER_coarse_cell.end(); coarse_coor++) {
                        output<<"b{"<<*nid<<" dt0 xy("<<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                                                     <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<" "
                                                     <<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                                                     <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                                                     <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                                                     <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                                                     <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                                                     <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<")}\n";
                    }
                }
                idx++;
            }
        }
        idx = 500;
        for (auto g=router->DDR_LCS_group.begin(); g!=router->DDR_LCS_group.end(); g++) {
            for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
                vector<int> non_LCS_net;
                for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                        if (nid_shift->second==1000){
                            non_LCS_net.push_back(nid_shift->first);
                            continue;
                        }
                        for (auto pid=router->net_list.at(nid_shift->first).net_pinID.begin(); pid!=router->net_list.at(nid_shift->first).net_pinID.end(); pid++) {
                            auto p=router->pin_list.at(*pid);
                            // printf("{%d,%d} ",nid_shift->first,nid_shift->second);
                            if (p.CPU_side)
                                continue;
                            if (p.escape_dir==TOP || p.escape_dir==BOT) {
                                if (p.escape_dir==TOP)
                                    nid_shift->second*=-1;
                                output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+100<<" "
                                                             <<p.real_pos.X<<" "<<p.real_pos.Y-100<<" "
                                                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y-100<<" "
                                                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y+100<<")}\n";
                                if(p.escape_dir==TOP)
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<")}\n";
                                if(p.escape_dir==BOT)
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<" "
                                                                <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<")}\n";
                                
                            }
                            if (p.escape_dir==RIGHT || p.escape_dir==LEFT) {
                                if (p.escape_dir==LEFT)
                                    nid_shift->second*=-1;
                                output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+100<<" "<<p.real_pos.Y<<" "
                                                             <<p.real_pos.X-100<<" "<<p.real_pos.Y<<" "
                                                             <<p.real_pos.X-100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<" "
                                                             <<p.real_pos.X+100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<")}\n";
                                if(p.escape_dir==RIGHT)
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                                                 <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                if(p.escape_dir==LEFT)
                                    output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                                                 <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                                                 <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                
                            }
                        }
                    
                }
                for (auto nid=non_LCS_net.begin(); nid!=non_LCS_net.end(); nid++) {
                    Pin* p;
                    if (!router->pin_list.at(router->net_list.at(*nid).net_pinID.at(0)).CPU_side)
                        p=&router->pin_list.at(router->net_list.at(*nid).net_pinID.at(0));
                    else 
                        p=&router->pin_list.at(router->net_list.at(*nid).net_pinID.at(1));
                    for (auto coarse_coor=p->ER_coarse_cell.begin(); coarse_coor!=p->ER_coarse_cell.end(); coarse_coor++) {
                        output<<"b{"<<*nid<<" dt0 xy("<<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                                                     <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<" "
                                                     <<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                                                     <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                                                     <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                                                     <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                                                     <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                                                     <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<")}\n";
                    }
                }
                idx++;
            }
        }
        idx = 500;
        
        for (auto g=router->LCS_group_net_line_align_bdy.begin(); g!=router->LCS_group_net_line_align_bdy.end(); g++) {
            for (auto sub_g=g->begin(); sub_g!=g->end();  sub_g++) {
                for (auto nid_l=sub_g->begin(); nid_l!=sub_g->end(); nid_l++) {
                    output<<"b{"<<idx<<" dt0 xy("<<nid_l->second.ep1.X<<" "<<nid_l->second.ep1.Y<<" "
                                                 <<nid_l->second.ep1.X<<" "<<nid_l->second.ep1.Y<<" "
                                                 <<nid_l->second.ep2.X<<" "<<nid_l->second.ep2.Y<<" "
                                                 <<nid_l->second.ep2.X<<" "<<nid_l->second.ep2.Y<<")}\n";
                }
                idx++;
            }
        }

        // group partition by postition
        // idx=200;
        // for (auto g=router->coarse_group.begin(); g!=router->coarse_group.end(); g++) {
        //     for (auto nid=g->begin(); nid!=g->end(); nid++) {
        //         for (auto pid=router->net_list.at(*nid).net_pinID.begin(); pid!=router->net_list.at(*nid).net_pinID.end(); pid++) {
        //             auto p=router->pin_list.at(*pid);
        //             // cout<<"idx: "<<idx<<endl;
        //             output<<"p{"<<idx<<" pt1 w4400.00 xy("<<p.real_pos.X<<" "<<p.real_pos.Y
        //                                 <<" "<<p.real_pos.X<<" "<<p.real_pos.Y<<")}\n";
        //         }
                
        //     }
        //     idx++;
        // }
        // idx=100;
        //output group
        // for (auto g=router->group_list.begin(); g!=router->group_list.end(); g++) {
        //     for (auto nid=g->begin(); nid!=g->end(); nid++) {
        //         for (auto pid=router->net_list.at(*nid).net_pinID.begin(); pid!=router->net_list.at(*nid).net_pinID.end(); pid++) {
        //             auto p=router->pin_list.at(*pid);
        //             output<<"p{"<<idx<<" pt1 w1700.00 xy("<<p.real_pos.X<<" "<<p.real_pos.Y
        //                 <<" "<<p.real_pos.X<<" "<<p.real_pos.Y<<")}\n";
        //         }
        //     }
        //     idx++;
        // }
        
        //output obs 
        idx = 0;
        for (auto o=router->obs_list.begin(); o!=router->obs_list.end(); o++) {
            if (o->layer == *lid)
                output<<"b{"<<idx+o->layer<<" dt0 xy("<<o->bd.left<<" "<<o->bd.bot<<" "<<o->bd.left<<" "<<o->bd.top
                    <<" "<<o->bd.right<<" "<<o->bd.top<<" "<<o->bd.right<<" "<<o->bd.bot<<")}\n";
        }

        for (size_t i=0; i<router->coarse_GRcell.at(0).size(); i++) {
            for (size_t j=0; j<router->coarse_GRcell.at(0).at(0).size(); j++) {
                int coarse_cell_size = (MIN_SPACING+WIRE_WIDTH)*cell_coe;
                pair<int,int> pos_L = {router->total_boundary.left+i*coarse_cell_size+coarse_cell_size/5,\
                                     router->total_boundary.bot +j*coarse_cell_size+coarse_cell_size/2};
                pair<int,int> pos_R = {router->total_boundary.left+i*coarse_cell_size+coarse_cell_size*4/5,\
                                     router->total_boundary.bot +j*coarse_cell_size+coarse_cell_size/2};
                pair<int,int> pos_T = {router->total_boundary.left+i*coarse_cell_size+coarse_cell_size/2,\
                                     router->total_boundary.bot +j*coarse_cell_size+coarse_cell_size*4/5};
                pair<int,int> pos_B = {router->total_boundary.left+i*coarse_cell_size+coarse_cell_size/2,\
                                     router->total_boundary.bot +j*coarse_cell_size+coarse_cell_size/5};
                int left_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[LEFT] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[LEFT];
                int right_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[RIGHT] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[RIGHT];
                int bot_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[BOT] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[BOT];
                int top_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[TOP] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[TOP];
                output<<"t{10 tt1 mc m0.025 xy("<<pos_L.X<<" "<<pos_L.Y<<") '"<<left_r<<"'}\n";
                output<<"t{10 tt1 mc m0.025 xy("<<pos_R.X<<" "<<pos_R.Y<<") '"<<right_r<<"'}\n";
                output<<"t{10 tt1 mc m0.025 xy("<<pos_B.X<<" "<<pos_B.Y<<") '"<<bot_r<<"'}\n";
                output<<"t{10 tt1 mc m0.025 xy("<<pos_T.X<<" "<<pos_T.Y<<") '"<<top_r<<"'}\n";
            }
        }
        output<<"}\n}\n";
        output.close();
        lcu_idx++;
    }
    
}



