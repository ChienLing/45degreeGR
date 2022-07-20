#include "IO_interface.h"
#include "Func.h"
#define X first
#define Y second
#define PI 3.14159265
using namespace std;
struct Const {
    public:
    double C0;
    double C1;
    double C2;
    double bound;
};

void output_gds(std::string filename, GR* router) {
    int lcu_idx(0);
    int pitch = MIN_SPACING+WIRE_WIDTH;
    for (auto lid=router->layer_can_use.begin(); lid!=router->layer_can_use.end();lid++) {
        std::string layer = router->layer_list.at(*lid);
        printf("lcu idx:%d\n",lcu_idx);
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
        for (size_t i = 0; i < router->coarse_GRcell.at(0).size()+1; i++) {
            for (size_t j = 0; j < router->coarse_GRcell.at(0).at(0).size()+1; j++)
            {
                output<<"b{0"<<" dt0 xy("<<left<<" "<<bot+j*coarse_cell_size<<" "<<left<<" "<<bot+j*coarse_cell_size
                    <<" "<<right<<" "<<bot+j*coarse_cell_size<<" "<<right<<" "<<bot+j*coarse_cell_size<<")}\n";
            }
            output<<"b{0"<<" dt0 xy("<<left+i*coarse_cell_size<<" "<<bot<<" "<<left+i*coarse_cell_size<<" "<<bot
                    <<" "<<left+i*coarse_cell_size<<" "<<top<<" "<<left+i*coarse_cell_size<<" "<<top<<")}\n";
            
        }
        // for (size_t i = 0; i < router->fine_GRcell.at(0).size()+1; i++) {
        //     for (size_t j = 0; j < router->fine_GRcell.at(0).at(0).size()+1; j++)
        //     {
        //         output<<"b{999"<<" dt1 xy("<<left<<" "<<bot+j*fine_cell_size<<" "<<left<<" "<<bot+j*fine_cell_size
        //             <<" "<<right<<" "<<bot+j*fine_cell_size<<" "<<right<<" "<<bot+j*fine_cell_size<<")}\n";
        //     }
        //     output<<"b{999"<<" dt1 xy("<<left+i*fine_cell_size<<" "<<bot<<" "<<left+i*fine_cell_size<<" "<<bot
        //             <<" "<<left+i*fine_cell_size<<" "<<top<<" "<<left+i*fine_cell_size<<" "<<top<<")}\n";
            
        // }
        int idx(1);
        
        for (auto n=router->net_list.begin(); n!=router->net_list.end(); n++) {
            if (n->net_name.find("VDD")!=std::string::npos || n->net_pinID.size()==1)
                continue;
            // if (n->belong_group.size()!=0 )
            //     continue;
            if (n->net_pinID.size()!=2 && n->net_pinID.size()>router->DDR_name.size()+1) {
                if (!(n->net_name.find("CKE")!=std::string::npos||n->net_name.find("CKN")!=std::string::npos||
                      n->net_name.find("CKP")!=std::string::npos||n->net_name.find("RAS")!=std::string::npos||
                      n->net_name.find("CAS")!=std::string::npos||n->net_name.find("WE")!=std::string::npos||
                      n->net_name.find("CS") !=std::string::npos||n->net_name.find("ODT")!=std::string::npos||
                      n->net_name.find("ALERT")!=std::string::npos))
                    continue;
            }
            // printf("other nets: %s  ",n->net_name.c_str());
            for (auto pid=n->net_pinID.begin(); pid!=n->net_pinID.end(); pid++) {
                auto p=router->pin_list.at(*pid);
                // printf("%s ",p.pin_name.c_str());
                int nid = p.net_ID;
                output<<"p{"<<idx<<" pt1 w2000.00 xy("<<p.real_pos.X<<" "<<p.real_pos.Y
                <<" "<<p.real_pos.X<<" "<<p.real_pos.Y<<")}\n"; 
                output<<"t{1000 tt0 mc m0.025 xy("<<p.real_pos.X<<" "<<p.real_pos.Y<<") '"<<nid<<"'}\n";
            }
            // printf("\n");
        }
        idx = 200;

        // output merged group 
        for (auto g=router->merged_groupID.begin(); g!=router->merged_groupID.end(); g++) {
            for (auto real_g=g->begin(); real_g!=g->end(); real_g++) {
                for (auto nid=router->group_list.at(*real_g).begin(); nid!=router->group_list.at(*real_g).end(); nid++) {
                    for (auto pid=router->net_list.at(*nid).net_pinID.begin(); pid!=router->net_list.at(*nid).net_pinID.end(); pid++) {
                        auto p=router->pin_list.at(*pid);
                        output<<"p{"<<idx<<" pt1 w1000.00 xy("<<p.real_pos.X<<" "<<p.real_pos.Y
                            <<" "<<p.real_pos.X<<" "<<p.real_pos.Y<<")}\n";    
                        // output<<"t{1000 tt0 mc m0.025 xy("<<p.real_pos.X<<" "<<p.real_pos.Y<<") '"<<*nid<<"'}\n";
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

        //print common sequence
        /**/
        if (true) {
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
                                        pair<int,int> p1 = {p.real_pos.X+pitch*nid_shift->second, p.real_pos.Y};
                                        pair<int,int> p2 = {p.real_pos.X+pitch*nid_shift->second, router->comp_boundary.at(p.comp_name).top+1000};
                                        // if (router->DDR_list.at(p.ddr_name).rotate) {
                                        // int left = router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        // int bot = router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x1 = p1.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y1 = p1.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x2 = p2.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y2 = p2.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                        //     p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                        //     p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                        //     p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                        // }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<")}\n";
                                    }
                                    if(p.escape_dir==BOT) {
                                        // if(router->ddr_escape.at(p.ddr_name)==RIGHT)
                                        pair<int,int> p1 = {p.real_pos.X+pitch*nid_shift->second, p.real_pos.Y};
                                        pair<int,int> p2 = {p.real_pos.X+pitch*nid_shift->second, router->comp_boundary.at(p.comp_name).bot-1000};
                                        // if (router->DDR_list.at(p.ddr_name).rotate) {
                                        // int left = router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        // int bot = router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x1 = p1.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y1 = p1.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x2 = p2.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y2 = p2.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                        //     p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                        //     p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                        //     p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                        // }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<")}\n";
                                    }
                                    pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y};
                                    pair<int,int> p2 = {p.real_pos.X+pitch*nid_shift->second, p.real_pos.Y};
                                    // if (router->DDR_list.at(p.ddr_name).rotate) {
                                    //     int left = router->unrotate_comp_boundary.at(p.ddr_name).left;
                                    //     int bot = router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                    //     int x1 = p1.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                    //     int y1 = p1.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                    //     int x2 = p2.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                    //     int y2 = p2.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                    //     p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                    //     p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                    //     p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                    //     p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                    // }
                                    output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                    // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+100<<" "
                                    //                             <<p.real_pos.X<<" "<<p.real_pos.Y-100<<" "
                                    //                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y-100<<" "
                                    //                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y+100<<")}\n";
                                    
                                }
                                if (p.escape_dir==RIGHT || p.escape_dir==LEFT) {
                                    string ddr_name=p.ddr_name;
                                    // printf("ddr name:%s\n",ddr_name.c_str());
                                    if(p.escape_dir==RIGHT) {
                                        // if(router->ddr_escape.at(p.ddr_name)==BOT)
                                        pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y+pitch*nid_shift->second};
                                        pair<int,int> p2 = {router->comp_boundary.at(p.comp_name).right+1000, p.real_pos.Y+pitch*nid_shift->second};
                                        // if (router->DDR_list.at(p.ddr_name).rotate) {
                                        // int left = router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        // int bot = router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x1 = p1.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y1 = p1.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x2 = p2.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y2 = p2.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                        //     p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                        //     p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                        //     p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                        // }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                        //                             <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                    }
                                    if(p.escape_dir==LEFT) {
                                        pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y+pitch*nid_shift->second};
                                        pair<int,int> p2 = {router->comp_boundary.at(p.comp_name).left-1000, p.real_pos.Y+pitch*nid_shift->second};
                                        // if (router->DDR_list.at(p.ddr_name).rotate) {
                                        // int left = router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        // int bot = router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x1 = p1.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y1 = p1.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     int x2 = p2.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                        //     int y2 = p2.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                        //     p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                        //     p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                        //     p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                        //     p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                        // }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                        //                             <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                    }
                                    pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y};
                                    pair<int,int> p2 = {p.real_pos.X, p.real_pos.Y+pitch*nid_shift->second};
                                    // if (router->DDR_list.at(p.ddr_name).rotate) {
                                    //     int left = router->unrotate_comp_boundary.at(p.ddr_name).left;
                                    //     int bot = router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                    //     int x1 = p1.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                    //     int y1 = p1.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                    //     int x2 = p2.X-router->unrotate_comp_boundary.at(p.ddr_name).left;
                                    //     int y2 = p2.Y-router->unrotate_comp_boundary.at(p.ddr_name).bot;
                                    //     p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                    //     p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                    //     p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                    //     p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                    // }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                    // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+100<<" "<<p.real_pos.Y<<" "
                                    //                             <<p.real_pos.X-100<<" "<<p.real_pos.Y<<" "
                                    //                             <<p.real_pos.X-100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<" "
                                    //                             <<p.real_pos.X+100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<")}\n";
                                        // cout<<"("<<p.real_pos.X<<","<<p.real_pos.Y+pitch*nid_shift->second+100<<") ("
                                        //          <<p.real_pos.X<<","<<p.real_pos.Y+pitch*nid_shift->second-100<<") ("
                                        //          <<router->comp_boundary.at(p.comp_name).left-1000<<","<<p.real_pos.Y+pitch*nid_shift->second-100<<") ("
                                        //          <<router->comp_boundary.at(p.comp_name).left-1000<<","<<p.real_pos.Y+pitch*nid_shift->second+100<<")\n";
                                    
                                }
                            }
                        
                    }
                    // cout<<"non_LCS_net size: "<<non_LCS_net.size()<<"\n";
                    for (auto nid=non_LCS_net.begin(); nid!=non_LCS_net.end(); nid++) {
                        Pin* p;
                        if (router->pin_list.at(router->net_list.at(*nid).net_pinID.at(0)).CPU_side)
                            p=&router->pin_list.at(router->net_list.at(*nid).net_pinID.at(0));
                        else 
                            p=&router->pin_list.at(router->net_list.at(*nid).net_pinID.at(1));
                        for (auto coarse_coor=p->ER_coarse_cell.begin(); coarse_coor!=p->ER_coarse_cell.end(); coarse_coor++) {
                            output<<"b{"<<*nid<<" dt"<<p->layer<< " xy("<<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                                                        <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<" "
                                                        <<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                                                        <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                                                        <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                                                        <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                                                        <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                                                        <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<")}\n";
                            // cout<<"b{"<<*nid<<" dt"<<p->layer<< " xy("<<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                            //                              <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<" "
                            //                              <<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
                            //                              <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                            //                              <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                            //                              <<(coarse_coor->y+1)*coarse_y_length+router->total_boundary.bot<<" "
                            //                              <<(coarse_coor->x+1)*coarse_x_length+router->total_boundary.left<<" "
                            //                              <<coarse_coor->y*coarse_y_length+router->total_boundary.bot<<")}\n";
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
                                    // if (p.escape_dir==TOP)                    
                                    // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+100<<" "
                                    //                             <<p.real_pos.X<<" "<<p.real_pos.Y-100<<" "
                                    //                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y-100<<" "
                                    //                             <<p.real_pos.X+pitch*nid_shift->second<<" "<<p.real_pos.Y+100<<")}\n";
                                    if(p.escape_dir==TOP) {
                                        pair<int,int> p1 = {p.real_pos.X+pitch*nid_shift->second, p.real_pos.Y};
                                        pair<int,int> p2 = {p.real_pos.X+pitch*nid_shift->second, router->comp_boundary.at(p.comp_name).top+1000};
                                        if (router->DDR_list.at(p.ddr_name).rotate) {
                                            int left = p.real_pos.X;
                                            int bot = p.real_pos.Y;
                                            // printf("net:%d pin:(%d,%d)\n",p.net_ID,p.real_pos.X,p.real_pos.Y);
                                            int x1 = p1.X-left;
                                            int y1 = p1.Y-bot;
                                            int x2 = p2.X-left;
                                            int y2 = p2.Y-bot;
                                            p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                            p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                            p2.X = p.fanout_pos.X;
                                            p2.Y = p.fanout_pos.Y;
                                        }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";

                                    }
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).top+1000<<")}\n";
                                    if(p.escape_dir==BOT){
                                        pair<int,int> p1 = {p.real_pos.X+pitch*nid_shift->second, p.real_pos.Y};
                                        pair<int,int> p2 = {p.real_pos.X+pitch*nid_shift->second, router->comp_boundary.at(p.comp_name).bot-1000};
                                        if (router->DDR_list.at(p.ddr_name).rotate) {
                                            int left = p.real_pos.X;
                                            int bot = p.real_pos.Y;
                                            int x1 = p1.X-left;
                                            int y1 = p1.Y-bot;
                                            int x2 = p2.X-left;
                                            int y2 = p2.Y-bot;
                                            p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                            p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                            p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                            p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                        }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";

                                    }
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+pitch*nid_shift->second+100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<p.real_pos.Y<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second-100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<" "
                                        //                             <<p.real_pos.X+pitch*nid_shift->second+100<<" "<<router->comp_boundary.at(p.comp_name).bot-1000<<")}\n";
                                    pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y};
                                    pair<int,int> p2 = {p.real_pos.X+pitch*nid_shift->second, p.real_pos.Y};
                                    if (router->DDR_list.at(p.ddr_name).rotate) {
                                        int left = p.real_pos.X;
                                        int bot = p.real_pos.Y;
                                        int x1 = p1.X-left;
                                        int y1 = p1.Y-bot;
                                        int x2 = p2.X-left;
                                        int y2 = p2.Y-bot;
                                        p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                        p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                        p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                        p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                    }
                                    output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                    
                                }
                                if (p.escape_dir==RIGHT || p.escape_dir==LEFT) {
                                    // if (p.escape_dir==LEFT)
                                    // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X+100<<" "<<p.real_pos.Y<<" "
                                    //                             <<p.real_pos.X-100<<" "<<p.real_pos.Y<<" "
                                    //                             <<p.real_pos.X-100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<" "
                                    //                             <<p.real_pos.X+100<<" "<<p.real_pos.Y+pitch*nid_shift->second<<")}\n";
                                    if(p.escape_dir==RIGHT) {
                                        pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y+pitch*nid_shift->second};
                                        pair<int,int> p2 = {router->comp_boundary.at(p.comp_name).right+1000, p.real_pos.Y+pitch*nid_shift->second};
                                        if (router->DDR_list.at(p.ddr_name).rotate) {
                                            int left = p.real_pos.X;
                                            int bot = p.real_pos.Y;
                                            int x1 = p1.X-left;
                                            int y1 = p1.Y-bot;
                                            int x2 = p2.X-left;
                                            int y2 = p2.Y-bot;
                                            p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                            p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                            p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                            p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                        }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";

                                    }
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                        //                             <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).right+1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                    if(p.escape_dir==LEFT) {
                                        pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y+pitch*nid_shift->second};
                                        pair<int,int> p2 = {router->comp_boundary.at(p.comp_name).left-1000, p.real_pos.Y+pitch*nid_shift->second};
                                        if (router->DDR_list.at(p.ddr_name).rotate) {
                                            int left = p.real_pos.X;
                                            int bot = p.real_pos.Y;
                                            int x1 = p1.X-left;
                                            int y1 = p1.Y-bot;
                                            int x2 = p2.X-left;
                                            int y2 = p2.Y-bot;
                                            p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                            p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                            p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                            p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                        }
                                        output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";

                                    }
                                        // output<<"b{"<<idx<<" dt0 xy("<<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<" "
                                        //                             <<p.real_pos.X<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second-100<<" "
                                        //                             <<router->comp_boundary.at(p.comp_name).left-1000<<" "<<p.real_pos.Y+pitch*nid_shift->second+100<<")}\n";
                                    pair<int,int> p1 = {p.real_pos.X, p.real_pos.Y};
                                    pair<int,int> p2 = {p.real_pos.X, p.real_pos.Y+pitch*nid_shift->second};
                                    if (router->DDR_list.at(p.ddr_name).rotate) {
                                        int left = p.real_pos.X;
                                        int bot = p.real_pos.Y;
                                        int x1 = p1.X-left;
                                        int y1 = p1.Y-bot;
                                        int x2 = p2.X-left;
                                        int y2 = p2.Y-bot;
                                        p1.X = x1*(1/sqrt(2))-y1*(1/sqrt(2))+left;
                                        p1.Y = x1*(1/sqrt(2))+y1*(1/sqrt(2))+bot;
                                        p2.X = x2*(1/sqrt(2))-y2*(1/sqrt(2))+left;
                                        p2.Y = x2*(1/sqrt(2))+y2*(1/sqrt(2))+bot;
                                    }
                                    output<<"p{"<<idx<<" dt0 pt1 w100.00 xy("<<p1.X<<" "<<p1.Y<<" "<<p2.X<<" "<<p2.Y<<")}\n";
                                    
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
                            output<<"b{"<<*nid<<" dt"<<p->layer<< " xy("<<coarse_coor->x*coarse_x_length+router->total_boundary.left<<" "
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
                        // output<<"b{"<<idx<<" dt0 xy("<<nid_l->second.ep1.X<<" "<<nid_l->second.ep1.Y<<" "
                        //                              <<nid_l->second.ep1.X<<" "<<nid_l->second.ep1.Y<<" "
                        //                              <<nid_l->second.ep2.X<<" "<<nid_l->second.ep2.Y<<" "
                        //                              <<nid_l->second.ep2.X<<" "<<nid_l->second.ep2.Y<<")}\n";
                        int cluster_idx = router->net_list.at(nid_l->first).cluster_relative_idx;
                        output<<"b{"<<idx<<" dt"<<cluster_idx<<" xy("<<nid_l->second.ep1.X-100<<" "<<nid_l->second.ep1.Y<<" "
                                                    <<nid_l->second.ep1.X+100<<" "<<nid_l->second.ep1.Y<<" "
                                                    <<nid_l->second.ep2.X+100<<" "<<nid_l->second.ep2.Y<<" "
                                                    <<nid_l->second.ep2.X-100<<" "<<nid_l->second.ep2.Y<<")}\n";
                        // output<<"p{"<<idx<<"dt"<<cluster_idx<<" pt1 w8.00 xy("<<nid_l->second.ep1.X<<" "<<nid_l->second.ep1.Y
                        //     <<" "<<nid_l->second.ep2.X<<" "<<nid_l->second.ep2.Y<<")}\n"; 
                    }
                    idx++;
                }
            }
            
            idx = 500;
            for (auto g=router->CPU_LCS_group.begin(); g!=router->CPU_LCS_group.end(); g++) {
                for (auto sub_g=g->begin(); sub_g!=g->end(); sub_g++) {
                    for (auto nid_shift=sub_g->begin(); nid_shift!=sub_g->end(); nid_shift++) {
                        for (auto pid=router->net_list.at(nid_shift->first).net_pinID.begin(); pid!=router->net_list.at(nid_shift->first).net_pinID.end(); pid++) { 
                            pair<int,int> p1=router->pin_list.at(*pid).fanout_pos;
                            output<<"p{"<<idx<<" dt0 pt2 w800.00 xy("<<p1.X<<" "<<p1.Y
                                    <<" "<<p1.X<<" "<<p1.Y<<")}\n"; 
                        }
                    }
                idx++;
                }
            }
        }
        // idx=700;
        // for (auto l=router->l_restnet_line.begin(); l!=router->l_restnet_line.end(); l++) {
        //     for (auto line=l->begin(); line!=l->end(); line++) {
        //         output<<"p{"<<idx<<" dt"<< line->net <<" pt1 w200.00 xy("<<line->ep1.X<<" "<<line->ep1.Y
        //                     <<" "<<line->ep2.X<<" "<<line->ep2.Y<<")}\n"; 
        //     }
        //     idx++;
        // }
        idx = 600;
        
        int hor_edge_num = (router->coarse_x_size-1)*router->coarse_y_size;
        int ver_edge_num = router->coarse_x_size*(router->coarse_y_size-1);
        int RT_edge_num = (router->coarse_x_size-1)*(router->coarse_y_size-1);
        int RB_edge_num = (router->coarse_x_size-1)*(router->coarse_y_size-1);
        int stage1 = hor_edge_num;
        int stage2 = hor_edge_num+ver_edge_num;
        int stage3 = hor_edge_num+ver_edge_num+RT_edge_num;
        // for (auto mg=router->merged_group_net.begin(); mg!=router->merged_group_net.end(); mg++) {
        //     for (auto pos_g=mg->sgn.begin(); pos_g!=mg->sgn.end(); pos_g++) {
        //         for (auto coor=pos_g->path_node.path.begin();coor!=pos_g->path_node.path.end();coor++) {
        //             auto next_coor=coor;    next_coor++;
        //             if (next_coor==pos_g->path_node.path.end())
        //                 break;
        //             if (coor->z!=lcu_idx || next_coor->z!=lcu_idx)
        //                 continue;
        //             pair<int,int> p1 = {router->total_boundary.left+(double)(coor->x+0.5)*coarse_x_length , router->total_boundary.bot+(double)(coor->y+0.5)*coarse_y_length};
        //             pair<int,int> p2 = {router->total_boundary.left+(double)(next_coor->x+0.5)*coarse_x_length , router->total_boundary.bot+(double)(next_coor->y+0.5)*coarse_y_length};
        //                     output<<"p{"<<idx<<" pt1 w800.00 xy("<<p1.X<<" "<<p1.Y
        //                     <<" "<<p2.X<<" "<<p2.Y<<")}\n"; 
        //         }
        //         idx++;
        //     }
        // }
        int layer_num(0);
        for (auto l=router->GR_unit.begin(); l!=router->GR_unit.end(); l++) {
            int c(0);
            // printf("layer:%d\n",layer_num);
            for (auto cluster=l->begin(); cluster!=l->end(); cluster++) {
                // printf("c:%d\n",c);
                for (auto coor=cluster->path.begin(); coor!=cluster->path.end(); coor++) {
                    auto next_coor=coor;    next_coor++;
                    if (next_coor==cluster->path.end())
                        break;
                    // if (coor->z!=lcu_idx || next_coor->z!=lcu_idx)
                    //     continue;
                    pair<int,int> p1 = {router->total_boundary.left+(double)(coor->x+0.5)*coarse_x_length , 
                                        router->total_boundary.bot+(double)(coor->y+0.5)*coarse_y_length};
                    pair<int,int> p2 = {router->total_boundary.left+(double)(next_coor->x+0.5)*coarse_x_length , 
                                        router->total_boundary.bot+(double)(next_coor->y+0.5)*coarse_y_length};
                            output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt1 w800.00 xy("<<p1.X<<" "<<p1.Y
                            <<" "<<p2.X<<" "<<p2.Y<<")}\n"; 
                }
                for (auto coor=cluster->CW_path.begin(); coor!=cluster->CW_path.end(); coor++) {
                    pair<int,int> p1 = {router->total_boundary.left+(double)(coor->x+0.5)*coarse_x_length , 
                                        router->total_boundary.bot+(double)(coor->y+0.5)*coarse_y_length};
                    output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt1 w800.00 xy("<<p1.X<<" "<<p1.Y
                            <<" "<<p1.X<<" "<<p1.Y<<")}\n"; 
                }                
                for (auto coor=cluster->CCW_path.begin(); coor!=cluster->CCW_path.end(); coor++) {
                    pair<int,int> p1 = {router->total_boundary.left+(double)(coor->x+0.5)*coarse_x_length , 
                                        router->total_boundary.bot+(double)(coor->y+0.5)*coarse_y_length};
                    output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt2 w800.00 xy("<<p1.X<<" "<<p1.Y
                            <<" "<<p1.X<<" "<<p1.Y<<")}\n"; 
                }
                for (auto edge_idx=cluster->CW_fbd_edge.begin(); edge_idx!=cluster->CW_fbd_edge.end(); edge_idx++) {
                    pair<int,int> p1;
                    int edge_i = *edge_idx;
                    int y,x;
                    if (edge_i<stage1) {//hor
                        y = edge_i/(router->coarse_x_size-1);
                        x = edge_i%(router->coarse_x_size-1);
                        p1 = {router->total_boundary.left+(double)(x+1)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+0.5)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt1 w400.00 xy("<<p1.X-1000<<" "<<p1.Y
                                <<" "<<p1.X+1000<<" "<<p1.Y<<")}\n"; 
                    }
                    else if (edge_i<stage2) {
                        edge_i-=stage1;
                        y = edge_i/(router->coarse_y_size-1);
                        x = edge_i%(router->coarse_y_size-1);
                        p1 = {router->total_boundary.left+(double)(x+0.5)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+1)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt1 w400.00 xy("<<p1.X<<" "<<p1.Y-1000
                                <<" "<<p1.X<<" "<<p1.Y+1000<<")}\n"; 
                    }
                    else if (edge_i<stage3) {
                        edge_i-=stage2;
                        y = edge_i/(router->coarse_x_size-1);
                        x = edge_i%(router->coarse_x_size-1);
                        p1 = {router->total_boundary.left+(double)(x+1)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+1)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt1 w400.00 xy("<<p1.X-500<<" "<<p1.Y-500
                                <<" "<<p1.X+500<<" "<<p1.Y+500<<")}\n"; 
                    }
                    else {
                        edge_i-=stage3;
                        y = edge_i/(router->coarse_x_size-1);
                        x = edge_i%(router->coarse_x_size-1);
                        p1 = {router->total_boundary.left+(double)(x+1)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+1)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt1 w400.00 xy("<<p1.X-500<<" "<<p1.Y+500
                                <<" "<<p1.X+500<<" "<<p1.Y-500<<")}\n"; 
                    }
                    // output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt1 w800.00 xy("<<p1.X<<" "<<p1.Y
                    //         <<" "<<p1.X<<" "<<p1.Y<<")}\n"; 
                    // cout<<"1.cluster:"<<c<<" xy("<<x<<" "<<y<<")\n";  
                }
                for (auto edge_idx=cluster->CCW_fbd_edge.begin(); edge_idx!=cluster->CCW_fbd_edge.end(); edge_idx++) {
                    pair<int,int> p1;
                    int edge_i = *edge_idx;
                    int y,x;
                    if (edge_i<stage1) {//hor
                        y = edge_i/(router->coarse_x_size-1);
                        x = edge_i%(router->coarse_x_size-1);
                        p1 = {router->total_boundary.left+(double)(x+1)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+0.5)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt3 w400.00 xy("<<p1.X-1000<<" "<<p1.Y
                                <<" "<<p1.X+1000<<" "<<p1.Y<<")}\n"; 
                    }
                    else if (edge_i<stage2) {
                        edge_i-=stage1;
                        y = edge_i/(router->coarse_x_size);
                        x = edge_i%(router->coarse_x_size);
                        p1 = {router->total_boundary.left+(double)(x+0.5)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+1)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt3 w400.00 xy("<<p1.X<<" "<<p1.Y-1000
                                <<" "<<p1.X<<" "<<p1.Y+1000<<")}\n"; 
                    }
                    else if (edge_i<stage3) {
                        edge_i-=stage2;
                        y = edge_i/(router->coarse_x_size-1);
                        x = edge_i%(router->coarse_x_size-1);
                        p1 = {router->total_boundary.left+(double)(x+1)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+1)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt3 w400.00 xy("<<p1.X-500<<" "<<p1.Y-500
                                <<" "<<p1.X+500<<" "<<p1.Y+500<<")}\n"; 
                    }
                    else {
                        edge_i-=stage3;
                        y = edge_i/(router->coarse_x_size-1);
                        x = edge_i%(router->coarse_x_size-1);
                        p1 = {router->total_boundary.left+(double)(x+1)*coarse_x_length , 
                                            router->total_boundary.bot+(double)(y+1)*coarse_y_length};
                        output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt3 w400.00 xy("<<p1.X-500<<" "<<p1.Y+500
                                <<" "<<p1.X+500<<" "<<p1.Y-500<<")}\n"; 
                    }
                    // output<<"p{"<<idx+layer_num<<" dt"<<c<<" pt2 w800.00 xy("<<p1.X<<" "<<p1.Y
                    //         <<" "<<p1.X<<" "<<p1.Y<<")}\n"; 
                    // cout<<"2.cluster:"<<c<<" xy("<<x<<" "<<y<<")\n"; 
                }
                c++;
            }
            layer_num++;
        }/*
        layer_num=0;
        for (auto l=router->seg_list.begin(); l!=router->seg_list.end(); l++) {
            for (auto c=l->begin(); c!=l->end(); c++) {
                for (auto s=c->second.begin(); s!=c->second.end(); s++) {
                    int pitch = MIN_SPACING+WIRE_WIDTH; 
                    pair<int,int> p1 = {(double)(s->end1.x+0.5)*coarse_x_length+router->total_boundary.left,(double)(s->end1.y+0.5)*coarse_y_length+router->total_boundary.bot};
                    pair<int,int> p2 = {(double)(s->end2.x+0.5)*coarse_x_length+router->total_boundary.left,(double)(s->end2.y+0.5)*coarse_y_length+router->total_boundary.bot};
                    pair<int,int> p3 = {(p1.X+p2.X)/2,(p1.Y+p2.Y)/2};
                    
                    output<<"p{"<<idx+layer_num<<" dt"<<s->cluster_idx<<" pt1 w800.00 xy("<<p1.X<<" "<<p1.Y
                            <<" "<<p2.X<<" "<<p2.Y<<")}\n";  
                    output<<"t{"<<idx+layer_num<<" tt"<<s->cluster_idx<<" mc m0.025 xy("<<p3.X<<" "<<p3.Y<<") '"<<s->demand/pitch<<"'}\n";
                    //                                                                                              +s->net_num
                    // cout<<"demand val: "<<s->demand<<"\n";
                }
            }
            layer_num++;
        }*/


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
                output<<"b{"<<idx+o->layer<<" dt1 xy("<<o->bd.left<<" "<<o->bd.bot<<" "<<o->bd.left<<" "<<o->bd.top
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
                // int left_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[LEFT] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[LEFT];
                // int right_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[RIGHT] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[RIGHT];
                // int bot_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[BOT] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[BOT];
                // int top_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_cap[TOP] - router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_ini_demand[TOP];
                
                int left_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_r[LEFT];
                int right_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_r[RIGHT];
                int bot_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_r[BOT];
                int top_r = router->coarse_GRcell.at(lcu_idx).at(i).at(j).edge_r[TOP];
                output<<"t{1000 tt1 mc m0.025 xy("<<pos_L.X<<" "<<pos_L.Y<<") '"<<left_r<<"'}\n";
                output<<"t{1000 tt1 mc m0.025 xy("<<pos_R.X<<" "<<pos_R.Y<<") '"<<right_r<<"'}\n";
                output<<"t{1000 tt1 mc m0.025 xy("<<pos_B.X<<" "<<pos_B.Y<<") '"<<bot_r<<"'}\n";
                output<<"t{1000 tt1 mc m0.025 xy("<<pos_T.X<<" "<<pos_T.Y<<") '"<<top_r<<"'}\n";
            }
            // printf("output GDS (0,6,7) edge_r=%f",router->coarse_GRcell.at(0).at(6).at(7).edge_r[LEFT]);
            // printf("\n");
        }
        output<<"}\n}\n";
        output.close();
        lcu_idx++;
    }
    
}

bool Satisfiable(string &sat_out){
    ifstream input;
    input.open(sat_out, ios::in);
    string strLine = "";
    while (!input.eof())
    {
        std::getline(input, strLine);
        string str = strLine;
        if (str.find("OPTIMUM FOUND") != std::string::npos)
        {
            input.close();
            return true;
        }
        if (str.find("UNSATISFIABLE") != std::string::npos)
        {
            input.close();
            return false;
        }
        if (str.find("SATISFIABLE") != std::string::npos)
        {
            input.close();
            return true;
        }


    }
    input.close();
    return false;
}

void output_CNF(vector<string>& s_vec,const vector<map<int,pair<Line,Line>>>& restnet_line ){
    ofstream CNF("CNF.sat");
    int total_layer =  restnet_line.size();
    int total_net(0);
    int cardinal_num(0);
    int hard_c(s_vec.size()), soft_c;
    int total_soft_weight(0);
    int max_dd(0);
    if (total_layer>0)
        total_net = restnet_line.at(0).size();
    for (auto l=restnet_line.begin(); l!=restnet_line.end(); l++) {
        for (auto n_2l=l->begin(); n_2l!=l->end(); n_2l++) {
            total_soft_weight += n_2l->second.first.detour_dist;
            total_soft_weight += n_2l->second.second.detour_dist;
        }
    }
    cardinal_num = total_layer*total_net*2;
    soft_c       = total_layer*total_net*2;
    hard_c += ((2*total_layer)*(2*total_layer-1)/2)*total_net;
    CNF <<  "p wcnf " << cardinal_num << ' ' << hard_c+soft_c << ' ' << total_soft_weight + cardinal_num + 1  << endl;
    int layer(0);
    for (auto l=restnet_line.begin(); l!=restnet_line.end(); l++) {
        for (auto n_2l=l->begin(); n_2l!=l->end(); n_2l++) {
            stringstream SS;
            string s1,s2, S, S1, S2, S3, S4;
            s1 = to_string(-1*n_2l->second.first.temp_nid);
            s2 = to_string(-1*n_2l->second.second.temp_nid);
            S = s1+" "+s2+" 0\n";
            // cout<<S;
            s_vec.push_back(S);
            if (n_2l->second.first.detour_dist > max_dd)
                max_dd = n_2l->second.first.detour_dist;
            if (n_2l->second.second.detour_dist > max_dd)
                max_dd = n_2l->second.second.detour_dist;
            for (int next_l=layer+1; next_l<total_layer; next_l++) {
                string ns1, ns2;
                ns1 = to_string(-1*restnet_line.at(next_l).at(n_2l->first).first.temp_nid);
                ns2 = to_string(-1*restnet_line.at(next_l).at(n_2l->first).second.temp_nid);
                S1 = s1+" "+ns1+" 0\n";
                S2 = s1+" "+ns2+" 0\n";
                S3 = s2+" "+ns1+" 0\n";
                S4 = s2+" "+ns2+" 0\n";
                // cout<<S1<<S2<<S3<<S4;
                s_vec.push_back(S1);
                s_vec.push_back(S2);
                s_vec.push_back(S3);
                s_vec.push_back(S4);
            }
        }
        layer++;
    }
    // cout<<"hard c = "<<s_vec.size()<<" esti hard c = "<<hard_c<<endl;
    for (auto clause=s_vec.begin(); clause!=s_vec.end(); clause++) {
        CNF<<total_soft_weight+ cardinal_num + 1<<" "<<*clause;
    }
    //soft clause
    for (auto l=restnet_line.begin(); l!=restnet_line.end(); l++) {
        for (auto n_2l=l->begin(); n_2l!=l->end(); n_2l++) {
            CNF<<max_dd-n_2l->second.first.detour_dist+1 <<" "<<n_2l->second.first.temp_nid <<" 0\n";
            CNF<<max_dd-n_2l->second.second.detour_dist+1<<" "<<n_2l->second.second.temp_nid<<" 0\n";
            // cout<<max_dd-n_2l->second.first.detour_dist <<" "<<n_2l->second.first.temp_nid <<" 0\n";
            // cout<<max_dd-n_2l->second.second.detour_dist<<" "<<n_2l->second.second.temp_nid<<" 0\n";
        }
    }
    CNF.close();
    string command = "./uwrmaxsat CNF.sat -m > CNF.out";
    cout<<"MAX SAT commend: "<<command<<endl;
    system(command.c_str());
    cout << "uwrmaxsat finish" << endl;
}

map<int,Detour_info> Load_MAXSAT_Output(const map<int,vector<pair<Detour_info,Detour_info>>>& coarse_detour_info, const map<int,int>& ID_restnet){
    //ifstream input("sat_route.out");
    map<int,Detour_info> ans;
    ifstream input("CNF.out");
    int net_num=coarse_detour_info.size();
    int total_layer(0);
    int cardinal_num(net_num*2);
    if (net_num==0) {
        printf("Load_MAXSAT_Output WRONG!!!!!!!\n");
        return ans;
    }
    total_layer = coarse_detour_info.begin()->second.size();
    int SAT_BOOL_VAR_ID = net_num*total_layer*2;
    if (!input.is_open())
    {
        cout << "Unable to open the file" << endl;
        exit(1);
    }

    vector<int> solution_value(SAT_BOOL_VAR_ID+1, -1);
    vector<int> true_val;
    while (!input.eof())
    {
        string strLine;
        std::getline(input, strLine);
        //printf("%s \n", strLine.c_str());
        if (strLine.size() > 0)
        {
            if (strLine.at(0) == 'v')
            {
                // Solution
                vector<string> solution;
                string sep = " ";
                //printf("SplitString\n");
                // cout<<strLine<<endl;
                SplitString(strLine, sep ,solution);
                // cout<<"solution.size="<<solution.size()<<endl;
                //printf("SplitString: %s\n",solution.at(0).c_str());
                for (int i = 1; i < solution.size(); i++)
                {
                    // printf("i=%d\n",i);

                    if (i >= static_cast<int>(solution_value.size())) {
                         break;
                    }
                    //printf("(%d)\n",i);
                    // cout<<solution.at(i)<<endl;
                    if (solution.at(i).size() > 0)
                    {
                        if (solution.at(i).at(0) == '-')
                        {
                            //printf("%s\t-> id(%d) = 0\n",solution.at(i).c_str(),i);
                            solution_value.at(i) = 0;
                        }
                        else
                        {
                            //printf("%s\t-> id(%d) = 1\n", solution.at(i).c_str(), i);
                            solution_value.at(i) = 1;
                            true_val.push_back(i);
                        }
                    }
                }
            }
        }
    }
    input.close();

    for (auto var=true_val.begin(); var!=true_val.end(); var++) {
        int layer = (*var-1)/cardinal_num;
        int nid = *var%cardinal_num;
        bool DDR_detour(false);
        if (nid%2==0)
            DDR_detour = true;
        nid = (nid+1)/2;
        if (nid==0)
            nid = net_num;
        nid = ID_restnet.at(nid);

        if (!DDR_detour)
            ans[nid] = coarse_detour_info.at(nid).at(layer).first;
        else 
            ans[nid] = coarse_detour_info.at(nid).at(layer).second;
        printf("var:%d = layer:%d net:%d (%d)\n",*var, layer, nid, DDR_detour);
    }
    return ans;
}

void output_ideal_lp(int layer, string output_name, map<int,Edge> edge_table, vector<Cluster>& GR_unit, map<int,vector<Segment>>& seg_list, vector<Net>& net_list){
    printf("output_ideal_lp\n");
    int pitch = WIRE_WIDTH + MIN_SPACING;
    double st = MIN_SPACING*tan(22.5*PI / 180.0);

    string base_file_name = output_name + "_" + to_string(layer);
    // output_name = base_file_name + ".lp";
    string comman_file_name = base_file_name + ".cmd";
    //remove .lp file and .sol file
    string rm_command = "rm -rf "+base_file_name+".lp";
    system(rm_command.c_str());
    rm_command = "rm -rf "+base_file_name+".sol";
    system(rm_command.c_str());

    ofstream output(base_file_name+".lp");
    ofstream command(comman_file_name);
    output<<"Minimize\n";
    output<<"obj:\n";
    bool print(false);
    if (print){
        cout<<"Minimize\n";
        cout<<"obj:\n";
    }
    string obj = " ";
    int c_idx(0);
    if (print)
        printf("seg list size:%d\n",seg_list.size());
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        if (print)
            cout<<"c_idx"<<c_idx<<"\n";
        int s_idx(0);
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            if (print)
                cout<<"seg"<<s_idx<<"\n";
            obj += seg->name + " + ";
            if (print)
                cout<< seg->name << " + ";
            s_idx++;
        }
        c_idx++;
    }
    // cout<<obj;
    {
        size_t plus_pos = obj.rfind("+");
        // if (plus_pos == std::string::npos)
        //     return;
        obj.erase(plus_pos,2);
        obj += "\n";
    }

    output<<obj;
    // cout<<obj;
    vector<string> ieq;
    vector<string> bound;
    vector<string> pwl;
    vector<Const> const_table;
    int max_n(0);
    for (auto c=GR_unit.begin(); c!=GR_unit.end(); c++) {
        if (c->net.size() > max_n)
            max_n = c->net.size();
    }
    if (print)
        cout<<"max n = "<<max_n<<endl;
    // printf("const:\nst:%f, MIN_SPACING:%d, WIRE_WIDTH:%d, l_dia:%d, MSL:%d\n", st, MIN_SPACING, WIRE_WIDTH, l_dia, MSL);
    const_table.resize(max_n+1);
    for (int n=1; n<=max_n; n++) {
        const_table.at(n).C0 = 6*(n-1)*st+2*MSL+4*l_dia - 2*(n*(MIN_SPACING+WIRE_WIDTH)+l_dia/sqrt(2)+(n-1)*st/sqrt(2))-((2+2*sqrt(2))*(n-1)*st+2*MSL+2*sqrt(2)*l_dia);
        // printf("6*(n-1)*st=%f + 2*MSL:%d + 4*l_dia:%d + (n*(MIN_SPACING+WIRE_WIDTH):%d + l_dia/sqrt(2):%f + (n-1)*st/sqrt(2):%f\n",
        //         6*(n-1)*st,     2*MSL,     4*l_dia,     n*(MIN_SPACING+WIRE_WIDTH),      l_dia/sqrt(2),     (n-1)*st/sqrt(2));
        const_table.at(n).C1 = 2;
        const_table.at(n).C2 = coarse_x_length/((2+2*sqrt(2))*(n-1)*st+2*MSL+2*sqrt(2)*l_dia);//snack pattern number
        const_table.at(n).bound = n*(MIN_SPACING+WIRE_WIDTH)+l_dia/sqrt(2)+(n-1)*st/sqrt(2);
        printf("#n:%d  C0:%f  C1:%f  bound:%f\n",n, const_table.at(n).C0, const_table.at(n).C1, const_table.at(n).bound);
    }
    // inequation: x1 + x2 <= 3
    output<<"Subject To\n";
    for (auto edge=edge_table.begin(); edge!=edge_table.end(); edge++) {
        output<<"  c"<<edge->first<<": ";
        string temp_s = "";
        for (auto seg=edge->second.segment.begin(); seg!=edge->second.segment.end(); seg++) {
            string temp_s2 =  seg->name + " + ";
            // string temp_s2 =  "C" + to_string(seg->cluster_idx) + "I" + to_string(seg->index) + " + ";
            temp_s += temp_s2;
            // output<<"C"<<to_string(seg->cluster_idx)<<"I"<<to_string(seg->index);
        }
        size_t plus_pos = temp_s.rfind("+");
        temp_s.erase(plus_pos,2);
        temp_s += "<= " + to_string(edge->second.resource*pitch) + "\n";
        if (print)
            cout<<"edge: "<<edge->first<<" "<<temp_s;
        output<<temp_s;
        ieq.push_back(temp_s);
    }

    // wl constraint
    int idx(0);
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        string slack_constraint;
            string s= "  r" + to_string(idx) + ": ";
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            s += "S_" + seg->name + " + ";
        }
        size_t plus_pos = s.rfind("+");
        s.erase(plus_pos,2);
        s += ">= " + to_string(c->second.begin()->slack) + "\n";
        output<<s;
        idx++;
    }
    // Bound
    output<<"Bounds\n";
    // cout<<"Bounds\n";
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            int n=seg->net_num;
            // cout<<"n="<<n<<endl;
            string s = "  " + seg->name + " >= " + to_string(const_table.at(n).bound) + "\n";
            output<<s;
            // cout<<s;
        }        
    }


    // Semi-continuous
    output<<"Semi-continuous\n";
    // cout<<"Semi-continuous\n";
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            output<<seg->name<<" ";
            // cout<<seg->name<<" ";
        }
    }
    output<<"\n";
    // cout<<"\n";
