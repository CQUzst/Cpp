#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include "graph.h"
#include "t_state.h"
using namespace std;

int all_cars_num=0;
void RunTest();

int main(int argc, char *argv[])
{
    Graph the_graph;
    TState t_state;

    std::cout << "Begin" << std::endl;
    if(argc < 5){
        std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
        exit(1);
    }
    std::string carpath=argv[1];
    std::string roadpath=argv[2];
    std::string crosspath = argv[3];
    std::string answerpath = argv[4];
	//readData读入数据，存入t_state中,并保存数据大小
//    std::string carpath = "E://HW2019//map1//car.txt";
//    std::string roadpath = "E://HW2019//map1//road.txt";
//    std::string crosspath = "E://HW2019//map1//cross.txt";
//    std::string answerpath = "E://HW2019//map1//answer.txt";




	//readData读入数据，存入t_state中,并保存数据大小
	the_graph.readData(crosspath, roadpath, carpath, t_state);

	//建图，graph利用t_state建立邻接矩阵和权值矩阵
	the_graph.buildGraph(the_graph.m_crossNum, t_state);

	//利用t_state进行路径规划
	the_graph.routePlan(answerpath, t_state);
	
	int t = 1;//t时刻开始调度
	int err = 0;

    int reach_car_nums=0;

	cout << endl << "time=" << t << endl;
	while (1)
	{
        cout<<"1test"<<endl;
		//更新T时刻所有道路上车的状态
		err = t_state.UpdateTState(t);

        //更新当前每一个道路上的车辆数，记录在road成员函数中：int m_cars_num_in_(pos/neg)_road;
        the_graph.calCarNumInRoad(t_state);

        cout<<"全图现有车辆="<<the_graph.all_cars_num_in_graph<<endl;
        all_cars_num=the_graph.all_cars_num_in_graph;
        //更新权重
        the_graph.updateGraphWeights(t_state);

        //动态规划
        the_graph.updateRoutePlan(t_state);

//        //输出当前时刻的动态规划后的路径
//        ofstream ofile;
//        ofile.open("/home/zst/map1/dp.txt");
//        for(int k=0;k<t_state.m_carvec_t.size();k++)
//        {
//            string new_plan="";
//            Car car=t_state.m_carvec_t[k];
//            new_plan+= "(" + to_string(car.m_car_id)
//                    + "," + to_string(car.m_start_time_infact);
//            for(int l=0;l<car.m_path.size();l++)
//            {
//                new_plan+=","+to_string(t_state.m_roadvec_t[car.m_path[l]].m_road_id);
//            }
//            new_plan+=")";
//            ofile<<new_plan<<endl;
//        }

        int deadlock_num=t_state.m_deadlock_carvec_t.size();
        //cout<<"deadlock_num="<<deadlock_num<<endl;
        if(deadlock_num>500)
        {
            cout<<"等待车辆过多,大于500"<<endl;
            //break;
        }
        for (int i = 0; i < t_state.m_carvec_t.size(); ++i)
        {
            if (t_state.m_carvec_t[i].m_current_path_index == -1)
            {
                continue;
            }

			//			cout << "current s0: " << t_state.m_carvec_t[i].m_current_s0 << " ";
			//			cout << "current road: " << t_state.m_roadvec_t[temp_road_index].m_road_id << " ";


			//			if (t_state.m_carvec_t[i].m_start_time_infact == t)
			//			{
			//				cout << "lauched time:" << t << " ";
			//			}
			//			cout << endl;
			//	}

			//}
			//else
			//{
				if (t_state.m_carvec_t[i].m_reach_end_t == t)
				{
                    reach_car_nums++;
					int temp_road_index = t_state.m_carvec_t[i].m_path[t_state.m_carvec_t[i].m_current_path_index];
					cout << "car id: " << t_state.m_carvec_t[i].m_car_id << " ";
					//<< "current road index: " << temp_road_index << " ";
					if (t_state.m_carvec_t[i].m_start_time_infact == t)
					{
						cout << "lauched time:" << t << " ";
					}
					cout << "current s0: " << t_state.m_carvec_t[i].m_current_s0 << " ";
					cout << "current road: " << t_state.m_roadvec_t[temp_road_index].m_road_id << " ";

					cout << "reach time:" << t << " ";
                    cout<<"reach cars="<<reach_car_nums<<" ";
					cout << endl;
				}

			//}

		}
		if (err == 1)
		{
            //输出当前时刻的动态规划后的路径
            ofstream ofile;
            ofile.open(answerpath);
            for(int k=0;k<t_state.m_carvec_t.size();k++)
            {
                string new_plan="";
                Car car=t_state.m_carvec_t[k];
                new_plan+= "(" + to_string(car.m_car_id)
                        + "," + to_string(car.m_start_time_infact);
                for(int l=0;l<car.m_path.size();l++)
                {
                    new_plan+=","+to_string(t_state.m_roadvec_t[car.m_path[l]].m_road_id);
                }
                new_plan+=")";
                ofile<<new_plan<<endl;
            }
			cout << "成功，全部到达终点" << endl;
			break;
		}
		else
		{
			if (err == -1)
			{
				cout << "发生死锁" << endl;
			}

			////更新当前每一个道路上的车辆数，记录在road成员函数中：int m_cars_num_in_(pos/neg)_road;
			//the_graph.calCarNumInRoad(t_state);
			////输出道路上的车辆数观察
			///*cout << "正向道路上的车辆数";
			//for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
			//{
			//cout << t_state.m_roadvec_t[i].m_cars_num_in_pos_road << " ";
			//}
			//cout << endl << "反向道路上的车辆数";
			//for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
			//{
			//cout << t_state.m_roadvec_t[i].m_cars_num_in_neg_road << " ";
			//}*/

			////根据道路拥挤情况更新道路权重
			//the_graph.updateGraphWeights(t_state);

			////输出权值网络观察
			///*for (int i = 0; i < the_graph.m_crossNum; i++)
			//{
			//for (int j = 0; j < the_graph.m_crossNum; j++)
			//{
			//cout<<the_graph.m_edge_weight[i][j]<<" ";
			//}
			//cout << endl;
			//}*/
			////更新道路上车辆的新路径
			//the_graph.updateRoutePlan(t_state);



			//cout << "all_cars_num_in_graph="<<the_graph.all_cars_num_in_graph << endl;
		}
		t++;
		cout << endl << "time=" << t << endl;
	}

	//计算调度时间、运行时间


	//RunTest();
	system("pause");

    return 0;
}
