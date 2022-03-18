#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>

//#define CONFIG_PATH  "../data/config.ini"
//#define SITE_BANDWIDTH_PATH  "../data/site_bandwidth.csv"
//#define QOS_PATH  "../data/qos.csv"
//#define DEMAND_PATH  "../data/demand.csv"
//#define OUTPUT_PATH  "../output/solution.txt"

#define CONFIG_PATH  "/data/config.ini"
#define SITE_BANDWIDTH_PATH  "/data/site_bandwidth.csv"
#define QOS_PATH  "/data/qos.csv"
#define DEMAND_PATH  "/data/demand.csv"
#define OUTPUT_PATH  "/output/solution.txt"
/**
 * String split by delimiters
 */
static std::vector<std::string> split(const std::string& s, const std::string& delimiters = ",")
{
    std::vector<std::string> tokens;
    std::size_t lastPos = s.find_first_not_of(delimiters, 0);
    std::size_t pos = s.find_first_of(delimiters, lastPos);
    while (pos != std::string::npos || lastPos != std::string::npos) {
        tokens.emplace_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }

    return tokens;
}
static std::vector<int> split2(const std::string& s, const std::string& delimiters = ",")
{
    std::string str;
    std::vector<int> tokens;
    std::size_t lastPos = s.find_first_not_of(delimiters, 0);//查找第一个非,
    std::size_t pos = s.find_first_of(delimiters, lastPos);//查找第一个,
    str = s.substr(lastPos, pos - lastPos);
    lastPos = s.find_first_not_of(delimiters, pos);
    pos = s.find_first_of(delimiters, lastPos);
    while (pos != std::string::npos || lastPos != std::string::npos) {
        tokens.emplace_back(atoi(s.substr(lastPos, pos - lastPos).c_str()));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }
    return tokens;
}
/**
 * read Config
 */
int readConf()
{
    int QOS_LIMIT;
    std::ifstream config;
    config.open(CONFIG_PATH);
    std::string tmp_line;
    //getline(config, tmp_line);//丢弃第一行
    //getline(config, tmp_line);
    config >> tmp_line;
    config >> tmp_line;
    //std::cout << tmp_line << std::endl;
    QOS_LIMIT = atoi(std::string(tmp_line.begin() + tmp_line.find('=') + 1, tmp_line.end()).c_str());
    //std::cout << QOS_LIMIT << std::endl;
    config.close();
    return QOS_LIMIT;
}

/**
 * 通过site_bandwidth行数获取边缘节点数目
 */
int getSiteNum()
{
    std::ifstream data;
    std::string tmp_line;
    data.open(SITE_BANDWIDTH_PATH);
    int counter = 0;
    //while (getline(data, tmp_line))
    while(data >> tmp_line)
    {
        counter++;
    }
    data.close();
    return counter-1;
}

/**
 * read site_bandwidth
 */
void readSiteBandWidth(int* siteBandWidth,std::string* siteName)
{
    std::ifstream data;
    std::string tmp_line;
    std::vector<std::string> tmp_vec;
    data.open(SITE_BANDWIDTH_PATH);
    //getline(data, tmp_line);
    data >> tmp_line;
    int counter = 0;
    //while (getline(data, tmp_line))
    while(data >> tmp_line)
    {
        tmp_vec = split(tmp_line, ",");
        siteName[counter] = tmp_vec[0];
        siteBandWidth[counter] = atoi(tmp_vec[1].c_str());
        counter++;
    }
    data.close();
}

/**
 * read qos
 * 返回qos列数获取客户节点数目
 */
