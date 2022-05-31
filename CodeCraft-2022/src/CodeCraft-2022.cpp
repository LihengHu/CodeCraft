#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <cmath>


//本地路径
#define CONFIG_PATH  "../../../data/config.ini"
#define SITE_BANDWIDTH_PATH  "../../../data/site_bandwidth.csv"
#define QOS_PATH  "../../../data/qos.csv"
#define DEMAND_PATH  "../../../data/demand.csv"
#define OUTPUT_PATH  "../../../output/solution.txt"

//提交华为官网的路径
//#define CONFIG_PATH  "/data/config.ini"
//#define SITE_BANDWIDTH_PATH  "/data/site_bandwidth.csv"
//#define QOS_PATH  "/data/qos.csv"
//#define DEMAND_PATH  "/data/demand.csv"
//#define OUTPUT_PATH  "/output/solution.txt"
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
void readQos(std::string* clientName,std::vector<std::vector<int>>& sitesOfClient, std::vector<std::vector<int>>& clientsOfSite, int QOS_LIMIT,std::string * siteName,int siteNum)
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

    //while (getline(data, tmp_line))
    while (data>>tmp_line)
    {
        int index = 0;//寻找index
        std::vector<std::string> tmpForName;
        tmpForName = split(tmp_line, ",");
        while (tmpForName[0] != siteName[index])
        {
            index++;
            if (index >= siteNum)
            {
                printf("error \n");
            }

        }
        //clientsOfSite.emplace_back(temp);
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
void readDemand(int** demand,std::string *clientName,int clientNum)
{
    std::ifstream data;
    std::string tmp_line;
    std::vector<int> tmp_vec;
    std::vector<std::string> tempForCiteName;
    std::vector<int> tempForCiteNameIndx;
    data.open(DEMAND_PATH);
    //getline(data, tmp_line);
    data >> tmp_line;
    tempForCiteName = split(tmp_line, ",");
    for (int i = 1; i < tempForCiteName.size(); i++)
    {
        int ii = 0;
        while (tempForCiteName[i] != clientName[ii])
        {
            ii++;
            if (ii > clientNum)
            {
                printf("error citeNum\n");
                break;
            }
        }
        tempForCiteNameIndx.emplace_back(ii);
    }
    int counter = 0;
    //while (getline(data, tmp_line))
    while(data >> tmp_line)
    {
        tmp_vec = split2(tmp_line, ",");
        for (int j = 0; j < tmp_vec.size(); ++j)
        {
            int indx = tempForCiteNameIndx[j];
            demand[counter][indx] = tmp_vec[indx];
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
    srand(1222);
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
    std::vector<std::vector<int>> clientsOfSite(siteNum);//对于某个边缘节点符合qos的可承担任务的客户节点列表
    const int clientNum = getClientSiteNum();
    std::string* clientName = new std::string[clientNum];
    readQos(clientName, sitesOfClient, clientsOfSite, QOS_LIMIT, siteName, siteNum);
    const int timeNum = getTimeNum();
    int** demand = new int*[timeNum];
    for (int i = 0; i < timeNum; i++)
    {
        demand[i] = new int[clientNum];
    }
    readDemand(demand, clientName, clientNum);
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
    //获取site顺序
    //std::vector<std::pair<int, int>>siteIndx;
    //for (int i = 0; i < siteNum; ++i)
    //{
    //    siteIndx.emplace_back(i, clientsOfSite[i].size());
    //}
    //std::sort(siteIndx.begin(), siteIndx.end(), [&](std::pair<int, int> x, std::pair<int, int> y) {return x.second > y.second; });//降序



    //按时间顺序逐步分配 (后面可以优化成按带宽压力大小顺序)
    for (int t = 0; t < timeNum; t++)
    {
        for (int c = 0; c < clientNum; c++)
        {
            int flow = demand[t][c];
            int siteCounter = 0;//可分配site数量
            for (int si = 0; si < sitesOfClient[c].size(); si++)
            {
                int s = sitesOfClient[c][si];
                if (distribute[s][t] < siteBandWidth[s])
                {
                    siteCounter++;
                }
            }
            while (flow > 0)
            {
                int averFlow = flow / siteCounter + 1;//加一避免因为精度丢失导致没有全部分配
                for (int si = 0; si < sitesOfClient[c].size(); si++)
                {
                    int s = sitesOfClient[c][si];
                    if (flow == 0)
                    {
                        break;
                    }
                    if (distribute[s][t] < siteBandWidth[s])
                    {
                        if (flow < averFlow)//由于+1的存在，导致最后一组averFlow已经多于flow了
                        {
                            averFlow = flow;
                        }
                        if (distribute[s][t] + averFlow < siteBandWidth[s])//
                        {
                            distribute[s][t] = distribute[s][t] + averFlow;
                            resultForOutput[t][c][s] = resultForOutput[t][c][s] + averFlow;
                            flow = flow - averFlow;
                        }
                        else
                        {
                            int canFlow = siteBandWidth[s] - distribute[s][t];
                            distribute[s][t] = distribute[s][t] + canFlow;
                            resultForOutput[t][c][s] = resultForOutput[t][c][s] + canFlow;
                            flow = flow - canFlow;
                            siteCounter--;//节点达到最大限制以后，对可分配节点计数减一
                        }
                    }
                }
            }
        }
    }


    /***********************************反向检查后一半数据******************************/
    int* siteIndxNum = new int[siteNum]();
    for (int reNum = 1; reNum < 100; reNum++)
    {
        std::vector<std::vector<std::pair<int, int>>>distributeInsiteAll;
        std::vector<int>distributeSplitVlaue1, distributeSplitVlaue2;
        for (int s = 0; s < siteNum; s++)
        {
            std::vector<std::pair<int, int>> distributeInsite;
            for (int t = 0; t < timeNum; ++t)
            {
                distributeInsite.emplace_back(t, distribute[s][t]);
            }
            std::sort(distributeInsite.begin(), distributeInsite.end(), [&](std::pair<int, int> x, std::pair<int, int> y) {return x.second < y.second; }); // 升序
            distributeInsiteAll.emplace_back(distributeInsite);
            distributeSplitVlaue1.emplace_back(distributeInsite[std::ceil((timeNum * (0 + reNum * 0.95 / 100)))].second);//分界值
            distributeSplitVlaue2.emplace_back(distributeInsite[(timeNum * 0.95)+1].second);//分界值，减一是因为数组下标从0开始
        }

        //3 2917___
        for (int si = 0; si < siteNum * 3; si++)
        {
            //randPerm(siteIndxNum, siteNum);
            //int s = siteIndxNum[si];
            int s = (rand() % (siteNum - 0)) + 0;
            int halfTimeeNum = static_cast<int>((timeNum * 0.93));
            int startNum = timeNum - 1;
            if (reNum > 90)
            {
                startNum = std::ceil(timeNum * 0.97);//0.97 282
            }
            for (int i = startNum; i > halfTimeeNum; i--)
            {
                int t = distributeInsiteAll[s][i].first;//取到节点s未优化过的流量最大的时刻t
                int maxBwDiff = distributeInsiteAll[s][i].second - distributeInsiteAll[s][static_cast<int>(timeNum * 0)].second;//理论上值得分流出去的最大带宽

                for (int j = 0; j < clientsOfSite[s].size(); j++)//通过对应的client查看能否分流
                {
                    if (maxBwDiff <= 0)
                    {
                        break;
                    }
                    int c = clientsOfSite[s][j];
                    int canOutput = resultForOutput[t][c][s];
                    canOutput = std::min(canOutput, maxBwDiff);//避免过度流出
                    for (int k = 0; k < sitesOfClient[c].size(); k++)//寻找流出目标
                    {
                        if (canOutput <= 0)//可以流出的已经全部流出
                        {
                            break;
                        }
                        int ss = sitesOfClient[c][k];
                        if (distribute[ss][t] < distributeSplitVlaue1[ss] && distribute[ss][t] < siteBandWidth[ss])//如果t时刻的节点ss带宽小于分界值则向他流出
                        {
                            //int canInput = (distributeSplitVlaue1[ss] - distribute[ss][t]) / 2;//仅使用一半，其余一半留给其他节点优化
                            int canInput;
                            if (distributeSplitVlaue1[ss] < siteBandWidth[ss])
                            {
                                canInput = (distributeSplitVlaue1[ss] - distribute[ss][t]) / 2;//仅使用一半，其余一半留给其他节点优化
                            }
                            else
                            {
                                canInput = (siteBandWidth[ss] - distribute[ss][t]) / 2;//仅使用一半，其余一半留给其他节点优化
                            }
                            int flow = std::min(canOutput, canInput);//以可流出与可流入中的小值为流动量
                            if (distribute[s][t] - flow >= 0 && distribute[ss][t] + flow <= siteBandWidth[ss])
                            {
                                resultForOutput[t][c][s] = resultForOutput[t][c][s] - flow;
                                distribute[s][t] = distribute[s][t] - flow;

                                resultForOutput[t][c][ss] = resultForOutput[t][c][ss] + flow;
                                distribute[ss][t] = distribute[ss][t] + flow;

                                maxBwDiff = maxBwDiff - flow;//优化进度
                                canOutput = canOutput - flow;//可流出减少，准备寻找下一个可流入节点
                            }
                        }
                        else if (distribute[ss][t] > distributeSplitVlaue2[ss] && distribute[ss][t] < siteBandWidth[ss])//如果t时刻的节点ss带宽大于分界值则尽量塞满该节点
                        {
                            int canInput = (siteBandWidth[ss] - distribute[ss][t]) / 2;//仅使用一半，其余一半留给其他节点优化
                            int flow = std::min(canOutput, canInput);//以可流出与可流入中的小值为流动量
                            if (distribute[s][t] - flow >= 0 && distribute[ss][t] + flow <= siteBandWidth[ss])
                            {
                                resultForOutput[t][c][s] = resultForOutput[t][c][s] - flow;
                                distribute[s][t] = distribute[s][t] - flow;

                                resultForOutput[t][c][ss] = resultForOutput[t][c][ss] + flow;
                                distribute[ss][t] = distribute[ss][t] + flow;

                                maxBwDiff = maxBwDiff - flow;//优化进度
                                canOutput = canOutput - flow;//可流出减少，准备寻找下一个可流入节点
                            }
                        }
                    }
                }
            }
        }
    }





    Output(clientName, siteName, resultForOutput, clientNum, siteNum, timeNum);

    //计算分数
    int score = 0;
    for (int i = 0; i < siteNum; i++)
    {
        std::sort(distribute[i], distribute[i] + timeNum, std::less<int>());
        int t95 = std::ceil(timeNum * 0.95)-1;
        score = score + distribute[i][t95];
    }
    printf("score = %d\n", score);

    /***********结束程序*******************/
    //delete[] siteIndxNum;
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