#include <iostream>
#include <vector>
using namespace std;
//next数组的长度与短字符串T的长度一致，next[j]代表使T[0]~T[k-1]=T[j-k]~T[j-1]成立的最大k值
//即找最大对称长度的前缀后缀，然后整体右移一位，初值赋为-1
//其作用是减少回溯的距离，从而减少比较次数
vector<int> getNext(string T){//建立T串失配函数数组
    //next[j]的值（也就是k）表示，当P[j] != T[i]时，j指针的下一步移动位置
    vector<int> next(T.size(), 0);
    next[0] = -1;// next矩阵的第0位为-1
    int k = -1 ,j=0;
    while (j < T.size() - 1) {
       //如果当前匹配成功，next[j]置为当前的k，表示如果下一个匹配失败j回退的移动距离
       if (k == -1 || T[j] == T[k]) {
           j++;
           k++;
           next[j] = k;
       }
       else//如果匹配失败，将k指针移动到next[k]，表示j回退到前面可以匹配的位置
           k = next[k];
    }
    return next;
}

int KMP(string S, string T){
    vector<int> next = getNext(T);
    int i = 0, j = 0;
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