void readQos(std::string* clientName,std::vector<std::vector<int>>& sitesOfClient, std::vector<std::vector<int>>& clientsOfSite, int QOS_LIMIT)
{
    std::ifstream data;
    std::string tmp_line;
    data.open(QOS_PATH);
    //getline(data, tmp_line);
    data >> tmp_line;
    std::vector<std::string> tmp_vec;
    std::vector<int> tmp_vec2;
    tmp_vec = split(tmp_line, ",");
    int colNum = tmp_vec.size();
    std::vector<int> temp;//空vector
    for (int i = 0; i < colNum-1; i++)
    {
        clientName[i] = tmp_vec[i+1];
        sitesOfClient.emplace_back(temp);
    }

    int index = 0;
    //while (getline(data, tmp_line))
    while (data>>tmp_line)
    {
        clientsOfSite.emplace_back(temp);
        tmp_vec2 = std::move(split2(tmp_line, ","));
        for (int k = 0; k < tmp_vec2.size(); ++k)//indx指示边缘节点， k指示客户节点
        {
            if (tmp_vec2[k] < QOS_LIMIT)//qos合格
            {
                clientsOfSite[index].emplace_back(k);
                sitesOfClient[k].emplace_back(index);
            }
        }
        index++;
    }

    data.close();
}
/**
 * 通过qos列数获取客户节点数目
 */
int getClientSiteNum()
{
    std::ifstream data;
    std::string tmp_line;
    data.open(QOS_PATH);
    //getline(data, tmp_line);
    data >> tmp_line;
    std::vector<std::string> tmp_vec;
    tmp_vec = split(tmp_line, ",");
    return tmp_vec.size() - 1;
}

/**
 * read demand
 */
void readDemand(int** demand)
{
    std::ifstream data;
    std::string tmp_line;
    std::vector<int> tmp_vec;
    data.open(DEMAND_PATH);
    //getline(data, tmp_line);
    data >> tmp_line;
    int counter = 0;
    //while (getline(data, tmp_line))
    while(data >> tmp_line)
    {
        tmp_vec = split2(tmp_line, ",");
        for (int j = 0; j < tmp_vec.size(); ++j)
        {
            demand[counter][j] = tmp_vec[j];
        }
        counter++;
    }
    data.close();
}
/**
 * 通过demand行数获取时刻数目
 */
int getTimeNum()
{
    std::ifstream data;
    std::string tmp_line;
    data.open(DEMAND_PATH);
    //getline(data, tmp_line);
    data >> tmp_line;
    int counter = 0;
    //while (getline(data, tmp_line))
    while (data >> tmp_line)
    {
        counter++;
    }
    data.close();
    return counter;
}


void Output(std::string* clientName, std::string* siteName, int*** resultForOutput, int clientNum, int siteNum, int timeNum)
{
    //FILE* fid = fopen(OUTPUT_PATH, "w");
    std::ofstream outfile(OUTPUT_PATH);
    for (int i = 0; i < timeNum; i++)
    {

        for (int j = 0; j < clientNum; j++)
        {
            outfile << clientName[j] << ":";
            bool flag = false;
            std::string temp;
            for (int k = 0; k < siteNum; k++)
            {
                if (resultForOutput[i][j][k] > 0)//i时刻第k个边缘节点有分配
                {
                    //fprintf(fid, "%s", ("<" + siteName[k] + "," + std::to_string(distribute[timeNum][k]) + ">,").c_str());
                    temp = temp + "<" + siteName[k] + "," + std::to_string(resultForOutput[i][j][k]) + ">,";
                    flag = true;
                }
            }
            if (flag == true)
            {
                temp.pop_back();
                outfile << temp;
                if (i != timeNum - 1 || j != clientNum - 1)
                {
                    outfile << std::endl;
                }
            }
            else if (i != timeNum - 1 || j != clientNum - 1)
            {
                outfile << std::endl;
            }
        }
    }

    outfile.close();
    //fclose(fid);
}

