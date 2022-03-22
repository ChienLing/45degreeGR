#include "Parser.h"
#define X first
#define Y second
using namespace std;

int MIN_SPACING = -1;
int WIRE_WIDTH = -1;
int obs_ext = 2000;


void SplitString(const std::string& s, const std::string& c, std::vector<std::string>& v)
{
      std::string::size_type pos1, pos2;
      pos2 = s.find(c);
      pos1 = 0;
      while(std::string::npos != pos2)
      {
        v.push_back(s.substr(pos1, pos2-pos1));
     
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
      }
      if(pos1 != s.length())
        v.push_back(s.substr(pos1));
} 
std::string &trim(std::string &s)
{
    if (s.empty())
    {
        return s;
    }

    s.erase(0, s.find_first_not_of('\r'));
    s.erase(s.find_last_not_of('\r') + 1);

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

Parser::Parser(){}

void Parser::read_inputfile(const std::string filename) {
    read_netlist();
    read_drc();
    read_group();
    // print_group();
    read_diff();
    print_diff();
    read_layer();
    read_obs();
}

void Parser::read_inputfile() {
    read_netlist();
    read_drc();
    read_group();
    // print_group();
    read_diff();
    print_diff();
    read_layer();
    read_obs();
    // print_netlist();
}

void Parser::read_obs() {
    std::string filename = iFile_name+".obs";
    ifstream input(filename);
    string S = "";
    while (!input.eof())
    {
        std::getline(input, S);
        string str = S;
        if (S.size() > 0)
        {
            vector<string> string_list;
            string sep = " ";
            SplitString(str, sep, string_list);

            if (string_list.size() < 5)
            {
                continue;
            }

            int x1, y1, x2, y2, l;
            l = atoi(string_list.at(4).c_str());
            x1 = atoi(string_list.at(0).c_str()) - (obs_ext / 2);
            y1 = atoi(string_list.at(1).c_str()) - (obs_ext / 2);
            x2 = atoi(string_list.at(2).c_str()) + (obs_ext / 2);
            y2 = atoi(string_list.at(3).c_str()) + (obs_ext / 2);

            for (int x = 0; x < router->layer_can_use.size(); x++)
            {
                //why only reserve obs in Top layer ?
                if (router->layer_can_use.at(x) == l )//&& l == 0
                {
                    Obs obs;
                    Boundary b(x1,y1,x2,y2);
                    obs.setup(b, l, -1);
                    obs.center={(x1+x2)/2,(y1+y2)/2};
                    //Obs.setup(x1, y1, x2, y2, l, 0);
                    router->obs_list.push_back(obs);
                    // Pin_Obs_list.push_back(Obs);
                }
            }
        }
    }

    input.close();
}

void Parser::read_drc() {
    std::string filename = iFile_name+".drc";
    ifstream input(filename);
    if(!input) {
    cerr<<"cannot open Drc file!\n";
    exit(1);
    }
    int _MIN_SPACING = -1;
    int _WIRE_WIDTH = -1;
    printf("open file %s\n", filename.c_str());
    string S = "";
    while (!input.eof())
    {
        std::getline(input, S);
        string str = S;
        // printf("DRC %s\n", str.c_str());
        if (S.size() > 0)
        {
            vector<string> string_list;
            string sep = " ";
            SplitString(str, sep, string_list);

            if (string_list.size() < 3)
            {
                continue;
            }

            string name;
            int s, w;
            name = trim(string_list.at(0));
            s = atoi(string_list.at(1).c_str());
            w = atoi(string_list.at(2).c_str());

            Drc Drc_temp(name, s, w);
            //Drc_temp.setup(name, 2*w, w);

            if (Drc_temp.spacing > _MIN_SPACING)
                _MIN_SPACING = Drc_temp.spacing;
            if (Drc_temp.wire_width > _WIRE_WIDTH)
                _WIRE_WIDTH = Drc_temp.wire_width;
            router->drc_list.push_back(Drc_temp);
        }
    }

    //only for test
    if (_MIN_SPACING > 0)
        MIN_SPACING = _MIN_SPACING;
    if (_WIRE_WIDTH > 0)
        WIRE_WIDTH = _WIRE_WIDTH;

    MIN_SPACING = 400;//...
    WIRE_WIDTH = 400;
    printf("Min spacing : %d\n", MIN_SPACING);
    printf("wire width : %d\n", WIRE_WIDTH);
    input.close();
}

void Parser::read_diff() {
    std::string filename = iFile_name+".diff";
    std::ifstream input(filename);
    std::string S = "";
    while (!input.eof())
    {
        std::getline(input, S);
        std::string str = S;
        if (S.size() > 0)
        {
            std::vector<string> string_list;
            string sep = " ";
            SplitString(str, sep, string_list);

            if (string_list.size() < 1)
            {
                continue;
            }
            // printf("diff %s\n", S.c_str());
            if (router->diff_NameID.find(string_list.at(1))!=router->diff_NameID.end()) {
                int i = router->diff_NameID.at(string_list.at(1));
                // printf("idx %d \n",i);
                router->diff_list.at(i).netname_list.push_back(string_list.at(0));
                router->diff_list.at(i).netID_list.push_back(router->net_NameID.at(string_list.at(0)));
                if (router->net_NameID.find(string_list.at(0))!=router->net_NameID.end()) {
                    int nid=router->net_NameID.at(string_list.at(0));
                    router->net_list.at(nid).isdiff = true;
                }
                // printf("diff  ");
                // for (auto dp=router->diff_list.at(i).netname_list.begin(); dp!=router->diff_list.at(i).netname_list.end(); dp++) {
                //     printf("%d ",router->net_NameID.at(*dp));
                // }
                // printf("\n");
                
            }
            else 
            {
                if (router->net_NameID.find(string_list.at(0))==router->net_NameID.end()) 
                    continue;
                Diff diff_temp;
                diff_temp.diff_name = string_list.at(1);
                diff_temp.netname_list.push_back(string_list.at(0));
                diff_temp.netID_list.push_back(router->net_NameID.at(string_list.at(0)));
                router->diff_NameID[string_list.at(1)] = router->diff_list.size();
                router->diff_list.push_back(diff_temp);
            }
        }
    }

    input.close();
}

void Parser::read_layer() {
    std::string filename = iFile_name+".layer";
    std::ifstream input(filename);
    string S = "";
    int _max_layer = 0;

    std::getline(input, S);
    string sep = " ";
    if (S.size() > 0)
    {
        vector<string> string_list;
        SplitString(S, sep, string_list);

        if (string_list.size() == 0)
        {
            //do nothing
        }
        else
        {
            router->layer_num = atoi(string_list.at(0).c_str());
            //printf("max layer : %d\n",layer_num);
        }
    }
    router->layer_list.resize(router->layer_num);

    while (!input.eof())
    {
        std::getline(input, S);
        string str1 = S;
        if (S.size() > 0)
        {
            vector<string> string_list_temp;
            SplitString(str1, sep, string_list_temp);
            bool find = false;
            if (string_list_temp.size() < 2)
            {
                continue;
            }

            for (int s = 0; s < string_list_temp.size() - 1; s++)
            {
                if (string_list_temp.at(s).find("TOP") != std::string::npos ||
                    (string_list_temp.at(s).find("BOTTOM") != std::string::npos) ||
                    string_list_temp.at(s).find("S") != std::string::npos)
                {

                    find = true;
                    _max_layer++;
                    break;
                }
            }

            string name;
            int i;

            name += trim(string_list_temp.at(0));
            for (int s = 1; s < string_list_temp.size() - 1; s++)
                name += "_" + trim(string_list_temp.at(s));
            i = atoi(string_list_temp.back().c_str());
            router->layer_list.at(i) = name;
            router->layer_NameID[name] = i;

            if (find == true)
            {
                router->layer_can_use.push_back(i);
            }
        }
    }

    sort(router->layer_can_use.begin(), router->layer_can_use.end());
    router->can_use_layer_num = _max_layer;
    //layer_name.resize(MAX_LAYER);
    printf("max layer : %d\n", router->can_use_layer_num);
    input.close();
}

void Parser::read_group() {
    std::string filename = iFile_name+".group";
    std::ifstream input(filename);
    printf("file name: %s\n",filename.c_str());
    
    if(!input) {
        cerr<<"cannot open group file!\n";
        exit(1);
    }
    string S = "";
    router->group_NameID.clear();
    while (!input.eof())
    {
        std::getline(input, S);
        // printf("group S = %s\n",S.c_str());
        string str = S;
        if (S.size() > 0)
        {
            vector<string> string_list;
            string sep = " ";
            SplitString(str, sep, string_list);
            // printf("string_list size:%d\n",string_list.size());
            // for (auto s=string_list.begin(); s!=string_list.end(); s++) {
            //     printf("%s ",(*s).c_str());
            // }
            //     printf("\n ");
            if (string_list.size() < 4)
            {
                continue;
            }

            // group group_temp;
            // group_temp.net_name = trim(string_list.at(0));
            // group_temp.group_name = trim(string_list.back());
            // group_temp.pin_pair.push_back(trim(string_list.at(1)));
            // group_temp.pin_pair.push_back(trim(string_list.at(2)));
            // group_list.push_back(group_temp);
            if (router->net_NameID.find(trim(string_list.at(0))) == router->net_NameID.end())
                continue;
            if (router->group_NameID.find(string_list.back()) == router->group_NameID.end()) {
                int idx = router->group_NameID.size();
                router->group_NameID[string_list.back()] = idx;
                // printf("group %s idx: %d\n",string_list.back().c_str(), idx);
            }
            int gID = router->group_NameID[string_list.back()];
            int netID = router->net_NameID.at(trim(string_list.at(0)));
            if (router->group_list.size() < router->group_NameID.size()) {
                router->group_list.resize(router->group_NameID.size());
            }
            router->group_list.at(gID).push_back(netID);
            router->net_list.at(netID).belong_group.push_back(string_list.back());
            // printf("input net name: %s, net name:%s \ninput group name: %s\n",
            //             trim(string_list.at(0)).c_str(), router->net_list.at(netID).net_name.c_str(), string_list.back().c_str());
        }
    }

    input.close();
}

void Parser::read_netlist() {
    std::string filename = iFile_name+".netlist";
    printf("file name: %s\n",filename.c_str());
    std::ifstream input(filename);
    if(!input) {
        std::cerr<<"cannot open netlist file!\n";
        exit(1);
    }
    std::string S;
    map<string,int> compname_record;
    while (getline(input,S)) {
        // printf("netlist S = %s\n",S.c_str());
        std::string netName_temp;
        if (S.size() > 0)
        {
            std::vector<std::string> string_list;
            std::string sep = " ";
            SplitString(S, sep, string_list);

            if (string_list.size() == 0)
                continue;

            if (string_list.at(0) == "NetName")
            {
                netName_temp = trim(string_list.at(1));
            }
        }


        getline(input, S);
        {
            // printf("netlist S = %s\n",S.c_str());
            std::stringstream SS(S);
            S = SS.str();
        }
        
        if (S == "PIN START")
        {
            std::vector<int> pin_info;
            int net_num = static_cast<int>(router->net_list.size());
            while (getline(input, S))
            {
                // printf("netlist S = %s\n",S.c_str());
                std::stringstream SS(S);
                S = SS.str();

                if (S == "PIN END")
                {
                    Net net_temp(router->net_list.size(), netName_temp);
                    // cout<<"Net constructor is2pin_net: "<<net_temp.is2pin_net<<endl;
                    for (int i = 0; i < static_cast<int>(pin_info.size()); i++)
                    {
                        int pinID = pin_info.at(i);
                        // printf("pinID:%d\n", pinID);
                        net_temp.net_pinID.push_back(pinID);
                    }
                    if (net_temp.net_pinID.size()!=2) {
                        // cout<<"net_temp.net_pinID.size(): "<<net_temp.net_pinID.size()<<endl;
                         net_temp.is2pin_net = false;
                    }
                    // else
                        
                    net_temp.net_ID = router->net_list.size();
                    router->net_list.push_back(net_temp);
                    router->net_NameID[net_temp.get_netname()] = router->net_list.size()-1;
                    
                    break;
                }
                else
                {
                    std::string name, XY, type;
                    std::vector<std::string> XY_Slist;
                    bool repeat = false;
                    int pinx, piny;

                    SS >> name >> XY >> type;
                    std::string sep = ",";
                    SplitString(XY, sep, XY_Slist);
                    if (name.find("U")==std::string::npos)
                        continue;
                    string compname; compname.assign(name,0,name.find("."));
                    compname_record[compname]++;
                    if (static_cast<int>(XY_Slist.size()) == 2 && isdigit(XY_Slist[0][0]) == true)
                    {
                        pinx = round(stof(XY_Slist[0]) * 100);
                        piny = round(stof(XY_Slist[1]) * 100);
                        if (pinx < router->comp_boundary[compname].left) 
                            router->comp_boundary[compname].left = pinx;
                        else if (pinx > router->comp_boundary[compname].right)
                            router->comp_boundary[compname].right = pinx;
                        if (piny < router->comp_boundary[compname].bot) 
                            router->comp_boundary[compname].bot = piny;
                        else if (piny > router->comp_boundary[compname].top)
                            router->comp_boundary[compname].top = piny;
                        //temp = type + "." + name;
                        // Pin pintemp;
                        // pintemp.pin_name = name;
                        // pintemp.pos.first = pinx;
                        // pintemp.pos.second = piny;

                        //printf("Pin Name %s , original coor : (%.2f,%.2f) -> coor (%d,%d)\n",pintemp.name.c_str(),stof(temps[0]),stof(temps[1]),pinx,piny);
                        //need modify
                        for (int i = 0; i < static_cast<int>(router->pin_list.size()); i++)
                        {
                            if (router->pin_list.at(i).get_pinname().compare(name) == 0)
                            {
                                repeat = true;
                                break;
                            }
                        }

                        if (repeat != true)
                        {
                            std::pair<int,int> pos({pinx,piny});
                            Pin pintemp(router->pin_list.size(), router->net_list.size(),pos,name,netName_temp, compname);
                            router->pin_list.push_back(pintemp);
                            router->pin_NameID[pintemp.get_pinname()] = router->pin_list.size() - 1;
                        }
                    }

                    for (int i = 0; i < static_cast<int>(router->pin_list.size()); i++)
                    {
                        if (router->pin_list.at(i).get_pinname().compare(name) == 0)
                        {
                            if (router->pin_list[i].pin_ID == 0)
                            {
                                pin_info.push_back(i);
                                router->pin_list.at(i).pin_ID = i;
                                router->pin_list.at(i).net_ID = net_num;
                            }
                            else
                            {
                                pin_info.push_back(router->pin_list.at(i).pin_ID);
                            }

                            break;
                        }
                    }
                }
            }
        }
    
    }
    int count(0); router->CPU_name="";
    //decide which is cpu name
    for (auto cp=compname_record.begin(); cp!=compname_record.end(); cp++){
        printf("%s  %d\n", cp->first.c_str(), cp->second);
        if (cp->second>count) {
            if (router->CPU_name!="") {
                router->DDR_name.push_back(router->CPU_name);
            }
            router->CPU_name = cp->first;
            count = cp->second;
        }
        else {
                router->DDR_name.push_back(cp->first);
        }
    }
    printf("CPU:%s (%d,%d)~(%d,%d)\nDDR ",router->CPU_name.c_str(), router->comp_boundary.at(router->CPU_name).left,router->comp_boundary.at(router->CPU_name).bot,
                                            router->comp_boundary.at(router->CPU_name).right,router->comp_boundary.at(router->CPU_name).top);
    for (int i=0; i<router->DDR_name.size(); i++) {
        printf("%s (%d,%d)~(%d,%d)   ",router->DDR_name.at(i).c_str(), router->comp_boundary.at(router->DDR_name.at(i)).left,router->comp_boundary.at(router->DDR_name.at(i)).bot,
                                            router->comp_boundary.at(router->DDR_name.at(i)).right,router->comp_boundary.at(router->DDR_name.at(i)).top);
    }
    printf("\n ");

    for (auto p=router->pin_list.begin(); p!=router->pin_list.end(); p++) {
        p->ddr_name = p->comp_name;
        if (p->pin_name.find(router->CPU_name)!=std::string::npos) {
            if (router->net_list.at(p->net_ID).net_pinID.size()==2) {
                Pin p0=router->pin_list.at(router->net_list.at(p->net_ID).net_pinID.at(0));
                Pin p1=router->pin_list.at(router->net_list.at(p->net_ID).net_pinID.at(1));
                if (p0.pin_name.find(router->CPU_name)==std::string::npos)
                    p->ddr_name = p0.comp_name;
                else
                    p->ddr_name = p1.comp_name;

            }
            p->CPU_side = true;
        }
    }

    for (auto comp=router->comp_boundary.begin(); comp!=router->comp_boundary.end(); comp++) {
        comp->second.name = comp->first;
    }
}

void Parser::print_obs() {
    for (auto o=router->obs_list.begin(); o!=router->obs_list.end(); o++) {
        printf("layer: %d   LB:(%d,%d) RT(%d,%d)\n",o->layer, o->bd.left, o->bd.bot, o->bd.right, o->bd.top);
    }
}

void Parser::print_drc() {
    for (auto d=router->drc_list.begin(); d!=router->drc_list.end(); d++) {
        printf("DRC name:%s, spacing: %d, wire width:%d\n",d->drc_name.c_str(), d->spacing, d->wire_width);
    }
}

void Parser::print_diff() {
    for( auto d=router->diff_list.begin(); d!=router->diff_list.end(); d++) {
        printf("diff name:%s ",d->diff_name.c_str());
        // for (auto n=d->netname_list.begin(); n!=d->netname_list.end(); n++) {
        //     // printf("%s %d ",(*n).c_str());
        //     if (router->net_NameID.find(*n)!=router->net_NameID.end())
        //         printf("%d ", router->net_NameID.at(*n));
        // }
        for (auto nid=d->netID_list.begin(); nid!=d->netID_list.end(); nid++) {
            printf("%d ", *nid);
        }
        printf("\n");
    }
}

void Parser::print_layer() {
    for (auto l=router->layer_list.begin(); l!=router->layer_list.end(); l++) {
        printf("layer:%s\n",l->c_str());
    }
    for (int i=0; i< static_cast<int>(router->layer_can_use.size()); i++) {
        printf("can use layer: %d\n", router->layer_can_use.at(i));
    }
}

void Parser::print_group() {
    for (auto g=router->group_list.begin(); g!=router->group_list.end(); g++) {
        for (auto nid=g->begin(); nid!=g->end(); nid++) {
            printf("net %s    ",router->net_list.at(*nid).net_name.c_str());
            for (auto bg=router->net_list.at(*nid).belong_group.begin(); bg!=router->net_list.at(*nid).belong_group.end(); bg++){
                printf("%s  ",(*bg).c_str());
            }
        printf("\n");
        }
    }
}

void Parser::print_netlist(){
    for (auto n=router->net_list.begin(); n!=router->net_list.end(); n++) {
        printf("net %d  %s  #pin %d ", n->net_ID, n->net_name.c_str(),n->net_pinID.size());
        cout<<"  is2pin_net: "<<n->is2pin_net<<endl;
        for (auto pid=n->net_pinID.begin(); pid!=n->net_pinID.end(); pid++) {
            printf("pin id: %d  pin name: %s comp:%s, ddr:%s\n",router->pin_list.at(*pid).pin_ID, router->pin_list.at(*pid).pin_name.c_str(),\
                                                                router->pin_list.at(*pid).comp_name.c_str(), router->pin_list.at(*pid).ddr_name.c_str());
        }
        printf("\n");
    }
}
