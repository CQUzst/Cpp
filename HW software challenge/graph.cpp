#include "graph.h"
#include "t_state.h"
using namespace std;

Graph::Graph()
{
}

Graph::~Graph()
{
}

//读入cross数据，返回数据个数
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
        if(str.size()!=0&&str[0]!='#')//逐行读入数据
        {
            Cross one_cross;
            int data[5];
            int j=0;//数组下标
            for(size_t i=0;i<str.size();i++)
            {
                int num=0;//数字
                bool flag=0;
                while(isdigit(str[i]))//连续读入数字
                {
                    flag=1;
                    num=num*10+str[i]-'0';
                    i++;
                }
                if(flag)//将数字赋值给数组，方便初始化
                    data[j++]=num;
            }
            //cout <<"cross"<< data[0] << " " << data[1] << " " << data[2] << " " << data[3]<< " " << data[4]<< endl;
            //类初始化
            one_cross.buildCross(data[0],data[1],data[2],data[3],data[4]);
            newState.m_crossvec_t.push_back(one_cross);
        }
    }
    infile.close();

    newState.SortCrossByID();
    return newState.m_crossvec_t.size();
}

//读入road数据，返回数据个数
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
        if(str.size()!=0)//逐行读入数据
        {
            Road one_road;
            int data[8];
            int j=0;//数组下标
            for(size_t i=0;i<str.size();i++)
            {
                int num=0;//数字
                bool flag=0;
                while(isdigit(str[i]))//连续读入数字
                {
                    flag=1;
                    num=num*10+str[i]-'0';
                    i++;
                }
                if(flag)//将数字赋值给数组，方便初始化
                    data[j++]=num;
            }
            //cout<<data[0]<<" "<<data[1]<<" "<<data[2]<<" "<<data[3]<<" "<<data[4]<<" "<<data[5]<<" "<<data[6]<<endl;
            //类初始化
            one_road.buildRoad(data[0],data[1],data[2],
                    data[3],data[4],data[5],data[6]);
            newState.m_roadvec_t.push_back(one_road);
        }
    }
    infile.close();

    newState.SortRoadByID();
    return newState.m_roadvec_t.size();
}

//读入car数据，返回数据个数
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
        if(str.size()!=0)//逐行读入数据
        {
            Car one_car;
            int data[6];
            int j=0;//数组下标
            for(size_t i=0;i<str.size();i++)
            {
                int num=0;//数字
                bool flag=0;
                while(isdigit(str[i]))//连续读入数字
                {
                    flag=1;
                    num=num*10+str[i]-'0';
                    i++;
                }
                if(flag)//将数字赋值给数组，方便初始化
                    data[j++]=num;
            }
            //类初始化
            one_car.buildCar(data[0],data[1],data[2],data[3],data[4]);
            newState.m_carvec_t.push_back(one_car);
        }
    }
    infile.close();

    newState.SortCarByID();
    return newState.m_carvec_t.size();
}

//t_state读入所有信息
void Graph::readData(string crossPath,string roadPath,string carPath,TState& t_state)
{
    m_crossNum=readCross(crossPath, t_state);
    m_roadNum=readRoad(roadPath, t_state);
    m_carNum=readCar(carPath, t_state);

    t_state.ChangeIdToIndex();
}

