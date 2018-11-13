/*ͼ�����������Ż��㷨���е�Dijkstra�㷨���裺
S���������ñ�ŵĶ��㼯��
l(v):v�ı�ǣ�
f(v):v�ĸ��ף�����ȷ�����·����������
�����Ȩͼ�Ĵ�Ȩ�ڽӾ���Wn*n=(w(u,v))n*n.
1) l(v)��ֵ0������v������v0��l��v�����������S����v0��u=v0��
2) ����v����s1=V-S����l(v)>l(u)+w(u,v),��l(v)=l(u)+w(u,v),f(v)=u��
3) ��v*��ʹl(v)ȡ��Сֵ��s1�еĶ��㣬S�м���v*��u=v*��
4) ��S1��Ϊ�գ�ת2)������ֹͣ*/ 
#include <iostream>  
#include <vector>  
#include <stack>  

using namespace std;
int k = INT_MAX;//INT_MAX=2147483647
int map[6][6] = 
{    //�����ڽӶ�������ͼ����k=INT_MAX����������岻��������  
	{ 0,	50,		k,		40,		25,		10 },
	{ 50,	0,		15,		20,		k,		25 },
	{ k,	15,		0,		10,		20,		k },
	{ 40,	20,		10,		0,		10,		25 },
	{25,	k ,		20,		10,		0,		55},
	{ 10,	25,		k,		 25,	55,		0 }
};
//����Dijkstra����
void Dijkstra(
	const int Vertex,    //�ڵ���Ŀ
	const int startVertex,    //Դ�ڵ�
	int(map)[6][6],            //����ͼ�ڽӾ���
	int *distance,            //�����ڵ㵽��Դ�ڵ�ľ���
	int *prevVertex)           //�����ڵ�ĸ��׽ڵ�
{
	vector<bool> S;                 //�Ƿ��Ѿ���S������  
	S.reserve(0);					//��ʼ��Ĭ�ϴ洢Ϊ0
	S.assign(Vertex, false);   //��ʼ�������еĽڵ㶼����S������  
	
	//��ʼ��distance��prevVertex����
	for (int i = 0; i < Vertex; ++i)
	{
		distance[i] = map[startVertex][i];
		if (map[startVertex][i] < k)
			prevVertex[i] = startVertex;
		else
			prevVertex[i] = -1;       //��ʾ����֪�����׽ڵ���ʲô  
	}
	prevVertex[startVertex] = -1;

	//��̰���㷨������S�����е�ÿһ���ڵ�
	S[startVertex] = true;          //��ʼ�ڵ����S������  
	int u = startVertex;			//����uΪ��ʼ�ڵ�
	//����ѭ����1��ʼ����Ϊ��ʼ�ڵ��Ѿ������S���ˣ�����numOfVertex-1���ڵ�Ҫ����
	for (int i = 1; i < Vertex; i++)       
	{
		//ѡ��distance��С��һ���ڵ�
		int nextVertex = u;
		int tempDistance = k;
		for (int j = 0; j < Vertex; ++j)
		{	
			//Ѱ�Ҳ���S�����е�distance��С�Ľڵ�
			if ((S[j] == false) && (distance[j] < tempDistance))  
			{
				nextVertex = j;
				tempDistance = distance[j];
			}
		}
		//���ҵ��Ĳ���S�����е�distance��С�Ľڵ����S������
		S[nextVertex] = true;  
		u = nextVertex;//��һ��Ѱ�ҵĿ�ʼ�ڵ�  

		//����distance
		for (int j = 0; j < Vertex; j++)
		{
			if (S[j] == false && map[u][j] < INT_MAX)
			{
				int temp = distance[u] + map[u][j];
				if (temp < distance[j])
				{
					distance[j] = temp;
					prevVertex[j] = u;
				}
			}
		}
	}
}

int main(int argc, const char * argv[])
{
	int distance[6];
	int preVertex[6];
	int final[6];
	for (int i = 0; i < 6; ++i)
	{
		//���iΪ���Ľ��
		Dijkstra(6, i, map, distance, preVertex);
		for (int j = 0; j < 6; ++j)
		{
			int index = j;
			stack<int> trace;
			while (preVertex[index] != -1)//�����׵�����ѹ��ջ��
			{
				trace.push(preVertex[index]);
				index = preVertex[index];
			}
			cout << "��"<<i<<"��ʼ��"<<j<<"�����·���ǣ�";
			while (!trace.empty()) 
			{
				cout << trace.top() << "--";
				trace.pop();
			}
			cout << j;
			cout << "\t\t�����ǣ�" << distance[j] << endl;
		}
		int total = 0;
		for (int j = 0; j < 6; ++j)
		{
			total += distance[j];
		}
		cout << "��" << i << "��ʼ���������еط������·��֮���ǣ�"<<total;
		final[i] = total;
		cout << endl<<endl;
	}
	system("pause");
	return 0;
}