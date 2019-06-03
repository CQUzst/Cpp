#include <iostream>
#include <vector>
using namespace std;
//next数组的长度与短字符串T的长度一致，next[j]代表使T[0]~T[k-1]=T[j-k]~T[j-1]成立的最大k值
//即找最大对称长度的前缀后缀，然后整体右移一位，初值赋为-1
//其作用是减少回溯的距离，从而减少比较次数
vector<int> getNext(string T){//建立T串失配函数数组
    vector<int> next(T.size(), 0);
    next[0] = -1;// next矩阵的第0位为-1
    int k = 0;// k值
    for (size_t j = 2; j < T.size(); ++j){// 从字符串T的第2个字符开始，计算每个字符的next值
        while (k > 0 && T[j - 1] != T[k])
            k = next[k];
        if (T[j - 1] == T[k])
            k++;
        next[j] = k;
    }
    return next;
}

int KMP(string S, string T){
    vector<int> next = getNext(T);
    size_t i = 0, j = 0;
    while (i<S.size()&&j<T.size()){
        if (S[i] == T[j]){
            ++i;
            ++j;
        }
        else
            j = next[j];
        if (j == -1){
            ++i;
            ++j;
        }
    }
    if (j=T.size())
        return i - j;
    else
        return -1;
}

int main()
{
    string S = "ababaababcb";
    string T = "ababc";
    int num = KMP(S, T);
    cout << num;
    return 0;
}
