#include "graph.h"
#include "t_state.h"
using namespace std;

Graph::Graph()
{
}

Graph::~Graph()
{
}

//����cross���ݣ��������ݸ���
int Graph::readCross(string crossPath,TState& newState)
{
    ifstream infile;
    infile.open(crossPath.data());
    assert(infile.is_open());
    string str;
    while (!infile.eof())
    {
        getline(infile,str);
        if (str.find('#') != string::npos)
        {
            continue;
        }
        if(str.size()!=0&&str[0]!='#')//���ж�������
        {
            Cross one_cross;
            int data[5];
            int j=0;//�����±�
            for(size_t i=0;i<str.size();i++)
            {
                int num=0;//����
                bool flag=0;
                while(isdigit(str[i]))//������������
                {
                    flag=1;
                    num=num*10+str[i]-'0';
                    i++;
                }
                if(flag)//�����ָ�ֵ�����飬�����ʼ��
                    data[j++]=num;
            }
            //cout <<"cross"<< data[0] << " " << data[1] << " " << data[2] << " " << data[3]<< " " << data[4]<< endl;
            //���ʼ��
            one_cross.buildCross(data[0],data[1],data[2],data[3],data[4]);
            newState.m_crossvec_t.push_back(one_cross);
        }
    }
    infile.close();

    newState.SortCrossByID();
    return newState.m_crossvec_t.size();
}

//����road���ݣ��������ݸ���
int Graph::readRoad(string roadPath, TState& newState)
{
    ifstream infile;
    infile.open(roadPath.data());
    assert(infile.is_open());
    string str;
    while (!infile.eof())
    {
        getline(infile,str);
        if (str.find('#') != string::npos)
        {
            continue;
        }
        if(str.size()!=0)//���ж�������
        {
            Road one_road;
            int data[8];
            int j=0;//�����±�
            for(size_t i=0;i<str.size();i++)
            {
                int num=0;//����
                bool flag=0;
                while(isdigit(str[i]))//������������
                {
                    flag=1;
                    num=num*10+str[i]-'0';
                    i++;
                }
                if(flag)//�����ָ�ֵ�����飬�����ʼ��
                    data[j++]=num;
            }
            //cout<<data[0]<<" "<<data[1]<<" "<<data[2]<<" "<<data[3]<<" "<<data[4]<<" "<<data[5]<<" "<<data[6]<<endl;
            //���ʼ��
            one_road.buildRoad(data[0],data[1],data[2],
                    data[3],data[4],data[5],data[6]);
            newState.m_roadvec_t.push_back(one_road);
        }
    }
    infile.close();

    newState.SortRoadByID();
    return newState.m_roadvec_t.size();
}

//����car���ݣ��������ݸ���
int Graph::readCar(string carPath, TState& newState)
{
    ifstream infile;
    infile.open(carPath.data());
    assert(infile.is_open());
    string str;
    while (!infile.eof())
    {
        getline(infile,str);
        if (str.find('#') != string::npos)
        {
            continue;
        }
        if(str.size()!=0)//���ж�������
        {
            Car one_car;
            int data[6];
            int j=0;//�����±�
            for(size_t i=0;i<str.size();i++)
            {
                int num=0;//����
                bool flag=0;
                while(isdigit(str[i]))//������������
                {
                    flag=1;
                    num=num*10+str[i]-'0';
                    i++;
                }
                if(flag)//�����ָ�ֵ�����飬�����ʼ��
                    data[j++]=num;
            }
            //���ʼ��
            one_car.buildCar(data[0],data[1],data[2],data[3],data[4]);
            newState.m_carvec_t.push_back(one_car);
        }
    }
    infile.close();

    newState.SortCarByID();
    return newState.m_carvec_t.size();
}

//t_state����������Ϣ
void Graph::readData(string crossPath,string roadPath,string carPath,TState& t_state)
{
    m_crossNum=readCross(crossPath, t_state);
    m_roadNum=readRoad(roadPath, t_state);
    m_carNum=readCar(carPath, t_state);

    t_state.ChangeIdToIndex();
}

