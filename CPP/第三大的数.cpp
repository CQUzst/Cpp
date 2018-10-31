class Solution {
public:
    int thirdMax(vector<int>& nums) {
        //����set���Զ�������Զ�ȥ�ظ��������
        set<int> s;
        for (int num : nums) {
            //���ڱ����������֣�����set�У��ظ�����Զ�ȥ����
            s.insert(num);
            //�����ʱset��С����3���ˣ���ô���ǰ�set�ĵ�һ��Ԫ��ȥ����Ҳ���ǽ����Ĵ������ȥ��
            if (s.size() > 3) 
                s.erase(s.begin());
        }
        //setʼ��ά����������������ͬ������
        //���������������ǿ�set�Ĵ�С�Ƿ�Ϊ3���ǵĻ��ͷ�����Ԫ�أ����ǵĻ��ͷ���βԪ��
        return s.size() == 3 ? *s.begin() : *s.rbegin();
        
        
        // set<int> tmp(nums.begin(), nums.end());
        // int max = INT_MIN;
        // int sec = max;
        // int third = sec;
        // for(int i = 0; i<nums.size(); ++i){
        //     if(nums[i]>max){
        //         third = sec;
        //         sec = max;
        //         max = nums[i];
        //     }
        //     else if(nums[i]!=max && nums[i]>sec){
        //         third = sec;
        //         sec = nums[i];
        //     }
        //     else if(nums[i]!=max && nums[i]!=sec && nums[i]>third)
        //         third = nums[i];
        // }
        // if(tmp.size()>2)
        //     return third;
        // else
        //     return max;
    }
};