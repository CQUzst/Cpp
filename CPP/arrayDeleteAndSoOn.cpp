//����һ������ nums����дһ������������ 0 �ƶ��������ĩβ��ͬʱ���ַ���Ԫ�ص����˳��
class Solution {
public:
    void moveZeroes(vector<int>& nums) 
    {
        // //����ָ��
        // int pos=0;
        // for(size_t i=0;i<nums.size();i++)
        //     if(nums[i]!=0){
        //         nums[pos]=nums[i];
        //         pos++;
        //     }
        // while(pos<nums.size()) {
        //     nums[pos]=0;
        //     ++pos;
        // }
        
        //����i������Ͱ�����ǰ����㽻����ԭ�ؽ��� i++��j++
        //nums[j]��һֱ�ӣ�ֱ��������Ͳ�����
        for(int i=0,j=0;i<nums.size();i++){
           if(nums[i]){
               swap(nums[i],nums[j++]);
           }
       }
    }
};


//����һ������ nums ��һ��ֵ val������Ҫԭ���Ƴ�������ֵ���� val ��Ԫ�أ������Ƴ���������³���
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        // for(int i=0;i<nums.size();i++){
        //     if(nums[i]==val){
        //         nums.erase(nums.begin()+i);
        //         i--;
        //     }
        // }
        // return nums.size();
        
          /*
            ˫ָ�뷨
            ����ÿһ��ֵ��ÿ��������valʱ���ı�nums[temp]����ֵ
            ��󷵻�temp ����Ϊ������val�ĸ���
        */
        int temp=0;
        for(int i=0;i<nums.size();i++){
            if(nums[i]!=val){
                nums[temp]=nums[i];
                temp++;
            }
        }
        return temp;
    }
};


//����һ���������飬����Ҫ��ԭ��ɾ���ظ����ֵ�Ԫ�أ�ʹ��ÿ��Ԫ��ֻ����һ�Σ������Ƴ���������³���
class Solution {
public:
    int removeDuplicates(vector<int>& nums)
    {
//         if(nums.size()==0)
//             return 0;
//         int length=1;
//         for(int i=0;i<nums.size()-1;i++)
//             {
//                 if(nums[i+1]!=nums[i])
//                 {
//                     nums[length]=nums[i+1];
//                     length++;
//                 }
                    
//             }
//         return length;

        nums.erase(unique(nums.begin(),nums.end()),nums.end());
        return nums.size();
    }
};


// ɾ�����������е��ظ��� II
//����һ���������飬����Ҫ��ԭ��ɾ���ظ����ֵ�Ԫ�أ�ʹ��ÿ��Ԫ�����������Σ������Ƴ���������³��ȡ�
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        for(int i=0;i<nums.size();i++){
            for(int j=i+2;j<nums.size();j++){
                if(nums[j]==nums[i]){
                    nums.erase(nums.begin()+j);
                    j--;
                }
                else break;
            }
        }
        return nums.size();;
    }
};