// 

    // piecewise linear function
    output<<"PWL\n";
    // cout<<"PWL\n";
    idx = 0;
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            int n=seg->net_num;
            double ini_val = (const_table.at(n).C0 + const_table.at(n).C1*const_table.at(n).bound)*const_table.at(n).C2;
            string s_iv = to_string(ini_val);
            s_iv.erase(s_iv.find("."), s_iv.size()-s_iv.find(".")+2);
            string slope = to_string(const_table.at(n).C1);
            string p = "  p" + to_string(idx) + ": S_" + seg->name + " = " +seg->name + " 0 (0, 0) (" + to_string(const_table.at(n).bound) + ", " + s_iv + ") "+ slope + "\n";
            pwl.push_back(p);
            output<<p;
            // cout<<p;
            idx++;
        }
    }
    output<<"END\n";
    // cout<<"END\n";
    output.close();
    command << "read " << base_file_name << ".lp\n";
    command << "set timelimit 60\n";
    command << "opt\n";
    command << "write " << base_file_name<<".sol\n";
    command << "y\n";
    command.close();
    time_t start = time(NULL);
    string command_line = "./cplex < " + comman_file_name;
    printf("command: %s\n",command_line.c_str());
    system(command_line.c_str());
    time_t end = time(NULL);
    cout<<"cplex consume : "<< end-start<<"\n";
}

