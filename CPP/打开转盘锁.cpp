#include<iostream>
#include<algorithm>
#include <string>
#include <vector>
#include <unordered_set>
#include <queue>
using namespace std;
class Solution {
public:
    int openLock(vector<string>& deadends, string target) {
        //���ʾ��Ǹ��Թ����������⣬ֻ��������λ�ò��������������ĸ�λ�ã�
        //������λ����ÿ������һ��һ���ܹ��а˸����ڵ�λ�ã��ù�����ȱ���BFS
        int res = 0;
        unordered_set<string> deadlock(deadends.begin(), deadends.end());
        if (deadlock.count("0000")) return -1;//deadlock������0000��ֱ�ӷ���-1
        unordered_set<string> visited{{"0000"}};
        queue<string> q{{"0000"}};
        while (!q.empty()) {
            ++res;//��res����¼BFS�����Ĳ���
            //������һ��qջ�е�ֵ������ȡ��������Χ�˸�λ��
            for (int k = q.size(); k > 0; --k) {
                auto t = q.front(); q.pop();
                //������λ���ֵ�ÿһλ��Ȼ��ֱ��1��1��������j��-1������1������0������Ҳ����ʵ���˼�1��1�Ĺ���
                for (int i = 0; i < 4; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        if (j == 0) continue;
                        string str = t;
                        //Ϊ�˴���9��1��0����0��1��9�����
                        //����ͳһ����λ���ּ��ϸ�10��Ȼ���ټӻ��1������ٶ�10ȡ�༴��
                        str[i] = ((t[i] - '0') + 10 + j) % 10 + '0';
                        //�����ʱ�����ɵ��ַ�������target�ˣ�ֱ�ӷ��ؽ��res
                        if (str == target) return res;
                        //������ַ������������������֮ǰû�б������������ظ����������������queue�У�������һ������
                        if (!visited.count(str) && !deadlock.count(str))
                            q.push(str);
                        //�ٽ����ַ�������visited������
                        visited.insert(str);
                    }
                }
            }
        }
        return -1;
    }
}s;
int main()
{
    string str[]={"0201","0101","0102","1212","2002"};

    vector<string>deadends(str,str+5);
    string target("0202");
    s.openLock(deadends,target);
    return 0;
}
