#include "Parser.h"
using namespace std;

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

void Parser::read_inputfile(const std::string filename) {}

void Parser::read_inputfile() {
    read_netlist();
    // read_obs();
    // read_diff();
    // read_group();
}

void Parser::read_group() {
    std::string filename = iFile_name+".group";
    std::ifstream input(filename);
    printf("file name: %s\n",filename.c_str());
    
    if(!input) {
        cerr<<"cannot open group file!\n";
        exit(1);
    }
    string strLine = "";
    while (!input.eof())
    {
        std::getline(input, strLine);
        // printf("group S = %s\n",strLine.c_str());
        string str = strLine;
        if (strLine.size() > 0)
        {
            vector<string> string_list;
            string sep = " ";
            SplitString(str, sep, string_list);

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
            int netID = router->net_NameID.at(trim(string_list.at(0)));
            router->group[string_list.back()].push_back(router->net_list.at(netID));
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
                    for (int i = 0; i < static_cast<int>(pin_info.size()); i++)
                    {
                        int pinID = pin_info.at(i);
                        // printf("pinID:%d\n", pinID);
                        net_temp.net_pinID.push_back(pinID);
                    }
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
                    if (static_cast<int>(XY_Slist.size()) == 2 && isdigit(XY_Slist[0][0]) == true)
                    {
                        pinx = round(stof(XY_Slist[0]) * 100);
                        piny = round(stof(XY_Slist[1]) * 100);
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
                            Pin pintemp(router->pin_list.size(), router->net_list.size(),pos,name,netName_temp);
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
}

void Parser::print_netlist(){
    for (auto n=router->net_list.begin(); n!=router->net_list.end(); n++) {
        printf("net %d  %s\n", n->net_ID, n->net_name.c_str());
        for (auto pid=n->net_pinID.begin(); pid!=n->net_pinID.end(); pid++) {
            printf("%d  %s\n",router->pin_list.at(*pid).pin_ID, router->pin_list.at(*pid).pin_name.c_str());
        }
        printf("\n");
    }
}