#include "Func.h"
using namespace std;
#define L first
#define R second
#define X first
#define Y second


int find_root(vector<int>& root_table, int idx) {
    vector<int> record;
    while(root_table.at(idx)!=idx) {
        record.push_back(idx);
        idx = root_table.at(idx);
    }
    for (auto i=record.begin(); i!=record.end(); i++)
        root_table.at(*i) = idx;
    return idx;
}

bool occupy (Boundary b1, Boundary b2) {
    if (b1.bot >= b2.top)
        return false;
    if (b1.top  <= b2.bot)
        return false;
    if (b1.left >= b2.right)
        return false;
    if (b1.right <= b2.left)
        return false;
    return true;
}

int overlap_area(Boundary b1, Boundary b2) {
    int width(0), height(0);
    if (b1.right>=b2.right) {
        if (b1.left <=b2.left) 
            width = b2.right-b2.left;
        else 
            width = b2.right-b1.left;
    }
    else {
        if (b1.left <=b2.left) 
            width = b1.right-b2.left;
        else 
            width = b1.right-b1.left;
    }
    if (b1.top>=b2.top) {
        if (b1.bot<=b2.bot)
            height = b2.top-b2.bot;
        else
            height = b2.top-b1.bot;
    }
    else {
        if (b1.bot<=b2.bot)
            height = b1.top-b2.bot;
        else
            height = b1.top-b1.bot;
    }
    
    // printf("b1 h(%d,%d)  b2 h(%d,%d) overlap h:%d\n",b1.bot, b1.top, b2.bot, b2.top,height);
    // printf("b1 w(%d,%d)  b2 w(%d,%d) overlap w:%d\n",b1.left, b1.right, b2.left, b2.right,width);
    return width*height;
}

int overlap_h(Boundary b1, Boundary b2) {
    int width(0), height(0);
    if (b1.top>=b2.top) {
        if (b1.bot<=b2.bot)
            height = b2.top-b2.bot;
        else
            height = b2.top-b1.bot;
    }
    else {
        if (b1.bot<=b2.bot)
            height = b1.top-b2.bot;
        else
            height = b1.top-b1.bot;
    }
    // printf("b1 h(%d,%d)  b2 h(%d,%d) overlap h:%d\n",b1.bot, b1.top, b2.bot, b2.top, height);
    return height;
}
int overlap_w(Boundary b1, Boundary b2) {
    int width(0), height(0);
    if (b1.right>=b2.right) {
        if (b1.left <=b2.left) 
            width = b2.right-b2.left;
        else 
            width = b2.right-b1.left;
    }
    else {
        if (b1.left <=b2.left) 
            width = b1.right-b2.left;
        else 
            width = b1.right-b1.left;
    }
    // printf("b1 w(%d,%d)  b2 w(%d,%d) overlap w:%d\n",b1.left, b1.right, b2.left, b2.right, width);
    return width;
}

bool pin_less_x (const Pin* _p1, const Pin* _p2) {
    return _p1->real_pos.first > _p2->real_pos.first;
}
bool pin_greater_x (const Pin* _p1, const Pin* _p2) {
    return _p1->real_pos.first < _p2->real_pos.first;
}
bool pin_less_y (const Pin* _p1, const Pin* _p2) {
    return _p1->real_pos.second > _p2->real_pos.second;
}
bool pin_greater_y (const Pin* _p1, const Pin* _p2) {
    return _p1->real_pos.second < _p2->real_pos.second;
}

bool Bdy_less_x (const Boundary _b1, const Boundary _b2) {
    return _b1.left > _b2.left;
}
bool Bdy_greater_x (const Boundary _b1, const Boundary _b2) {
    return _b1.left < _b2.left;
}
bool Bdy_less_y (const Boundary _b1, const Boundary _b2) {
    return _b1.bot > _b2.bot;
}
bool Bdy_greater_y (const Boundary _b1, const Boundary _b2) {
    return _b1.bot < _b2.bot;
}