int main()
{
    const int QOS_LIMIT = readConf();
    const int siteNum = getSiteNum();
    int* siteBandWidth = new int[siteNum];
    std::string* siteName = new std::string[siteNum];
    readSiteBandWidth(siteBandWidth, siteName);
    //for (int i = 0; i < siteNum;++i)
    //{
    //        printf("name:%s,bw=%d \n", siteName[i].c_str(), siteBandWidth[i]);
    //}
    std::vector<std::vector<int>> sitesOfClient;//对于某个客户节点符合qos的边缘节点列表
    std::vector<std::vector<int>> clientsOfSite;//对于某个边缘节点符合qos的可承担任务的客户节点列表
    const int clientNum = getClientSiteNum();
    std::string* clientName = new std::string[clientNum];
    readQos(clientName, sitesOfClient, clientsOfSite, QOS_LIMIT);
    const int timeNum = getTimeNum();
    int** demand = new int*[timeNum];
    for (int i = 0; i < timeNum; i++)
    {
        demand[i] = new int[clientNum];
    }
    readDemand(demand);
    //for (int i = 0; i < timeNum; i++)
    //{
    //    for (int j = 0; j < clientNum; j++)
    //    {
    //        printf("%d  ", demand[i][j]);
    //    }
    //    printf("\n");
    //}
    /**************************分配策略***************************/
    //建立siteNum个长为timeNum的数组，用于存储分配结果
    int** distribute = new int* [siteNum];
    for (int i = 0; i < siteNum; i++)
    {
        distribute[i] = new int[timeNum]();
    }
    //建立一个长siteNum数组，用于存储对应site最大值
    int* siteMaxBW = new int[siteNum]();

    //建立timeNum个长为clientNum的siteNum的二维数组,用于存储分配结果
    int*** resultForOutput = new int**[timeNum];
    for (int i = 0; i < timeNum; i++)
    {
        resultForOutput[i] = new int*[clientNum];
        for (int j = 0; j < clientNum; j++)
        {
            resultForOutput[i][j] = new int[siteNum]();
        }
    }
    //存储demand副本
    int** demandBank = new int* [timeNum];
    for (int i = 0; i < timeNum; i++)
    {
        demandBank[i] = new int[clientNum];
        for (int j = 0; j < clientNum; j++)
        {
            demandBank[i][j] = demand[i][j];
        }
    }

    for(int i = 0;i<timeNum ;i++)//遍历每个时间点
    {
        //每次遍历时间点都要对边缘节点带宽恢复初始值
        int* siteBandWidth_copy = new int [siteNum];
        for(int a = 0 ;a<siteNum ;a++)
        {
            siteBandWidth_copy[a] = siteBandWidth[a];
        }
        for(int j = clientNum-1 ;j>=0 ; j--)//遍历每个用户
        {
            int need = demand[i][j];//每个客户需要的流量
            std::vector<int> sites = sitesOfClient[j];//该客户能到达的边缘节点的序列
            for(int index= sites.size()-1 ;index>=0 ;index--)//遍历能到达的边缘节点
            {
                if(need == 0)//need为0即客户获得所需流量
                {
                    break;
                }
                int k = sites[index];//该边缘节点的序号
                int bandwidth = siteBandWidth_copy[k];//该边缘节点的带宽
                if(need <= bandwidth)//如果所需小于带宽
                {
                    siteBandWidth_copy[k] = siteBandWidth_copy[k]-need;//减掉用去的带宽
                    resultForOutput[i][j][k] =  need;//i时间点，j客户向k节点申请的流量
                    need = 0;//所需归0
                }
                else
                {
                    need = need - siteBandWidth_copy[k];//获取剩余的所需
                    resultForOutput[i][j][k] = siteBandWidth_copy[k];
                    siteBandWidth_copy[k] = 0;
                }
            }
        }
    }


    Output(clientName, siteName, resultForOutput, clientNum, siteNum, timeNum);

    /***********结束程序*******************/
    for (int i = 0; i < timeNum; i++)
    {
        for (int j = 0; j < clientNum; j++)
        {
            delete resultForOutput[i][j];
        }
        delete[] resultForOutput[i];
    }
    delete[] resultForOutput;

    for (int i = 0; i < siteNum; i++)
    {
        delete[] distribute[i];
    }
    delete[] distribute;
    for (int i = 0; i < timeNum; i++)
    {
        delete[] demand[i];
    }
    delete[] demand;
    for (int i = 0; i < timeNum; i++)
    {
        delete[] demandBank[i];
    }
    delete[] demandBank;
    delete[] clientName;
    delete[] siteName;
    delete[] siteBandWidth;
    return 0;
}