#include <bits/stdc++.h>
using namespace std;

struct Node{
    double xMin, yMin, size;
    int rep = -1;
    int child[4]; // -1 means no child
    Node(double _xMin = 0, double _yMin = 0, double _size = 0)
    {
        xMin = _xMin;
        yMin = _yMin;
        size = _size;
        rep = -1;
        for (int i = 0; i < 4; i++)
            child[i] = -1;
    }
};
struct Pts{
    int x, y;
};
vector<Node> tree;
void insert(int nodeIDx, int ptIDx, const vector<Pts> &pts){
    // 1. 把一个点加进去。
    if (tree[nodeIDx].rep == -1)
        tree[nodeIDx].rep = ptIDx;
    // 2. 太小的格子就不动了
    if (tree[nodeIDx].size < 0.1)
        return;
    // 3. 确定新的边界，田子形切分
    double midX = tree[nodeIDx].xMin + tree[nodeIDx].size / 2.0;
    double midY = tree[nodeIDx].yMin + tree[nodeIDx].size / 2.0;
    int quad = 0; // 象限编号：0(左下), 1(右下), 2(左上), 3(右上)
    if (pts[ptIDx].x >= midX)
        quad += 1;
    if (pts[ptIDx].y >= midY)
        quad += 2;
    // 这是AI告诉我的一个超级巧妙的方法。

    // 建造小方格
    if (tree[nodeIDx].child[quad] == -1)
    {
        double nx;
        if (quad % 2 == 1)
            nx = midX;
        else
            nx = tree[nodeIDx].xMin;
        double ny;
        if (quad / 2 == 1)
            ny = midY;
        else
            ny = tree[nodeIDx].yMin;
        tree.push_back(Node(nx, ny, tree[nodeIDx].size / 2.0));
        tree[nodeIDx].child[quad] = tree.size() - 1; //将原来的树与新生成的树产生联系
    }
    //递归
    insert(tree[nodeIDx].child[quad], ptIDx, pts);
}

double dis(double x1, double y1, double x2, double y2){
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}   

int main()
{
    int n, q;
    cin >> n >> q;
    vector<Pts> pts(n);
    for(int i = 0; i < n; i++){
        cin >> pts[i].x >> pts[i].y;
    }
    tree.push_back(Node(0, 0, 1000001)); // 初始化根节点，覆盖所有点
    for (int i = 0; i < n; i++){
        insert(0, i, pts);
    }
    // 查询
    for(int i = 0; i < q; i++){
        double queryX, queryY;
        cin >> queryX >> queryY;
        int bestIdx = tree[0].rep;
        double minDist = dis(queryX, queryY, pts[bestIdx].x, pts[bestIdx].y);
        vector<int> todo;
        todo.push_back(0);
        int head = 0;
        while(head < todo.size()){
            int curr = todo[head++];
            for(int j=0; j<4; j++){
                int c = tree[curr].child[j];
                if(c == -1) continue;
                // 看看这个子方格的代表点
                int p_idx = tree[c].rep;
                double d = dis(queryX, queryY, pts[p_idx].x, pts[p_idx].y);
                // 更新我们找到的最短距离
                if(d < minDist){
                    minDist = d;
                    bestIdx = p_idx;
                }
                // 核心：剪枝（如果方格太远，就不管它了）
                // 方格直径约等于 size * 1.414
                if(minDist > 1.15 * (d - tree[c].size * 1.414)){
                    todo.push_back(c);
                }
            }
        }
        cout << bestIdx << endl;
    }
    return 0;
}