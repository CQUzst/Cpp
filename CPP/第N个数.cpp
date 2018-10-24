#include <iostream>
#include <string>
using namespace std;
//��λ����1-9��һ��9��,����9������
//2λ����10-99,һ��90��������180������
//3λ����100-999��һ��900��������2700������
//4λ����1000-9999��һ��9000��������36000������
//�Դ�����,�������ǾͿ������ȶ�λ�����ĸ��������ҵ����Ӧ������

class Solution {
public:
    int findNthDigit(int n) {
        long digitType = 1;
        long base = 9;
        long ith =1;
        //��λ���Ǽ�λ��
        while (n> base*digitType ) {
            n-=base *digitType ;
            digitType +=1;
            ith += base;
            base = base*10;
        }
        //��λ������Щ��λ������ĵ�indexInSubRange���ĵ�indexλ
        int indexRange=(n-1)/digitType;
        int index = (n -1) % digitType;
        string s=to_string(ith+indexRange);
        return s[index]-'0';
    }
};
int main(){
    Solution s;
    cout<<s.findNthDigit(1111);
    return 0;
}


