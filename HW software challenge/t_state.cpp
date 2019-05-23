#include"t_state.h"
#include <algorithm>
#include<iostream>
#include <string.h>

#include"test.h"
extern int all_cars_num;
using namespace std;


Car::Car()
{
    /*
     * lzm add 初始化标准 20190323
     * （1）所有的编号初始化为-1；
     * （2）时间初始化为0
     * （3）bool初始化为false
     * （4）vector初始化为空
     * （5）vector<vector>初始化需要对for+pushback保证每一层都是有对象的，防止直接访问报错
    */
    m_car_id=-1;
    m_index=-1;
    m_start_cross_index=-1;
    m_end_cross_index=-1;

    m_start_time_expect=0;
    m_start_time_infact=0;

    m_is_reach_end=false;
    m_reach_end_t=0;
    m_v_max=0;
    m_current_road=-1;
    m_current_v=0;
    m_current_is_reverse=0;
    m_current_channel=0;
    m_current_s0=0;
    m_expect_s0 = 0;
    m_move_capability=0;

    m_path.clear();
    m_current_path_index=-1;

    m_going=false;
}
Car::~Car()
{
}

bool Car::operator<(const Car& c) const//车辆道路内优先级排序比较器，注意要  分  不同向进行排序
{
    if (this->m_current_s0<c.m_current_s0)
    {
        return true;
    }
    else if (this->m_current_s0 == c.m_current_s0)
    {
        if (this->m_current_channel>c.m_current_channel)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
//Car& Car::operator=(const Car& c)//复制定义
//{
//	this->m_car_id = c.m_car_id;
//}


TState::TState()
{
    buildT_State();
}
TState::~TState()
{
}
Road::Road()
{
    m_road_id=-1;
    m_index=-1;

    m_in_cross_index=-1;
    m_out_cross_index=-1;
    m_n_channel=0;
    m_is_duplex=1;
    m_v_road_max=0;
    m_road_length=0;

    m_cars_num_in_pos_road = 0;
    m_cars_num_in_neg_road = 0;
    //    其余几个的初始化，见buildRoad
    //    m_cars_index_in_road
    //    m_car_waiting_order
    //    m_channel_going
    //    m_going
}
Road::~Road()
{
}
Cross::Cross()
{
    m_cross_id=-1;
    m_index=-1;
    memset(m_road_index,-1,4*sizeof(int));
}
Cross::~Cross()
{
}

void Road::buildRoad(int is_duplex, int n_channel)
{
    //20190322 lzm add
    if (is_duplex == 1)
    {
        list<int> no_car_in_channel;
        no_car_in_channel.clear();
        for (int i = 0; i < n_channel; i++)
        {
            //初始化n_channel个链表
            this->m_cars_index_in_road[0].push_back(no_car_in_channel);
            m_channel_going[0].push_back(true);

            this->m_cars_index_in_road[1].push_back(no_car_in_channel);
            m_channel_going[1].push_back(true);
        }

        m_car_waiting_order[0].clear();
        m_car_waiting_order[1].clear();

        m_direction_going[0] = true;
        m_direction_going[1] = true;

    }
    else if (is_duplex == 0)
    {
        list<int> no_car_in_channel;
        no_car_in_channel.clear();
        for (int i = 0; i < n_channel; i++)
        {
            //初始化n_channel个链表
            this->m_cars_index_in_road[0].push_back(no_car_in_channel);
            m_channel_going[0].push_back(true);
        }
        this->m_cars_index_in_road[1].clear();
        m_channel_going[1].clear();

        m_car_waiting_order[0].clear();
        m_car_waiting_order[1].clear();

        m_direction_going[0] = true;
        m_direction_going[1] = true;
    }
    else
    {
        //错误
        return;
    }
}
void Road::buildRoad(int r_id, int road_length, int v_road_max, int n_channel, int in_cross, int out_cross, int is_duplex)
{
    this->m_road_id = r_id;
    this->m_in_cross_index = in_cross;
    this->m_out_cross_index = out_cross;
    this->m_n_channel = n_channel;
    this->m_is_duplex = is_duplex;
    this->m_v_road_max = v_road_max;
    this->m_road_length = road_length;

    //20190322 lzm add
    buildRoad(is_duplex, n_channel);
}

void Car::buildCar(int id,int start_cross, int end_cross, int v_max, int start_time_expect)//11?ìoˉêy
{
    this->m_car_id = id;
    this->m_start_cross_index = start_cross;
    this->m_end_cross_index = end_cross;
    this->m_v_max = v_max;
    this->m_start_time_expect = start_time_expect;
    this->m_start_time_infact = start_time_expect;
}

void Cross::buildCross(int id, int up, int right, int down, int left)
{
    m_cross_id = id;

    //20190320 lzm modified: use array instead of 4 int
    //m_road1 = up;
    //m_road2 = right;
    //m_road3 = down;
    //m_road4 = left;

    m_road_index[0] = up;
    m_road_index[1] = right;
    m_road_index[2] = down;
    m_road_index[3] = left;
}
void TState::buildT_State()
{
    m_roadvec_t.clear();
    m_carvec_t.clear();
    m_crossvec_t.clear();

    n_car_waiting_before_adjust = 100000;
}
void TState::buildT_State(int t, RoadVector roadvec_t,
                          CarVector carvec_t, CrossVector crossvec_t)
{
    m_roadvec_t.clear();
    m_carvec_t.clear();
    m_crossvec_t.clear();

    m_roadvec_t.assign(roadvec_t.begin(), roadvec_t.end());
    m_carvec_t.assign(carvec_t.begin(), carvec_t.end());
    m_crossvec_t.assign(crossvec_t.begin(), crossvec_t.end());
}
void TState::SetPath(vector<int> car_path,int car_index)
{
    m_carvec_t[car_index].SetCarPath(car_path);
}
void TState::SetPath(vector<vector<int>> car_path)
{
    int n_car = m_carvec_t.size();
    for (int i = 0; i < n_car; i++)
    {
        m_carvec_t[i].SetCarPath(car_path[i]);
    }
}
void TState::ChangePathIdToIndex(vector<int>& path)
{
    int n_path = path.size();
    for (int i = 0; i < n_path; i++)
    {
        path[i] = GetRoadIndex(path[i]);
    }
}

void TState::ChangePathIdToIndex(vector<vector<int>>& paths)
{
    int n_path = paths.size();
    for (int i = 0; i < n_path; i++)
    {
        for (int j = 0; j < paths[i].size(); j++)
        {
            paths[i][j] = GetRoadIndex(paths[i][j]);
        }
    }
}
void Car::SetCarPath(const vector<int>& path)
{
    m_path.assign(path.cbegin(), path.cend());
}
//void Car::SetCarLauchTime(int t)
//{
//	m_start_time_expect = t;
//}
//int TState::Launch(int ready_car_index, const vector<int>& path, int t)//一辆发车起步调度
int TState::Launch(int ready_car_index, int t)//一辆发车起步调度
{
    /*函数逻辑：Launch
    输出说明：

    返回值说明：
    -1：堵塞，后车也不能进
    -2：没有path，异常错误
    0 ：前车移动状态为等待，可能是死锁了，暂定逻辑不发车
    1 ：正常
    （1）初始化车辆起步信息，填入Car数据结构
    m_start_time_infact=t
    m_current_road=path[m_current_path_index],m_current_path_index=0
    m_current_v=;可以不给值，在InitCarOrderInRoad会重新计算
    m_current_is_reverse：根据起步路口，定义车辆的行驶方向
    m_current_channel根据调度结束后的道路的vacancy再判断上路之后的车道
    m_current_s0=0
    （2）发车调度
    UpdateCrossCar
    */

    ///////////////////////////////////////////////////////////////////////////
    //（1）初始化车辆起步信息，填入Car数据结构
    Car& car = m_carvec_t[ready_car_index];

    //car.m_path.assign(path.begin(), path.end());//重定义全部路线，如果需要重定义部分路段逻辑还得再考虑
    if (car.m_path.size() == 0)
    {
        //是因为路径规划失败，还是起点就是终点所以没有规划道路？
        //为了避免误判，这里没有路径的车，都设置为到达目的地
        car.m_is_reach_end = true;
        car.m_reach_end_t = t;

        return -2;//没有path信息，无法发车
    }
    Road& road = m_roadvec_t[car.m_path[0]];//车取出当前道路

    car.m_current_path_index = 0;
    car.m_start_time_infact = t;
    car.m_current_road = car.m_path[0];
    car.m_current_v = min(road.m_v_road_max, car.m_v_max);;
    if (car.m_start_cross_index == road.m_in_cross_index)
    {
        //正向
        car.m_current_is_reverse = 0;
    }
    else
    {
        car.m_current_is_reverse = 1;
    }
    //car.m_current_channel;//等发车调度之后才能确定
    car.m_current_s0 = 0;
    car.m_move_capability = car.m_current_v;
    car.m_going = true;

    //（2）发车调度
    bool is_set = false;//默认放不进
    int vacancy = 0;//空位

    for (int i = 0; i<road.m_n_channel; i++)
    {
        if (road.m_cars_index_in_road[car.m_current_is_reverse][i].size() == 0)
        {
            //前方没有车
            is_set = true;
            vacancy = road.m_road_length;
            int expect_pos = car.m_move_capability;
            if (expect_pos <= vacancy)
            {
                //不过路口
                car.m_going = false;
                car.m_current_s0 = expect_pos;
                car.m_move_capability = 0;
                car.m_current_channel = i;

                //更新道路包含的车
                road.m_cars_index_in_road[car.m_current_is_reverse][i].push_back(car.m_index);
            }
            else
            {
                //发车就过路口
                //这里是异常处理，这的处理逻辑有问题，因为这个逻辑允许发车之后存在等待状态，但这会可能会导致后续判断死锁错误——如果死锁判断放在发车之后的话——当然现在是放在路口调度之后，所以这个函数之后没有判断死锁，暂时不会有问题。
                car.m_going = true;
                car.m_move_capability -= vacancy;//消耗的行车能力
                car.m_current_s0 = vacancy;
                car.m_current_channel = i;

                //更新道路包含的车
                road.m_cars_index_in_road[car.m_current_is_reverse][i].push_back(car.m_index);
                road.m_car_waiting_order[car.m_current_is_reverse].push_back(car.m_index);


                //不用判断转向冲突了，直接跑就行
                //因为路上的车辆都已经跑完了

                //注意：由于UpdateCrossCar会更新道路对车的包含关系，如果在UpdateCrossCar成功过路，那么
                if (car.m_path.size() > car.m_current_path_index + 1)
                {
                    //存在下一段路
                    if (car.m_current_is_reverse == 0)
                    {
                        UpdateCrossCar(m_crossvec_t[road.m_out_cross_index], car, m_roadvec_t[car.m_path[car.m_current_path_index]], m_roadvec_t[car.m_path[car.m_current_path_index + 1]]);
                    }
                    else
                    {
                        UpdateCrossCar(m_crossvec_t[road.m_in_cross_index], car, m_roadvec_t[car.m_path[car.m_current_path_index]], m_roadvec_t[car.m_path[car.m_current_path_index + 1]]);
                    }

                }
                else
                {
                    //发车就到达终点啊
                    car.m_is_reach_end = true;
                    car.m_reach_end_t = t;

                    //为了调试方便，虽然已经到达目的地，也设置完整信息
                    car.m_going = true;
                    car.m_move_capability -= vacancy;//消耗的行车能力
                    car.m_current_s0 = vacancy;
                    car.m_current_channel = i;
                }

            }

            break;
        }
        else
        {
            const Car& former_car = m_carvec_t[road.m_cars_index_in_road[car.m_current_is_reverse/*考虑同向车道上的车*/][i].back()];//如果车道没有车，会导致back报错
            if (former_car.m_going)
            {
                //异常错误
                //可能是死锁了，暂定不能发车
                //这里的断点不要删除
                return 0;
            }

            vacancy = former_car.m_current_s0 - 1;
            if (vacancy>0)
            {
                is_set = true;
                car.m_current_channel = i;

                if (car.m_move_capability <= vacancy)
                {
                    //允许最大s2
                    //更新car位置
                    car.m_going = false;
                    car.m_current_s0 = car.m_move_capability;
                    car.m_move_capability = 0;

                }
                else
                {
                    car.m_going = false;
                    car.m_current_s0 = former_car.m_current_s0 - 1;
                    car.m_move_capability -= former_car.m_current_s0 - 1;
                }

                //更新道路包含的车
                road.m_cars_index_in_road[car.m_current_is_reverse][i].push_back(car.m_index);

                break;
            }
        }
    }
    if (is_set)
    {
        return 1;
    }
    else
    {
        //所有车道都没有空位
        car.m_going = false;
        //如果发生堵塞，则将后续优先级较低的车的运动状态都设置为终结
        car.ReSetCarAfterLauchFail();
        return -1;
    }
}
void Car::ReSetCarAfterLauchFail()
{
    //重置出发失败的车辆
    m_start_time_infact++;
}
TState TState::TStateInitByPath(vector<int> car_index_vec,vector<vector<int>> cars_path, int t)//初始化发车，传入需要发动的car_vec以及时间t，这里car_index_vec存放默认以car_id升序
{
    //函数功能：
    //（1）加上t时刻出发的车路径，生成一个新的TState，初始化t时刻行驶状态；
    //（2）最开始初始化时，t从1开始

    //t:this
    //t+1:生成

    TState new_state = (*this);

    //car_id小的先发车
    for (int i = 0; i<car_index_vec.size(); i++)
    {
        int is_lauch = new_state.Launch(car_index_vec[i], t);
        if (is_lauch != 1)
        {
            break;
        }
    }
    return new_state;
}

int TState::UpdateTState(vector<int> car_index_vec, int t, vector<int>& car_is_not_lauched)//初始化发车，传入需要发动的car_vec以及时间t，这里car_index_vec存放默认以car_id升序
{
    //函数功能：
    //（1）加上t时刻出发的车路径，生成一个新的TState，初始化t时刻行驶状态；
    //（2）最开始初始化时，t从1开始
    //（3）注意发车的car在函数外要保证序号小优先

    int is_all_lauched = 1;
    for (int i = 0; i<car_index_vec.size(); i++)
    {
        int is_lauched = Launch(car_index_vec[i], t);
        if (is_lauched != 1)
        {
            is_all_lauched = 0;
            car_is_not_lauched.push_back(car_index_vec[i]);
        }
    }
    return is_all_lauched;
}
void TState::ResetCarTurning(vector<vector<int>> car_path, vector<int> car_index)//根据给定部分车的路径方案，重新设置这些车的规划路线
{
    /*
    //ResetCarTurning：
    //后续如果需要实时规划，只需要修改path的后续路段即可，比较方便
    */
}
vector<int> TState::FindLauchCar(int t)//找到到点发车的车辆
{
    int n_car = m_carvec_t.size();
    vector<int> ready_to_lauch_cars(0);
    for (int i = 0; i < n_car; i++)
    {
        Car& car = m_carvec_t[i];
        if (car.m_start_time_infact == t&&car.m_start_time_infact >= car.m_start_time_expect)
        {
            //判断发车方向是在道路正向还是反向
            int dir_in_next_road=0;
            Road &next_road=m_roadvec_t[car.m_path[0]];
            if(next_road.m_in_cross_index==car.m_start_cross_index)
                dir_in_next_road=0;//正
            else
                dir_in_next_road=1;//反

            if(next_road.m_congestion[dir_in_next_road]>0.8)
            {
                car.ReSetCarAfterLauchFail();//下个路径拥堵，就不发车了
                continue;
            }

            int b=i%1000;
            if(all_cars_num>1800&&b)//如果路上车多了，n辆取余数，减小发车量
            {
                //cout<<"all_cars_num="<<all_cars_num<<endl;
                car.ReSetCarAfterLauchFail();//路上车太多也不发车
                continue;
            }
            //delay start car
            if(t<5&&i%2)
            {
                car.ReSetCarAfterLauchFail();//开始发车发少一点
                continue;
            }

            ready_to_lauch_cars.push_back(car.m_index);//按照升序发车
        }
    }
    //    int n_car = m_carvec_t.size();
    //    vector<int> ready_to_lauch_cars(0);
    //    for (int i = 0; i < n_car; i++)
    //    {
    //        const Car& car = m_carvec_t[i];
    //        if (car.m_start_time_infact == t&&car.m_start_time_infact >= car.m_start_time_expect)
    //        {
    //            ready_to_lauch_cars.push_back(car.m_index);//按照升序发车
    //        }
    //    }

    return ready_to_lauch_cars;
}
int TState::UpdateTState(int t)//更新t->t1时刻状态
{
    /*
    //返回值说明：
    //	-1：死锁
    //	0 ：当前时刻跑完
    //	1 ：所有车到达终点
    //UpdateTState：
    //	（1）道路内的调度和过路口的路内优先级InitCarOrderInRoad
    //	（2）循环更新每个路口的过路车辆位置，直到没有车发生位置更新，结束
    //	（3）判断有没有车处于等待状态，有，就说明发生了死锁——互相等待，谁也更新不了位置；为等待状态的车辆即为死锁车
    //	（4）判断是否所有车都到达终点UpdateIsReachFlag
    */
    //返回值说明：
    //-1：表示死锁
    //0：表示一切正常，但还有车没有到达目的地
    //1：表示全部车到达目的地


    //（1）道路内的调度和过路口的路内优先级InitCarOrderInRoad
    for (int i = 0; i < m_roadvec_t.size(); i++)
    {
        InitCarOrderInRoad(i);
    }
    //（2）循环更新每个路口的过路车辆位置，直到没有车发生位置更新，结束
    int is_all_car_stop = UpdateCross(t);

    //（3）判断有没有车处于等待状态，有，就说明发生了死锁——互相等待，谁也更新不了位置；为等待状态的车辆即为死锁车
    //if (CheckLocked())
    if (is_all_car_stop==-1)
    {
        return -1;//死锁结束
    }
    else
    {
        //等待能跑的车都跑完之后，再发新车
        //找到到点要出发的车
        vector<int> ready_to_lauch = FindLauchCar(t);
        //根据新发车更新TState
        vector<int> cars_is_not_lauched(0);
        int is_all_lauched= UpdateTState(ready_to_lauch, t, cars_is_not_lauched);

        if (is_all_lauched == 0)
        {
            return 0;//部分车延迟发车，无需继续判断到达终点
        }
    }

    //（4）判断是否所有车都到达终点
    if (CheckAllCarReach())
    {
        return 1;
    }
    return 0;
}
bool Road::CheckIsFront(int direction, int channel, int car_index)//判断当前调度的车是否为最前车
{
    if (m_cars_index_in_road[direction][channel].size() != 0 && m_cars_index_in_road[direction][channel].front() == car_index)
    {
        return true;
    }
    else
    {
        //（1）道路上意外没有车，怎么会对该道路判断car_index？很奇怪的情况，返回false作为警示
        //（2）不是最前车，返回false
        return false;
    }
}

bool cmp_road(Road a, Road b)
{
    return a.m_road_id < b.m_road_id;//升序
}
void TState::SortRoadByID()//根据ID排序，并设置m_index
{
    std::sort(m_roadvec_t.begin(), m_roadvec_t.end(), cmp_road);
    int n_road = m_roadvec_t.size();
    for (int i = 0; i < n_road; i++)
    {
        m_roadvec_t[i].m_index = i;
    }
}
bool cmp_cross(Cross a, Cross b)
{
    return a.m_cross_id < b.m_cross_id;//升序
}
void TState::SortCrossByID()//根据ID排序，并设置m_index
{
    std::sort(m_crossvec_t.begin(), m_crossvec_t.end(), cmp_cross);

    int n_cross = m_crossvec_t.size();
    for (int i = 0; i < n_cross; i++)
    {
        m_crossvec_t[i].m_index = i;
    }
}
bool cmp_car(Car a, Car b)
{
    return a.m_car_id < b.m_car_id;//升序
}
void TState::SortCarByID()//根据ID排序，并设置m_index
{
    std::sort(m_carvec_t.begin(), m_carvec_t.end(), cmp_car);

    int n_car = m_carvec_t.size();
    for (int i = 0; i < n_car; i++)
    {
        m_carvec_t[i].m_index = i;
    }
}

int TState::InitCrossPolling()//初始化轮询跳出条件
{
    n_car_waiting_before_adjust = 100000;
    return CheckWaitingCar();
}
int TState::CheckWaitingCar()
{
    //函数说明：检查死锁或者轮询路口死循环
    //（1）每次路口轮询之前，检查各路口、道路、方向、车道所在车的等待状态，
    //（2）并更新路口、道路、方向、车道的等待标记
    //（3）每条路的等待车辆数量标记如果不变，说明死锁了；否则更新

    //返回值说明：
    //（4）如果全部都终止，返回1，
    //（5）如果轮询路口后，没有变化，则认为死锁终止,返回-1
    //（6）一般情况，返回0

    //注意：这里不是对所有car遍历，因为有些car可能还没有上路，当然判断车到达、出发还是挺简单的。
    //基本逻辑：
    //（1）要么等待，要么移动之后终止，没有第三状态。也不存在过了路口之后成为等待状态的情况。
    //（2）当所有路的等待状态变量清空，或者在一次轮询路口后，没有变化，则认为死锁终止
    int n_cross = m_crossvec_t.size();
    vector<int > cross_roads_id;
    int road_index = -1;
    int direction = -1;

    m_deadlock_carvec_t.clear();

    int current_waiting_number = 0;//初始化当前图等待状态标记
    for (int i = 0; i < n_cross; i++)
    {
        //按升序遍历道路ID
        Cross& cross = m_crossvec_t[i];
        cross.m_going = false;
        cross_roads_id.clear();
        for (int j = 0; j < 4; j++)
        {
            if (cross.m_road_index[j] == -1)
                continue;
            cross_roads_id.push_back(m_roadvec_t[cross.m_road_index[j]].m_road_id);
        }
        std::sort(cross_roads_id.begin(), cross_roads_id.end());
        for (int j = 0; j < cross_roads_id.size(); j++)
        {
            road_index = GetRoadIndex(cross_roads_id[j]);
            if (road_index < 0)
                continue;
            Road& road = m_roadvec_t[road_index];

            if (road.m_out_cross_index == cross.m_index)//Get Direction
            {
                direction = 0;
            }
            else
            {
                direction = 1;
            }
            road.m_direction_going[direction] = false;
            if (road.m_is_duplex == 0&&direction==1)//单向道
            {
                //就只有direction=0
                //继续访问就会越界

                //那就不能进入
                continue;
            }
            for (int k = 0; k < road.m_n_channel; k++)
            {
                list<int > car_in_channel = road.m_cars_index_in_road[direction][k];
                road.m_channel_going[direction][k] = false;
                for (list<int >::iterator it = car_in_channel.begin(); it != car_in_channel.end(); it++)
                {
                    Car& car = m_carvec_t[*it];

                    if (car.m_going)
                    {
                        //is_channel_going = true;//有车是等待状态，那么车道就不是终结状态
                        road.m_channel_going[direction][k] = true;
                        road.m_direction_going[direction] = true;
                        cross.m_going = true;

                        current_waiting_number++;//全局等待状态车辆个数标记

                        m_deadlock_carvec_t.push_back(car);
                    }
                }
            }
        }
    }

    if (current_waiting_number == 0)//当所有路的等待状态变量清空，轮询结束
    {
        n_car_waiting_before_adjust = 0;
        return 1;//无需继续轮询
    }
    else if (current_waiting_number < n_car_waiting_before_adjust)//在一次轮询路口后，等待状态的车数量减少
    {
        //当状态发生更新，current_waiting_number会逐渐减小
        n_car_waiting_before_adjust = current_waiting_number;
        return 0;//正常
    }
    else//在一次轮询路口后，没有变化，则认为死锁终止
    {
        //没有发生状态变化,发生死锁
        return -1;
    }
}
bool TState::CheckTurningConflict(Cross cross, int from_road_index, int to_road_index)
{
    int turning_direction = CheckTurning(cross, from_road_index, to_road_index);
    bool is_conflict = false;

    int straight_conflict_road = -1;
    int leftturning_conflict_road = -1;
    switch (turning_direction)
    {

    case 1://a）车辆直走
        break;

    case 2://b）车辆左转
        straight_conflict_road = FindStraightConflictRoad(cross, to_road_index);
        if (straight_conflict_road != -1)
        {
            //无冲突
            is_conflict = true;
        }
        break;

    case 3://c）车辆右转
        straight_conflict_road = FindStraightConflictRoad(cross, to_road_index);
        if (straight_conflict_road == -1)
        {
            //无冲突
            leftturning_conflict_road = FindLeftTurningConflictRoad(cross, to_road_index);
            if (leftturning_conflict_road != -1)
            {
                //无冲突
                is_conflict = true;
            }
        }
        else
        {
            //有冲突
            is_conflict = true;
        }
        break;
    default:
        //异常错误
        is_conflict = true;//容错处理：如果发现有未预料的转向结果，则认为当前车被冲突
        break;
    }

    return is_conflict;
}
void TState::UpdateCrossOnce(int t)//执行一次路口调度
{
    int n_cross = m_crossvec_t.size();
    vector<int > cross_roads_id;
    int road_index = -1;
    int direction = -1;
    for (int i = 0; i < n_cross; i++)
    {
        //按升序遍历道路ID
        Cross& cross = m_crossvec_t[i];

        if (!(cross.m_going))
        {
            continue;
        }
        cross_roads_id.clear();
        for (int j = 0; j < 4; j++)
        {
            if (cross.m_road_index[j] == -1)
                continue;
            cross_roads_id.push_back(m_roadvec_t[cross.m_road_index[j]].m_road_id);
        }
        std::sort(cross_roads_id.begin(), cross_roads_id.end());//对道路ID升序轮询
        for (int j = 0; j < cross_roads_id.size(); j++)
        {
            road_index = GetRoadIndex(cross_roads_id[j]);
            if (road_index < 0)
                continue;
            Road& road = m_roadvec_t[road_index];

            direction = -1;
            int direction = 0;
            if (road.m_out_cross_index == cross.m_index)
            {
                direction = 0;
            }
            else if (road.m_in_cross_index == cross.m_index)
            {
                direction = 1;
            }
            else
            {
                //异常错误！
                continue;
            }

            if (road.m_is_duplex == 0 && direction == 1)//单向道
            {
                //就只有direction=0,继续访问就会越界

                //那就不用继续遍历车道
                continue;
            }
            for (int k = 0; k < road.m_n_channel; k++)
            {
                //为了避免在过路之后，删除了道路上的部分车，导致it遍历错误，这里复制一个road用于遍历，注意这里不是引用
                list<int> car_order(0);
                car_order.assign(road.m_cars_index_in_road[direction][k].begin(), road.m_cars_index_in_road[direction][k].end());
                for (list<int >::iterator it = car_order.begin(); it != car_order.end(); it++)
                {
                    Car& car = m_carvec_t[*it];//GetRoadFirstCar

                    if (!(car.m_going))//已经处理过的车
                    {
                        //车为终止状态
                        //这种情况应该是前方原来有等待状态的车，在遍历的过程中，已经过了路口
                        //在前方车过了路口之后，会执行路内调度，将能够移动到终止状态的车先跑了
                        //所以会有部分车在出路口遍历之前变为终止状态！
                        continue;
                    }
                    if (car.m_is_reach_end)
                    {
                        //这里可能出错了
                        //为什么会有车在路口调度之前变为到达终点？
                        continue;
                    }

                    //容错处理：车道有前车阻挡
                    //检查当前车的同车道前向是否有阻挡
                    int pre_car_index = GetPreCar(car.m_index);
                    if (pre_car_index != -1)
                    {
                        //前车有阻挡
                        //应该是当前车道没有更新状态的原因
                        UpdateBackCarStateAfterPass(road.m_index, direction, car.m_channel_befor_adjust);//m_channel_befor_adjust注意看一下初始化？
                        break;
                    }

                    //终点调度
                    if (car.m_end_cross_index == cross.m_index)
                    {
                        //到达终点，判断是否能够走过路口即可，判定车子是否能够到达终点
                        //注意：这里到达终点的调度，优先级最高，在转向冲突判定之前——是自加规则
                        int s1 = road.m_road_length - car.m_current_s0;
                        if (car.m_move_capability > s1)
                        {
                            car.m_going = false;
                            car.m_is_reach_end = true;

                            car.m_reach_end_t = t;
                            car.m_current_s0 = road.m_road_length;

                            //从road.m_car_waiting_order[direction]中删除
                            road.RemoveWaitingCarFromCarOrder(car.m_index, direction);
                            road.RemoveCarFromRoad(car.m_index, direction, car.m_channel_befor_adjust);

                            //后车位置更新
                            if (road.m_cars_index_in_road[direction][car.m_channel_befor_adjust].size()>0)
                            {
                                UpdateBackCarStateAfterPass(road.m_index, direction, car.m_channel_befor_adjust);
                            }
                        }

                        continue;
                    }

                    bool check_conflict = CheckTurningConflict(cross, car.m_path[car.m_current_path_index], car.m_path[car.m_current_path_index + 1]);

                    if (check_conflict)
                    {
                        //跳出当前道路，轮询其他道路
                        break;
                    }
                    else
                    {
                        UpdateCrossCar(cross, car, m_roadvec_t[car.m_path[car.m_current_path_index]], m_roadvec_t[car.m_path[car.m_current_path_index + 1]]); //如果成功插入到新的道路，在UpdateCrossCar中完成道路车辆集合的更新

                        if (car.m_going)
                        {
                            check_conflict = true;
                            //跳出当前道路，轮询其他道路
                            break;
                        }
                        else
                        {
                            if (road.m_cars_index_in_road[direction][car.m_channel_befor_adjust].size()>0)
                            {
                                UpdateBackCarStateAfterPass(road.m_index, direction, car.m_channel_befor_adjust);
                            }
                        }

                    }
                }
            }
        }
    }


    return ;
}
int TState::UpdateCross(int t)
{
    //函数作用：循环更新每个路口的过路车辆位置，直到没有车发生位置更新，结束：

    int graph_going = InitCrossPolling();
    if (graph_going == 1)//结束轮询
        return 1;
    else if (graph_going == -1)
        return -1;


    while (1)
    {

        UpdateCrossOnce(t);

        graph_going = CheckWaitingCar();
        if (graph_going == 1)//结束轮询
            return 1;
        else if (graph_going == -1)
            return -1;
    }



    return true;

}

bool TState::CheckLocked()
{
    bool is_locked = false;
    for (int i = 0; i<m_carvec_t.size(); i++)
    {
        const Car& car = m_carvec_t[i];
        if (car.m_going)
        {
            is_locked = true;
            break;//如果需要输出死锁车就不要break；
        }
    }
    return is_locked;
}
bool TState::CheckAllCarReach()
{
    bool is_reach = true;
    for (int i = 0; i < m_carvec_t.size(); i++)
    {
        const Car& car = m_carvec_t[i];
        if (!(car.m_is_reach_end))
        {
            is_reach = false;
            break;//如果需要输出死锁车就不要break；
        }
    }
    return is_reach;
}
list<int> Road::GetBackListInChannel(int direction,int channel, int car_index)//获取当前车的同车道后向列表
{
    list<int> back_list;
    list<int>::reverse_iterator rit = m_cars_index_in_road[direction][channel].rbegin();
    for (;;)
    {
        if (*rit == car_index || rit == m_cars_index_in_road[direction][channel].rend())
        {
            break;
        }
        else
        {
            back_list.push_front(*rit);
            rit++;
        }
    }
    return back_list;
}
void TState::UpdateCrossCar(Cross& cross, Car& car, Road& from_road, Road& to_road)
{
    //UpdateCrossCar要保证：
    //（1）能过路的，能跑为终止状态才动，否则不能动，比如前方道路需要进入的车道上，末尾车为等待状态，当前车在前进方向上会与之冲突，此时不能跑，不冲突可以跑。
    //（2）不能过路的，跑到当前路的最大位置，变为终止。
    //（3）整理
    //	1），可以过路，移动后为终止状态；
    //	2），可以过路，但前车卡位，为等待状态；
    //	3），不可过路，前车为等待状态，发生冲突；
    //	4），不可过路，前方道路全部被终止状态的车堵塞。
    //（4）按照（1）（2），只有1、4可以移动，之后变为终止；其余都不能动，按冲突处理。
    //（5）简化规则处理：
    //	5.1）UpdateCrossCar
    //		a）判断是否能够过路，
    //		b）如果能够过路，判断进入车道末尾车是否为等待状态，如果是转d）
    //		c）如果不能过路，按路内调度处理
    //		d）如果进入车道末尾车有等待状态，本车为等待，返回冲突；否则本车前行至终止状态
    //
    //返回处理：判断当前车是否为等待即可

    /////////////////////开始编程///////////////////////////////
    //
    //容错处理：车的等待状态或者行动能力发生逻辑冲突
    if (!(car.m_going) || car.m_move_capability <= 0)
    {
        car.m_going = false;

        //很奇怪的错误，注意调试
        //错误信息输出
        cout << "UpdateCrossCar ERROR _0_\t:" << endl;
        cout << VName(cross.m_index) << ":" << cross.m_index << endl;
        cout << VName(car.m_index) << ":" << car.m_index << endl;
        cout << VName(from_road.m_index) << ":" << from_road.m_index << endl;
        cout << VName(to_road.m_index) << ":" << to_road.m_index << endl;
        return;
    }

    int direction1 = 0;//默认正向
    if (cross.m_index == from_road.m_out_cross_index)//正向
    {
        direction1 = 0;

    }
    else
    {
        direction1 = 1;
    }

    int direction2 = 0;//默认正向
    if (cross.m_index == to_road.m_in_cross_index)//正向
    {
        direction2 = 0;

    }
    else
    {
        direction2 = 1;
    }

    //注意：这里的容错应该判断一下:
    //(1)当前车道是否有车阻碍
    //(2)是否单行道的同向车道
    //为了简便，这里只判读，进入的道路是否单行同向——因为在路口调度时，应该就判断过有没有前车阻碍
    //只要不是胡乱调用该函数，应该还是能偷偷懒的啦
    if (to_road.m_is_duplex == 0 && direction2 == 1)
    {
        //单行反向
        return;
    }

    //容错判断：计算s1，判断是否满足过路条件
    int s1 = from_road.m_road_length - car.m_current_s0;//注意：认为在等待状态car是不会发生位置变动的
    //否则应该使用：from_road.m_road_length - (car.m_current_s0 + car.m_move_capability-car.m_current_v);
    //容错处理：
    if (s1 < 0)
    {
        //很奇怪的错误，注意调试
        //返回前先矫正当前车的错误数据
        car.m_current_s0 = from_road.m_road_length;

        //错误信息输出
        cout << "UpdateCrossCar ERROR _s1_\t:" << endl;
        cout << VName(cross.m_index) << ":" << cross.m_index << endl;
        cout << VName(car.m_index) << ":" << car.m_index << endl;
        cout << VName(from_road.m_index) << ":" << from_road.m_index << endl;
        cout << VName(to_road.m_index) << ":" << to_road.m_index << endl;
        return;
    }
    if (s1 >= car.m_move_capability)
    {
        //异常错误，居然不可以过路，不是应该在路内调度里面就丢掉了吗？很奇怪

        car.m_going = false;
        //从优先级容器中删除
        from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);

        car.m_current_s0 = car.m_current_s0 + car.m_move_capability;
        car.m_move_capability = 0;
        return;
    }

    //继续检查s2的条件
    int v2 = min(to_road.m_v_road_max, car.m_v_max); //进入新道路时，车的车速
    int s2 = max(v2 - s1, 0); //新进入车可行驶最大距离

    if (s2 <= 0)
    {
        //车辆不能过路口

        //设置车辆运动终止状态
        car.m_going = false;
        from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);

        car.m_move_capability -= (from_road.m_road_length - car.m_current_s0);
        car.m_current_s0 = from_road.m_road_length;

        return;
    }

    //有能力过路口，以下看有没有空位

    bool is_set = false;//默认放不进
    int vacancy = 0;//空位

    for (int i = 0; i < to_road.m_n_channel; i++)
    {
        if (to_road.m_cars_index_in_road[direction2][i].size() == 0)
        {
            //车道上没有车
            vacancy = to_road.m_road_length;

            is_set = true;

            if (s2 <= vacancy)
            {
                //允许最大s2
                //更新car位置
                car.m_going = false;
                car.m_current_s0 = s2;
                car.m_move_capability = 0;
            }
            else
            {
                //容错处理：自加规则，不让车继续跑下一段道路
                //错误信息输出
                cout << "UpdateCrossCar Warning _s2_\t:" << endl;
                cout << VName(cross.m_index) << ":" << cross.m_index << endl;
                cout << VName(car.m_index) << ":" << car.m_index << endl;
                cout << VName(from_road.m_index) << ":" << from_road.m_index << endl;
                cout << VName(to_road.m_index) << ":" << to_road.m_index << endl;

                //过了上一个路口，看样子还得再过路口呢！
                //在函数外，需判断所在道路是否变化
                car.m_going = false;
                car.m_current_s0 = vacancy;
                car.m_move_capability = s2 - vacancy;
            }


            from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);
            from_road.RemoveCarFromRoad(car.m_index, direction1, car.m_current_channel);

            car.m_current_road = to_road.m_index;
            car.m_current_is_reverse = direction2;
            car.m_current_channel = i;
            car.m_current_v = v2;

            //更新道路包含的车
            to_road.m_cars_index_in_road[direction2][i].push_back(car.m_index);
            break;
        }
        else
        {
            //车道上有车
            const Car& former_car = m_carvec_t[to_road.m_cars_index_in_road[direction2][i].back()];

            if (former_car.m_going == true)
            {
                return;//检测到需要等前车先走的情况，直接返回
            }

            vacancy = former_car.m_current_s0 - 1;

            if (vacancy>0)
            {
                is_set = true;
                if (s2 <= vacancy)
                {
                    //允许最大s2
                    //更新car位置
                    car.m_going = false;
                    car.m_current_s0 = s2;
                    car.m_move_capability = 0;
                }
                else
                {
                    car.m_going = false;
                    car.m_current_s0 = vacancy;
                    car.m_move_capability = s2 - vacancy;
                }
                from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);
                from_road.RemoveCarFromRoad(car.m_index, direction1, car.m_current_channel);

                car.m_current_road = to_road.m_index;
                car.m_current_is_reverse = direction2;
                car.m_current_channel = i;
                car.m_current_v = v2;

                //更新道路包含的车
                to_road.m_cars_index_in_road[direction2][i].push_back(car.m_index);
                break;
            }
            else
            {
                continue;
            }
        }
    }

    if (is_set)
    {
        car.m_current_path_index++;//已无需继续保存t时刻的初始所在道路
        return ;
    }
    else
    {
        //所有车道都没有空位，而且都是终止状态
        car.m_going = false;

        //将car从优先级容器中删除
        from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);

        car.m_move_capability -= from_road.m_road_length - car.m_current_s0;
        car.m_current_s0 = from_road.m_road_length;

        return;
    }
}
int TState::GetPreCar(int car_index)//寻找同车道前车，如果存在前车，返回前车index，如果不存在前车，返回-1
{
    const Car& car = m_carvec_t[car_index];
    const Road& road = m_roadvec_t[car.m_current_road];
    for (list<int>::const_iterator it = road.m_cars_index_in_road[car.m_current_is_reverse][car.m_current_channel].cbegin();
         it != road.m_cars_index_in_road[car.m_current_is_reverse][car.m_current_channel].cend(); it++)
    {
        if (*it == car_index)
        {
            if (it != road.m_cars_index_in_road[car.m_current_is_reverse][car.m_current_channel].cbegin())
            {
                //不是第一个，就必然存在前车
                return *(--it);
            }
            else
            {
                return -1;
            }
        }
    }
    return -1;
}

