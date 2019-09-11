
//典型01背包问题
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
class Solution{
public:
    void bagProblem(int space,vector<int> v,vector<int> h){
        if(space<0)
            return;
        //申请二维动态数组，dp[i][j]表示到当前第i件物品为止，对于总重价格j的结果
        int dp[v.size()][space+1]={0};
        //i=0时候要进行初始化，必要；表示只有第一件物品时最大价值是多少
        //拥有的space小于第一件物品的时候最大价值为0，拥有space大于第一件物品的体积的时候最大价值为第一件物品的价值
        for(int j=0;j<=space;j++){
            if(j>=h[0])
                dp[0][j]=v[0];
        }
        //遍历物品，分每个物品能放下的情况和放不下的情况
        for(size_t i=1;i<v.size();i++){
            for(int j=0;j<=space;j++){
                if(h[i]>j)//放不下的情况
                    dp[i][j]=dp[i-1][j];
                else//放的下的时候，在之前的space刚好够放之前的所有物品的情况下加上当前物品的价值，与放之前对比取较大值
                    dp[i][j]=max(dp[i-1][j-h[i]]+v[i],dp[i-1][j]);
            }
        }
        for(size_t i=0;i<v.size();i++){
            for(int j=0;j<=space;j++){
                cout<<dp[i][j]<<" ";
            }
            cout<<endl;
        }
        cout<<dp[v.size()-1][space];
    }
};
int main(){
    int space=10;
    vector<int> v={2,6,1,1,3,4};
    vector<int> h={6,10,3,4,5,8};
    Solution so;
    so.bagProblem(space,v,h);
    return 0;
}