void output_passable_lp(int layer, string output_name, map<int,Edge> edge_table, vector<Cluster>& GR_unit, map<int,vector<Segment>>& seg_list, vector<Net>& net_list){
    printf("output_passable_lp\n");
    int pitch = WIRE_WIDTH + MIN_SPACING;
    double st = MIN_SPACING*tan(22.5*PI / 180.0);

    string base_file_name = output_name + "_" + to_string(layer);
    // output_name = base_file_name + ".lp";
    string comman_file_name = base_file_name + ".cmd";
    //remove .lp file and .sol file
    string rm_command = "rm -rf "+base_file_name+"_fail.lp";
    system(rm_command.c_str());
    rm_command = "rm -rf "+base_file_name+"_fail.sol";
    system(rm_command.c_str());

    ofstream output(base_file_name+"_fail.lp");
    ofstream command(comman_file_name);
    output<<"Minimize\n";
    output<<"obj:\n";
    string obj = " ";
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++)
            obj += seg->name + " + ";
    }
    obj += " [ ";
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        obj += "C" + to_string(c->first) + "_wl ^2 + ";
    }
    {
        size_t plus_pos = obj.rfind("+");
        obj.erase(plus_pos,2);
        obj += " ] /2";
        obj += "\n";
    }

    output<<obj;
    vector<string> ieq;
    vector<string> bound;
    vector<string> pwl;
    vector<Const> const_table;
    int max_n(0);
    for (auto c=GR_unit.begin(); c!=GR_unit.end(); c++) {
        if (c->net.size() > max_n)
            max_n = c->net.size();
    }
    // cout<<"max n = "<<max_n<<endl;
    // printf("const:\nst:%f, MIN_SPACING:%d, WIRE_WIDTH:%d, l_dia:%d, MSL:%d\n", st, MIN_SPACING, WIRE_WIDTH, l_dia, MSL);
    const_table.resize(max_n+1);
    for (int n=1; n<=max_n; n++) {
        const_table.at(n).C0 = 6*(n-1)*st+2*MSL+4*l_dia - 2*(n*(MIN_SPACING+WIRE_WIDTH)+l_dia/sqrt(2)+(n-1)*st/sqrt(2))-((2+2*sqrt(2))*(n-1)*st+2*MSL+2*sqrt(2)*l_dia);
        // printf("6*(n-1)*st=%f + 2*MSL:%d + 4*l_dia:%d + (n*(MIN_SPACING+WIRE_WIDTH):%d + l_dia/sqrt(2):%f + (n-1)*st/sqrt(2):%f\n",
        //         6*(n-1)*st,     2*MSL,     4*l_dia,     n*(MIN_SPACING+WIRE_WIDTH),      l_dia/sqrt(2),     (n-1)*st/sqrt(2));
        const_table.at(n).C1 = 2;
        const_table.at(n).C2 = coarse_x_length/((2+2*sqrt(2))*(n-1)*st+2*MSL+2*sqrt(2)*l_dia);//snack pattern number
        const_table.at(n).bound = n*(MIN_SPACING+WIRE_WIDTH)+l_dia/sqrt(2)+(n-1)*st/sqrt(2);
        printf("#n:%d  C0:%f  C1:%f  bound:%f\n",n, const_table.at(n).C0, const_table.at(n).C1, const_table.at(n).bound);
    }
    // inequation: x1 + x2 <= 3
    output<<"Subject To\n";
    for (auto edge=edge_table.begin(); edge!=edge_table.end(); edge++) {
        output<<"  c"<<edge->first<<": ";
        string temp_s = "";
        for (auto seg=edge->second.segment.begin(); seg!=edge->second.segment.end(); seg++) {
            string temp_s2 =  seg->name + " + ";
            // string temp_s2 =  "C" + to_string(seg->cluster_idx) + "I" + to_string(seg->index) + " + ";
            temp_s += temp_s2;
            // output<<"C"<<to_string(seg->cluster_idx)<<"I"<<to_string(seg->index);
        }
        size_t plus_pos = temp_s.rfind("+");
        temp_s.erase(plus_pos,2);
        temp_s += "<= " + to_string(edge->second.resource*pitch) + "\n";
        // cout<<"edge: "<<edge->first<<" "<<temp_s;
        output<<temp_s;
        ieq.push_back(temp_s);
    }

    // wl constraint
    int idx(0);
    /*for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        string slack_constraint;
            string s= "  r" + to_string(idx) + ": ";
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            s += "S_" + seg->name + " + ";
        }
        size_t plus_pos = s.rfind("+");
        s.erase(plus_pos,2);
        s += ">= " + to_string(c->second.begin()->slack) + "\n";
        output<<s;
        idx++;
    }*/
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        string slack_constraint;
            string s= "  R" + to_string(idx) + ": ";
        s += "C" + to_string(c->second.begin()->cluster_idx) + "_wl + ";
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            s += "S_" + seg->name + " + ";
        }
        size_t plus_pos = s.rfind("+");
        s.erase(plus_pos,2);
        s += "= " + to_string(c->second.begin()->slack) + "\n";
        // s += ">= " + to_string(c->second.begin()->slack) + "\n";
        output<<s;
        idx++;
    }
    // Bound
    output<<"Bounds\n";
    // cout<<"Bounds\n";
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            int n=seg->net_num;
            // cout<<"n="<<n<<endl;
            string s = "  " + seg->name + " >= " + to_string(const_table.at(n).bound) + "\n";
            output<<s;
            // cout<<s;
        }        
    }


    // Semi-continuous
    output<<"Semi-continuous\n";
    // cout<<"Semi-continuous\n";
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            output<<seg->name<<" ";
            // cout<<seg->name<<" ";
        }
    }
    output<<"\n";
    // cout<<"\n";


    // piecewise linear function
    output<<"PWL\n";
    // cout<<"PWL\n";
    idx = 0;
    for (auto c=seg_list.begin(); c!=seg_list.end(); c++) {
        for (auto seg=c->second.begin(); seg!=c->second.end(); seg++) {
            int n=seg->net_num;
            double ini_val = (const_table.at(n).C0 + const_table.at(n).C1*const_table.at(n).bound)*const_table.at(n).C2;
            string s_iv = to_string(ini_val);
            s_iv.erase(s_iv.find("."), s_iv.size()-s_iv.find(".")+2);
            string slope = to_string(const_table.at(n).C1);
            string p = "  p" + to_string(idx) + ": S_" + seg->name + " = " +seg->name + " 0 (0, 0) (" + to_string(const_table.at(n).bound) + ", " + s_iv + ") "+ slope + "\n";
            pwl.push_back(p);
            output<<p;
            // cout<<p;
            idx++;
        }
    }
    output<<"END\n";
    // cout<<"END\n";
    output.close();
    command << "read " << base_file_name << "_fail.lp\n";
    command << "set timelimit 60\n";
    command << "opt\n";
    command << "write " << base_file_name<<"_fail.sol\n";
    command << "y\n";
    command.close();
    time_t start = time(NULL);
    string command_line = "./cplex < " + comman_file_name;
    printf("command: %s\n",command_line.c_str());
    system(command_line.c_str());
    time_t end = time(NULL);
    cout<<"cplex consume : "<< end-start<<"\n";

}