//��ͼ�����һ���ڽӾ����һ��Ȩֵ����
void Graph::buildGraph(int crossNum,TState& t_state)
{
    //��ʼ���ڽӾ����edge����
    //��ʼ��edge��ʹ�������Ȩֵ�������ֵ
    for (int i = 0; i < crossNum; i++)
    {
        for (int j = 0; j < crossNum; j++)
        {
            m_edge_weight[i][j] = MAX_NUM;
            m_connect_matrix[i][j]=0;
        }
    }

    //����·��
    for (size_t i = 0; i < t_state.m_crossvec_t.size(); i++)
    {
        Cross the_cross = t_state.m_crossvec_t[i];
        //������·
        for (size_t j = 0; j < t_state.m_roadvec_t.size(); j++)
        {
            Road the_road = t_state.m_roadvec_t[j];
            //�ҵ�·�ڶ�Ӧ�ĵ�·
            if (the_road.m_index == the_cross.m_road_index[0] ||
                    the_road.m_index == the_cross.m_road_index[1] ||
                    the_road.m_index == the_cross.m_road_index[2] ||
                    the_road.m_index == the_cross.m_road_index[3])
            {
                int road_from = the_road.m_in_cross_index;
                int road_to = the_road.m_out_cross_index;
                int road_length = the_road.m_road_length;
                int road_v = the_road.m_v_road_max;
                if(road_from==the_cross.m_index)//·��Ϊ��·���
                {
                    m_connect_matrix[road_from][road_to] = the_road.m_index;
                    double weight = road_length * 5*1.0 / (road_v*the_road.m_n_channel*(1+the_road.m_is_duplex));
                    //cout << weight << " "<<endl;
                    m_edge_weight[road_from][road_to] = weight;
                    if (the_road.m_is_duplex==1)
                    {
                        m_connect_matrix[road_to][road_from] = the_road.m_index;
                        m_edge_weight[road_to][road_from] = m_edge_weight[road_from][road_to];
                    }
                }
            }
        }
    }
    //��·�������������ʼ�����Լ�ǿ��ȡ��û����ͨ��·
    for (size_t j = 0; j < t_state.m_roadvec_t.size(); j++)
    {
        Road &the_road = t_state.m_roadvec_t[j];
        //��·�������������ʼ��
        the_road.m_cars_max_capacity = the_road.m_road_length*the_road.m_n_channel;

        int road_from = the_road.m_in_cross_index;
        int road_to = the_road.m_out_cross_index;
        if(the_road.m_is_duplex==0)
        {
            m_edge_weight[road_to][road_from] = MAX_NUM;
            m_connect_matrix[road_to][road_from]=0;
        }
    }
}

//D�㷨·���滮,����Ϊ���·��,����������grapgh���m_distance��m_preVertex��
void Graph::DijkstraSolve(int from,int *m_preVertex,int *m_distance)
{
    if(from<0){
        cout<<"D�㷨����±���󣬼���Ƿ�Ϊ-1"<<endl;
        return;
    }
    vector<bool> S;                 //�Ƿ��Ѿ���S������
    S.reserve(0);					//��ʼ��Ĭ�ϴ洢Ϊ0
    S.assign(m_crossNum+1, false);   //��ʼ�������еĽڵ㶼����S������

    for (int i = 0; i < m_crossNum; ++i)
    {
        if (m_edge_weight[from][i] < MAX_NUM)
            m_preVertex[i] = from;
        else
            m_preVertex[i] = -1;       //��ʾ����֪�����׽ڵ���ʲô
        m_distance[i] = m_edge_weight[from][i];
    }
    m_preVertex[from] = -1;

    //��̰���㷨������S�����е�ÿһ���ڵ�
    S[from] = true;          //��ʼ�ڵ����S������
    int u = from;			//����uΪ��ʼ�ڵ�
    //����ѭ����1��ʼ����Ϊ��ʼ�ڵ��Ѿ������S���ˣ�����numOfVertex-1���ڵ�Ҫ����
    for (int i = 0; i < m_crossNum; i++)
    {
        //ѡ��distance��С��һ���ڵ�
        int nextVertex = u;
        int tempDistance = MAX_NUM;
        for (int j = 0; j < m_crossNum; ++j)
        {
            //Ѱ�Ҳ���S�����е�distance��С�Ľڵ�
            if ((S[j] == false) && (m_distance[j] < tempDistance))
            {
                nextVertex = j;
                tempDistance = m_distance[j];
            }
        }
        //���ҵ��Ĳ���S�����е�distance��С�Ľڵ����S������
        S[nextVertex] = true;
        u = nextVertex;//��һ��Ѱ�ҵĿ�ʼ�ڵ�

        //����distance
        for (int j = 0; j < m_crossNum; j++)
        {
            if (S[j] == false && m_edge_weight[u][j] < MAX_NUM)
            {
                int temp = m_distance[u] + m_edge_weight[u][j];
                if (temp < m_distance[j])
                {
                    m_distance[j] = temp;
                    m_preVertex[j] = u;
                }
            }
        }
    }
}

