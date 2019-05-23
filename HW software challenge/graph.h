#ifndef _GRAPH_H_
#define _GRAPH_H_

#include<fstream>
#include<assert.h>
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <algorithm>
#include "t_state.h"


//graph类完成数据读入，建图，路径规划，结果输出
//宏定义数大小根据map调整
#define MAX_NUM_CROSS (200)
#define MAX_NUM_ROAD (200)
#define MAX_NUM_CAR (20000)
#define MAX_NUM (9999)

using namespace std;

class Graph
{
public:
    Graph();
    ~Graph();
    //读入数据
    int readCross(string crossPath, TState& newState);
    int readRoad(string roadPath, TState& newState);
    int readCar(string carPath, TState& newState);
    void readData(string crossPath, string roadPath,string carPath,TState& t_state);
    //初始化邻接矩阵
    void buildGraph(int crossNum,TState& t_state);//建图

    /////////////////路径规划///////////////////
    //D算法路径规划
    void DijkstraSolve(int from,int *m_preVertex,int *m_distance);
    vector<int> planOneCar(int from,int to);
    void routePlan(string answerpath,TState& t_state);

	//计算道路上车辆，确定车辆在当前道路正反向，放入exist_cars_on_graph，用于评价拥堵情况，以及更新调度
	void calCarNumInRoad(TState& t_state);
	//更新权重
    void updateGraphWeights(TState& t_state);
	//更新路径规划
	void updateRoutePlan(TState& t_state);

//private:
    double m_edge_weight[MAX_NUM_CROSS][MAX_NUM_CROSS];//边集，数值表示权重，可能有小数，用double
    int m_connect_matrix[MAX_NUM_CROSS][MAX_NUM_CROSS];//邻接矩阵,表路口之间的道路ID

    //存放数据数量
    int m_crossNum;
    int m_roadNum;
    int m_carNum;

	//全图车辆总数
	int all_cars_num_in_graph;




    //T_StateVector t_state_vec;//测试用，查看上一时间T的全局图
};

#endif
