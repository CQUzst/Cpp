#include <iostream>
#include <algorithm>
using namespace std;
//��һ������a[N]˳����0~N-1��Ҫ��ÿ��������ɾ��һ��������ĩβʱѭ������ͷ�������У�
//�����һ����ɾ��������ԭʼ�±�λ�á���8����(N=7)Ϊ��:��0��1��2��3��4��5��6��7����
//0->1->2(ɾ��)->3->4->5(ɾ��)->6->7->0(ɾ��),���ѭ��ֱ�����һ������ɾ����
//���������ÿ������Ϊһ��һ������n(С�ڵ���1000)��Ϊ�����Ա��,�������1000�����a[999]���м��㡣
//����������һ��������һ����ɾ��������ԭʼ�±�λ��
int main(){
    int n=0;
    while(cin>>n){
        if(n>1000)n=999;
        int a[n];
        for(size_t i=0;i<n;i++) a[i]=i;
        int deleteNumber=0;
        int res=0;
        int cnt=2;
        while (deleteNumber!=n){
            for(size_t j=0;j<n;j++){
                if(a[j]==-1)continue;
                if(cnt>0) {
                    cnt--;
                    continue;
                }
                if(cnt==0) {
                    a[j]=-1;
                    deleteNumber++;
                    res=j;
                    cnt=2;
                }
            }
        }
        cout<<res<<endl;
    }
    return 0;
}
