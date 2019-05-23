#include"t_state.h"
#include"graph.h"
#include"test.h"

Graph the_graph;

int T_UpdateTState()
{
	Graph the_graph;
	TState t_state;
	//readData读入数据，存入t_state中,并保存数据大小
	std::string carpath = "C://Users//whliz//Desktop//1//car.txt";
	std::string roadpath = "C://Users//whliz//Desktop//1//road.txt";
	std::string crosspath = "C://Users//whliz//Desktop//1//cross.txt";
	std::string answerpath = "C://Users//whliz//Desktop//1//answer.txt";

	the_graph.readData(crosspath, roadpath, carpath, t_state);


	vector<vector<int>> path;
	int n_car = 5;
	vector<int> car_path1 = { 501, 514, 505, 519, 523, 512 };
	vector<int> car_path2 = { 501, 514, 505, 519, 523, 512 };
	vector<int> car_path3 = { 501, 514, 505, 519, 523, 512 };
	vector<int> car_path4 = { 514, 518, 522, 510 };
	vector<int> car_path5 = { 503, 515, 519, 523 };
	vector<vector<int>> car_path;
	car_path.push_back(car_path1);
	car_path.push_back(car_path2);
	car_path.push_back(car_path3);
	car_path.push_back(car_path4);
	car_path.push_back(car_path5);

	t_state.ChangePathIdToIndex(car_path);
	t_state.SetPath(car_path);

	int t = 1;//t时刻开始调度
	while (1)
	{
		if (t_state.UpdateTState(t) == 1)
			break;
		
		for (int i = 0; i < 5; ++i)
		{
			if (t_state.m_carvec_t[i].m_current_path_index == -1)
			{
				continue;
			}
			int temp_road_index = t_state.m_carvec_t[i].m_path[t_state.m_carvec_t[i].m_current_path_index];
			cout << "car id: " << t_state.m_carvec_t[i].m_car_id << " "
				<< "current road index: " << temp_road_index << " "
				<< "current road: " << t_state.m_roadvec_t[temp_road_index].m_road_id << " "
				<< "current time:  " << t << endl;

		}
		t++;
	}

	cout<<"CheckWaitingCar"<<t_state.CheckWaitingCar() << endl;

	//计算调度时间、运行时间
	return -1;
}

int T_UpdateCross()
{
	return -1;
}

void T_buildRoad()
{
	Road r1;
	r1.buildRoad(5, 5, 5, 5, 5, 5, 1);
	r1.m_cars_index_in_road[0][4].end();
	r1.m_cars_index_in_road[1][4].end();

	//单向
	Road r2;
	r2.buildRoad(5, 5, 5, 5, 5, 5, 0);
	r2.m_cars_index_in_road[0][4].end();

}

void T_VName()
{
	int abc = 0;
	cout << VName(abc) << endl;
}
void TestDeadLock(TState& t_state)
{
	//测试数据


	//测试函数：
	t_state.UpdateCrossOnce(1);
}


void TState::T_WriteAndReadFile()
{
	std::string logpath = "C://Users//whliz//Desktop//1//log.txt";
	std::string logpath1 = "C://Users//whliz//Desktop//1//log1.txt";
	//czt add ,添加日志文件log
	WriteInfoINLog(logpath);
	ReadInfoINLog(logpath);
	WriteInfoINLog(logpath1);

	//测试方法：利用Beyond Compare对比log和log1，是否一致。判断是否读写正确。
}
void RunTest()
{
	char i = getchar();
	switch (i)
	{
	case'0':
		T_UpdateTState();
		break;
	case '1':
		T_VName();
		break;
	default:
		break;
	}
}