int TState::UpdateBackCarStateAfterPass(int road_index, int direction, int channel)//过路，更新后续车的状态
{
    //注意：更新时，如果发现部分车辆由等待变为终止，要从出路口优先级顺序容器m_car_waiting_order把车index删了
    //否则可能会造成：前车卡位终止状态，后车却总是要往前跑，导致假死锁
    list<int> back_list;
    back_list.clear();
    if (channel >= m_roadvec_t[road_index].m_cars_index_in_road[direction].size())
    {
        //应该是程序错误，越界的容错处理
        return -1;
    }
    back_list = m_roadvec_t[road_index].m_cars_index_in_road[direction][channel];

    if (back_list.size() == 0)
    {
        //可能是car在函数外就被删除了，导致车道为空
        return -1;
    }

    int pre_car_index = -1;
    int is_update = -1;
    for (list<int>::iterator it = back_list.begin(); it != back_list.end(); it++)
    {
        if (m_carvec_t[*it].m_going)
        {
            pre_car_index = GetPreCar(*it);
            if (pre_car_index == -1)
            {
                //最前车,不存在前车
                if (m_carvec_t[*it].m_current_s0 < m_roadvec_t[road_index].m_road_length)
                {
                    //可以跑
                    if (m_carvec_t[*it].m_move_capability > 0)
                    {
                        is_update = 1;
                        if (m_carvec_t[*it].m_current_s0 + m_carvec_t[*it].m_move_capability > m_roadvec_t[road_index].m_road_length)
                        {
                            //不是终止状态不更新位置
                            //m_carvec_t[*it].m_move_capability -= (m_roadvec_t[road_index].m_road_length - m_carvec_t[*it].m_current_s0);
                            //m_carvec_t[*it].m_current_s0 = m_roadvec_t[road_index].m_road_length;
                        }
                        else
                        {
                            m_carvec_t[*it].m_current_s0 = m_carvec_t[*it].m_current_s0 + m_carvec_t[*it].m_move_capability;
                            m_carvec_t[*it].m_move_capability = 0;
                            m_carvec_t[*it].m_going = false;

                            m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
                        }
                    }
                    else
                    {
                        //没有行动能力
                        //很奇怪的情况，为什么等待状态的，移动能力为0，注意调试
                        if (m_carvec_t[*it].m_going)
                        {
                            m_carvec_t[*it].m_going = false;//只修改等待状态，无需调度位置
                            if (is_update<0)
                                is_update = 0;//发生了更新
                            m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
                        }
                        else
                        {
                            //分支错误，与前面逻辑冲突
                            cout << "Error UpdateBackCarStateWhenNotPass 21:" << "Car Index:" << *it << ".m_going" << "is wrong" << endl;
                        }
                    }
                }
                else
                {
                    //只能是m_carvec_t[*it].m_current_s0 == m_roadvec_t[road_index].m_road_length
                    //无法更新位置，状态由出路口决定，这里无法处理
                }
            }
            else
            {
                //存在前车
                if (m_carvec_t[*it].m_current_s0 >= m_carvec_t[pre_car_index].m_current_s0)
                {
                    //异常，注意调试
                    cout << "Error UpdateBackCarStateWhenNotPass 1:" << "Car Index:" << *it << "conflicts with" << "Pre Car Index:" << pre_car_index << endl;
                }
                //本函数真正要跑的是这一段逻辑，其他都是容错
                if (m_carvec_t[*it].m_current_s0 + m_carvec_t[*it].m_move_capability < m_carvec_t[pre_car_index].m_current_s0)//有足够间距可以跑
                {
                    if (m_carvec_t[*it].m_move_capability > 0)//有移动能力
                    {
                        is_update = 1;
                        m_carvec_t[*it].m_current_s0 = m_carvec_t[*it].m_current_s0 + m_carvec_t[*it].m_move_capability;
                        m_carvec_t[*it].m_move_capability = 0;
                        m_carvec_t[*it].m_going = false;

                        m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
                    }
                    else
                    {
                        //没有行动能力
                        //很奇怪的情况，为什么等待状态的，移动能力为0，注意调试
                        if (m_carvec_t[*it].m_going)
                        {
                            m_carvec_t[*it].m_going = false;//只修改等待状态，无需调度位置
                            m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
                            if (is_update<0)
                                is_update = 0;//发生了更新
                        }
                        else
                        {
                            //分支错误，与前面逻辑冲突
                            cout << "Error UpdateBackCarStateWhenNotPass 22:" << "Car Index:" << *it << ".m_going" << "is wrong" << endl;
                        }
                    }
                }
                else
                {
                    //没有足够间距可以跑
                    if (!(m_carvec_t[pre_car_index].m_going))
                    {
                        //前车为终止状态，更新后车位置
                        if (m_carvec_t[*it].m_going)
                        {
                            m_carvec_t[*it].m_going = false;
                            if (is_update<0)
                                is_update = 0;

                            m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);

                            int going = m_carvec_t[pre_car_index].m_current_s0 - 1 - m_carvec_t[*it].m_current_s0;
                            if (going > 0)
                            {
                                is_update = 1;
                                m_carvec_t[*it].m_move_capability -= going;
                                m_carvec_t[*it].m_current_s0 = m_carvec_t[pre_car_index].m_current_s0 - 1;
                            }
                            else
                            {
                                //车子没有移动空间
                            }
                        }
                        else
                        {
                            //分支错误，与前面逻辑冲突
                            cout << "Error UpdateBackCarStateWhenNotPass 23:" << "Car Index:" << *it << ".m_going" << "is wrong" << endl;
                        }
                    }
                }
            }
        }
    }

    return is_update;
}
int TState::GetRoadIndex(int road_id)
{
    //查询
    for (int i = 0; i < m_roadvec_t.size(); i++)
    {
        if (m_roadvec_t[i].m_road_id == road_id)
        {
            return i;
        }
    }
    return -1;
}
int TState::GetCarIndex(int car_id)
{
    //查询
    for (int i = 0; i < m_carvec_t.size(); i++)
    {
        if (m_carvec_t[i].m_car_id == car_id)
        {
            return i;
        }
    }
    return -1;
}
int TState::GetCrossIndex(int cross_id)
{
    //查询
    for (int i = 0; i < m_crossvec_t.size(); i++)
    {
        if (m_crossvec_t[i].m_cross_id == cross_id)
        {
            return i;
        }
    }
    return -1;
}

