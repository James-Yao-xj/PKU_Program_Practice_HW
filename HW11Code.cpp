#include<bits/stdc++.h>
using namespace std;
struct Point {
    double x, y;
};

bool cmp(const Point &a, const Point &b){
    if(fabs(a.x - b.x) < 1e-12) return a.y < b.y;
    return a.x < b.x;
}

bool eq(const Point &a, const Point &b){
    return fabs(a.x - b.x) < 1e-12 && fabs(a.y - b.y) < 1e-12;
}

int main(){
    int n;
    cin >> n;
    vector<Point> pts(n);
    for(int i = 0; i < n; ++i) 
        scanf("%lf %lf", &pts[i].x, &pts[i].y);
    
    srand(time(0));
    int _center = rand() % n;//随手捞一个点作为中心
    double rMax = 0.0; //半径肯定比这个小噻
    for(int i = 0; i < n; i++){
        double dx = pts[i].x - pts[_center].x;
        double dy = pts[i].y - pts[_center].y;
        double dist = dx * dx + dy * dy;
        if(dist > rMax) rMax = dist;
    }
    rMax = sqrt(rMax);

    double epsilon = 0.1;
    double l = epsilon * rMax / 2.0; //格子边长

    vector<Point> centers(n);
    for(int i = 0; i < n; ++i){
        double x = pts[i].x;
        double y = pts[i].y;
        double cx = floor(x / l) * l + l / 2.0;
        double cy = floor(y / l) * l + l / 2.0;
        centers[i] = {cx, cy};
    }

    sort(centers.begin(), centers.end(), cmp);
    centers.erase(unique(centers.begin(), centers.end(), eq), centers.end());

    //暴力求直径
    double diam = 0.0;
    int m = centers.size();
    for(int i = 0; i < m; ++i){
        for(int j = i + 1; j < m; ++j){
            double dx = centers[i].x - centers[j].x;
            double dy = centers[i].y - centers[j].y;
            double dist = sqrt(dx * dx + dy * dy);
            if(dist > diam) diam = dist;
        }
    }

    printf("%.6lf\n", diam);
    return 0;

}