std::vector<int> LCS(const vector<vector<int>>& v1, const vector<vector<int>>& v2) {
    vector<int> list1, list2;
    map<int,double> weight;
    printf("v1: \n");
    for (auto it1=v1.begin(); it1!=v1.end(); it1++) {
        for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
            printf("%d ",*it2);
        }
        printf("\n");
    }
    for (int i=0; i<v1.size(); i++) {
        int size = v1.at(i).size();
        int end = (1-v1.at(i).size());
        for (int j=v1.at(i).size()-1; j>=end; j--) {
            list1.push_back(v1.at(i).at(abs(j)));
            if (j>=0) {
                weight[v1.at(i).at(j)] = 1+(double)(10-j)/10.0;
            }
        }
    }
    for (auto it=list1.begin(); it!=list1.end(); it++) {
            printf("%d ",*it);
    }
    printf("\n");
    printf("v2: \n");
    for (auto it1=v2.begin(); it1!=v2.end(); it1++) {
        for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
            printf("%d ",*it2);
        }
        printf("\n");
    }
    for (int i=0; i<v2.size(); i++) {
        int end = (1-v2.at(i).size());
        for (int j=v2.at(i).size()-1; j>=end; j--) {
            list2.push_back(v2.at(i).at(abs(j)));
        }
    }
    for (auto it=list2.begin(); it!=list2.end(); it++) {
            printf("%d ",*it);
    }
    printf("\n");
    if (list1.size()==0 || list2.size()==0)
        return vector<int>();
    return LCS(list1,list2);
}

vector<int> LCS(const vector<int>& v1, const vector<int>& v2) {
    vector<vector<DPnode>> DP(v1.size(),vector<DPnode>(v2.size()));
    map<int,int> check_list;
    if (v1.at(0)==v2.at(0)) {
        DP.at(0).at(0).length = 1;
        DP.at(0).at(0).num = 1;
        DP.at(0).at(0).backward = true;
        DP.at(0).at(0).upward = true;
    }
    else {
        DP.at(0).at(0).length = 0;
        DP.at(0).at(0).num = 0;
        DP.at(0).at(0).upward = true;
    }
    check_list[v1.at(0)]=0;
    check_list[v2.at(0)]=0;
    for (int i=1; i<v1.size(); i++) {
        if (v2.at(0) == v1.at(i)) {
            DP.at(i).at(0).num = 1;
            DP.at(i).at(0).length = 1;
            DP.at(i).at(0).backward = true;
            DP.at(i).at(0).upward = true;
        }
        else {
            DP.at(i).at(0).num = DP.at(i-1).at(0).num;
            DP.at(i).at(0).length = DP.at(i-1).at(0).length;
            DP.at(i).at(0).backward = true;
        }
        check_list[v1.at(i)]=0;
    }
    for (int i=1; i<v2.size(); i++) {
        if (v1.at(0) == v2.at(i)) {
            DP.at(0).at(i).num = 1;
            DP.at(0).at(i).length = 1;
            DP.at(0).at(i).backward = true;
            DP.at(0).at(i).upward = true;
        }
        else {
            DP.at(0).at(i).num = DP.at(0).at(i-1).num;
            DP.at(0).at(i).length = DP.at(0).at(i-1).length;
            DP.at(0).at(i).upward = true;
        }
        check_list[v2.at(i)]=0;
    }
    for (int i=1; i<v1.size(); i++) {
        for (int j=1; j<v2.size(); j++) {
            if (v1.at(i)==v2.at(j)) {
                check_list.at(v1.at(i))++;
                DP.at(i).at(j).length = DP.at(i-1).at(j-1).length+1;
                // if (check_list.at(v1.at(i))<=1)
                //     DP.at(i).at(j).num = DP.at(i-1).at(j-1).num+1;
                // else 
                //     DP.at(i).at(j).num = DP.at(i-1).at(j-1).num;
                DP.at(i).at(j).backward = true;
                DP.at(i).at(j).upward = true;

            }
            else {
                if (DP.at(i-1).at(j).length > DP.at(i).at(j-1).length) {
                    DP.at(i).at(j).length = DP.at(i-1).at(j).length;
                    // DP.at(i).at(j).num = DP.at(i-1).at(j).num;
                    DP.at(i).at(j).backward = true;
                }
                else {
                    DP.at(i).at(j).length = DP.at(i).at(j-1).length;
                    // DP.at(i).at(j).num = DP.at(i).at(j-1).num;
                    DP.at(i).at(j).upward = true;
                }
            }
        }
    }
    // printf("   ");
    // for (int i=0; i<v1.size(); i++) {
    //     printf("%8d",v1.at(i));
    // }
    // printf("\n");
    // for (int j=0; j<v2.size(); j++) {
    //     printf("%d  ",v2.at(j));
    //     for (int i=0; i<v1.size(); i++) {
    //         printf("%3d/%d/%d ",DP.at(i).at(j).length, DP.at(i).at(j).backward,DP.at(i).at(j).upward);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    int i(v1.size()-1),j(v2.size()-1);
    vector<int> ans;
    while (i>=0 && j>=0) {
        if (DP.at(i).at(j).backward && DP.at(i).at(j).upward) {
            ans.push_back(v1.at(i));
            i--;
            j--;
        }
        else if (DP.at(i).at(j).upward)
            j--;
        else if (DP.at(i).at(j).backward)
            i--;
        else
            cout<<"DP wrong\n";
    }
    //     printf ("LCS ans\n");
    // for (auto it=ans.rbegin(); it!=ans.rend(); it++) {
    //     printf ("%d ", *it);
    // }
    //     printf ("\n");
    return ans;
}

