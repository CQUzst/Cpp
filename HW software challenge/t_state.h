#ifndef STRUCT_H
#define STRUCT_H

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <string>
#include <stdlib.h>

#define MAX_N_channel (10)
#define MAX_ROAD_LENGTH (50)

using namespace std;


//struct class建立全部类  car，road,cross三类组合成TState类 类名首字母大写
//车类
class  Car
{
public:
    int m_car_id;
	int m_index;//容器序号，用于取数据

	int m_start_cross_index;
	int m_end_cross_index;

    int m_start_time_expect;//原定出发时间
    int m_start_time_infact;//实际出发时间

	bool m_is_reach_end;//是否到达终点
	int m_reach_end_t;//到达时间

    int m_v_max;//车自身最大车速

	int m_current_road;//当前所在道路index，注意：有可能与path[m_current_path_index]不同！
	int m_current_v;//当前时刻的速度
    int m_current_is_reverse;//在路上是正向还是反向，注意进入路口时要给定行驶方向
	int m_channel_befor_adjust;//在调度前未修改的车道号，每时刻初始化为m_current_channel
	int m_current_channel;//当前时刻所在路的车道号
    int m_current_s0;//当前时刻在当前道路已经行驶的距离
	int m_expect_s0;//在路内调度时，未更新位置，只标记等待状态；用于记录路内调度位置
	int m_move_capability;//剩余可行驶距离，每次更新需要消耗行车能力

	vector<int> m_path;//规划的路径
	int m_current_path_index;//当前道路在规划路径中的序号，下一段路用[m_current_path_index+1]即可

	///运动终结标记
	bool m_going;//true:等待状态；false:终结状态。
	bool in_positive_road;//确定在当前道路上是正向还是反向，正向为true

    ////////////////////函数/////////////////////
    Car();
    ~Car();
    void buildCar(int id,int start_cross, int end_cross, int v_max, int start_time_expect);//构造函数

	void ReSetCarAfterLauchFail();
	//void SetCarLauchTime(int t);
	void SetCarPath(const vector<int>& path);
	void UpdateCarCurrentPosition();

	/////////////////操作符定义//////////////////
	bool operator<(const Car& c) const;//车辆道路内优先级排序比较器，注意要  分  不同向进行排序
	//Car& operator=(const Car& c);//复制定义
};

//路口类
class Cross
{
public:
    int m_cross_id;
	int m_index;//容器序号，用于取数据
    
	//20190320 lzm modified: use array instead of 4 int
	// int road1;
	// int road2;
	// int road3;
	// int road4;
	int m_road_index[4];//-1也要录进来

	///20190320 lzm add：用于优化搜索过程：
	/*优化搜索过程，利用对象标记避免重复搜索：
	//当车道全部车的运动状态都为终止，车道终止；
	//当道路所有车道的运动都为终止，道路终止。
	//当路口所有道路都为终止，路口终止。
	*/
	bool m_going;//true:等待状态；false:终结状态。


    ////////////////////函数/////////////////////
    Cross();
    ~Cross();
    void buildCross(int id, int up, int right, int down, int left);//构造函数


	/////////////////操作符定义//////////////////
	//Cross& operator=(const Car& c);//复制定义
};

//道路类
class Road
{
public:
    int m_road_id;
	int m_index;

	int m_in_cross_index;//入口路口索引号
	int m_out_cross_index;//出口路口索引号

	int m_n_channel;//几个车道
    int m_is_duplex;//是否双向；0表示单向，1表示双向
    int m_v_road_max;
    int m_road_length;

	int m_cars_num_in_pos_road;//当前道路的汽车总数，正向
	int m_cars_num_in_neg_road;//当前道路的汽车总数，反向
	int m_cars_max_capacity;//道路最大汽车容量，正反向容量是一样的，在buildgraph初始化

	vector<list<int>> m_cars_index_in_road[2];//[0]表示正向车道,[1]表示反向车道；vector容器表示多车道；list顺序容器表达同车道车辆的前后关系

	list<int> m_car_waiting_order[2];//当前道路上，等待状态车辆的行车优先级;[0]表示正向车道,[1]表示反向车道；
	
	///20190320 lzm add：用于优化搜索过程：
	//注意初始化指针
	vector<bool> m_channel_going[2];//true:车道等待状态；false:车道终结状态。第一维表示正反方向，第二维表示车道
	bool m_direction_going[2];// true:道路等待状态；false:道路终结状态。


    ////////////////////函数/////////////////////
    Road();//构造函数，注意根据单双向、车道、路长
    ~Road();

    double m_congestion[2];//道路拥挤度，0-1
    
    void buildRoad(int r_id, int road_length, int v_road_max,int n_channel, int in_cross, int out_cross, int is_reverse);
	void buildRoad(int is_duplex, int n_channel);

	bool RemoveWaitingCarFromCarOrder(int car_index, int is_reverse);
	bool RemoveCarFromRoad(int car_index, int is_reverse,int channel);
	int GetCarOrderInChannel(int car_index, int is_reverse, int channel);//没写
	int GetCarNumberInChannel(int is_reverse, int channel);//没写
	list<int> GetBackListInChannel(int direction, int channel, int car_index);//获取当前车的同车道后向列表

