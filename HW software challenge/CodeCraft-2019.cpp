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
	//readData�������ݣ�����t_state��,���������ݴ�С
//    std::string carpath = "E://HW2019//map1//car.txt";
//    std::string roadpath = "E://HW2019//map1//road.txt";
//    std::string crosspath = "E://HW2019//map1//cross.txt";
//    std::string answerpath = "E://HW2019//map1//answer.txt";




	//readData�������ݣ�����t_state��,���������ݴ�С
	the_graph.readData(crosspath, roadpath, carpath, t_state);

	//��ͼ��graph����t_state�����ڽӾ����Ȩֵ����
	the_graph.buildGraph(the_graph.m_crossNum, t_state);

	//����t_state����·���滮
	the_graph.routePlan(answerpath, t_state);
	
	int t = 1;//tʱ�̿�ʼ����
	int err = 0;

    int reach_car_nums=0;

	cout << endl << "time=" << t << endl;
	while (1)
	{
        cout<<"1test"<<endl;
		//����Tʱ�����е�·�ϳ���״̬
		err = t_state.UpdateTState(t);

        //���µ�ǰÿһ����·�ϵĳ���������¼��road��Ա�����У�int m_cars_num_in_(pos/neg)_road;
        the_graph.calCarNumInRoad(t_state);

        cout<<"ȫͼ���г���="<<the_graph.all_cars_num_in_graph<<endl;
        all_cars_num=the_graph.all_cars_num_in_graph;
        //����Ȩ��
        the_graph.updateGraphWeights(t_state);

        //��̬�滮
        the_graph.updateRoutePlan(t_state);

//        //�����ǰʱ�̵Ķ�̬�滮���·��
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
            cout<<"�ȴ���������,����500"<<endl;
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
            //�����ǰʱ�̵Ķ�̬�滮���·��
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
			cout << "�ɹ���ȫ�������յ�" << endl;
			break;
		}
		else
		{
			if (err == -1)
			{
				cout << "��������" << endl;
			}

			////���µ�ǰÿһ����·�ϵĳ���������¼��road��Ա�����У�int m_cars_num_in_(pos/neg)_road;
			//the_graph.calCarNumInRoad(t_state);
			////�����·�ϵĳ������۲�
			///*cout << "�����·�ϵĳ�����";
			//for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
			//{
			//cout << t_state.m_roadvec_t[i].m_cars_num_in_pos_road << " ";
			//}
			//cout << endl << "�����·�ϵĳ�����";
			//for (int i = 0; i < t_state.m_roadvec_t.size(); i++)
			//{
			//cout << t_state.m_roadvec_t[i].m_cars_num_in_neg_road << " ";
			//}*/

			////���ݵ�·ӵ��������µ�·Ȩ��
			//the_graph.updateGraphWeights(t_state);

			////���Ȩֵ����۲�
			///*for (int i = 0; i < the_graph.m_crossNum; i++)
			//{
			//for (int j = 0; j < the_graph.m_crossNum; j++)
			//{
			//cout<<the_graph.m_edge_weight[i][j]<<" ";
			//}
			//cout << endl;
			//}*/
			////���µ�·�ϳ�������·��
			//the_graph.updateRoutePlan(t_state);



			//cout << "all_cars_num_in_graph="<<the_graph.all_cars_num_in_graph << endl;
		}
		t++;
		cout << endl << "time=" << t << endl;
	}

	//�������ʱ�䡢����ʱ��


	//RunTest();
	system("pause");

    return 0;
}
