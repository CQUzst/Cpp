//����һ���� '1'��½�أ��� '0'��ˮ����ɵĵĶ�ά���񣬼��㵺���������
//һ������ˮ��Χ����������ͨ��ˮƽ�����ֱ���������ڵ�½�����Ӷ��ɵġ�
//����Լ���������ĸ��߾���ˮ��Χ.
class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        if(grid.empty()||grid[0].empty())return 0;
        int m=grid.size(),n=grid[0].size(),res=0;
        vector<vector<bool>>visited(m,vector<bool>(n,false));
        for(size_t i=0;i<m;i++){
            for(size_t j=0;j<n;j++){
                if(grid[i][j]=='1'&&!visited[i][j]){
                    DFS(grid,visited,i,j);
                    res++;
                }
            }
        }
    }
private:
    void islandDFS(vector<vector<char>> &grid,
                   vector<vector<bool>> &visited,int x,int y){
        if(x<0||x>=grid.size()) return;
        if(y<0||y>=grid[0].size()) return;
        if(grid[x][y]!='1'||visited[x][y]) return;
        visited[x][y]=true;
        islandDFS(grid,visited,x-1,y);
        islandDFS(grid,visited,x+1,y);
        islandDFS(grid,visited,x,y-1);
        islandDFS(grid,visited,x,y+1);
    }    
};