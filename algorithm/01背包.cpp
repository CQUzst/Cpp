/*
 * vivo2019提前批笔试第三题：
 * 小v负责一次活动礼品采购，每一款礼品的受欢迎程度(热度值)各不相同，现给出总金额以及各个礼品的单价
 * 和热度值，且每个礼品只购买一个，如何购买可以使得所有礼品的总热度值最高。
 * 输入：
 * 第一行是一个正整数，表示总金额(不大于1000)
 * 第二行是一个长度为n的正整数数组，表示礼品单价(n不大于100)
 * 第三行是一个长度为n的正整数数组，表示对应礼品的热度值
 * 输出：
 * 一个正整数，表示可以获得的最高总热度值
 *
 * 样例输入：1000
 *           200 600 100 180 300 450
 *           6 10 3 4 5 8
 * 样例输出：21
 */
//典型01背包问题
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
class Solution{
public:
    void bagProblem(int money,vector<int> v,vector<int> h){
        if(money<0)
            return;
        //申请二维动态数组，dp[i][j]表示到当前第i件物品为止，对于总重价格j的结果
        int dp[v.size()][money+1]={0};
        //i=0时候要进行初始化，必要；表示只有第一件物品时最大价值是多少
        //拥有的钱小于第一件物品的时候最大价值为0，拥有价格大于第一件物品的时候最大价值为第一件物品的价值
        for(int j=0;j<=money;j++){
            if(j>=v[0])
                dp[0][j]=h[0];
        }
        //遍历物品，分每个物品能放下的情况和放不下的情况
        for(size_t i=1;i<v.size();i++){
            for(int j=0;j<=money;j++){
                if(v[i]>j)//放不下的情况
                    dp[i][j]=dp[i-1][j];
                else//放的下的时候，在之前的钱刚好够放之前的所有物品的情况下加上当前物品的重量，与放之前对比取较大值
                    dp[i][j]=max(dp[i-1][j-v[i]]+h[i],dp[i-1][j]);
            }
        }
        for(size_t i=0;i<v.size();i++){
            for(int j=0;j<=money;j++){
                cout<<dp[i][j]<<" ";
            }
            cout<<endl;
        }
        cout<<dp[v.size()-1][money];
    }
};
int main(){
    int money=10;
    vector<int> v={2,6,1,1,3,4};
    vector<int> h={6,10,3,4,5,8};
    Solution so;
    so.bagProblem(money,v,h);
    return 0;
}