//deal with pin close to cpi boundary
//The closer the pin is to the CPU boundary, the higher the weight



pair<vector<pair<int,int>>,vector<pair<int,int>>> LCS(const vector<vector<int>>& v1, const vector<vector<int>>& v2, 
                     const vector<pair<int,int>>& dp_list, vector<int>& ED_table1, vector<int>& ED_table2, 
                     vector<int>& location1, vector<int>& location2) {
    vector<decode_node> list1, list2;
    map<int,double> weight;
    printf("v1: \n");
    for (auto it1=v1.begin(); it1!=v1.end(); it1++) {
        for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
            printf("%d ",*it2);
        }
        printf("\n");
    }
    for (int i=0; i<v1.size(); i++) {
        int size = v1.at(i).size();
        int end = (1-v1.at(i).size());
        for (int j=v1.at(i).size()-1; j>=end; j--) {
            decode_node node;
            if (j>=0) {
                weight[v1.at(i).at(j)] = 1+(double)(10-j)/10.0;
            }
            int e_d = ED_table1.at(i);
            if (j>0){//last
                node.setup(v1.at(i).at(abs(j)),e_d,-1,{i,abs(j)});
            }
            else if (j<0)
                node.setup(v1.at(i).at(abs(j)),e_d,1,{i,abs(j)});
            else
                node.setup(v1.at(i).at(abs(j)),e_d,0,{i,abs(j)});

            list1.push_back(node);
        }
    }
    for (auto it=list1.begin(); it!=list1.end(); it++) {
            printf("%d ",it->ID);
    }
    printf("\n");
    printf("v2: \n");
    for (auto it1=v2.begin(); it1!=v2.end(); it1++) {
        for (auto it2=it1->begin(); it2!=it1->end(); it2++) {
            printf("%d ",*it2);
        }
        printf("\n");
    }
    for (int i=0; i<v2.size(); i++) {
        int end = (1-v2.at(i).size());
        for (int j=v2.at(i).size()-1; j>=end; j--) {
            decode_node node;
            int e_d = ED_table2.at(i);
            
            // if (j>=0) {
            //     weight[v2.at(i).at(j)] = 1+(double)(10-j)/10.0;
            // }
            if (j>0){//last
                node.setup(v2.at(i).at(abs(j)),e_d,-1,{i,abs(j)});
            }
            else if (j<0)
                node.setup(v2.at(i).at(abs(j)),e_d,1,{i,abs(j)});
            else
                node.setup(v2.at(i).at(abs(j)),e_d,0,{i,abs(j)});

            list2.push_back(node);
        }
    }
    for (auto it=list2.begin(); it!=list2.end(); it++) {
            printf("%d ",it->ID);
    }
    printf("\n");
    if (list1.size()==0 || list2.size()==0)
        return pair<vector<pair<int,int>>,vector<pair<int,int>>>();
    return LCS(list1,list2,weight,dp_list,location1,location2); 
}


