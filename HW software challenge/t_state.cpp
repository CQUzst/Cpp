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
     * lzm add ��ʼ����׼ 20190323
     * ��1�����еı�ų�ʼ��Ϊ-1��
     * ��2��ʱ���ʼ��Ϊ0
     * ��3��bool��ʼ��Ϊfalse
     * ��4��vector��ʼ��Ϊ��
     * ��5��vector<vector>��ʼ����Ҫ��for+pushback��֤ÿһ�㶼���ж���ģ���ֱֹ�ӷ��ʱ���
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

bool Car::operator<(const Car& c) const//������·�����ȼ�����Ƚ�����ע��Ҫ  ��  ��ͬ���������
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
//Car& Car::operator=(const Car& c)//���ƶ���
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
    //    ���༸���ĳ�ʼ������buildRoad
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
            //��ʼ��n_channel������
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
            //��ʼ��n_channel������
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
        //����
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

void Car::buildCar(int id,int start_cross, int end_cross, int v_max, int start_time_expect)//11?��o����y
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
//int TState::Launch(int ready_car_index, const vector<int>& path, int t)//һ�������𲽵���
int TState::Launch(int ready_car_index, int t)//һ�������𲽵���
{
    /*�����߼���Launch
    ���˵����

    ����ֵ˵����
    -1����������Ҳ���ܽ�
    -2��û��path���쳣����
    0 ��ǰ���ƶ�״̬Ϊ�ȴ��������������ˣ��ݶ��߼�������
    1 ������
    ��1����ʼ����������Ϣ������Car���ݽṹ
    m_start_time_infact=t
    m_current_road=path[m_current_path_index],m_current_path_index=0
    m_current_v=;���Բ���ֵ����InitCarOrderInRoad�����¼���
    m_current_is_reverse��������·�ڣ����峵������ʻ����
    m_current_channel���ݵ��Ƚ�����ĵ�·��vacancy���ж���·֮��ĳ���
    m_current_s0=0
    ��2����������
    UpdateCrossCar
    */

    ///////////////////////////////////////////////////////////////////////////
    //��1����ʼ����������Ϣ������Car���ݽṹ
    Car& car = m_carvec_t[ready_car_index];

    //car.m_path.assign(path.begin(), path.end());//�ض���ȫ��·�ߣ������Ҫ�ض��岿��·���߼������ٿ���
    if (car.m_path.size() == 0)
    {
        //����Ϊ·���滮ʧ�ܣ������������յ�����û�й滮��·��
        //Ϊ�˱������У�����û��·���ĳ���������Ϊ����Ŀ�ĵ�
        car.m_is_reach_end = true;
        car.m_reach_end_t = t;

        return -2;//û��path��Ϣ���޷�����
    }
    Road& road = m_roadvec_t[car.m_path[0]];//��ȡ����ǰ��·

    car.m_current_path_index = 0;
    car.m_start_time_infact = t;
    car.m_current_road = car.m_path[0];
    car.m_current_v = min(road.m_v_road_max, car.m_v_max);;
    if (car.m_start_cross_index == road.m_in_cross_index)
    {
        //����
        car.m_current_is_reverse = 0;
    }
    else
    {
        car.m_current_is_reverse = 1;
    }
    //car.m_current_channel;//�ȷ�������֮�����ȷ��
    car.m_current_s0 = 0;
    car.m_move_capability = car.m_current_v;
    car.m_going = true;

    //��2����������
    bool is_set = false;//Ĭ�ϷŲ���
    int vacancy = 0;//��λ

    for (int i = 0; i<road.m_n_channel; i++)
    {
        if (road.m_cars_index_in_road[car.m_current_is_reverse][i].size() == 0)
        {
            //ǰ��û�г�
            is_set = true;
            vacancy = road.m_road_length;
            int expect_pos = car.m_move_capability;
            if (expect_pos <= vacancy)
            {
                //����·��
                car.m_going = false;
                car.m_current_s0 = expect_pos;
                car.m_move_capability = 0;
                car.m_current_channel = i;

                //���µ�·�����ĳ�
                road.m_cars_index_in_road[car.m_current_is_reverse][i].push_back(car.m_index);
            }
            else
            {
                //�����͹�·��
                //�������쳣������Ĵ����߼������⣬��Ϊ����߼�������֮����ڵȴ�״̬���������ܻᵼ�º����ж��������󡪡���������жϷ��ڷ���֮��Ļ�������Ȼ�����Ƿ���·�ڵ���֮�������������֮��û���ж���������ʱ���������⡣
                car.m_going = true;
                car.m_move_capability -= vacancy;//���ĵ��г�����
                car.m_current_s0 = vacancy;
                car.m_current_channel = i;

                //���µ�·�����ĳ�
                road.m_cars_index_in_road[car.m_current_is_reverse][i].push_back(car.m_index);
                road.m_car_waiting_order[car.m_current_is_reverse].push_back(car.m_index);


                //�����ж�ת���ͻ�ˣ�ֱ���ܾ���
                //��Ϊ·�ϵĳ������Ѿ�������

                //ע�⣺����UpdateCrossCar����µ�·�Գ��İ�����ϵ�������UpdateCrossCar�ɹ���·����ô
                if (car.m_path.size() > car.m_current_path_index + 1)
                {
                    //������һ��·
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
                    //�����͵����յ㰡
                    car.m_is_reach_end = true;
                    car.m_reach_end_t = t;

                    //Ϊ�˵��Է��㣬��Ȼ�Ѿ�����Ŀ�ĵأ�Ҳ����������Ϣ
                    car.m_going = true;
                    car.m_move_capability -= vacancy;//���ĵ��г�����
                    car.m_current_s0 = vacancy;
                    car.m_current_channel = i;
                }

            }

            break;
        }
        else
        {
            const Car& former_car = m_carvec_t[road.m_cars_index_in_road[car.m_current_is_reverse/*����ͬ�򳵵��ϵĳ�*/][i].back()];//�������û�г����ᵼ��back����
            if (former_car.m_going)
            {
                //�쳣����
                //�����������ˣ��ݶ����ܷ���
                //����Ķϵ㲻Ҫɾ��
                return 0;
            }

            vacancy = former_car.m_current_s0 - 1;
            if (vacancy>0)
            {
                is_set = true;
                car.m_current_channel = i;

                if (car.m_move_capability <= vacancy)
                {
                    //�������s2
                    //����carλ��
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

                //���µ�·�����ĳ�
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
        //���г�����û�п�λ
        car.m_going = false;
        //��������������򽫺������ȼ��ϵ͵ĳ����˶�״̬������Ϊ�ս�
        car.ReSetCarAfterLauchFail();
        return -1;
    }
}
void Car::ReSetCarAfterLauchFail()
{
    //���ó���ʧ�ܵĳ���
    m_start_time_infact++;
}
TState TState::TStateInitByPath(vector<int> car_index_vec,vector<vector<int>> cars_path, int t)//��ʼ��������������Ҫ������car_vec�Լ�ʱ��t������car_index_vec���Ĭ����car_id����
{
    //�������ܣ�
    //��1������tʱ�̳����ĳ�·��������һ���µ�TState����ʼ��tʱ����ʻ״̬��
    //��2���ʼ��ʼ��ʱ��t��1��ʼ

    //t:this
    //t+1:����

    TState new_state = (*this);

    //car_idС���ȷ���
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

int TState::UpdateTState(vector<int> car_index_vec, int t, vector<int>& car_is_not_lauched)//��ʼ��������������Ҫ������car_vec�Լ�ʱ��t������car_index_vec���Ĭ����car_id����
{
    //�������ܣ�
    //��1������tʱ�̳����ĳ�·��������һ���µ�TState����ʼ��tʱ����ʻ״̬��
    //��2���ʼ��ʼ��ʱ��t��1��ʼ
    //��3��ע�ⷢ����car�ں�����Ҫ��֤���С����

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
void TState::ResetCarTurning(vector<vector<int>> car_path, vector<int> car_index)//���ݸ������ֳ���·������������������Щ���Ĺ滮·��
{
    /*
    //ResetCarTurning��
    //���������Ҫʵʱ�滮��ֻ��Ҫ�޸�path�ĺ���·�μ��ɣ��ȽϷ���
    */
}
vector<int> TState::FindLauchCar(int t)//�ҵ����㷢���ĳ���
{
    int n_car = m_carvec_t.size();
    vector<int> ready_to_lauch_cars(0);
    for (int i = 0; i < n_car; i++)
    {
        Car& car = m_carvec_t[i];
        if (car.m_start_time_infact == t&&car.m_start_time_infact >= car.m_start_time_expect)
        {
            //�жϷ����������ڵ�·�����Ƿ���
            int dir_in_next_road=0;
            Road &next_road=m_roadvec_t[car.m_path[0]];
            if(next_road.m_in_cross_index==car.m_start_cross_index)
                dir_in_next_road=0;//��
            else
                dir_in_next_road=1;//��

            if(next_road.m_congestion[dir_in_next_road]>0.8)
            {
                car.ReSetCarAfterLauchFail();//�¸�·��ӵ�£��Ͳ�������
                continue;
            }

            int b=i%1000;
            if(all_cars_num>1800&&b)//���·�ϳ����ˣ�n��ȡ��������С������
            {
                //cout<<"all_cars_num="<<all_cars_num<<endl;
                car.ReSetCarAfterLauchFail();//·�ϳ�̫��Ҳ������
                continue;
            }
            //delay start car
            if(t<5&&i%2)
            {
                car.ReSetCarAfterLauchFail();//��ʼ��������һ��
                continue;
            }

            ready_to_lauch_cars.push_back(car.m_index);//�������򷢳�
        }
    }
    //    int n_car = m_carvec_t.size();
    //    vector<int> ready_to_lauch_cars(0);
    //    for (int i = 0; i < n_car; i++)
    //    {
    //        const Car& car = m_carvec_t[i];
    //        if (car.m_start_time_infact == t&&car.m_start_time_infact >= car.m_start_time_expect)
    //        {
    //            ready_to_lauch_cars.push_back(car.m_index);//�������򷢳�
    //        }
    //    }

    return ready_to_lauch_cars;
}
int TState::UpdateTState(int t)//����t->t1ʱ��״̬
{
    /*
    //����ֵ˵����
    //	-1������
    //	0 ����ǰʱ������
    //	1 �����г������յ�
    //UpdateTState��
    //	��1����·�ڵĵ��Ⱥ͹�·�ڵ�·�����ȼ�InitCarOrderInRoad
    //	��2��ѭ������ÿ��·�ڵĹ�·����λ�ã�ֱ��û�г�����λ�ø��£�����
    //	��3���ж���û�г����ڵȴ�״̬���У���˵��������������������ȴ���˭Ҳ���²���λ�ã�Ϊ�ȴ�״̬�ĳ�����Ϊ������
    //	��4���ж��Ƿ����г��������յ�UpdateIsReachFlag
    */
    //����ֵ˵����
    //-1����ʾ����
    //0����ʾһ�������������г�û�е���Ŀ�ĵ�
    //1����ʾȫ��������Ŀ�ĵ�


    //��1����·�ڵĵ��Ⱥ͹�·�ڵ�·�����ȼ�InitCarOrderInRoad
    for (int i = 0; i < m_roadvec_t.size(); i++)
    {
        InitCarOrderInRoad(i);
    }
    //��2��ѭ������ÿ��·�ڵĹ�·����λ�ã�ֱ��û�г�����λ�ø��£�����
    int is_all_car_stop = UpdateCross(t);

    //��3���ж���û�г����ڵȴ�״̬���У���˵��������������������ȴ���˭Ҳ���²���λ�ã�Ϊ�ȴ�״̬�ĳ�����Ϊ������
    //if (CheckLocked())
    if (is_all_car_stop==-1)
    {
        return -1;//��������
    }
    else
    {
        //�ȴ����ܵĳ�������֮���ٷ��³�
        //�ҵ�����Ҫ�����ĳ�
        vector<int> ready_to_lauch = FindLauchCar(t);
        //�����·�������TState
        vector<int> cars_is_not_lauched(0);
        int is_all_lauched= UpdateTState(ready_to_lauch, t, cars_is_not_lauched);

        if (is_all_lauched == 0)
        {
            return 0;//���ֳ��ӳٷ�������������жϵ����յ�
        }
    }

    //��4���ж��Ƿ����г��������յ�
    if (CheckAllCarReach())
    {
        return 1;
    }
    return 0;
}
bool Road::CheckIsFront(int direction, int channel, int car_index)//�жϵ�ǰ���ȵĳ��Ƿ�Ϊ��ǰ��
{
    if (m_cars_index_in_road[direction][channel].size() != 0 && m_cars_index_in_road[direction][channel].front() == car_index)
    {
        return true;
    }
    else
    {
        //��1����·������û�г�����ô��Ըõ�·�ж�car_index������ֵ����������false��Ϊ��ʾ
        //��2��������ǰ��������false
        return false;
    }
}

bool cmp_road(Road a, Road b)
{
    return a.m_road_id < b.m_road_id;//����
}
void TState::SortRoadByID()//����ID���򣬲�����m_index
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
    return a.m_cross_id < b.m_cross_id;//����
}
void TState::SortCrossByID()//����ID���򣬲�����m_index
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
    return a.m_car_id < b.m_car_id;//����
}
void TState::SortCarByID()//����ID���򣬲�����m_index
{
    std::sort(m_carvec_t.begin(), m_carvec_t.end(), cmp_car);

    int n_car = m_carvec_t.size();
    for (int i = 0; i < n_car; i++)
    {
        m_carvec_t[i].m_index = i;
    }
}

int TState::InitCrossPolling()//��ʼ����ѯ��������
{
    n_car_waiting_before_adjust = 100000;
    return CheckWaitingCar();
}
int TState::CheckWaitingCar()
{
    //����˵�����������������ѯ·����ѭ��
    //��1��ÿ��·����ѯ֮ǰ������·�ڡ���·�����򡢳������ڳ��ĵȴ�״̬��
    //��2��������·�ڡ���·�����򡢳����ĵȴ����
    //��3��ÿ��·�ĵȴ������������������䣬˵�������ˣ��������

    //����ֵ˵����
    //��4�����ȫ������ֹ������1��
    //��5�������ѯ·�ں�û�б仯������Ϊ������ֹ,����-1
    //��6��һ�����������0

    //ע�⣺���ﲻ�Ƕ�����car��������Ϊ��Щcar���ܻ�û����·����Ȼ�жϳ������������ͦ�򵥵ġ�
    //�����߼���
    //��1��Ҫô�ȴ���Ҫô�ƶ�֮����ֹ��û�е���״̬��Ҳ�����ڹ���·��֮���Ϊ�ȴ�״̬�������
    //��2��������·�ĵȴ�״̬������գ�������һ����ѯ·�ں�û�б仯������Ϊ������ֹ
    int n_cross = m_crossvec_t.size();
    vector<int > cross_roads_id;
    int road_index = -1;
    int direction = -1;

    m_deadlock_carvec_t.clear();

    int current_waiting_number = 0;//��ʼ����ǰͼ�ȴ�״̬���
    for (int i = 0; i < n_cross; i++)
    {
        //�����������·ID
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
            if (road.m_is_duplex == 0&&direction==1)//�����
            {
                //��ֻ��direction=0
                //�������ʾͻ�Խ��

                //�ǾͲ��ܽ���
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
                        //is_channel_going = true;//�г��ǵȴ�״̬����ô�����Ͳ����ս�״̬
                        road.m_channel_going[direction][k] = true;
                        road.m_direction_going[direction] = true;
                        cross.m_going = true;

                        current_waiting_number++;//ȫ�ֵȴ�״̬�����������

                        m_deadlock_carvec_t.push_back(car);
                    }
                }
            }
        }
    }

    if (current_waiting_number == 0)//������·�ĵȴ�״̬������գ���ѯ����
    {
        n_car_waiting_before_adjust = 0;
        return 1;//���������ѯ
    }
    else if (current_waiting_number < n_car_waiting_before_adjust)//��һ����ѯ·�ں󣬵ȴ�״̬�ĳ���������
    {
        //��״̬�������£�current_waiting_number���𽥼�С
        n_car_waiting_before_adjust = current_waiting_number;
        return 0;//����
    }
    else//��һ����ѯ·�ں�û�б仯������Ϊ������ֹ
    {
        //û�з���״̬�仯,��������
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

    case 1://a������ֱ��
        break;

    case 2://b��������ת
        straight_conflict_road = FindStraightConflictRoad(cross, to_road_index);
        if (straight_conflict_road != -1)
        {
            //�޳�ͻ
            is_conflict = true;
        }
        break;

    case 3://c��������ת
        straight_conflict_road = FindStraightConflictRoad(cross, to_road_index);
        if (straight_conflict_road == -1)
        {
            //�޳�ͻ
            leftturning_conflict_road = FindLeftTurningConflictRoad(cross, to_road_index);
            if (leftturning_conflict_road != -1)
            {
                //�޳�ͻ
                is_conflict = true;
            }
        }
        else
        {
            //�г�ͻ
            is_conflict = true;
        }
        break;
    default:
        //�쳣����
        is_conflict = true;//�ݴ������������δԤ�ϵ�ת����������Ϊ��ǰ������ͻ
        break;
    }

    return is_conflict;
}
void TState::UpdateCrossOnce(int t)//ִ��һ��·�ڵ���
{
    int n_cross = m_crossvec_t.size();
    vector<int > cross_roads_id;
    int road_index = -1;
    int direction = -1;
    for (int i = 0; i < n_cross; i++)
    {
        //�����������·ID
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
        std::sort(cross_roads_id.begin(), cross_roads_id.end());//�Ե�·ID������ѯ
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
                //�쳣����
                continue;
            }

            if (road.m_is_duplex == 0 && direction == 1)//�����
            {
                //��ֻ��direction=0,�������ʾͻ�Խ��

                //�ǾͲ��ü�����������
                continue;
            }
            for (int k = 0; k < road.m_n_channel; k++)
            {
                //Ϊ�˱����ڹ�·֮��ɾ���˵�·�ϵĲ��ֳ�������it�����������︴��һ��road���ڱ�����ע�����ﲻ������
                list<int> car_order(0);
                car_order.assign(road.m_cars_index_in_road[direction][k].begin(), road.m_cars_index_in_road[direction][k].end());
                for (list<int >::iterator it = car_order.begin(); it != car_order.end(); it++)
                {
                    Car& car = m_carvec_t[*it];//GetRoadFirstCar

                    if (!(car.m_going))//�Ѿ�������ĳ�
                    {
                        //��Ϊ��ֹ״̬
                        //�������Ӧ����ǰ��ԭ���еȴ�״̬�ĳ����ڱ����Ĺ����У��Ѿ�����·��
                        //��ǰ��������·��֮�󣬻�ִ��·�ڵ��ȣ����ܹ��ƶ�����ֹ״̬�ĳ�������
                        //���Ի��в��ֳ��ڳ�·�ڱ���֮ǰ��Ϊ��ֹ״̬��
                        continue;
                    }
                    if (car.m_is_reach_end)
                    {
                        //������ܳ�����
                        //Ϊʲô���г���·�ڵ���֮ǰ��Ϊ�����յ㣿
                        continue;
                    }

                    //�ݴ���������ǰ���赲
                    //��鵱ǰ����ͬ����ǰ���Ƿ����赲
                    int pre_car_index = GetPreCar(car.m_index);
                    if (pre_car_index != -1)
                    {
                        //ǰ�����赲
                        //Ӧ���ǵ�ǰ����û�и���״̬��ԭ��
                        UpdateBackCarStateAfterPass(road.m_index, direction, car.m_channel_befor_adjust);//m_channel_befor_adjustע�⿴һ�³�ʼ����
                        break;
                    }

                    //�յ����
                    if (car.m_end_cross_index == cross.m_index)
                    {
                        //�����յ㣬�ж��Ƿ��ܹ��߹�·�ڼ��ɣ��ж������Ƿ��ܹ������յ�
                        //ע�⣺���ﵽ���յ�ĵ��ȣ����ȼ���ߣ���ת���ͻ�ж�֮ǰ�������Լӹ���
                        int s1 = road.m_road_length - car.m_current_s0;
                        if (car.m_move_capability > s1)
                        {
                            car.m_going = false;
                            car.m_is_reach_end = true;

                            car.m_reach_end_t = t;
                            car.m_current_s0 = road.m_road_length;

                            //��road.m_car_waiting_order[direction]��ɾ��
                            road.RemoveWaitingCarFromCarOrder(car.m_index, direction);
                            road.RemoveCarFromRoad(car.m_index, direction, car.m_channel_befor_adjust);

                            //��λ�ø���
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
                        //������ǰ��·����ѯ������·
                        break;
                    }
                    else
                    {
                        UpdateCrossCar(cross, car, m_roadvec_t[car.m_path[car.m_current_path_index]], m_roadvec_t[car.m_path[car.m_current_path_index + 1]]); //����ɹ����뵽�µĵ�·����UpdateCrossCar����ɵ�·�������ϵĸ���

                        if (car.m_going)
                        {
                            check_conflict = true;
                            //������ǰ��·����ѯ������·
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
    //�������ã�ѭ������ÿ��·�ڵĹ�·����λ�ã�ֱ��û�г�����λ�ø��£�������

    int graph_going = InitCrossPolling();
    if (graph_going == 1)//������ѯ
        return 1;
    else if (graph_going == -1)
        return -1;


    while (1)
    {

        UpdateCrossOnce(t);

        graph_going = CheckWaitingCar();
        if (graph_going == 1)//������ѯ
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
            break;//�����Ҫ����������Ͳ�Ҫbreak��
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
            break;//�����Ҫ����������Ͳ�Ҫbreak��
        }
    }
    return is_reach;
}
list<int> Road::GetBackListInChannel(int direction,int channel, int car_index)//��ȡ��ǰ����ͬ���������б�
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
    //UpdateCrossCarҪ��֤��
    //��1���ܹ�·�ģ�����Ϊ��ֹ״̬�Ŷ��������ܶ�������ǰ����·��Ҫ����ĳ����ϣ�ĩβ��Ϊ�ȴ�״̬����ǰ����ǰ�������ϻ���֮��ͻ����ʱ�����ܣ�����ͻ�����ܡ�
    //��2�����ܹ�·�ģ��ܵ���ǰ·�����λ�ã���Ϊ��ֹ��
    //��3������
    //	1�������Թ�·���ƶ���Ϊ��ֹ״̬��
    //	2�������Թ�·����ǰ����λ��Ϊ�ȴ�״̬��
    //	3�������ɹ�·��ǰ��Ϊ�ȴ�״̬��������ͻ��
    //	4�������ɹ�·��ǰ����·ȫ������ֹ״̬�ĳ�������
    //��4�����գ�1����2����ֻ��1��4�����ƶ���֮���Ϊ��ֹ�����඼���ܶ�������ͻ����
    //��5���򻯹�����
    //	5.1��UpdateCrossCar
    //		a���ж��Ƿ��ܹ���·��
    //		b������ܹ���·���жϽ��복��ĩβ���Ƿ�Ϊ�ȴ�״̬�������תd��
    //		c��������ܹ�·����·�ڵ��ȴ���
    //		d��������복��ĩβ���еȴ�״̬������Ϊ�ȴ������س�ͻ�����򱾳�ǰ������ֹ״̬
    //
    //���ش����жϵ�ǰ���Ƿ�Ϊ�ȴ�����

    /////////////////////��ʼ���///////////////////////////////
    //
    //�ݴ������ĵȴ�״̬�����ж����������߼���ͻ
    if (!(car.m_going) || car.m_move_capability <= 0)
    {
        car.m_going = false;

        //����ֵĴ���ע�����
        //������Ϣ���
        cout << "UpdateCrossCar ERROR _0_\t:" << endl;
        cout << VName(cross.m_index) << ":" << cross.m_index << endl;
        cout << VName(car.m_index) << ":" << car.m_index << endl;
        cout << VName(from_road.m_index) << ":" << from_road.m_index << endl;
        cout << VName(to_road.m_index) << ":" << to_road.m_index << endl;
        return;
    }

    int direction1 = 0;//Ĭ������
    if (cross.m_index == from_road.m_out_cross_index)//����
    {
        direction1 = 0;

    }
    else
    {
        direction1 = 1;
    }

    int direction2 = 0;//Ĭ������
    if (cross.m_index == to_road.m_in_cross_index)//����
    {
        direction2 = 0;

    }
    else
    {
        direction2 = 1;
    }

    //ע�⣺������ݴ�Ӧ���ж�һ��:
    //(1)��ǰ�����Ƿ��г��谭
    //(2)�Ƿ��е���ͬ�򳵵�
    //Ϊ�˼�㣬����ֻ�ж�������ĵ�·�Ƿ���ͬ�򡪡���Ϊ��·�ڵ���ʱ��Ӧ�þ��жϹ���û��ǰ���谭
    //ֻҪ���Ǻ��ҵ��øú�����Ӧ�û�����͵͵������
    if (to_road.m_is_duplex == 0 && direction2 == 1)
    {
        //���з���
        return;
    }

    //�ݴ��жϣ�����s1���ж��Ƿ������·����
    int s1 = from_road.m_road_length - car.m_current_s0;//ע�⣺��Ϊ�ڵȴ�״̬car�ǲ��ᷢ��λ�ñ䶯��
    //����Ӧ��ʹ�ã�from_road.m_road_length - (car.m_current_s0 + car.m_move_capability-car.m_current_v);
    //�ݴ���
    if (s1 < 0)
    {
        //����ֵĴ���ע�����
        //����ǰ�Ƚ�����ǰ���Ĵ�������
        car.m_current_s0 = from_road.m_road_length;

        //������Ϣ���
        cout << "UpdateCrossCar ERROR _s1_\t:" << endl;
        cout << VName(cross.m_index) << ":" << cross.m_index << endl;
        cout << VName(car.m_index) << ":" << car.m_index << endl;
        cout << VName(from_road.m_index) << ":" << from_road.m_index << endl;
        cout << VName(to_road.m_index) << ":" << to_road.m_index << endl;
        return;
    }
    if (s1 >= car.m_move_capability)
    {
        //�쳣���󣬾�Ȼ�����Թ�·������Ӧ����·�ڵ�������Ͷ������𣿺����

        car.m_going = false;
        //�����ȼ�������ɾ��
        from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);

        car.m_current_s0 = car.m_current_s0 + car.m_move_capability;
        car.m_move_capability = 0;
        return;
    }

    //�������s2������
    int v2 = min(to_road.m_v_road_max, car.m_v_max); //�����µ�·ʱ�����ĳ���
    int s2 = max(v2 - s1, 0); //�½��복����ʻ������

    if (s2 <= 0)
    {
        //�������ܹ�·��

        //���ó����˶���ֹ״̬
        car.m_going = false;
        from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);

        car.m_move_capability -= (from_road.m_road_length - car.m_current_s0);
        car.m_current_s0 = from_road.m_road_length;

        return;
    }

    //��������·�ڣ����¿���û�п�λ

    bool is_set = false;//Ĭ�ϷŲ���
    int vacancy = 0;//��λ

    for (int i = 0; i < to_road.m_n_channel; i++)
    {
        if (to_road.m_cars_index_in_road[direction2][i].size() == 0)
        {
            //������û�г�
            vacancy = to_road.m_road_length;

            is_set = true;

            if (s2 <= vacancy)
            {
                //�������s2
                //����carλ��
                car.m_going = false;
                car.m_current_s0 = s2;
                car.m_move_capability = 0;
            }
            else
            {
                //�ݴ����Լӹ��򣬲��ó���������һ�ε�·
                //������Ϣ���
                cout << "UpdateCrossCar Warning _s2_\t:" << endl;
                cout << VName(cross.m_index) << ":" << cross.m_index << endl;
                cout << VName(car.m_index) << ":" << car.m_index << endl;
                cout << VName(from_road.m_index) << ":" << from_road.m_index << endl;
                cout << VName(to_road.m_index) << ":" << to_road.m_index << endl;

                //������һ��·�ڣ������ӻ����ٹ�·���أ�
                //�ں����⣬���ж����ڵ�·�Ƿ�仯
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

            //���µ�·�����ĳ�
            to_road.m_cars_index_in_road[direction2][i].push_back(car.m_index);
            break;
        }
        else
        {
            //�������г�
            const Car& former_car = m_carvec_t[to_road.m_cars_index_in_road[direction2][i].back()];

            if (former_car.m_going == true)
            {
                return;//��⵽��Ҫ��ǰ�����ߵ������ֱ�ӷ���
            }

            vacancy = former_car.m_current_s0 - 1;

            if (vacancy>0)
            {
                is_set = true;
                if (s2 <= vacancy)
                {
                    //�������s2
                    //����carλ��
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

                //���µ�·�����ĳ�
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
        car.m_current_path_index++;//�������������tʱ�̵ĳ�ʼ���ڵ�·
        return ;
    }
    else
    {
        //���г�����û�п�λ�����Ҷ�����ֹ״̬
        car.m_going = false;

        //��car�����ȼ�������ɾ��
        from_road.RemoveWaitingCarFromCarOrder(car.m_index, direction1);

        car.m_move_capability -= from_road.m_road_length - car.m_current_s0;
        car.m_current_s0 = from_road.m_road_length;

        return;
    }
}
int TState::GetPreCar(int car_index)//Ѱ��ͬ����ǰ�����������ǰ��������ǰ��index�����������ǰ��������-1
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
                //���ǵ�һ�����ͱ�Ȼ����ǰ��
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

int TState::UpdateBackCarStateAfterPass(int road_index, int direction, int channel)//��·�����º�������״̬
{
    //ע�⣺����ʱ��������ֲ��ֳ����ɵȴ���Ϊ��ֹ��Ҫ�ӳ�·�����ȼ�˳������m_car_waiting_order�ѳ�indexɾ��
    //������ܻ���ɣ�ǰ����λ��ֹ״̬����ȴ����Ҫ��ǰ�ܣ����¼�����
    list<int> back_list;
    back_list.clear();
    if (channel >= m_roadvec_t[road_index].m_cars_index_in_road[direction].size())
    {
        //Ӧ���ǳ������Խ����ݴ���
        return -1;
    }
    back_list = m_roadvec_t[road_index].m_cars_index_in_road[direction][channel];

    if (back_list.size() == 0)
    {
        //������car�ں�����ͱ�ɾ���ˣ����³���Ϊ��
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
                //��ǰ��,������ǰ��
                if (m_carvec_t[*it].m_current_s0 < m_roadvec_t[road_index].m_road_length)
                {
                    //������
                    if (m_carvec_t[*it].m_move_capability > 0)
                    {
                        is_update = 1;
                        if (m_carvec_t[*it].m_current_s0 + m_carvec_t[*it].m_move_capability > m_roadvec_t[road_index].m_road_length)
                        {
                            //������ֹ״̬������λ��
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
                        //û���ж�����
                        //����ֵ������Ϊʲô�ȴ�״̬�ģ��ƶ�����Ϊ0��ע�����
                        if (m_carvec_t[*it].m_going)
                        {
                            m_carvec_t[*it].m_going = false;//ֻ�޸ĵȴ�״̬���������λ��
                            if (is_update<0)
                                is_update = 0;//�����˸���
                            m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
                        }
                        else
                        {
                            //��֧������ǰ���߼���ͻ
                            cout << "Error UpdateBackCarStateWhenNotPass 21:" << "Car Index:" << *it << ".m_going" << "is wrong" << endl;
                        }
                    }
                }
                else
                {
                    //ֻ����m_carvec_t[*it].m_current_s0 == m_roadvec_t[road_index].m_road_length
                    //�޷�����λ�ã�״̬�ɳ�·�ھ����������޷�����
                }
            }
            else
            {
                //����ǰ��
                if (m_carvec_t[*it].m_current_s0 >= m_carvec_t[pre_car_index].m_current_s0)
                {
                    //�쳣��ע�����
                    cout << "Error UpdateBackCarStateWhenNotPass 1:" << "Car Index:" << *it << "conflicts with" << "Pre Car Index:" << pre_car_index << endl;
                }
                //����������Ҫ�ܵ�����һ���߼������������ݴ�
                if (m_carvec_t[*it].m_current_s0 + m_carvec_t[*it].m_move_capability < m_carvec_t[pre_car_index].m_current_s0)//���㹻��������
                {
                    if (m_carvec_t[*it].m_move_capability > 0)//���ƶ�����
                    {
                        is_update = 1;
                        m_carvec_t[*it].m_current_s0 = m_carvec_t[*it].m_current_s0 + m_carvec_t[*it].m_move_capability;
                        m_carvec_t[*it].m_move_capability = 0;
                        m_carvec_t[*it].m_going = false;

                        m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
                    }
                    else
                    {
                        //û���ж�����
                        //����ֵ������Ϊʲô�ȴ�״̬�ģ��ƶ�����Ϊ0��ע�����
                        if (m_carvec_t[*it].m_going)
                        {
                            m_carvec_t[*it].m_going = false;//ֻ�޸ĵȴ�״̬���������λ��
                            m_roadvec_t[road_index].RemoveWaitingCarFromCarOrder(*it, direction);
                            if (is_update<0)
                                is_update = 0;//�����˸���
                        }
                        else
                        {
                            //��֧������ǰ���߼���ͻ
                            cout << "Error UpdateBackCarStateWhenNotPass 22:" << "Car Index:" << *it << ".m_going" << "is wrong" << endl;
                        }
                    }
                }
                else
                {
                    //û���㹻��������
                    if (!(m_carvec_t[pre_car_index].m_going))
                    {
                        //ǰ��Ϊ��ֹ״̬�����º�λ��
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
                                //����û���ƶ��ռ�
                            }
                        }
                        else
                        {
                            //��֧������ǰ���߼���ͻ
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
    //��ѯ
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
    //��ѯ
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
    //��ѯ
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
    //		1������˫����������ӵ�·����ǰͷ��ʼ
    //		2����������ٶ��µ�Ԥ��λ��expect_pos
    //		3���ж��˶���ֹ״̬������m_expect_s0
    //		4�����ȴ�״̬�ĳ�����ȴ��б�car_order
    //			ֻ�����������һ���ǵȴ���·�ģ�һ���Ǳ��ȴ�״̬��ǰ����λ��
    //		5���Եȴ��б��еĳ�������λ�ÿ�ǰ���ȡ�����С���ȣ�����������������Ϊ·�����ȼ���
    //		6������·�ڳ�����λ�ã������ڳ���λ��ʹ·�ڵ���ʱ�и���ĳ����Խ����·

    //		�����˳��Ԥ��λ��>���״̬>�������ȼ�>λ�ø���

    //		PS���������򣬾����迼��ǰ��Ž����ĵȴ�״̬�ĳ���λ�á������Ƚϡ�һ��������������ȼ���
    //		PS������·�ڵ����߼�����UpdateRoadCar
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
            //ע�⳵���복������˳�򣬱��ô���
            //�복push_back
            //����pop_front
            list<int>::iterator it = road.m_cars_index_in_road[d][channel].begin();
            list<int>::iterator it_begin = it;
            list<int>::iterator it_end = road.m_cars_index_in_road[d][channel].end();

            for (; it != it_end; it++)
            {
                // carvector[*it]�������
                // carvector[*rit]�������

                Car& car = m_carvec_t[*it];

                //��ʼ��tʱ���ж�����
                car.m_current_v = min(road.m_v_road_max, car.m_v_max);
                car.m_move_capability = car.m_current_v;
                car.m_channel_befor_adjust = channel;

                int expect_pos = car.m_current_s0 + car.m_move_capability;
                if (it == it_begin)
                {
                    //��ǰ������ǰ��
                    if (expect_pos <= road.m_road_length)
                    {
                        //����·��
                        car.m_going = false;
                        car.m_current_s0 = expect_pos;
                    }
                    else
                    {
                        //�ȴ���·
                        car.m_going = true;
                        road.m_car_waiting_order[d].push_back(car.m_index);
                    }
                }
                else
                {
                    list<int>::iterator pre_it = it;
                    pre_it--;
                    //��ǰ��ǰ��ĳ�
                    Car& pre_car = m_carvec_t[*pre_it];
                    if (expect_pos<pre_car.m_current_s0)//����ǰ����λ
                    {
                        car.m_going = false;
                        car.m_current_s0 = expect_pos;
                    }
                    else
                    {
                        //��ǰ����λ,ǰ����ֹ
                        if (pre_car.m_going == false)
                        {
                            car.m_going = false;
                            car.m_current_s0 = pre_car.m_current_s0 - 1;
                        }
                        else//��ǰ����λ,ǰ���ȴ�
                        {
                            car.m_going = true;
                            //���ȴ��ĳ�����ȴ��б�
                            road.m_car_waiting_order[d].push_back(car.m_index);
                        }
                    }
                }
            }
        }
        SortCarOrderInRoad(road.m_car_waiting_order[d]);//����·�ڳ�����·�ڵ����ȼ�
    }
}
void Car::UpdateCarCurrentPosition()
{
    m_current_s0 = m_expect_s0;
    m_expect_s0 = 0;
}
bool SolveMap(TState t0, int plan_time, int all_car_plan_time)
{
    //�������ܣ��������ʱ�䡢����ʱ�䡣
    //������Graph::routePlan�ظ���
    //�����������ʱ����������Ż������ܴ��ڵ���


    //ע�⣺���ڳ�ʼ��û���꣬δ��ʼ�����
    return true;
}

bool TState::cmp_car_index(int c1_index, int c2_index)
{
    const Car& car1 = m_carvec_t[c1_index];
    const Car& car2 = m_carvec_t[c2_index];
    return car1<car2;//Car��<�ȽϷ������Car���ݽṹ
}
void TState::SortCarOrderInRoad(list<int>& waiting_car_index_list)
{
    //���������У���һ�����ȼ����
    //ʹ��pop_front

    //дһ���򵥵�ð������
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
                //ǰ��ϴ�
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
    //�ж�ת�䷽�򣺵�����Ҫ��·��ʱ����Ҫ�����ó�����ת�䷽���Ա��ڳ�����·�ڵ����ȼ��жϡ�

    //�ѵ㣺��ô�жϳ�������ת/��ת/ֱ�У�
    //ע�⣺·�ڵ�·txt�еģ�˳����ܲ���������������ġ���P7 cross.txt

    //׼��·�����ӵĵ�·������˳ʱ���
    */
    int flag = 0;//0���쳣����
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
        //ֱ��
        flag = 1;
        break;

    case 3:
    case -1:
        //��ת
        flag = 2;
        break;

    case 1:
    case -3:
        //��ת
        flag = 3;
        break;
    }
    return flag;
}
int TState::CheckTurning(Cross cross, Car car)
{
    if (car.m_current_path_index + 1 >= car.m_path.size())
    {
        //˵���Ѿ������·��
        //�൱��ֱ��
        return 1;
    }
    return CheckTurning(cross, car.m_path[car.m_current_path_index], car.m_path[car.m_current_path_index + 1]);
}
int TState::CheckIsReverse(Cross cross, Car car)
{
    if (m_roadvec_t[car.m_current_road].m_out_cross_index == cross.m_index)
    {
        return 0;//����
    }
    else if (m_roadvec_t[car.m_current_road].m_in_cross_index == cross.m_index)
    {
        return 1;//����
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
        return 0;//����
    }
    else if (m_roadvec_t[road_index].m_in_cross_index == cross.m_index)
    {
        return 1;//����
    }
    else
    {
        return -1;
    }
}
int TState::FindStraightConflictRoad(Cross cross, int target_road_index)
{
    /*
    �������ܣ��ڵ�ǰ·���У���Ŀ��·��ֱ���ͻ�ĵ�·��ţ���ȡʧ�ܣ����� - 1
    �൱���ж�ֱ�г�ͻ
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
    ��������Ҫ��ת�����Ϸ���·ʱ����Ҫ�Ȼ�ȡֱ�߳�ͻ��·���ж�ֱ���ͻ��·�ĵ�һ���ȼ������Ƿ�ֱ�ߣ��У����ǳ�ͻ�����س�ͻ��·ID���񣬾��ǲ���ͻ������ - 1��
    */

    //��1���ȸ���·�ں�Ŀ��·���ҳ�ֱ�г���
    //��2��Ȼ���ж�ֱ�г����ϵĵ�һ���ȼ��ĳ��Ƿ�ֱ��

    int i=0;
    for (i = 0; i < 4; i++)
    {
        if (target_road_index == cross.m_road_index[i])
        {
            break;
        }
    }

    if (4 == i) //û�ҵ����쳣
    {
        //�쳣�˳�
        return -1;
    }

    int reverse_road_index = cross.m_road_index[(i + 2) % 4];//excellent job!
    if (-1 == reverse_road_index)  //û·
    {
        return -1;
    }

    int reverse_road_direction = CheckIsReverse(cross, reverse_road_index);
    if (m_roadvec_t[reverse_road_index].m_car_waiting_order[reverse_road_direction].size()==0)
    {
        //û�д��ڵȴ�״̬�ĳ�
        //���е�Ҳ������ж��б�����
        return -1;
    }

    int conflict_car_index = GetRoadFirstMovingCar(reverse_road_index, reverse_road_direction);

    if (conflict_car_index == -1)
    {
        return -1;
    }
    if (CheckTurning(cross, m_carvec_t[conflict_car_index]) == 1)
    {
        //ֱ��
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
    �������ܣ��ڵ�ǰ·���У���Ŀ��·����ת�����ͻ�ĵ�·��ţ���ȡʧ�ܣ�����-1
    �൱���ж���ת��ͻ
                            |
                    target_road_index
                            |
                            ^
                            |
    left turning conflict>-----<car waiting for right turning
                            |
                            ^
                            |
    ��������Ҫ��ת�����Ϸ���·ʱ����Ҫ�Ȼ�ȡ��ת��ĳ�ͻ��·���ж���ת���ͻ��·�ĵ�һ���ȼ������Ƿ���ת���У����ǳ�ͻ�����س�ͻ��·ID���񣬾��ǲ���ͻ������-1��
    */
    int i;
    for (i = 0; i < 4; i++)
    {
        if (target_road_index == cross.m_road_index[i])
        {
            break;
        }
    }

    if (4 == i) //û�ҵ����쳣
    {
        //�쳣�˳�
        return -1;
    }

    int left_turning_road_index = cross.m_road_index[(i + 3) % 4];//excellent job!
    if (-1 == left_turning_road_index)  //û·
    {
        return -1;
    }

    int reverse_road_direction = CheckIsReverse(cross, left_turning_road_index);
    if (m_roadvec_t[left_turning_road_index].m_car_waiting_order[reverse_road_direction].size()==0)
    {
        //û�д��ڵȴ�״̬�ĳ�
        //���е�Ҳ������ж��б�����
        return -1;
    }
    int conflict_car_index = GetRoadFirstMovingCar(left_turning_road_index,reverse_road_direction);

    if (conflict_car_index == -1)
    {
        return -1;
    }
    if (CheckTurning(cross, m_carvec_t[conflict_car_index]) == 2)
    {
        //��ת
        return left_turning_road_index;
    }
    else
    {
        return -1;
    }
}
int TState::GetRoadFirstMovingCar(int road_index, int direction)
{
    list<int > temp_list;//�ȸ���һ������
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
            //��ʱ��֪��Ϊʲô�����ط�û�а�ǰ���Ѿ������m_goingɾ��
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
    if (!fin) //��ȡ���ɹ�
    {
        cout << "��ȡ log �ļ����ɹ�" << endl;
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

            getline(fin, str); //��ȡ��������

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

            getline(fin, str); //��ȡ��������

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

                //��ȡ������Ϣ��Ȼ��buildroad
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
                        num = stoi(ret_vt[bias]);//�����ϳ�������
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
                        num = stoi(ret_vt[bias]);//�����ϳ�������
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

            getline(fin, str); //��ȡ��������

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
            //cout << "number_of_variables," << 4 << "," << endl //д���������
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
    if (!fout) //�������ɹ�
    {
        cout << "���� log �ļ����ɹ�" << endl;
        return;
    }


    for (int i = 0; i < m_carvec_t.size(); ++i)
    {
        //д�� Class Car�ı���
        fout << "CAR" << endl;
        fout << "number_of_variables," << 21 << "," << endl //д���������
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
        //д�� Class Road�ı���
        fout << "ROAD" << endl;
        fout << "number_of_variables," << 18 << "," << endl //д���������
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

        vector<list<int>> vt1 = m_roadvec_t[i].m_cars_index_in_road[0]; //һ��vector��һ��
        fout << "m_cars_index_in_road_0,";
        fout << vt1.size() << ",";  //vector��list�ĸ���
        for (int j = 0; j < vt1.size(); ++j) //�ܹ��ж��ٸ�list
        {
            fout << vt1[j].size() << ",";  //list��int�ĸ���

            while (!vt1[j].empty())
            {
                fout << vt1[j].front() << ",";
                vt1[j].pop_front();
                //cout << "fuck:" << vt1[i].front() << endl;
            }

        }
        fout << endl;

        vt1 = m_roadvec_t[i].m_cars_index_in_road[1]; //һ��vector��һ��
        fout << "m_cars_index_in_road_1,";
        fout << vt1.size() << ",";  //vector��list�ĸ���
        for (int j = 0; j < vt1.size(); ++j) //�ܹ��ж��ٸ�list
        {
            fout << vt1[j].size() << ",";  //list��int�ĸ���

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
        fout << list1.size() << ",";  //list��int�ĸ���

        while (!list1.empty())
        {
            fout << list1.front() << ",";
            list1.pop_front();
        }
        fout << endl;

        list1 = m_roadvec_t[i].m_car_waiting_order[1];
        fout << "m_car_waiting_order_1,";
        fout << list1.size() << ",";  //list��int�ĸ���

        while (!list1.empty())
        {
            fout << list1.front() << ",";
            list1.pop_front();
        }
        fout << endl;

        vector<bool> vector1 = m_roadvec_t[i].m_channel_going[0];
        fout << "m_channel_going_0,";
        fout << vector1.size() << ",";  //vector1��int�ĸ���
        for (int j = 0; j < vector1.size(); j++)
        {
            fout << (vector1[j] == true ? 1 : 0)  << ",";
        }
        fout << endl;

        vector1 = m_roadvec_t[i].m_channel_going[1];
        fout << "m_channel_going_1,";
        fout << vector1.size() << ",";  //vector1��int�ĸ���
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
        //д�� Class Cross�ı���
        fout << "CROSS" << endl;
        fout << "number_of_variables," << 4 << "," << endl //д���������
             << "m_cross_id," << m_crossvec_t[i].m_cross_id << "," << endl
             << "m_index," << m_crossvec_t[i].m_index << "," << endl

             << "m_road_index," << m_crossvec_t[i].m_road_index[0] << ","
             << m_crossvec_t[i].m_road_index[1] << ","
             << m_crossvec_t[i].m_road_index[2] << ","
             << m_crossvec_t[i].m_road_index[3] << "," << endl

             << "m_going," << (m_crossvec_t[i].m_going == true ? 1 : 0) << "," << endl;
    }


    fout.close();  // ִ���������ر��ļ����
}