bool Load_ideal_LP_Output(std::string sol_file_name, map<int,vector<Segment>>& seg_list, vector<Cluster>& _GR_unit) {
    ifstream input(sol_file_name);
    printf("Load %s file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", sol_file_name.c_str());
    if (!input) {
        printf("%s not found, Infeasible!!!\n",sol_file_name.c_str());
        // exit(1);
        return false;
    }
    string S;
    while (!input.eof()){
        getline(input, S);
        if (S.find("<variables>")!=std::string::npos) {
            stringstream SS;
            string sc, si;
            // cout<<"string: "<<S<<endl;
                getline(input, S);
            while (S.find("</variables>")==std::string::npos) {
                // cout<<"find variable: "<<S<<endl;
                string sep = "\"";
                vector<string> s_vec;
                SplitString(S,sep,s_vec);
                // cout<<"s_vec.size="<<s_vec.size()<<endl;
                string name = s_vec.at(1);
                string s_val = s_vec.at(5);
                if (name.at(0)=='S') {
                    getline(input, S);
                    continue;
                }
                    
                size_t c_pos = name.find("C");
                size_t i_pos = name.find("I");
                int c,i;
                double val;
                sc.assign(name,c_pos+1,i_pos-c_pos-1);
                si.assign(name,i_pos+1,name.size()-i_pos-1);
                // cout<<"C:"<<sc<<" I:"<<si<<endl;
                SS<<sc; SS>>c;  SS.clear();
                SS<<si; SS>>i;  SS.clear();
                SS<<s_val; SS>>val;  SS.clear();
                seg_list.at(c).at(i).demand = val;
                _GR_unit.at(c).remain_slack = 0;
                // printf("c:%d remain slack:%d\n",c, _GR_unit.at(c).remain_slack);
                getline(input, S);
            }
            // cout<<S<<endl;
        }
    }
    return true;
}

