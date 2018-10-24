#include <iostream>
#include <vector>
using namespace std;
class Solution {
public:
    vector<vector<string> > solveNQueens(int n) {
        vector<vector<string> > res;
        vector<string>cur(n, string(n,'.'));
        helper(cur, 0,res);
        for(size_t i=0;i<res.size();i++){
            for(size_t j=0;j<res[i].size();j++){
                cout<<res[i][j]<<endl;
            }
            cout<<endl;
        }
        return res;
    }
    void helper(vector<string> &cur, int row,vector<vector<string> > &res)
    {
        if(row == cur.size())//N���ʺ��Ѿ��ں�
        {
            res.push_back(cur);//����һ����
            return;
        }
        for(int col = 0; col < cur.size(); col++)//��γ��Ե�k���ʺ��λ��
            if(isValid(cur, row, col)){//λ�úϷ��ͽ�����һ�����
                cur[row][col] = 'Q';
                helper(cur, row+1,res);
                cur[row][col] = '.';//�ݹ�ľ��裬������һ��ݹ�ǰ�ı䶯Ҫ����ԭ״
            }
    }
    bool isValid(vector<string> &cur, int row, int col)
    {
        //�ų����ظ�
        for(int i = 0; i < row; i++)
            if(cur[i][col] == 'Q')return false;
        //�ҶԽ���(ֻ��Ҫ�ж϶Խ����ϰ벿�֣���Ϊ������л�û�п�ʼ����)������
        for(int i = row-1, j=col-1; i >= 0 && j >= 0; i--,j--)
            if(cur[i][j] == 'Q')return false;
        //��Խ���(ֻ��Ҫ�ж϶Խ����ϰ벿�֣���Ϊ������л�û�п�ʼ����)������
        for(int i = row-1, j=col+1; i >= 0 && j < cur.size(); i--,j++)
            if(cur[i][j] == 'Q')return false;
        return true;
    }
};
int main(){
    Solution s;
    int n=4;
    s.solveNQueens(n);
    return 0;
}