//建图，完成一个邻接矩阵和一个权值矩阵
void Graph::buildGraph(int crossNum,TState& t_state)
{
    //初始化邻接矩阵和edge矩阵
    //初始化edge，使得里面的权值都是最大值
    for (int i = 0; i < crossNum; i++)
    {
        for (int j = 0; j < crossNum; j++)
        {
            m_edge_weight[i][j] = MAX_NUM;
            m_connect_matrix[i][j]=0;
        }
    }

    //遍历路口
    for (size_t i = 0; i < t_state.m_crossvec_t.size(); i++)
    {
        Cross the_cross = t_state.m_crossvec_t[i];
        //遍历道路
        for (size_t j = 0; j < t_state.m_roadvec_t.size(); j++)
        {
            Road the_road = t_state.m_roadvec_t[j];
            //找到路口对应的道路
            if (the_road.m_index == the_cross.m_road_index[0] ||
                    the_road.m_index == the_cross.m_road_index[1] ||
                    the_road.m_index == the_cross.m_road_index[2] ||
                    the_road.m_index == the_cross.m_road_index[3])
            {
                int road_from = the_road.m_in_cross_index;
                int road_to = the_road.m_out_cross_index;
                int road_length = the_road.m_road_length;
                int road_v = the_road.m_v_road_max;
                if(road_from==the_cross.m_index)//路口为道路入口
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
    //道路最大汽车容量初始化，以及强制取消没有连通道路
    for (size_t j = 0; j < t_state.m_roadvec_t.size(); j++)
    {
        Road &the_road = t_state.m_roadvec_t[j];
        //道路最大汽车容量初始化
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

//D算法路径规划,输入为起点路口,保存数据在grapgh类的m_distance和m_preVertex中
void Graph::DijkstraSolve(int from,int *m_preVertex,int *m_distance)
{
    if(from<0){
        cout<<"D算法起点下标错误，检查是否为-1"<<endl;
        return;
    }
    vector<bool> S;                 //是否已经在S集合中
    S.reserve(0);					//初始化默认存储为0
    S.assign(m_crossNum+1, false);   //初始化，所有的节点都不在S集合中

    for (int i = 0; i < m_crossNum; ++i)
    {
        if (m_edge_weight[from][i] < MAX_NUM)
            m_preVertex[i] = from;
        else
            m_preVertex[i] = -1;       //表示还不知道父亲节点是什么
        m_distance[i] = m_edge_weight[from][i];
    }
    m_preVertex[from] = -1;

    //用贪心算法处理不在S集合中的每一个节点
    S[from] = true;          //开始节点放入S集合中
    int u = from;			//定义u为开始节点
    //这里循环从1开始是因为开始节点已经存放在S中了，还有numOfVertex-1个节点要处理
    for (int i = 0; i < m_crossNum; i++)
    {
        //选择distance最小的一个节点
        int nextVertex = u;
        int tempDistance = MAX_NUM;
        for (int j = 0; j < m_crossNum; ++j)
        {
            //寻找不在S集合中的distance最小的节点
            if ((S[j] == false) && (m_distance[j] < tempDistance))
            {
                nextVertex = j;
                tempDistance = m_distance[j];
            }
        }
        //将找到的不在S集合中的distance最小的节点放入S集合中
        S[nextVertex] = true;
        u = nextVertex;//下一次寻找的开始节点

        //更新distance
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

//起点到终点的路径规划，输入参数是起点和终点的cross_index，返回的是路径规划的road_index下标集合
vector<int> Graph::planOneCar(int from,int to)
{
    //dijstra算法用数据
    int m_distance[800];
    int m_preVertex[800];
    memset(m_distance,0,800*sizeof(int));
    memset(m_preVertex,-1,800*sizeof(int));
    vector<int> one_route;//路径规划的road_index下标集合
    //car[i]以cross_id为起点的最短路径规划
    DijkstraSolve(from,m_preVertex,m_distance);
    int j = to;
    int end = j;
    stack<int> trace;

    while (m_preVertex[j]!=-1)//将父亲点依次压入栈中
    {
        trace.push(m_preVertex[j]);
        j = m_preVertex[j];
    }

    int cnt = 0;
    int a_road[800];//lu路径长度

    while (!trace.empty())
    {
        a_road[cnt++] = trace.top();
        trace.pop();
    }
    a_road[cnt] = end;
    vector<int> road_array;//一辆车的行驶路径，里面存放的是road_id
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

//全局规划
void Graph::routePlan(string answerpath,TState& t_state)
{
    cout<<"start plan"<<endl;
    //调整car的顺序，按发车时间重新排序，然后按速度快慢排序
    //sort(t_state.m_carvec_t.begin(), t_state.m_carvec_t.end(), cmp);

//    ofstream ofile;               //定义输出文件
//    ofile.open(answerpath);
    //下面是循环遍历每一辆车路径
    for (size_t i = 0; i < t_state.m_carvec_t.size(); ++i)
    {
        string one_res = "";

        int bias = i/43;
        t_state.m_carvec_t[i].m_start_time_infact += bias;
        one_res+= "(" + to_string(t_state.m_carvec_t[i].m_car_id)
                + "," + to_string(t_state.m_carvec_t[i].m_start_time_infact);
        //每辆车路径规划
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
//更新道路上的车辆数,现存车放入exist_cars_on_graph，用于更新调度路径
void Graph::calCarNumInRoad(TState& t_state)
{
    all_cars_num_in_graph = 0;
    t_state.exist_cars_on_graph.clear();
    //遍历道路
    for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
    {
        Road& road = t_state.m_roadvec_t[i];
        road.m_cars_num_in_pos_road=0;
        road.m_cars_num_in_neg_road=0;
        //处理道路正反向的所有车道
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
                    if(d==0)//正向
                    {
                        road.m_cars_num_in_pos_road+=1;
                        car.in_positive_road = true;
                    }
                    else if(d==1)//反向
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

//根据道路情况更新权重
void Graph::updateGraphWeights(TState& t_state)
{
    for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
    {
        Road &road = t_state.m_roadvec_t[i];
        int from = road.m_in_cross_index;
        int to = road.m_out_cross_index;
        //汽车占有容量为0-1，大于60%，说明道路比较拥堵了，阈值要调整

        //处理道路正反向,评价拥挤程度
        for(int d=0;d<=road.m_is_duplex;d++)
        {
            if(d==0)
            {
                double rate = road.m_cars_num_in_pos_road*1.0/road.m_cars_max_capacity;
                road.m_congestion[d]=rate;
                int gain = 1;//增大权重，值待调整
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

                int gain = 1;//增大权重，值待调整
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

//更新路径规划
void Graph::updateRoutePlan(TState& t_state)
{
    for (int i = 0; i < t_state.exist_cars_on_graph.size(); i++)
    {
        Car &car= t_state.m_carvec_t[t_state.exist_cars_on_graph[i]];
        //找到车辆正在行驶道路的下一个路口，这里已经区分正反向
        Road &road = t_state.m_roadvec_t[car.m_current_road];
        Road &next_road=t_state.m_roadvec_t[car.m_path[car.m_current_path_index+1]];

        int next_cross_index;
        //判断当前车辆是在当前道路正向还是反向，正向情况
        if (car.in_positive_road == true)
            next_cross_index = road.m_out_cross_index;
        else//反向
            next_cross_index = road.m_in_cross_index;

        if(next_cross_index==car.m_end_cross_index)
            continue;

        //不过路口的车不考虑重新调度
        if((car.m_current_s0+min(car.m_v_max,road.m_v_road_max))<road.m_road_length)
            continue;

        //根据下一道路的拥堵情况判断是否需要更新路径
        int dir_in_next_road=0;

        if(next_road.m_in_cross_index==next_cross_index)
            dir_in_next_road=0;//正
        else
            dir_in_next_road=1;//反

        if(next_road.m_congestion[dir_in_next_road]<0.5)
            continue;//下个路径不拥堵，就不重新调度了

        vector<int> new_path=planOneCar(next_cross_index, car.m_end_cross_index);

        //更新调度：避免下个路口反向规划
        if(new_path[0]==car.m_current_road)
            continue;

        //将原m_path后续路径替换为new_path
        int k=car.m_current_path_index;//车走过的下标，从0开始
        while(car.m_path.size()>k+1)
            car.m_path.pop_back();

        for (int k = 0; k < new_path.size(); k++)
        {
            car.m_path.push_back(new_path[k]);
        }

    }
}