//��㵽�յ��·���滮����������������յ��cross_index�����ص���·���滮��road_index�±꼯��
vector<int> Graph::planOneCar(int from,int to)
{
    //dijstra�㷨������
    int m_distance[800];
    int m_preVertex[800];
    memset(m_distance,0,800*sizeof(int));
    memset(m_preVertex,-1,800*sizeof(int));
    vector<int> one_route;//·���滮��road_index�±꼯��
    //car[i]��cross_idΪ�������·���滮
    DijkstraSolve(from,m_preVertex,m_distance);
    int j = to;
    int end = j;
    stack<int> trace;

    while (m_preVertex[j]!=-1)//�����׵�����ѹ��ջ��
    {
        trace.push(m_preVertex[j]);
        j = m_preVertex[j];
    }

    int cnt = 0;
    int a_road[800];//lu·������

    while (!trace.empty())
    {
        a_road[cnt++] = trace.top();
        trace.pop();
    }
    a_road[cnt] = end;
    vector<int> road_array;//һ��������ʻ·���������ŵ���road_id
    int start_cross = 0;
    while (cnt--)
    {
        int road_in= a_road[start_cross];
        int road_out= a_road[start_cross + 1];
        int a_road_index = m_connect_matrix[road_in][road_out];
        road_array.push_back(a_road_index);
        start_cross++;
    }

    for(size_t i = 0; i < road_array.size(); i++)
    {
        one_route.push_back(road_array[i]);
    }
    road_array.clear();

    return one_route;
}

//ȫ�ֹ滮
void Graph::routePlan(string answerpath,TState& t_state)
{
    cout<<"start plan"<<endl;
    //����car��˳�򣬰�����ʱ����������Ȼ���ٶȿ�������
    //sort(t_state.m_carvec_t.begin(), t_state.m_carvec_t.end(), cmp);

//    ofstream ofile;               //��������ļ�
//    ofile.open(answerpath);
    //������ѭ������ÿһ����·��
    for (size_t i = 0; i < t_state.m_carvec_t.size(); ++i)
    {
        string one_res = "";

        int bias = i/43;
        t_state.m_carvec_t[i].m_start_time_infact += bias;
        one_res+= "(" + to_string(t_state.m_carvec_t[i].m_car_id)
                + "," + to_string(t_state.m_carvec_t[i].m_start_time_infact);
        //ÿ����·���滮
        vector<int> one_route = planOneCar(t_state.m_carvec_t[i].m_start_cross_index,t_state.m_carvec_t[i].m_end_cross_index);

        for(int j=0;j<one_route.size();j++)
            one_res += ","+to_string(t_state.m_roadvec_t[one_route[j]].m_road_id);
        one_res += ")";

        //ofile << one_res << endl;

        t_state.SetPath(one_route, t_state.m_carvec_t[i].m_index);
    }
    //ofile.close();
    cout << "plan end" << endl;
}
//���µ�·�ϵĳ�����,�ִ泵����exist_cars_on_graph�����ڸ��µ���·��
void Graph::calCarNumInRoad(TState& t_state)
{
    all_cars_num_in_graph = 0;
    t_state.exist_cars_on_graph.clear();
    //������·
    for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
    {
        Road& road = t_state.m_roadvec_t[i];
        road.m_cars_num_in_pos_road=0;
        road.m_cars_num_in_neg_road=0;
        //�����·����������г���
        for(int d=0;d<=road.m_is_duplex;d++)
        {
            for (int channel = 0; channel < road.m_n_channel; channel++)
            {
                list<int>::iterator it = road.m_cars_index_in_road[d][channel].begin();
                list<int>::iterator it_end = road.m_cars_index_in_road[d][channel].end();

                for (; it != it_end; it++)
                {
                    Car &car= t_state.m_carvec_t[*it];
                    t_state.exist_cars_on_graph.push_back(car.m_index);
                    if(d==0)//����
                    {
                        road.m_cars_num_in_pos_road+=1;
                        car.in_positive_road = true;
                    }
                    else if(d==1)//����
                    {
                        road.m_cars_num_in_neg_road+=1;
                        car.in_positive_road = false;
                    }
                }
            }
        }
        all_cars_num_in_graph += road.m_cars_num_in_pos_road;
        all_cars_num_in_graph += road.m_cars_num_in_neg_road;
    }
}

