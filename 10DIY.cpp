#include <bits/stdc++.h>
using namespace std;

// 归一化函数：将所有点变成单位长度
void normalize(int d, int n, vector<vector<double> > &points) {
    for (int i = 0; i < n; i++) {
        double len = 0;
        for (int j = 0; j < d; j++) {
            len += points[i][j] * points[i][j];
        }
        len = sqrt(len);
        for (int j = 0; j < d; j++) {
            points[i][j] /= len;
        }
    }
}

// 快速哈达玛变换(FHT)：O(d log d)时间完成正交变换
vector<double> fastHadamardTransform(vector<double> x) {
    int m = x.size();
    for (int step = 1; step < m; step <<= 1) {
        for (int i = 0; i < m; i += 2 * step) {
            for (int j = 0; j < step; j++) {
                double u = x[i + j];
                double v = x[i + step + j];
                x[i + j] = u + v;
                x[i + step + j] = u - v;
            }
        }
    }
    double norm = 1.0 / sqrt((double)m);
    for (int i = 0; i < m; i++) {
        x[i] *= norm;
    }
    return x;
}

// 生成随机对角矩阵（元素只有+1和-1）
vector<int> randDiag(int d) {
    vector<int> diag(d);
    for (int i = 0; i < d; i++) {
        // rand()%2生成0或1，乘以2减1得到-1或+1
        diag[i] = (rand() % 2) * 2 - 1;
    }
    return diag;
}

// 快速旋转类：用三次哈达玛变换+随机对角矩阵模拟随机正交旋转
struct FastRotation {
    vector<int> D1, D2, D3;
    int dim;
    
    FastRotation(int d) {
        dim = d;
        D1 = randDiag(d);
        D2 = randDiag(d);
        D3 = randDiag(d);
    }
    
    vector<double> rotate(vector<double> x) {
        x = fastHadamardTransform(x);
        for (int i = 0; i < dim; i++) {
            x[i] *= D1[i];
        }
        
        x = fastHadamardTransform(x);
        for (int i = 0; i < dim; i++) {
            x[i] *= D2[i];
        }
        
        x = fastHadamardTransform(x);
        for (int i = 0; i < dim; i++) {
            x[i] *= D3[i];
        }
        return x;
    }
};

// 交叉多面体哈希：单个哈希函数
struct CrossPolyHash {
    FastRotation rot;
    int dim;
    
    CrossPolyHash(int d) : rot(d) {
        dim = d;
    }
    
    int operator()(const vector<double> &point) {
        vector<double> y = rot.rotate(point);
        
        // 找到绝对值最大的坐标
        int best = 0;
        double bestAbs = fabs(y[0]);
        for (int i = 1; i < dim; i++) {
            double cur = fabs(y[i]);
            if (cur > bestAbs) {
                bestAbs = cur;
                best = i;
            }
        }
        
        // 根据符号返回哈希值：正返回2i，负返回2i+1
        if (y[best] > 0) {
            return 2 * best;
        } else {
            return 2 * best + 1;
        }
    }
};

// 复合哈希：将K个单个哈希函数组合成一个长哈希
struct CompoundHash {
    vector<CrossPolyHash> hashes;
    int K;
    
    CompoundHash(int d, int K_val) {
        K = K_val;
        for (int i = 0; i < K; i++) {
            hashes.push_back(CrossPolyHash(d));
        }
    }
    
    unsigned long long encode(const vector<double> &point) {
        unsigned long long key = 0;
        for (int i = 0; i < K; i++) {
            int val = hashes[i](point);
            // 每个哈希值占7位，左移7位后拼接
            key = (key << 7) | (val & 0x7F);
        }
        return key;
    }
};

int main() {
    srand((unsigned int)time(0)); // 设置随机种子
    
    int d, n, q;
    cin >> d >> n >> q;
    
    // 读取数据点
    vector<vector<double> > points(n, vector<double>(d));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < d; j++) {
            cin >> points[i][j];
        }
    }
    normalize(d, n, points);
    
    // 读取查询点
    vector<vector<double> > queries(q, vector<double>(d));
    for (int i = 0; i < q; i++) {
        for (int j = 0; j < d; j++) {
            cin >> queries[i][j];
        }
    }
    normalize(d, q, queries);
    
    // LSH参数：L个哈希表，每个表用K个哈希函数
    const int L = 24;
    const int K = 8;
    
    // 初始化L个复合哈希函数和L个哈希表
    vector<CompoundHash> compoundHashes;
    vector<map<unsigned long long, vector<int> > > tables(L);
    
    for (int i = 0; i < L; i++) {
        compoundHashes.push_back(CompoundHash(d, K));
    }
    
    // 构建索引：将所有数据点插入到L个哈希表中
    for (int idx = 0; idx < n; idx++) {
        const vector<double> &pt = points[idx];
        for (int i = 0; i < L; i++) {
            unsigned long long key = compoundHashes[i].encode(pt);
            tables[i][key].push_back(idx);
        }
    }
    
    // 用于标记已经检查过的点，避免重复计算
    vector<int> seen(n, 0);
    int queryID = 0;
    
    // 处理每个查询
    for (int t = 0; t < q; t++) {
        queryID++;
        const vector<double> &qpt = queries[t];
        vector<int> candidates;
        // 从L个哈希表中收集候选点
        for (int i = 0; i < L; i++) {
            unsigned long long key = compoundHashes[i].encode(qpt);
            map<unsigned long long, vector<int> >::iterator it = tables[i].find(key);
            if (it != tables[i].end()) {
                const vector<int> &bucket = it->second;
                for (int j = 0; j < bucket.size(); j++) {
                    int idx = bucket[j];
                    if (seen[idx] != queryID) {
                        seen[idx] = queryID;
                        candidates.push_back(idx);
                    }
                }
            }
        }
        // 在候选点中找最近邻
        int bestIdx = -1;
        double bestDist2 = 1e100;
        for (int i = 0; i < candidates.size(); i++) {
            int idx = candidates[i];
            double dist2 = 0.0;
            for (int j = 0; j < d; j++) {
                double diff = qpt[j] - points[idx][j];
                dist2 += diff * diff;
            }
            if (dist2 < bestDist2) {
                bestDist2 = dist2;
                bestIdx = idx;
            }
        }
        cout << bestIdx << endl;
    } 
    return 0;
}