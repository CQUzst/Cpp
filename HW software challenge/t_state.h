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


//struct class����ȫ����  car��road,cross������ϳ�TState�� ��������ĸ��д
//����
class  Car
{
public:
    int m_car_id;
	int m_index;//������ţ�����ȡ����

	int m_start_cross_index;
	int m_end_cross_index;

    int m_start_time_expect;//ԭ������ʱ��
    int m_start_time_infact;//ʵ�ʳ���ʱ��

	bool m_is_reach_end;//�Ƿ񵽴��յ�
	int m_reach_end_t;//����ʱ��

    int m_v_max;//�����������

	int m_current_road;//��ǰ���ڵ�·index��ע�⣺�п�����path[m_current_path_index]��ͬ��
	int m_current_v;//��ǰʱ�̵��ٶ�
    int m_current_is_reverse;//��·���������Ƿ���ע�����·��ʱҪ������ʻ����
	int m_channel_befor_adjust;//�ڵ���ǰδ�޸ĵĳ����ţ�ÿʱ�̳�ʼ��Ϊm_current_channel
	int m_current_channel;//��ǰʱ������·�ĳ�����
    int m_current_s0;//��ǰʱ���ڵ�ǰ��·�Ѿ���ʻ�ľ���
	int m_expect_s0;//��·�ڵ���ʱ��δ����λ�ã�ֻ��ǵȴ�״̬�����ڼ�¼·�ڵ���λ��
	int m_move_capability;//ʣ�����ʻ���룬ÿ�θ�����Ҫ�����г�����

	vector<int> m_path;//�滮��·��
	int m_current_path_index;//��ǰ��·�ڹ滮·���е���ţ���һ��·��[m_current_path_index+1]����

	///�˶��ս���
	bool m_going;//true:�ȴ�״̬��false:�ս�״̬��
	bool in_positive_road;//ȷ���ڵ�ǰ��·���������Ƿ�������Ϊtrue

    ////////////////////����/////////////////////
    Car();
    ~Car();
    void buildCar(int id,int start_cross, int end_cross, int v_max, int start_time_expect);//���캯��

	void ReSetCarAfterLauchFail();
	//void SetCarLauchTime(int t);
	void SetCarPath(const vector<int>& path);
	void UpdateCarCurrentPosition();

	/////////////////����������//////////////////
	bool operator<(const Car& c) const;//������·�����ȼ�����Ƚ�����ע��Ҫ  ��  ��ͬ���������
	//Car& operator=(const Car& c);//���ƶ���
};

//·����
class Cross
{
public:
    int m_cross_id;
	int m_index;//������ţ�����ȡ����
    
	//20190320 lzm modified: use array instead of 4 int
	// int road1;
	// int road2;
	// int road3;
	// int road4;
	int m_road_index[4];//-1ҲҪ¼����

	///20190320 lzm add�������Ż��������̣�
	/*�Ż��������̣����ö����Ǳ����ظ�������
	//������ȫ�������˶�״̬��Ϊ��ֹ��������ֹ��
	//����·���г������˶���Ϊ��ֹ����·��ֹ��
	//��·�����е�·��Ϊ��ֹ��·����ֹ��
	*/
	bool m_going;//true:�ȴ�״̬��false:�ս�״̬��


    ////////////////////����/////////////////////
    Cross();
    ~Cross();
    void buildCross(int id, int up, int right, int down, int left);//���캯��


	/////////////////����������//////////////////
	//Cross& operator=(const Car& c);//���ƶ���
};

//��·��
class Road
{
public:
    int m_road_id;
	int m_index;

	int m_in_cross_index;//���·��������
	int m_out_cross_index;//����·��������

	int m_n_channel;//��������
    int m_is_duplex;//�Ƿ�˫��0��ʾ����1��ʾ˫��
    int m_v_road_max;
    int m_road_length;

	int m_cars_num_in_pos_road;//��ǰ��·����������������
	int m_cars_num_in_neg_road;//��ǰ��·����������������
	int m_cars_max_capacity;//��·�������������������������һ���ģ���buildgraph��ʼ��

	vector<list<int>> m_cars_index_in_road[2];//[0]��ʾ���򳵵�,[1]��ʾ���򳵵���vector������ʾ�೵����list˳���������ͬ����������ǰ���ϵ

	list<int> m_car_waiting_order[2];//��ǰ��·�ϣ��ȴ�״̬�������г����ȼ�;[0]��ʾ���򳵵�,[1]��ʾ���򳵵���
	
	///20190320 lzm add�������Ż��������̣�
	//ע���ʼ��ָ��
	vector<bool> m_channel_going[2];//true:�����ȴ�״̬��false:�����ս�״̬����һά��ʾ�������򣬵ڶ�ά��ʾ����
	bool m_direction_going[2];// true:��·�ȴ�״̬��false:��·�ս�״̬��


    ////////////////////����/////////////////////
    Road();//���캯����ע����ݵ�˫�򡢳�����·��
    ~Road();

    double m_congestion[2];//��·ӵ���ȣ�0-1
    
    void buildRoad(int r_id, int road_length, int v_road_max,int n_channel, int in_cross, int out_cross, int is_reverse);
	void buildRoad(int is_duplex, int n_channel);

