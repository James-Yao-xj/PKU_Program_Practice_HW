#include<bits/stdc++.h>
using namespace std;
using ll = long long;
struct Points{
    ll x[3]; //三维坐标储存
    int id; //点的编号
};

struct Node{
    Points p;
    ll minn[3], maxn[3]; //当前节点的边界
    int indexLeft, indexRight; //左右子树的编号
};

Points pts[100005];
Points temp_pts[100005];
Node trees[100005];
int cur_dim; //当前维度
ll ans = LLONG_MAX; //全局最小距离

bool cmp(const Points &a, const Points &b) {
    if (a.x[cur_dim] != b.x[cur_dim]) 
        return a.x[cur_dim] < b.x[cur_dim];
    return a.id < b.id;  // 坐标相同时按id排序，保证确定性
}

void upDate(int tr){
    int l = trees[tr].indexLeft, r = trees[tr].indexRight;
    //
    for(int i = 0 ; i < 3; i ++){
        // 初始包围盒为当前节点自身的坐标
        trees[tr].minn[i] = trees[tr].maxn[i] = trees[tr].p.x[i];
        if(l != 0){
            trees[tr].minn[i] = min(trees[tr].minn[i], trees[l].minn[i]);
            trees[tr].maxn[i] = max(trees[tr].maxn[i], trees[l].maxn[i]);
        }
        if(r != 0) {
            trees[tr].minn[i] = min(trees[tr].minn[i], trees[r].minn[i]);
            trees[tr].maxn[i] = max(trees[tr].maxn[i], trees[r].maxn[i]);
        }
    }
}
/*
nth_element(_RandomAccessIterator __first, _RandomAccessIterator __nth,
		_RandomAccessIterator __last, _Compare __comp)
    {
      // concept requirements
      __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
				  _RandomAccessIterator>)
      __glibcxx_function_requires(_BinaryPredicateConcept<_Compare,
	    typename iterator_traits<_RandomAccessIterator>::value_type,
	    typename iterator_traits<_RandomAccessIterator>::value_type>)
      __glibcxx_requires_valid_range(__first, __nth);
      __glibcxx_requires_valid_range(__nth, __last);
      __glibcxx_requires_irreflexive_pred(__first, __last, __comp);

      if (__first == __last || __nth == __last)
	return;

      std::__introselect(__first, __nth, __last,
			 std::__lg(__last - __first) * 2,
			 __gnu_cxx::__ops::__iter_comp_iter(__comp));
    }

*/
int build(int l, int r, int d) {
    if (l > r) return 0;  // 空区间返回0（空节点）
    
    int mid = (l + r) >> 1;  // 选择中间位置作为当前节点 相当于除以2向下取整。
    cur_dim = d;             // 设置当前划分维度
    
    // 将第mid小的元素放到mid位置，左边≤它，右边≥它（O(n)时间）
    nth_element(temp_pts + l, temp_pts + mid, temp_pts + r + 1, cmp);
    
    trees[mid].p = temp_pts[mid];  // 存储当前节点的点

    // 递归构建左右子树，维度切换为(d+1)%3（三维循环）
    trees[mid].indexLeft = build(l, mid - 1, (d + 1) % 3);
    trees[mid].indexRight = build(mid + 1, r, (d + 1) % 3);
    upDate(mid);  // 更新当前节点的包围盒
    return mid;   // 返回当前节点的索引
}

// 计算两点之间的欧几里得距离平方（避免开根号，提高速度和精度）
inline ll dist_sq(const Points &a, const Points &b) {
    ll res = 0;
    for (int i = 0; i < 3; i++) {
        ll d = a.x[i] - b.x[i];
        res += d * d;
    }
    return res;
}

// 计算点pt到节点rt的包围盒的最短距离平方（用于剪枝）
inline ll guess_min_sq(int rt, const Points &pt) {
    ll res = 0;
    for (int i = 0; i < 3; i++) {
        if (pt.x[i] < trees[rt].minn[i]) {
            // 点在包围盒左侧，该维度距离为minn[i]-pt.x[i]
            ll d = trees[rt].minn[i] - pt.x[i];
            res += d * d;
        } else if (pt.x[i] > trees[rt].maxn[i]) {
            // 点在包围盒右侧，该维度距离为pt.x[i]-maxn[i]
            ll d = pt.x[i] - trees[rt].maxn[i];
            res += d * d;
        }
        // 点在包围盒内部，该维度距离为0
    }
    return res;
}
void query(int rt, const Points &pt) {
    if (!rt) return;  // 空节点直接返回
    
    // 计算当前节点的点与pt的距离平方，排除点自身
    if (trees[rt].p.id != pt.id) {
        ans = min(ans, dist_sq(trees[rt].p, pt));
    }
    
    int l = trees[rt].indexLeft, r = trees[rt].indexRight;
    // 计算左右子树的包围盒到pt的最短距离平方
    ll dl = l ? guess_min_sq(l, pt) : LLONG_MAX;
    ll dr = r ? guess_min_sq(r, pt) : LLONG_MAX;

    // 优先搜索距离更近的子树（更快找到更小的ans，增强剪枝效果）
    if (dl < dr) {
        if (dl < ans) query(l, pt);  // 只有下界小于当前最小值才搜索
        if (dr < ans) query(r, pt);
    } else {
        if (dr < ans) query(r, pt);
        if (dl < ans) query(l, pt);
    }
}

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    
    // 边界情况：少于2个点时距离为0
    if (n < 2) {
        printf("0.00\n");
        return 0;
    }
    
    // 读取所有点
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            scanf("%lld", &pts[i].x[j]);
        }
        pts[i].id = i;          // 给每个点分配唯一id
        temp_pts[i] = pts[i];   // 复制到临时数组用于构建KD-Tree
    }
    
    // 构建KD-Tree，根节点索引为build返回值
    int root = build(0, n - 1, 0);
    
    // 对每个原始点查询其最近邻
    for (int i = 0; i < n; i++) {
        query(root, pts[i]);
    }
    
    // 开根号得到实际距离，保留两位小数输出
    printf("%.2f\n", sqrt((double)ans));
    
    return 0;
}