void TState::ChangeIdToIndex()
{
    int n_road = m_roadvec_t.size();
    int n_cross = m_crossvec_t.size();
    int n_car = m_carvec_t.size();

    for (int i = 0; i < n_road; i++)
    {
        m_roadvec_t[i].m_in_cross_index = GetCrossIndex(m_roadvec_t[i].m_in_cross_index);
        m_roadvec_t[i].m_out_cross_index = GetCrossIndex(m_roadvec_t[i].m_out_cross_index);
    }
    for (int i = 0; i < n_cross; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m_crossvec_t[i].m_road_index[j] = GetRoadIndex(m_crossvec_t[i].m_road_index[j]);
        }
    }

    for (int i = 0; i < n_car; i++)
    {
        m_carvec_t[i].m_start_cross_index = GetCrossIndex(m_carvec_t[i].m_start_cross_index);
        m_carvec_t[i].m_end_cross_index = GetCrossIndex(m_carvec_t[i].m_end_cross_index);
    }
}

void TState::InitCarOrderInRoad(int road_index)
{
    //InitCarOrderInRoad:
    //		1）遍历双向各车道，从道路的最前头开始
    //		2）计算最大速度下的预期位置expect_pos
    //		3）判断运动终止状态，计算m_expect_s0
    //		4）将等待状态的车放入等待列表car_order
    //			只有两种情况，一种是等待过路的，一种是被等待状态的前车卡位的
    //		5）对等待列表中的车，按照位置靠前优先、车道小优先，进行排序。排序结果即为路内优先级。
    //		6）更新路内车辆的位置，尽量腾出空位，使路口调度时有更多的车可以进入道路

    //		☆☆☆处理顺序：预期位置>标记状态>出口优先级>位置更新

    //		PS：采用排序，就无需考虑前面放进来的等待状态的车的位置、车道比较。一次性排序完成优先级。
    //		PS：后续路内调度逻辑见：UpdateRoadCar
    Road& road = m_roadvec_t[road_index];
    road.m_car_waiting_order[0].clear();
    road.m_car_waiting_order[1].clear();

    for (int d = 0; d <= road.m_is_duplex; d++)
    {
        for (int channel = 0; channel<road.m_n_channel; channel++)
        {
            if (road.m_cars_index_in_road[d][channel].size() == 0)
            {
                continue;
            }
            //注意车道入车出车的顺序，别用错了
            //入车push_back
            //出车pop_front
            list<int>::iterator it = road.m_cars_index_in_road[d][channel].begin();
            list<int>::iterator it_begin = it;
            list<int>::iterator it_end = road.m_cars_index_in_road[d][channel].end();

            for (; it != it_end; it++)
            {
                // carvector[*it]正向遍历
                // carvector[*rit]反向遍历

                Car& car = m_carvec_t[*it];

                //初始化t时刻行动能力
                car.m_current_v = min(road.m_v_road_max, car.m_v_max);
                car.m_move_capability = car.m_current_v;
                car.m_channel_befor_adjust = channel;

                int expect_pos = car.m_current_s0 + car.m_move_capability;
                if (it == it_begin)
                {
                    //当前车道最前车
                    if (expect_pos <= road.m_road_length)
                    {
                        //不过路口
                        car.m_going = false;
                        car.m_current_s0 = expect_pos;
                    }
                    else
                    {
                        //等待过路
                        car.m_going = true;
                        road.m_car_waiting_order[d].push_back(car.m_index);
                    }
                }
                else
                {
                    list<int>::iterator pre_it = it;
                    pre_it--;
                    //当前车前面的车
                    Car& pre_car = m_carvec_t[*pre_it];
                    if (expect_pos<pre_car.m_current_s0)//不被前车卡位
                    {
                        car.m_going = false;
                        car.m_current_s0 = expect_pos;
                    }
                    else
                    {
                        //被前车卡位,前车终止
                        if (pre_car.m_going == false)
                        {
                            car.m_going = false;
                            car.m_current_s0 = pre_car.m_current_s0 - 1;
                        }
                        else//被前车卡位,前车等待
                        {
                            car.m_going = true;
                            //将等待的车放入等待列表
                            road.m_car_waiting_order[d].push_back(car.m_index);
                        }
                    }
                }
            }
        }
        SortCarOrderInRoad(road.m_car_waiting_order[d]);//设置路内车辆过路口的优先级
    }
}
void Car::UpdateCarCurrentPosition()
{
    m_current_s0 = m_expect_s0;
    m_expect_s0 = 0;
}
bool SolveMap(TState t0, int plan_time, int all_car_plan_time)
{
    //函数功能：计算调度时间、运行时间。
    //可能与Graph::routePlan重复。
    //但计算出来的时间可以用于优化，可能存在迭代


    //注意：由于初始化没做完，未初始化序号
    return true;
}