	bool RemoveWaitingCarFromCarOrder(int car_index, int is_reverse);
	bool RemoveCarFromRoad(int car_index, int is_reverse,int channel);
	int GetCarOrderInChannel(int car_index, int is_reverse, int channel);//ûд
	int GetCarNumberInChannel(int is_reverse, int channel);//ûд
	list<int> GetBackListInChannel(int direction, int channel, int car_index);//��ȡ��ǰ����ͬ���������б�

	bool CheckIsFront(int direction, int channel, int car_index);//�жϵ�ǰ���ȵĳ��Ƿ�Ϊ��ǰ��

	/////////////////����������//////////////////
	//Road& operator=(const Road& c);//���ƶ���

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

	CarVector m_deadlock_carvec_t;//��ʱֻ�������ã�ÿ��·�ڵ��Ƚ����󣬼�¼���еȴ�״̬�ĳ�
	int n_car_waiting_before_adjust;//���ڱȽϵ���ǰ���Ƿ��г��ĵȴ�״̬�����仯
	//֮������int������Ϊ���ȴ�״̬�ĳ��������ӣ�ֻ����٣���·�ڣ����٣�����ڣ�����仯������ˣ�ֻ����١�
    ////////////////////����////////////////////
    TState();
    ~TState();
    //���캯��
    void buildT_State(int t, RoadVector roadvec_t,
                      CarVector carvec_t, CrossVector crossvec_t);
	void buildT_State();
	

	/////////////////������////////////////////
	void SortRoadByID();//����ID���򣬲�����m_index
	void SortCrossByID();//����ID���򣬲�����m_index
	void SortCarByID();//����ID���򣬲�����m_index


	vector<int> FindLauchCar(int t);//�ҵ����㷢���ĳ���
	int Launch(int ready_car_index, int t);//һ�������𲽵���
	//int Launch(int ready_car_index, const vector<int>& path, int t);//һ�������𲽵���
	TState TStateInitByPath(vector<int> car_index_vec, vector<vector<int>> cars_path, int t);//��ʼ��������������Ҫ������car_vec�Լ�ʱ��t������car_index_vec���Ĭ����car_id����


	void ResetCarTurning(vector<vector<int>> car_path, vector<int> car_index);//���ݸ������ֳ���·������������������Щ���Ĺ滮·��

	int UpdateTState(int t);//����t->t1ʱ��״̬
	int UpdateTState(vector<int> car_index_vec, int t, vector<int>& car_is_lauched);//��ʼ��������������Ҫ������car_vec�Լ�ʱ��t������car_index_vec���Ĭ����car_id����;��ʵ��TStateInitByPath��һ���ģ����������Ҹ��£����˸�����

	bool CheckAllCarReach();

	void InitCarOrderInRoad(int road_index);

	void SortCarOrderInRoad(list<int>& waiting_car_index_list);
	bool cmp_car_index(int c1_index, int c2_index);

	int UpdateCross(int t);
	void UpdateCrossOnce(int t);//ִ��һ��·�ڵ���
	void UpdateCrossCar(Cross& cross, Car& car, Road& from_road, Road& to_road);
	void UpdateCurrentPathIndex();

	bool UpdateRoadCar(list<int>& backward_car_list, int limit, bool former_car_going, bool former_car_is_across); //��������UpdateBackCarStateWhenNotPass-->UpdateBackCarStateAfterPass�滻
	int UpdateBackCarStateWhenNotPass(int car_index);//������·�����º�������״̬//��������UpdateBackCarStateAfterPass�滻
	int UpdateBackCarStateAfterPass(int road_index, int direction, int channel);//��·�����º�������״̬
	int CheckIsReverse(Cross cross/*�յ�cross*/, Car car);
	int CheckIsReverse(Cross cross/*�յ�cross*/, int road_index);

	bool CheckLocked();

	void ChangeIdToIndex();
	int GetRoadIndex(int road_id);
	int GetCarIndex(int car_id);
	int GetCrossIndex(int cross_id);
	int GetPreCar(int car_index);//Ѱ��ͬ����ǰ�����������ǰ��������ǰ��index�����������ǰ��������-1

	int FindStraightConflictRoad(Cross cross, int target_road_id);
	int FindLeftTurningConflictRoad(Cross cross, int target_road_id);
	bool CheckTurningConflict(Cross cross, int from_road_index, int to_road_index);


	int CheckTurning(Cross cross, int original_road_id, int target_road_id);
	int CheckTurning(Cross cross, Car car);


	int GetRoadFirstMovingCar(int road_index, int direction);

    //ȫͼ�����±����������ڸ��µ���
    vector<int> exist_cars_on_graph;

	//����·�ڵ�����ѭ������
	int CheckWaitingCar();
	int InitCrossPolling();//��ʼ����ѯ��������

	/////////////////����������//////////////////
	//TState& operator=(const TState& c);//���ƶ���

	/////////////////���Ժ���////////////////////
	void ChangePathIdToIndex(vector<int>& path);
	void ChangePathIdToIndex(vector<vector<int>>& paths);
	void SetPath(vector<vector<int>> car_path);
	void SetPath(vector<int> car_path, int car_index);

	//CZT Add  2019/03/26
	void WriteInfoINLog(string logpath);
	void ReadInfoINLog(string logpath);
	void T_WriteAndReadFile();

};

bool SolveMap(TState t0, int plan_time, int all_car_plan_time);//��t0ʱ��Ϊ����������ȫͼ����ĵ���ʱ�䣬������ʱ�䣨ע��Ƚ�Graph::routePlan��


#endif // STRUCT_H
