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


//graph��������ݶ��룬��ͼ��·���滮��������
//�궨������С����map����
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
    //��������
    int readCross(string crossPath, TState& newState);
    int readRoad(string roadPath, TState& newState);
    int readCar(string carPath, TState& newState);
    void readData(string crossPath, string roadPath,string carPath,TState& t_state);
    //��ʼ���ڽӾ���
    void buildGraph(int crossNum,TState& t_state);//��ͼ

    /////////////////·���滮///////////////////
    //D�㷨·���滮
    void DijkstraSolve(int from,int *m_preVertex,int *m_distance);
    vector<int> planOneCar(int from,int to);
    void routePlan(string answerpath,TState& t_state);

	//�����·�ϳ�����ȷ�������ڵ�ǰ��·�����򣬷���exist_cars_on_graph����������ӵ��������Լ����µ���
	void calCarNumInRoad(TState& t_state);
	//����Ȩ��
    void updateGraphWeights(TState& t_state);
	//����·���滮
	void updateRoutePlan(TState& t_state);

//private:
    double m_edge_weight[MAX_NUM_CROSS][MAX_NUM_CROSS];//�߼�����ֵ��ʾȨ�أ�������С������double
    int m_connect_matrix[MAX_NUM_CROSS][MAX_NUM_CROSS];//�ڽӾ���,��·��֮��ĵ�·ID

    //�����������
    int m_crossNum;
    int m_roadNum;
    int m_carNum;

	//ȫͼ��������
	int all_cars_num_in_graph;




    //T_StateVector t_state_vec;//�����ã��鿴��һʱ��T��ȫ��ͼ
};

#endif