bool TState::cmp_car_index(int c1_index, int c2_index)
{
    const Car& car1 = m_carvec_t[c1_index];
    const Car& car2 = m_carvec_t[c2_index];
    return car1<car2;//Car的<比较符定义见Car数据结构
}
void TState::SortCarOrderInRoad(list<int>& waiting_car_index_list)
{
    //按降序排列，第一个优先级最高
    //使用pop_front

    //写一个简单的冒泡排序
    list<int>::iterator it_i = waiting_car_index_list.begin();
    list<int>::iterator it_j = waiting_car_index_list.begin();
    list<int>::iterator it_begin = waiting_car_index_list.begin();
    list<int>::iterator it_end = waiting_car_index_list.end();

    int temp = 0;
    for (; it_i != it_end; ++it_i)
    {
        for (; it_j != it_end; ++it_j)
        {
            if (cmp_car_index(*it_i,*it_j))
            {
                //前向较大
                temp = *it_i;
                *it_i = *it_j;
                *it_j = temp;
            }
        }
    }
}
bool Road::RemoveCarFromRoad(int car_index, int is_reverse, int channel)
{
    for (list<int>::iterator it = m_cars_index_in_road[is_reverse][channel].begin(); it != m_cars_index_in_road[is_reverse][channel].end(); it++)
    {
        if (*it == car_index)
        {
            m_cars_index_in_road[is_reverse][channel].erase(it);
            return true;
        }
    }
    return false;
}
bool Road::RemoveWaitingCarFromCarOrder(int car_index,int is_reverse)
{
    for (list<int>::iterator it = m_car_waiting_order[is_reverse].begin(); it != m_car_waiting_order[is_reverse].end(); it++)
    {
        if (*it == car_index)
        {
            m_car_waiting_order[is_reverse].erase(it);
            return true;
        }
    }
    return false;
}

