#import "@preview/ctyp:0.3.0": ctyp
#let (ctypset, cjk) = ctyp()
#let (song, hei, kai, fang) = cjk
#show: ctypset

#set page(
  paper: "a4",
  margin: (top: 2cm, bottom: 2cm, left: 1cm, right: 1cm),
  header: context [
    #text(8pt, gray)[程序设计实习（实验班）作业解答自制]
  ]
)

#set heading(numbering: "1.")

#align(center)[
  #text(size: 24pt, weight: "bold")[作业5]\
  #v(10pt)
  #text(size: 16pt)[——亚线性时间估算分位点——]\
  #v(10pt)
  #text(size:12pt)[_声明：题目来源于http://openjudge.cn/，解题思路参考教授上课讲解内容，代码实现由本人完成。分享这个文档仅限于学术交流，不用于任何形式的商业用途。_]
]

= 题目
*描述*

有$n$个互不相同的数$a_1,...,a_n$，你只能通过给出的函数接口来访问某一编号$i$的数的大小$a_i$，并返回你估计的这$n$个数的$p$分位数，$0 <= p <= 1$，$0$分位数表示最小值，$1$分位数表示最大值。

具体地，设你输出的数在n个数中从小到大的排名为$r$，要求$r∈ [(p-e)n, (p+e)n], e=0.03$

以交互题的形式提交，你需要实现一个函数``` rnk (n,p)```，返回估计的$p$分位数，唯一访问a的方式是利用``` query (i) ```来得到$a_i$，代码需包含头文件``` rank.h```。

$10^5<=n<=10^6$，要求访问次数不超过$5000$。

*输入*

第一行一个正整数$n$和一个实数$p$。

第二行$n$个int范围内的整数，表示这$n$个互不相同的数$a_1$到$a_n$。

注意：这里描述的是链接后的完整程序的输入/输出格式，仅供测试用途。你提交的程序只能通过query函数访问数据，请勿自行读入！

*输出*

无

*样例输入*
#align(center)[
```text
5 0.5
1 2 3 4 5
```

]

*样例输出*

#align(center)[
```text
your answer is : 3
the times you sample is : 5
the rank of your answer is : 3
correct!
```

]

*提示*

在以下链接下载交互库grader.cpp，rank.h，以及样例代码rank_sample.cpp：

https://github.com/crasysky/rank/tree/main

另外，本题只考虑n较大的情况，样例仅供参考输入输出格式。

= 思路

这道题目已经把思路说得非常详细了。我们主要说说为什么需要这样做。
传统上，我们可以对所有数据进行排序之后寻找$p$分位数，但是排序的时间复杂度是$O(n log n)$。这需要存储访问整个数据集。然而，如果这些数据分布式存储在非常多的机器硬盘上，无法载入内存，或者开销太大，我们能否找到一个近似解答呢？#footnote[曾经我们做的题目都是要求输出一个精确的解答，但是在实际生活中，由于采样的数据可能都是含有很多不可避免的噪声的，因此，我们得到的所谓的精确的解答并不一定是真正精确的。与此同时，我们还花费了很多资源在求解这样的“理论上精确解”，这并不是很好的，因此，很多时候，我们只需要得到一个近似解，并且保证它的精度不会太差就可以了。]

根据题目的意思，对于所有数据，我们不能全部获取。我们只能选择性地访问其中的至多$5000$个数据。这类似于“抽样调查”，通过抽样的结果来获得总体的情况。我们可以随机地选择$5000$个数据进行访问，然后对这些数据进行排序，找到其中的$p$分位数。由于这些数据是随机抽取的，因此它们的分布应该与总体数据的分布相似，因此我们得到的$p$分位数应该也是一个近似解。

接下来，我们从数学上论证这样做法的精确程度：

为了简化问题，我们考虑$p = 0.5$的情况，也就是中位数。我们希望输出的数据在$(0.5 - 2 epsilon, 0.5 + 2 epsilon)$范围内。设$alpha = 0.5 - 2 epsilon, beta = 0.5 + 2 epsilon$， 考虑子集$A_1, A_2, A_3$分别代表排序后位次是$[1, alpha n], [alpha n + 1, beta n], [beta n + 1, n]$的数。对于一个平均采样，各有$0.5 - 2 epsilon$的概率落在$A_1, A_3$中。

这里需要Chernoff Bound不等式：#footnote[这里的$log$是指以$e$为底的对数。并且要求$X_i in [0,1]$。对于更大的数据，可以通过数学手段映射到$[0,1]$区间内。]
$
Pr(abs(macron(X) -  mu) >= sqrt(log(2 / delta) / (2 n))) <= delta
$
其中，$macron(X)$是样本的均值，$mu$是它的期望值，$n$是采样的次数，$delta$是允许的失败概率。

在这里，我们设定$delta = 2 epsilon$，式子变成
$
Pr(abs(macron(X) -  mu) >= sqrt(log(1 / epsilon) / (2 n))) <= 2 epsilon
$

根据题目，$epsilon = 0.015$，因此，$sqrt(log(1 / epsilon) / (2 n)) < 0.015$，当$n > 2334$时，这个不等式成立。因此，采样$5000$次是足够的。

= 代码实现
#align(center)[
  ```cpp
#include <bits/stdc++.h>
#include "rank.h"
using namespace std;
int rnk(int n, double p){
    minstd_rand gen;
    int T = 5000;
    vector<int> v;
    for (int i = 1; i <= T; i++)
    {
        int idx = gen() % n + 1;
        v.push_back(query(idx));
    }
    sort(v.begin(), v.end());
    return v[max(int(p * T) - 1, 0)];
}
```
]