	bool CheckIsFront(int direction, int channel, int car_index);//判断当前调度的车是否为最前车

	/////////////////操作符定义//////////////////
	//Road& operator=(const Road& c);//复制定义

};

typedef vector<Cross> CrossVector;
typedef vector<Road> RoadVector;
typedef vector<Car> CarVector;

class TState
{
public:
    int m_dt;
    RoadVector m_roadvec_t;
    CarVector m_carvec_t;
    CrossVector m_crossvec_t;

	CarVector m_deadlock_carvec_t;//暂时只做测试用，每次路口调度结束后，记录所有等待状态的车
	int n_car_waiting_before_adjust;//用于比较调度前后，是否有车的等待状态发生变化
	//之所以用int，是因为，等待状态的车不会增加，只会减少：出路口，减少；进入口，不会变化——因此，只会减少。
    ////////////////////函数////////////////////
    TState();
    ~TState();
    //构造函数
    void buildT_State(int t, RoadVector roadvec_t,
                      CarVector carvec_t, CrossVector crossvec_t);
	void buildT_State();
	

	/////////////////处理函数////////////////////
	void SortRoadByID();//根据ID排序，并设置m_index
	void SortCrossByID();//根据ID排序，并设置m_index
	void SortCarByID();//根据ID排序，并设置m_index


	vector<int> FindLauchCar(int t);//找到到点发车的车辆
	int Launch(int ready_car_index, int t);//一辆发车起步调度
	//int Launch(int ready_car_index, const vector<int>& path, int t);//一辆发车起步调度
	TState TStateInitByPath(vector<int> car_index_vec, vector<vector<int>> cars_path, int t);//初始化发车，传入需要发动的car_vec以及时间t，这里car_index_vec存放默认以car_id升序


	void ResetCarTurning(vector<vector<int>> car_path, vector<int> car_index);//根据给定部分车的路径方案，重新设置这些车的规划路线

	int UpdateTState(int t);//更新t->t1时刻状态
	int UpdateTState(vector<int> car_index_vec, int t, vector<int>& car_is_lauched);//初始化发车，传入需要发动的car_vec以及时间t，这里car_index_vec存放默认以car_id升序;其实跟TStateInitByPath是一样的，不过是自我更新，换了个名字

	bool CheckAllCarReach();

	void InitCarOrderInRoad(int road_index);

	void SortCarOrderInRoad(list<int>& waiting_car_index_list);
	bool cmp_car_index(int c1_index, int c2_index);

	int UpdateCross(int t);
	void UpdateCrossOnce(int t);//执行一次路口调度
	void UpdateCrossCar(Cross& cross, Car& car, Road& from_road, Road& to_road);
	void UpdateCurrentPathIndex();

	bool UpdateRoadCar(list<int>& backward_car_list, int limit, bool former_car_going, bool former_car_is_across); //废弃：被UpdateBackCarStateWhenNotPass-->UpdateBackCarStateAfterPass替换
	int UpdateBackCarStateWhenNotPass(int car_index);//过不了路，更新后续车的状态//废弃，被UpdateBackCarStateAfterPass替换
	int UpdateBackCarStateAfterPass(int road_index, int direction, int channel);//过路，更新后续车的状态
	int CheckIsReverse(Cross cross/*终点cross*/, Car car);
	int CheckIsReverse(Cross cross/*终点cross*/, int road_index);

	bool CheckLocked();

	void ChangeIdToIndex();
	int GetRoadIndex(int road_id);
	int GetCarIndex(int car_id);
	int GetCrossIndex(int cross_id);
	int GetPreCar(int car_index);//寻找同车道前车，如果存在前车，返回前车index，如果不存在前车，返回-1

	int FindStraightConflictRoad(Cross cross, int target_road_id);
	int FindLeftTurningConflictRoad(Cross cross, int target_road_id);
	bool CheckTurningConflict(Cross cross, int from_road_index, int to_road_index);


	int CheckTurning(Cross cross, int original_road_id, int target_road_id);
	int CheckTurning(Cross cross, Car car);


	int GetRoadFirstMovingCar(int road_index, int direction);

    //全图车辆下标容器，用于更新调度
    vector<int> exist_cars_on_graph;

	//处理路口调度死循环问题
	int CheckWaitingCar();
	int InitCrossPolling();//初始化轮询跳出条件

	/////////////////操作符定义//////////////////
	//TState& operator=(const TState& c);//复制定义

	/////////////////测试函数////////////////////
	void ChangePathIdToIndex(vector<int>& path);
	void ChangePathIdToIndex(vector<vector<int>>& paths);
	void SetPath(vector<vector<int>> car_path);
	void SetPath(vector<int> car_path, int car_index);

	//CZT Add  2019/03/26
	void WriteInfoINLog(string logpath);
	void ReadInfoINLog(string logpath);
	void T_WriteAndReadFile();

};

bool SolveMap(TState t0, int plan_time, int all_car_plan_time);//以t0时刻为基础，计算全图跑完的调度时间，总运行时间（注意比较Graph::routePlan）


#endif // STRUCT_H