int TState::CheckTurning(Cross cross, int original_road_index, int target_road_index)
{
    /*
    //判断转弯方向：当车辆要过路口时，需要先设置车辆的转弯方向，以便于车辆过路口的优先级判断。

    //难点：怎么判断车辆是左转/右转/直行？
    //注意：路口道路txt中的，顺序可能并不总是上右下左的。见P7 cross.txt

    //准则：路口连接的道路，总是顺时针的
    */
    int flag = 0;//0：异常错误
    int original_road_order = 0;
    for (int i = 0; i < 4; i++)
    {
        if (cross.m_road_index[i] == original_road_index)
        {
            original_road_order = i;
            break;
        }
    }


    int target_road_order = 0;
    for (int i = 0; i < 4; i++)
    {
        if (cross.m_road_index[i] == target_road_index)
        {
            target_road_order = i;
            break;
        }
    }

    int n = original_road_order - target_road_order;
    switch (n)
    {
    case 2:
    case -2:
        //直行
        flag = 1;
        break;

    case 3:
    case -1:
        //左转
        flag = 2;
        break;

    case 1:
    case -3:
        //右转
        flag = 3;
        break;
    }
    return flag;
}
int TState::CheckTurning(Cross cross, Car car)
{
    if (car.m_current_path_index + 1 >= car.m_path.size())
    {
        //说明已经是最后路了
        //相当于直行
        return 1;
    }
    return CheckTurning(cross, car.m_path[car.m_current_path_index], car.m_path[car.m_current_path_index + 1]);
}
int TState::CheckIsReverse(Cross cross, Car car)
{
    if (m_roadvec_t[car.m_current_road].m_out_cross_index == cross.m_index)
    {
        return 0;//正向
    }
    else if (m_roadvec_t[car.m_current_road].m_in_cross_index == cross.m_index)
    {
        return 1;//反向
    }
    else
    {
        return -1;
    }
}