vector<int> Load_passable_LP_Output(int layer, std::string sol_file_name, map<int,vector<Segment>>& seg_list, vector<Cluster>& _GR_unit) {
    string base_file_name = sol_file_name + "_" + to_string(layer);;
    string fail_sol_file = base_file_name + "_fail.sol";
    ifstream input(fail_sol_file);
    vector<int> ripuped_cluster;
    printf("Load %s file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", fail_sol_file.c_str());
    if (!input) {
        printf("%s not found, Infeasible!!!\n",fail_sol_file.c_str());
        // exit(1);
        return ripuped_cluster;
    }
    string S;
    while (!input.eof()){
        getline(input, S);
        if (S.find("<variables>")!=std::string::npos) {
            stringstream SS;
            string sc, si;
            // cout<<"string: "<<S<<endl;
                getline(input, S);
            while (S.find("</variables>")==std::string::npos) {
                // cout<<"find variable: "<<S<<endl;
                string sep = "\"";
                vector<string> s_vec;
                SplitString(S,sep,s_vec);
                // cout<<"s_vec.size="<<s_vec.size()<<endl;
                string name = s_vec.at(1);
                string s_val = s_vec.at(5);
                if (name.at(0)=='S') {
                    getline(input, S);
                    continue;
                }
                if (name.find("_wl")!=std::string::npos) {
                    int c;
                    float remain_slack;
                    size_t c_pos = name.find("C");
                    size_t c_pos_end = name.find("_wl");
                    SS<<s_val;   SS>>remain_slack;   SS.clear();
                    sc.assign(name,c_pos+1,c_pos_end-c_pos-1);
                    SS<<sc; SS>>c;  SS.clear(); sc.clear();
                    printf("cluster:%d remain slack val:%d->%f\n",c,_GR_unit.at(c).max_slack,remain_slack);
                    _GR_unit.at(c).remain_slack = remain_slack;
                    if (remain_slack > slack_tol) {
                        ripuped_cluster.push_back(c);
                    }

                    getline(input, S);
                    continue;
                }
                size_t c_pos = name.find("C");
                size_t i_pos = name.find("I");
                int c,i;
                double val;
                sc.assign(name,c_pos+1,i_pos-c_pos-1);
                si.assign(name,i_pos+1,name.size()-i_pos-1);
                // cout<<"C:"<<sc<<" I:"<<si<<endl;
                SS<<sc; SS>>c;  SS.clear();
                SS<<si; SS>>i;  SS.clear();
                SS<<s_val; SS>>val;  SS.clear();
                if (layer==1 && seg_list.find(c)==seg_list.end()) {
                    printf("seg_list no cluster:%d\n",c);

                }
                seg_list.at(c).at(i).demand = val;
                getline(input, S);           
                    

            }
            // cout<<S<<endl;
        }
    }
    return ripuped_cluster;
}

void output_SER(GR* router, std::string file_name) {
    string output_FN = file_name+".ser";
    ofstream output(output_FN);
    output<<"Cell_width: "<<coarse_x_length<<" Cell_height: "<<coarse_y_length<<"\n";
    output<<"LEFT BOT: ( " <<router->total_boundary.left<<" "<<router->total_boundary.bot<<" )\n";
    for (auto n=router->net_list.begin(); n!=router->net_list.end(); n++) {
        if (n->ignore || !n->is2pin_net)
            continue;
        output<<"Net_name: "<<n->net_name<<" ID: "<<n->net_ID<<" WCS: "<<n->WCS<<"\n";
        for (auto pid=n->net_pinID.begin(); pid!=n->net_pinID.end(); pid++) {
            auto p=&router->pin_list.at(*pid);
            output<<"Pin_name: "<<p->pin_name<<" ID: "<<p->pin_ID<<" coor: ";
            for (auto coor=p->ER_coarse_cell.begin(); coor!=p->ER_coarse_cell.end(); coor++) {
                output<<"( "<<coor->x<<" "<<coor->y<<" "<<coor->z<<" ) ";
            }
            output<<"\n";
        }
        output<<"Net_END\n";
    }
    output.close();
}