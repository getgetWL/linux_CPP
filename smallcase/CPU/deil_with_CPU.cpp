#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
using namespace std;

struct processinfo
{
    double cpu =0.0;
    string programName;
};



bool containsAllSubstrings(const std::string& str, const std::vector<std::string>& substrings) {
    for (const auto& substr : substrings) {
        if (str.find(substr) != std::string::npos) {
            return true;
        }
    }
    return false;
}

/* 获取两个指定字符串之间的内容 */
bool ExtractSubStr(const std::string &input, const std::string &target1, const std::string &target2, std::string &output)
{
    size_t pos1 = input.find(target1);
    size_t pos2 = input.find(target2);

    if (pos1 != std::string::npos && pos2 != std::string::npos)
    {
        pos1 += target1.length();
        output = input.substr(pos1, pos2 - pos1);
        return true;
    }
    else
    {
        std::cout << "Extract string [" << input << "] between str1 [" << target1 << "] and str2 [" << target2 << "] fail.";
        return false;
    }
}

int main()
{
    // ifstream file("/proc/meminfo");
    ifstream file("./top.txt");
    std::string line;

    if (!file.is_open())
    {
       std::cout << "unable to open /top.txt";
        return 0;
    }
    std::multimap<int, processinfo> multiMap;
    bool id_flag =true;
    bool get_cpu =false;
    double id = 0.0;
    while (getline(file, line))
    {
        // 获取id值
        if(id_flag)
        {
            string str_id;
            if(line[0] == '%' && ExtractSubStr(line, "ni, ", " id", str_id))
            {
                id = std::stod(str_id);
                std::cout<<"id:"<<id;
                id_flag = false;
            }
            else
            {
                continue;
            }
        }

        istringstream iss(line);
        string key;
        int count =1;
        processinfo info;
        while (iss >> key)
        {
            // 找到数据起始行
            if(key == "PID")
            {
                get_cpu =true;
                break;
            }
            // cpu数据所在行
            if (get_cpu)
            {
                if (count == 9)
                {
                    info.cpu = std::stod(key);
                    // std::cout<<"CPU:"<<info.cpu;
                }
                // 程序名
                if (count == 12)
                {
                    info.programName = key;
                    std::vector<std::string> substr = {"Sf_VCt_L", "Sf_VCo_L3", "Sf_VCt_PlanAgent_VH_2", "view_tool", "Sf_VCt_AppM", "top"};
                    std::vector<std::string> substr_FT = {"FT_Apps","CIC_Server","SPI_Server","Sf_VCt_Diag","GeneralDiag"};
                    if (containsAllSubstrings(key, substr))
                    {
                        // std::cout<<" ----->>>>> ";
                        std::cout << "info.cpu: " << info.cpu << " info.programName:" << key << std::endl;
                        multiMap.insert(std::make_pair(1, info));
                    }
                    else if (containsAllSubstrings(key, substr_FT))
                    {
                        // std::cout<<" -----&&&&&& ";
                        std::cout << "info.cpu: " << info.cpu << " info.programName:" << key << std::endl;
                        multiMap.insert(std::make_pair(2, info));
                    }
                    else
                    {
                        multiMap.insert(std::make_pair(0, info));
                    }
                }
                count++;
            }
        }

    }
    double total_cpu = 0.0;
    for (int i = 0; i < 3; i++)
    {
        // 在map中查找键
        auto range = multiMap.equal_range(i);
        double sum_cpu =0.0;
        for (auto it = range.first; it != range.second; ++it)
        {
            // std::cout<<"CPU:"<<it->second.cpu;
            sum_cpu += it->second.cpu;
        }
        total_cpu += sum_cpu;
        if(i==0)
            std::cout<<"系统负载:"<<i<<" sum_cpu: "<<1.0*sum_cpu/8<<std::endl;
        else if(i==1)
            std::cout<<"自研负载:"<<i<<" sum_cpu: "<<1.0*sum_cpu/8<<std::endl;
        else
            std::cout<<"FT负载:"<<i<<" sum_cpu: "<<1.0*sum_cpu/8<<std::endl;
    }
    std::cout<<"总负载:"<<1.0*total_cpu/8<<std::endl;
    std::cout<<"空闲负载:"<<id<<std::endl;
    file.close();

    return 0;
}