int TState::CheckIsReverse(Cross cross, int road_index)
{
    if (m_roadvec_t[road_index].m_out_cross_index == cross.m_index)
    {
        return 0;//正向
    }
    else if (m_roadvec_t[road_index].m_in_cross_index == cross.m_index)
    {
        return 1;//反向
    }
    else
    {
        return -1;
    }
}
int TState::FindStraightConflictRoad(Cross cross, int target_road_index)
{
    /*
    函数功能：在当前路口中，对目标路的直向冲突的道路编号，获取失败，返回 - 1
    相当于判断直行冲突
            |
    target_road_index
            |
            ^
            |
    --car>------
            |
            ^
    straight conflict
            |
    当车辆需要左转进入上方道路时，需要先获取直走冲突道路，判断直向冲突道路的第一优先级车辆是否直走，有，就是冲突，返回冲突道路ID；否，就是不冲突，返回 - 1。
    */

    //（1）先根据路口和目标路，找出直行车道
    //（2）然后判断直行车道上的第一优先级的车是否直行

    int i=0;
    for (i = 0; i < 4; i++)
    {
        if (target_road_index == cross.m_road_index[i])
        {
            break;
        }
    }

    if (4 == i) //没找到，异常
    {
        //异常退出
        return -1;
    }

    int reverse_road_index = cross.m_road_index[(i + 2) % 4];//excellent job!
    if (-1 == reverse_road_index)  //没路
    {
        return -1;
    }

    int reverse_road_direction = CheckIsReverse(cross, reverse_road_index);
    if (m_roadvec_t[reverse_road_index].m_car_waiting_order[reverse_road_direction].size()==0)
    {
        //没有处于等待状态的车
        //单行道也在这个判断中被过滤
        return -1;
    }

    int conflict_car_index = GetRoadFirstMovingCar(reverse_road_index, reverse_road_direction);

    if (conflict_car_index == -1)
    {
        return -1;
    }
    if (CheckTurning(cross, m_carvec_t[conflict_car_index]) == 1)
    {
        //直走
        return reverse_road_index;
    }
    else
    {
        return -1;
    }
}

