//�����ظ�Ԫ��II
class Solution {
public:
    bool containsNearbyDuplicate(vector<int>& nums, int k) {
        // unordered_map<int,int>m;
        // for(int i=0;i<nums.size();i++){
        //     if(i>k)
        //         m[nums[i-k-1]]--;
        //     m[nums[i]]++;
        //     if(m[nums[i]]>1)
        //         return true;
        // }
        // return false;
        
        
        // unordered_map<int,int> record;
        // for(int i=0;i<nums.size(); i++){
        //     if(record.find(nums[i])==record.end())
        //         record[nums[i]] = i;
        //     else if(i-record[nums[i]]<=k)
        //         return true;
        //     else
        //         record[nums[i]] = i;//���������λ��
        // }
        // return false;
        
        
        unordered_map<int,int> m;
        int j = 0;
        for (int i = 0; i < nums.size(); ++i){
            if (i - j > k){
                m.erase(nums[j++]);
            }
            if (m.find(nums[i])!=m.end()){
                return true;
            }
            m.insert({nums[i],i});
        }
        return false;
    }
};

//�����ظ�Ԫ��III
class Solution {
public:
    bool containsNearbyAlmostDuplicate(vector<int>& nums, int k, int t) {
        // map<long long, int> m;
        // int j = 0;
        // for (int i = 0; i < nums.size(); ++i) 
        // {
        //     if (i - j > k)
        //     {
        //         m.erase(nums[j++]);
        //     }
        //     auto a = m.lower_bound((long long)nums[i] - t);
        //     if (a != m.end() && abs(a->first - nums[i]) <= t)
        //     {
        //         return true;
        //     }
        //     m[nums[i]] = i;
        // }
        // return false;
        
        set<long long> knear; //��������
        for (int i = 0; i < nums.size(); i++){
            if (i > k) //������k֮ǰ��set����ɾ��
                knear.erase(nums[i - k - 1]);   
            long long a = nums[i];
            //set<long long>::iterator
            //lower_bound(val)����set�д��ڻ����val�ĵ�һ��Ԫ��λ�á��������Ԫ�ض�С��val���򷵻�end
            auto p = knear.lower_bound(a - t);// *p-nums[i]>=-t
            if (p != knear.end() && *p - a <= t)//����Ҫ���� *p - nums[i] <= t
                return true;
            knear.insert(nums[i]);//��set�����������Ԫ�أ�ά��һ�����Ϊk�Ĵ���
        }
        return false;
    }
};