pair<vector<pair<int,int>>,vector<pair<int,int>>> LCS(const vector<decode_node>& v1, const vector<decode_node>& v2, 
                     map<int,double>& weight, const vector<pair<int,int>>& diff_pair, 
                     vector<int>& location1, vector<int>& location2) {
    pair<vector<pair<int,int>>,vector<pair<int,int>>> best_ans;
    bool all_dp_INorOUT(false);
    int round(0);
    while(!all_dp_INorOUT) {
        map<int,int> net_appearnum;
        vector<int> ans_pos_in_v1,ans_pos_in_v2;
        vector<pair<int,int>> ans1,ans2;
        map<int,vector<int>> net_in_ans1_pos, net_in_ans2_pos;
        vector<vector<DPnode>> DP(v1.size(),vector<DPnode>(v2.size()));
        map<int,int> check_list;
        //initial setting
        {
            if (v1.at(0).ID==v2.at(0).ID) {
                DP.at(0).at(0).length = 1;
                DP.at(0).at(0).num = weight.at(v1.at(0).ID);
                DP.at(0).at(0).backward = true;
                DP.at(0).at(0).upward = true;
            }
            else {
                DP.at(0).at(0).length = 0;
                DP.at(0).at(0).num = 0;
                DP.at(0).at(0).upward = true;
            }
                check_list[v1.at(0).ID]=0;
                check_list[v2.at(0).ID]=0;
            for (int i=1; i<v1.size(); i++) {
                if (v2.at(0).ID == v1.at(i).ID && DP.at(i-1).at(0).num <=weight.at(v2.at(0).ID)) {
                    DP.at(i).at(0).num = weight.at(v2.at(0).ID);
                    DP.at(i).at(0).length = 1;
                    DP.at(i).at(0).backward = true;
                    DP.at(i).at(0).upward = true;
                }
                else {
                    DP.at(i).at(0).num = DP.at(i-1).at(0).num;
                    DP.at(i).at(0).length = DP.at(i-1).at(0).length;
                    DP.at(i).at(0).backward = true;
                }
                check_list[v1.at(i).ID]=0;
            }
            for (int i=1; i<v2.size(); i++) {
                if (v1.at(0).ID == v2.at(i).ID && weight.at(v1.at(0).ID) >= DP.at(0).at(i-1).num) {
                    DP.at(0).at(i).num = weight.at(v1.at(0).ID);
                    DP.at(0).at(i).length = 1;
                    DP.at(0).at(i).backward = true;
                    DP.at(0).at(i).upward = true;
                }
                else {
                    DP.at(0).at(i).num = DP.at(0).at(i-1).num;
                    DP.at(0).at(i).length = DP.at(0).at(i-1).length;
                    DP.at(0).at(i).upward = true;
                }
                check_list[v2.at(i).ID]=0;
            }
        }
        //run DP 
        for (int i=1; i<v1.size(); i++) {
            for (int j=1; j<v2.size(); j++) {
                if (v1.at(i).ID==v2.at(j).ID) {
                    check_list.at(v1.at(i).ID)++;
                    DP.at(i).at(j).num = DP.at(i-1).at(j-1).num + weight[v1.at(i).ID];
                    if (DP.at(i-1).at(j).num > DP.at(i).at(j).num) {
                        DP.at(i).at(j).num = DP.at(i-1).at(j).num;
                        DP.at(i).at(j).length = DP.at(i-1).at(j).length;
                        DP.at(i).at(j).backward = true;
                        continue;
                    }
                    else if (DP.at(i).at(j-1).num > DP.at(i).at(j).num) {
                        DP.at(i).at(j).num = DP.at(i).at(j-1).num;
                        DP.at(i).at(j).length = DP.at(i).at(j-1).length;
                        DP.at(i).at(j).upward = true;
                        continue;
                    }
                    DP.at(i).at(j).length = DP.at(i-1).at(j-1).length+1;
                    DP.at(i).at(j).backward = true;
                    DP.at(i).at(j).upward = true;

                }
                else {
                    if (DP.at(i-1).at(j).num > DP.at(i).at(j-1).num) {
                        DP.at(i).at(j).length = DP.at(i-1).at(j).length;
                        DP.at(i).at(j).num = DP.at(i-1).at(j).num;
                        DP.at(i).at(j).backward = true;
                    }
                    else {
                        DP.at(i).at(j).length = DP.at(i).at(j-1).length;
                        DP.at(i).at(j).num = DP.at(i).at(j-1).num;
                        DP.at(i).at(j).upward = true;
                    }
                }
            }
        }
        /*printf("   ");
        for (int i=0; i<v1.size(); i++) {
            printf("%9d",v1.at(i).ID);
        }
        printf("\n");
        for (int j=0; j<v2.size(); j++) {
            printf("%d  ",v2.at(j).ID);
            for (int i=0; i<v1.size(); i++) {
                printf("%5.1f/%d/%d",DP.at(i).at(j).num, DP.at(i).at(j).backward,DP.at(i).at(j).upward);
            }
            printf("\n");
        }
        printf("\n");*/
        map<int,pair<vector<decode_node>,vector<decode_node>>> record_L_R1,record_L_R2;
        map<int,vector<decode_node>> record_M1,record_M2;
        vector<int> temp_ans;
        int i(v1.size()-1),j(v2.size()-1);
        while (i>=0 && j>=0) {
            if (DP.at(i).at(j).backward && DP.at(i).at(j).upward) {
                // printf("last_or_next %d  net %d\n",v1.at(i).last_or_next, v1.at(i).ID);
                if (v1.at(i).last_or_next==-1)
                   record_L_R1[v1.at(i).list_coor.first].L.push_back(v1.at(i));
                else if (v1.at(i).last_or_next==1)
                   record_L_R1[v1.at(i).list_coor.first].R.insert(record_L_R1[v1.at(i).list_coor.first].R.begin(),v1.at(i));
                else 
                   record_M1[v1.at(i).list_coor.first].push_back(v1.at(i));
                if (v2.at(j).last_or_next==-1)
                   record_L_R2[v2.at(j).list_coor.first].L.push_back(v2.at(j));
                else if (v2.at(j).last_or_next==1)
                   record_L_R2[v2.at(j).list_coor.first].R.insert(record_L_R2[v2.at(j).list_coor.first].R.begin(),v2.at(j));
                else 
                   record_M2[v2.at(j).list_coor.first].push_back(v2.at(j));
                temp_ans.push_back(v1.at(i).ID);
                net_appearnum[v1.at(i).ID]++;
                i--;
                j--;
            }
            else if (DP.at(i).at(j).upward)
                j--;
            else if (DP.at(i).at(j).backward)
                i--;
            else
                cout<<"DP wrong\n";
        }
        printf("temp ans(no remove anyting)\n");
        for (auto it=temp_ans.rbegin(); it!=temp_ans.rend(); it++) {
            printf("%d ",*it);
        }printf("\n");
        printf("record_L_R1 ans(no remove anyting)\n");
        for (auto it1=record_L_R1.begin(); it1!=record_L_R1.end(); it1++) {
            printf("%d: ",it1->first);
            for (auto it2=it1->second.first.rbegin(); it2!=it1->second.first.rend(); it2++) {
                printf("%d ",it2->ID);
            }
            printf("\\");
            for (auto it2=it1->second.second.begin(); it2!=it1->second.second.end(); it2++) {
                printf("%d ",it2->ID);
            }
            printf("\n");
        }printf("\n");

        for (auto it=record_L_R1.begin(); it!=record_L_R1.end(); it++) {
            auto next_it=it;    next_it++;
            if (next_it==record_L_R1.end())
                continue;
            int pitch=MIN_SPACING+WIRE_WIDTH;   pitch*=1.6;
            int it_f=it->first;
            int nextit_f=next_it->first;
            int l=location1.at(it->first), r=location1.at(next_it->first);
            int cap = abs((location1.at(it->first)-location1.at(next_it->first))/pitch);
            if (record_M1[it->first].empty()||record_M1[next_it->first].size()==0)
                cap++;
            printf("%d > cap(%d)\n",it->second.R.size()+next_it->second.L.size(),cap);
            int rr=it->second.R.size(), ll=next_it->second.L.size();
            while (it->second.R.size()+next_it->second.L.size() > cap) {
                bool del(false);
                for (auto erase_it=it->second.R.begin(); erase_it!=it->second.R.end(); erase_it++) {
                    if (net_appearnum[erase_it->ID] > 1) {
                        printf("del repeat %d\n",erase_it->ID);
                        net_appearnum[erase_it->ID]--;
                        it->second.R.erase(erase_it);
                        del = true;
                        break;
                    }
                }
                if (del)
                    continue;
                for (auto erase_it=next_it->second.L.begin(); erase_it!=next_it->second.L.end(); erase_it++) {
                    if (net_appearnum[erase_it->ID] > 1) {
                        printf("del repeat %d\n",erase_it->ID);
                        net_appearnum[erase_it->ID]--;
                        next_it->second.L.erase(erase_it);
                        del = true;
                        break;
                    }
                }
                if (del)
                    continue;
                if (it->second.R.size() > next_it->second.L.size()) {
                    printf("pop %d\n",it->second.R.back().ID);
                    it->second.R.pop_back();
                }
                else {
                    printf("pop %d\n",next_it->second.L.back().ID);
                    next_it->second.L.pop_back();
                }
            }
        }printf("\n");
        // printf("record_L_R2 ans(no remove anyting)\n");
        // for (auto it1=record_L_R2.begin(); it1!=record_L_R2.end(); it1++) {
        //     printf("%d: ",it1->first);
        //     for (auto it2=it1->second.first.rbegin(); it2!=it1->second.first.rend(); it2++) {
        //         printf("%d ",it2->ID);
        //     }
        //     printf("\\");
        //     for (auto it2=it1->second.second.begin(); it2!=it1->second.second.end(); it2++) {
        //         printf("%d ",it2->ID);
        //     }
        //     printf("\n");
        // }printf("\n");
        for (auto it=record_L_R2.begin(); it!=record_L_R2.end(); it++) {
            auto next_it=it;    next_it++;
            if (next_it==record_L_R2.end())
                continue;
            int pitch=MIN_SPACING+WIRE_WIDTH;   pitch*=1.4;
            int cap = abs((location2.at(it->first)-location2.at(next_it->first))/pitch);
            if (record_M2[it->first].size()==0||record_M2[next_it->first].size()==0)
                cap++;
            printf("%d > cap(%d)\n",it->second.R.size()+next_it->second.L.size(),cap);
            while (it->second.R.size()+next_it->second.L.size() > cap) {
                if (it->second.R.size() > next_it->second.L.size()) {
                    printf("pop %d\n",it->second.R.back().ID);
                    it->second.R.pop_back();
                }
                else {
                    printf("pop %d\n",next_it->second.L.back().ID);
                    next_it->second.L.pop_back();
                }
            }
        }

        for (int line=0; line<location1.size(); line++){
            auto it1=&record_L_R1[line];
            int idx1(it1->L.size()),idx2(1);
            for (auto it2=it1->L.rbegin(); it2!=it1->L.rend(); it2++) {
                net_in_ans1_pos[it2->ID].push_back(ans1.size());
                ans1.push_back({it2->ID,idx1});
                // net_appearnum[it2->ID]++;
                idx1--;
            }
            for (auto it2=record_M1[line].begin(); it2!=record_M1[line].end(); it2++) {
                net_in_ans1_pos[it2->ID].push_back(ans1.size());
                ans1.push_back({it2->ID,0});
                // net_appearnum[it2->ID]++;
            }
            for (auto it2=it1->R.begin(); it2!=it1->R.end(); it2++) {
                net_in_ans1_pos[it2->ID].push_back(ans1.size());
                ans1.push_back({it2->ID,idx2});
                // net_appearnum[it2->ID]++;
                idx2++;
            }
        }           
        for (int line=0; line<location2.size(); line++){
            auto it1=&record_L_R2[line];
            int idx1(it1->L.size()),idx2(1);
            for (auto it2=it1->L.rbegin(); it2!=it1->L.rend(); it2++) {
                net_in_ans1_pos[it2->ID].push_back(ans2.size());
                ans2.push_back({it2->ID,idx1});
                // net_appearnum[it2->ID]++;
                idx1--;
            }
            for (auto it2=record_M2[line].begin(); it2!=record_M2[line].end(); it2++) {
                net_in_ans1_pos[it2->ID].push_back(ans2.size());
                ans2.push_back({it2->ID,0});
                // net_appearnum[it2->ID]++;
            }
            for (auto it2=it1->R.begin(); it2!=it1->R.end(); it2++) {
                net_in_ans1_pos[it2->ID].push_back(ans2.size());
                ans2.push_back({it2->ID,idx2});
                // net_appearnum[it2->ID]++;
                idx2++;
            }
        }        

        printf ("LCS ans1\n");
        for (auto it=ans1.begin(); it!=ans1.end(); it++) {
            printf ("%d ", it->first);
        }
        printf ("\n");
        printf ("LCS ans2\n");
        for (auto it=ans2.begin(); it!=ans2.end(); it++) {
            printf ("%d ", it->first);
        }
        printf ("\n");
        vector<int> list1, list2;
        for (int i=0; i<ans1.size(); i++){
            list1.push_back(ans1.at(i).first);
        }
        for (int i=0; i<ans2.size(); i++){
            list2.push_back(ans2.at(i).first);
        }
        vector<int> _ans = LCS(list1,list2);
        printf("after CPU and DDR remove\n");
        // for (auto it=_ans.rbegin(); it!=_ans.rend(); it++) {
        //     printf("%d ",*it);
        //     net_appearnum[*it]++;
        // }
        {
            vector<pair<int,int>> temp_ans1,temp_ans2;
            int j(0);
            for (auto it=_ans.rbegin(); it!=_ans.rend(); it++) {
                while (*it != ans1.at(j).first) {
                    ans1.at(j).first=-1;
                    j++;
                }
                j++;
            }
            j=0;
            for (auto it=_ans.rbegin(); it!=_ans.rend(); it++) {
                while (*it != ans2.at(j).first) {
                    ans2.at(j).first=-1;
                    j++;
                }
                j++;
            }
            for (auto it=ans1.begin(); it!=ans1.end(); it++) {
                if (it->first != -1)
                    temp_ans1.push_back(*it);
            }
            for (auto it=ans2.begin(); it!=ans2.end(); it++) {
                if (it->first != -1)
                    temp_ans2.push_back(*it);
            }
            ans1.clear();ans2.clear();
            ans1=temp_ans1; ans2=temp_ans2;
        }
        //LCS solution is too crowded

        for(auto it=net_appearnum.begin(); it!=net_appearnum.end(); it++) {
            pair<int,int> p={it->first,it->second};
            if (it->second>1) {
                weight.at(it->first)-=0.1;
            }
        }
        if (net_appearnum.size() > best_ans.first.size())
            best_ans = {ans1,ans2};

        //deal with differential pair
        all_dp_INorOUT=true;
        for (auto it=diff_pair.begin(); it!=diff_pair.end(); it++) {
            if (net_appearnum[it->first]!=0 && net_appearnum[it->second]==0) {
                pair<int,int> p={it->first,it->second};
                all_dp_INorOUT = false;
                weight[it->second] +=0.1;
            }
            else if (net_appearnum[it->first]==0 && net_appearnum[it->second]!=0) {
                pair<int,int> p={it->first,it->second};
                all_dp_INorOUT = false;
                weight[it->first] +=0.1;
            }
        }
        if (round >=3) {
            
            for (auto it=diff_pair.begin(); it!=diff_pair.end(); it++) {
                if (net_appearnum[it->first]!=0 && net_appearnum[it->second]==0) {
                    for (auto pos=net_in_ans1_pos.at(it->first).begin(); pos!=net_in_ans1_pos.at(it->first).end(); pos++) {
                        ans1.at(*pos).first = -1;
                    }
                    for (auto pos=net_in_ans2_pos.at(it->first).begin(); pos!=net_in_ans2_pos.at(it->first).end(); pos++) {
                        ans2.at(*pos).first = -1;
                    }
                }
                else if (net_appearnum[it->first]==0 && net_appearnum[it->second]!=0) {
                    for (auto pos=net_in_ans1_pos.at(it->second).begin(); pos!=net_in_ans1_pos.at(it->second).end(); pos++) {
                        ans1.at(*pos).first = -1;
                    }
                    for (auto pos=net_in_ans2_pos.at(it->second).begin(); pos!=net_in_ans2_pos.at(it->second).end(); pos++) {
                        ans2.at(*pos).first = -1;
                    }
                }
            }
            vector<pair<int,int>> trim_ans1, trim_ans2;
            for (auto it=ans1.begin(); it!=ans1.end(); it++) {
                if (it->first != -1)
                    trim_ans1.push_back(*it);
            }
            for (auto it=ans2.begin(); it!=ans2.end(); it++) {
                if (it->first != -1)
                    trim_ans2.push_back(*it);
            }
            best_ans={trim_ans1,trim_ans2};
            break;
        }
        round++;
    }
    printf("best ans return\n");
    return best_ans;
}