//���ݵ�·�������Ȩ��
void Graph::updateGraphWeights(TState& t_state)
{
    for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
    {
        Road &road = t_state.m_roadvec_t[i];
        int from = road.m_in_cross_index;
        int to = road.m_out_cross_index;
        //����ռ������Ϊ0-1������60%��˵����·�Ƚ�ӵ���ˣ���ֵҪ����

        //�����·������,����ӵ���̶�
        for(int d=0;d<=road.m_is_duplex;d++)
        {
            if(d==0)
            {
                double rate = road.m_cars_num_in_pos_road*1.0/road.m_cars_max_capacity;
                road.m_congestion[d]=rate;
                int gain = 1;//����Ȩ�أ�ֵ������
                if (rate > 0.8)
                    gain = 100;
                else if(rate > 0.3)
                    gain=10;
                else
                    gain = 1;
                double one_weight = road.m_road_length * 5.0 / (road.m_v_road_max*road.m_n_channel*(1+road.m_is_duplex));
                m_edge_weight[from][to] = rate * gain + one_weight;
            }
            else if(d==1)
            {
                double rate = road.m_cars_num_in_neg_road*1.0 / road.m_cars_max_capacity;
                road.m_congestion[d]=rate;

                int gain = 1;//����Ȩ�أ�ֵ������
                if (rate > 0.8)
                    gain = 100;
                else if(rate > 0.3)
                    gain=10;
                else
                    gain = 1;
                double one_weight = road.m_road_length * 5.0 / (road.m_v_road_max*road.m_n_channel*(1+road.m_is_duplex));
                m_edge_weight[to][from] = rate * gain + one_weight;
            }
        }

        //        for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
        //        {
        //            Road& road = t_state.m_roadvec_t[i];
        //            cout<<"road "<<i<<" congest[0]="<<road.m_congestion[0]<<"  "
        //               <<" congest[1]="<<road.m_congestion[1]<<endl;

        //        }
    }
}

//����·���滮
void Graph::updateRoutePlan(TState& t_state)
{
    for (int i = 0; i < t_state.exist_cars_on_graph.size(); i++)
    {
        Car &car= t_state.m_carvec_t[t_state.exist_cars_on_graph[i]];
        //�ҵ�����������ʻ��·����һ��·�ڣ������Ѿ�����������
        Road &road = t_state.m_roadvec_t[car.m_current_road];
        Road &next_road=t_state.m_roadvec_t[car.m_path[car.m_current_path_index+1]];

        int next_cross_index;
        //�жϵ�ǰ�������ڵ�ǰ��·�����Ƿ����������
        if (car.in_positive_road == true)
            next_cross_index = road.m_out_cross_index;
        else//����
            next_cross_index = road.m_in_cross_index;

        if(next_cross_index==car.m_end_cross_index)
            continue;

        //����·�ڵĳ����������µ���
        if((car.m_current_s0+min(car.m_v_max,road.m_v_road_max))<road.m_road_length)
            continue;

        //������һ��·��ӵ������ж��Ƿ���Ҫ����·��
        int dir_in_next_road=0;

        if(next_road.m_in_cross_index==next_cross_index)
            dir_in_next_road=0;//��
        else
            dir_in_next_road=1;//��

        if(next_road.m_congestion[dir_in_next_road]<0.5)
            continue;//�¸�·����ӵ�£��Ͳ����µ�����

        vector<int> new_path=planOneCar(next_cross_index, car.m_end_cross_index);

        //���µ��ȣ������¸�·�ڷ���滮
        if(new_path[0]==car.m_current_road)
            continue;

        //��ԭm_path����·���滻Ϊnew_path
        int k=car.m_current_path_index;//���߹����±꣬��0��ʼ
        while(car.m_path.size()>k+1)
            car.m_path.pop_back();

        for (int k = 0; k < new_path.size(); k++)
        {
            car.m_path.push_back(new_path[k]);
        }

    }
}