int TState::FindLeftTurningConflictRoad(Cross cross, int target_road_index)
{
    /*
    函数功能：在当前路口中，对目标路的左转方向冲突的道路编号，获取失败，返回-1
    相当于判断左转冲突
                            |
                    target_road_index
                            |
                            ^
                            |
    left turning conflict>-----<car waiting for right turning
                            |
                            ^
                            |
    当车辆需要右转进入上方道路时，需要先获取左转向的冲突道路，判断左转向冲突道路的第一优先级车辆是否左转，有，就是冲突，返回冲突道路ID；否，就是不冲突，返回-1。
    */
    int i;
    for (i = 0; i < 4; i++)
    {
        if (target_road_index == cross.m_road_index[i])
        {
            break;
        }
    }

    if (4 == i) //没找到，异常
    {
        //异常退出
        return -1;
    }

    int left_turning_road_index = cross.m_road_index[(i + 3) % 4];//excellent job!
    if (-1 == left_turning_road_index)  //没路
    {
        return -1;
    }

    int reverse_road_direction = CheckIsReverse(cross, left_turning_road_index);
    if (m_roadvec_t[left_turning_road_index].m_car_waiting_order[reverse_road_direction].size()==0)
    {
        //没有处于等待状态的车
        //单行道也在这个判断中被过滤
        return -1;
    }
    int conflict_car_index = GetRoadFirstMovingCar(left_turning_road_index,reverse_road_direction);

    if (conflict_car_index == -1)
    {
        return -1;
    }
    if (CheckTurning(cross, m_carvec_t[conflict_car_index]) == 2)
    {
        //左转
        return left_turning_road_index;
    }
    else
    {
        return -1;
    }
}
int TState::GetRoadFirstMovingCar(int road_index, int direction)
{
    list<int > temp_list;//先复制一个序列
    temp_list.clear();
    temp_list.assign(m_roadvec_t[road_index].m_car_waiting_order[direction].begin(), m_roadvec_t[road_index].m_car_waiting_order[direction].end());

    for (list<int>::iterator it = temp_list.begin(); it != temp_list.end(); it++)
    {
        if (m_carvec_t[*it].m_going==true)
        {
            return *it;
        }
        else
        {
            //暂时不知道为什么其他地方没有把前向已经处理的m_going删除
            //m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
        }
    }

    return -1;
}

vector<string> getSegmentINString(string str)
{
    int  Num_of_commas = 0;
    string temp_str;
    vector<string> vt;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == ',')
        {
            vt.push_back(temp_str);
            temp_str.clear();
        }
        else
        {
            temp_str += str[i];
        }
    }
    return vt;
}

void TState::ReadInfoINLog(string logpath)
{
    ifstream fin(logpath);
    if (!fin) //读取不成功
    {
        cout << "读取 log 文件不成功" << endl;
        return;
    }

    //TState t_state;
    m_carvec_t.clear();
    m_roadvec_t.clear();
    m_crossvec_t.clear();


    int car_num = -1;
    int cross_num = -1;
    int road_num = -1;
    bool isReadCar = false;
    bool isReadRoad = false;
    bool isReadCross = false;
    string str;
    while ( !fin.eof() )
    {
        getline(fin, str);

        if (str == "CAR")
        {
            ++car_num;
            isReadCar = true;
            //cout << "car_num:" << car_num << endl;
        }
        if (str == "ROAD")
        {
            ++road_num;
            isReadRoad = true;
        }
        if (str == "CROSS")
        {
            ++cross_num;
            isReadCross = true;
        }

        if (isReadCar == true)
        {
            string temp_str;
            int Num_of_commas = 0;
            int Num_of_vars = 0;

            getline(fin, str); //读取变量个数

            vector<string> ret_vt = getSegmentINString(str);
            Num_of_vars = stoi(ret_vt[1]);

            Car car;

            for (int i = 1; i <= Num_of_vars; i++)
            {
                getline(fin, str);
                ret_vt = getSegmentINString(str);

                switch (i)
                {
                case 1:
                    car.m_car_id = stoi(ret_vt[1]);
                    break;
                case 2:
                    car.m_index = stoi(ret_vt[1]);
                    break;
                case 3:
                    car.m_start_cross_index = stoi(ret_vt[1]);
                    break;
                case 4:
                    car.m_end_cross_index = stoi(ret_vt[1]);
                    break;
                case 5:
                    car.m_start_time_expect = stoi(ret_vt[1]);
                    break;
                case 6:
                    car.m_start_time_infact = stoi(ret_vt[1]);
                    break;
                case 7:
                    car.m_is_reach_end = stoi(ret_vt[1]) == 0 ? false : true;
                    break;
                case 8:
                    car.m_reach_end_t = stoi(ret_vt[1]);
                    break;
                case 9:
                    car.m_v_max = stoi(ret_vt[1]);
                    break;
                case 10:
                    car.m_current_road = stoi(ret_vt[1]);
                    break;
                case 11:
                    car.m_current_v = stoi(ret_vt[1]);
                    break;
                case 12:
                    car.m_current_is_reverse = stoi(ret_vt[1]);
                    break;
                case 13:
                    car.m_channel_befor_adjust = stoi(ret_vt[1]);
                    break;
                case 14:
                    car.m_current_channel = stoi(ret_vt[1]);
                    break;
                case 15:
                    car.m_current_s0 = stoi(ret_vt[1]);
                    break;
                case 16:
                    car.m_expect_s0 = stoi(ret_vt[1]);
                    break;
                case 17:
                    car.m_move_capability = stoi(ret_vt[1]);
                    break;
                case 18:
                    for (int j = 1; j < ret_vt.size(); j++)
                    {
                        car.m_path.push_back(stoi(ret_vt[j]));
                    }

                    break;
                case 19:
                    car.m_current_path_index = stoi(ret_vt[1]);
                    break;
                case 20:
                    car.m_going = (stoi(ret_vt[1]) == 1 ? true : false);
                    break;
                case 21:
                    car.in_positive_road = (stoi(ret_vt[1]) == 1 ? true : false);
                    break;

                default:
                    cout << "error" << endl;
                }
            }
            m_carvec_t.push_back(car);
        }
        else if (isReadRoad == true)
        {
            //cout << "In Road" << endl;

            string temp_str;
            int Num_of_commas = 0;
            int Num_of_vars = 0;

            getline(fin, str); //读取变量个数

            vector<string> ret_vt = getSegmentINString(str);
            Num_of_vars = stoi(ret_vt[1]);

            Road road;

            for (int i = 1; i <= Num_of_vars; i++)
            {
                getline(fin, str);
                ret_vt = getSegmentINString(str);
                vector<list<int>> vt;
                int numoflist;
                int num;
                int bias;
                list<int> list1;
                vector<bool> temp_channel_going;

                //读取基本信息，然后buildroad
                switch (i)
                {
                case 1:
                    road.m_road_id = stoi(ret_vt[1]);
                    break;
                case 2:
                    road.m_index = stoi(ret_vt[1]);
                    break;
                case 3:
                    road.m_in_cross_index = stoi(ret_vt[1]);
                    break;
                case 4:
                    road.m_out_cross_index = stoi(ret_vt[1]);
                    break;
                case 5:
                    road.m_n_channel = stoi(ret_vt[1]);
                    break;
                case 6:
                    road.m_is_duplex = stoi(ret_vt[1]);
                    break;
                case 7:
                    road.m_v_road_max = stoi(ret_vt[1]);
                    break;
                case 8:
                    road.m_road_length = stoi(ret_vt[1]);
                    break;


                case 9:
                    road.m_cars_num_in_pos_road = stoi(ret_vt[1]);
                    break;
                case 10:
                    road.m_cars_num_in_neg_road = stoi(ret_vt[1]);
                    break;
                case 11:
                    road.m_cars_max_capacity = stoi(ret_vt[1]);
                    break;
                case 12://m_cars_index_in_road_0
                    //vector<list<int>> vt;
                    numoflist = stoi(ret_vt[1]);//5,0,0,0,0,0,
                    num = 0;
                    bias = 2;
                    for (int i = 0; i < numoflist; i++)
                    {
                        num = stoi(ret_vt[bias]);//车道上车的数量
                        list1.clear();
                        if (num > 0)
                        {
                            for (int j = bias + 1; j < num + bias + 1; j++)
                            {
                                list1.push_back(stoi(ret_vt[j]));
                            }
                        }
                        bias = bias + num + 1;
                        road.m_cars_index_in_road[0].push_back(list1);
                    }
                    //road.m_cars_index_in_road[0] = vt;

                    break;
                case 13://m_cars_index_in_road_1
                    numoflist = stoi(ret_vt[1]);//5,0,0,0,0,0,
                    num = 0;
                    bias = 2;
                    for (int i = 0; i < numoflist; i++)
                    {
                        num = stoi(ret_vt[bias]);//车道上车的数量
                        list1.clear();
                        if (num > 0)
                        {
                            for (int j = bias + 1; j < num + bias + 1; j++)
                            {
                                list1.push_back(stoi(ret_vt[j]));
                            }
                        }
                        bias = bias + num + 1;
                        road.m_cars_index_in_road[1].push_back(list1);
                    }

                    break;
                case 14://m_car_waiting_order_0
                    num = stoi(ret_vt[1]);
                    if (num > 0)
                    {
                        for (int i = 2; i < 2 + num; i++)
                        {
                            road.m_car_waiting_order[0].push_back(stoi(ret_vt[i]));
                        }
                    }

                    break;
                case 15://m_car_waiting_order_1
                    num = stoi(ret_vt[1]);
                    if (num > 0)
                    {
                        for (int i = 2; i < 2 + num; i++)
                        {
                            road.m_car_waiting_order[1].push_back(stoi(ret_vt[i]));
                        }
                    }

                    break;
                case 16:
                    //vector<bool> temp_channel_going;
                    num = stoi(ret_vt[1]);
                    for (int i = 2; i < 2 + num; i++)
                    {
                        road.m_channel_going[0].push_back((stoi(ret_vt[i]) == 1 ? true : false));
                    }
                    //road.m_channel_going[0] = temp_channel_going;

                    break;
                case 17:
                    //vector<bool> temp_channel_going;
                    temp_channel_going.clear();
                    num = stoi(ret_vt[1]);
                    for (int i = 2; i < 2 + num; i++)
                    {
                        road.m_channel_going[1].push_back((stoi(ret_vt[i]) == 1 ? true : false));
                    }
                    //road.m_channel_going[1] = temp_channel_going;

                    break;
                case 18:

                    road.m_direction_going[0] = (stoi(ret_vt[1]) == 1 ? true : false);
                    road.m_direction_going[1] = (stoi(ret_vt[2]) == 1 ? true : false);

                    break;
                default:
                    cout << "error" << endl;
                }
            }


            m_roadvec_t.push_back(road);

        }
        else if (isReadCross == true)
        {
            //cout << "In Cross" << endl;

            string temp_str;
            int Num_of_commas = 0;
            int Num_of_vars = 0;

            getline(fin, str); //读取变量个数

            vector<string> ret_vt = getSegmentINString(str);
            Num_of_vars = stoi(ret_vt[1]);

            Cross cross;
            for (int i = 1; i <= 4; i++)
            {
                getline(fin, str);
                ret_vt = getSegmentINString(str);

                switch (i)
                {
                case 1:
                    cross.m_cross_id = stoi(ret_vt[1]);
                    break;
                case 2:
                    cross.m_index = stoi(ret_vt[1]);
                    break;
                case 3:
                    cross.m_road_index[0] = stoi(ret_vt[1]);
                    cross.m_road_index[1] = stoi(ret_vt[2]);
                    cross.m_road_index[2] = stoi(ret_vt[3]);
                    cross.m_road_index[3] = stoi(ret_vt[4]);
                    break;
                case 4:
                    cross.m_going = (stoi(ret_vt[1]) == 1 ? true : false);
                    break;
                default:
                    cout << "error" << endl;
                }

            }

            m_crossvec_t.push_back(cross);
            //cout << "CROSS" << endl;
            //cout << "number_of_variables," << 4 << "," << endl //写入变量个数
            //	<< "m_cross_id," << cross.m_cross_id << "," << endl
            //	<< "m_index," << cross.m_index << "," << endl

            //	<< "m_road_index," << cross.m_road_index[0] << ","
            //	<< cross.m_road_index[1] << ","
            //	<< cross.m_road_index[2] << ","
            //	<< cross.m_road_index[3] << "," << endl

            //	<< "m_going," << (cross.m_going == true ? 1 : 0) << "," << endl;

        }

        isReadCar = false;
        isReadRoad = false;
        isReadCross = false;

    }

    fin.close();
}

void TState::WriteInfoINLog(string logpath)
{
    ofstream fout(logpath, ios::trunc);
    if (!fout) //创建不成功
    {
        cout << "创建 log 文件不成功" << endl;
        return;
    }


    for (int i = 0; i < m_carvec_t.size(); ++i)
    {
        //写入 Class Car的变量
        fout << "CAR" << endl;
        fout << "number_of_variables," << 21 << "," << endl //写入变量个数
             << "m_car_id," << m_carvec_t[i].m_car_id << "," << endl
             << "m_index," << m_carvec_t[i].m_index << "," << endl
             << "m_start_cross_index," << m_carvec_t[i].m_start_cross_index << "," << endl
             << "m_end_cross_index," << m_carvec_t[i].m_end_cross_index << "," << endl
             << "m_start_time_expect," << m_carvec_t[i].m_start_time_expect << "," << endl
             << "m_start_time_infact," << m_carvec_t[i].m_start_time_infact << "," << endl
             << "m_is_reach_end," << m_carvec_t[i].m_is_reach_end << "," << endl
             << "m_reach_end_t," << m_carvec_t[i].m_reach_end_t << "," << endl
             << "m_v_max," << m_carvec_t[i].m_v_max << "," << endl
             << "m_current_road," << m_carvec_t[i].m_current_road << "," << endl
             << "m_current_v," << m_carvec_t[i].m_current_v << "," << endl
             << "m_current_is_reverse," << m_carvec_t[i].m_current_is_reverse << "," << endl
             << "m_channel_befor_adjust," << m_carvec_t[i].m_channel_befor_adjust << "," << endl
             << "m_current_channel," << m_carvec_t[i].m_current_channel << "," << endl
             << "m_current_s0," << m_carvec_t[i].m_current_s0 << "," << endl
             << "m_expect_s0," << m_carvec_t[i].m_expect_s0 << "," << endl
             << "m_move_capability," << m_carvec_t[i].m_move_capability << "," << endl;

        fout << "m_path,";
        if (m_carvec_t[i].m_path.size() == 0)
        {
            fout << -10 << ",";
        }
        for (int j = 0;j < m_carvec_t[i].m_path.size();++j)
        {
            fout << m_carvec_t[i].m_path[j] << ",";
        }
        fout << endl;


        fout << "m_current_path_index," << m_carvec_t[i].m_current_path_index << "," << endl
             << "m_going," << (m_carvec_t[i].m_going == true ? 1 : 0) << "," << endl
             << "in_positive_road," << (m_carvec_t[i].in_positive_road == true ? 1 : 0) << "," << endl;
    }


    for (int i = 0; i < m_roadvec_t.size(); ++i)
    {
        //写入 Class Road的变量
        fout << "ROAD" << endl;
        fout << "number_of_variables," << 18 << "," << endl //写入变量个数
             << "m_road_id," << m_roadvec_t[i].m_road_id << "," << endl
             << "m_index," << m_roadvec_t[i].m_index << "," << endl
             << "m_in_cross_index," << m_roadvec_t[i].m_in_cross_index << "," << endl
             << "m_out_cross_index," << m_roadvec_t[i].m_out_cross_index << "," << endl
             << "m_n_channel," << m_roadvec_t[i].m_n_channel << "," << endl
             << "m_is_duplex," << m_roadvec_t[i].m_is_duplex << "," << endl
             << "m_v_road_max," << m_roadvec_t[i].m_v_road_max << "," << endl
             << "m_road_length," << m_roadvec_t[i].m_road_length << "," << endl
             << "m_cars_num_in_pos_road," << m_roadvec_t[i].m_cars_num_in_pos_road << "," << endl
             << "m_cars_num_in_neg_road," << m_roadvec_t[i].m_cars_num_in_neg_road << "," << endl
             << "m_cars_max_capacity," << m_roadvec_t[i].m_cars_max_capacity << "," << endl;

        vector<list<int>> vt1 = m_roadvec_t[i].m_cars_index_in_road[0]; //一个vector放一行
        fout << "m_cars_index_in_road_0,";
        fout << vt1.size() << ",";  //vector中list的个数
        for (int j = 0; j < vt1.size(); ++j) //总共有多少个list
        {
            fout << vt1[j].size() << ",";  //list中int的个数

            while (!vt1[j].empty())
            {
                fout << vt1[j].front() << ",";
                vt1[j].pop_front();
                //cout << "fuck:" << vt1[i].front() << endl;
            }

        }
        fout << endl;

        vt1 = m_roadvec_t[i].m_cars_index_in_road[1]; //一个vector放一行
        fout << "m_cars_index_in_road_1,";
        fout << vt1.size() << ",";  //vector中list的个数
        for (int j = 0; j < vt1.size(); ++j) //总共有多少个list
        {
            fout << vt1[j].size() << ",";  //list中int的个数

            while (!vt1[j].empty())
            {
                fout << vt1[j].front() << ",";
                vt1[j].pop_front();
                //cout << "fuck:" << vt1[i].front() << endl;
            }

        }
        fout << endl;

        list<int> list1 = m_roadvec_t[i].m_car_waiting_order[0];
        fout << "m_car_waiting_order_0,";
        fout << list1.size() << ",";  //list中int的个数

        while (!list1.empty())
        {
            fout << list1.front() << ",";
            list1.pop_front();
        }
        fout << endl;

        list1 = m_roadvec_t[i].m_car_waiting_order[1];
        fout << "m_car_waiting_order_1,";
        fout << list1.size() << ",";  //list中int的个数

        while (!list1.empty())
        {
            fout << list1.front() << ",";
            list1.pop_front();
        }
        fout << endl;

        vector<bool> vector1 = m_roadvec_t[i].m_channel_going[0];
        fout << "m_channel_going_0,";
        fout << vector1.size() << ",";  //vector1中int的个数
        for (int j = 0; j < vector1.size(); j++)
        {
            fout << (vector1[j] == true ? 1 : 0)  << ",";
        }
        fout << endl;

        vector1 = m_roadvec_t[i].m_channel_going[1];
        fout << "m_channel_going_1,";
        fout << vector1.size() << ",";  //vector1中int的个数
        for (int j = 0; j < vector1.size(); j++)
        {
            fout << (vector1[j] == true ? 1 : 0) << ",";
        }
        fout << endl;

        fout << "m_direction_going," << (m_roadvec_t[i].m_direction_going[0] == true ? 1 : 0) << ","
                                                                                              << (m_roadvec_t[i].m_direction_going[1] == true ? 1 : 0) << "," << endl;

    }


    for (int i = 0; i < m_crossvec_t.size(); ++i)
    {
        //写入 Class Cross的变量
        fout << "CROSS" << endl;
        fout << "number_of_variables," << 4 << "," << endl //写入变量个数
             << "m_cross_id," << m_crossvec_t[i].m_cross_id << "," << endl
             << "m_index," << m_crossvec_t[i].m_index << "," << endl

             << "m_road_index," << m_crossvec_t[i].m_road_index[0] << ","
             << m_crossvec_t[i].m_road_index[1] << ","
             << m_crossvec_t[i].m_road_index[2] << ","
             << m_crossvec_t[i].m_road_index[3] << "," << endl

             << "m_going," << (m_crossvec_t[i].m_going == true ? 1 : 0) << "," << endl;
    }


    fout.close();  // 执行完操作后关闭文件句柄
}

