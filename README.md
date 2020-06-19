Implementation of

*Variational Sphere set Approximation for Solid Objects*

Rui Wang Kun Zhou John Snyder Xinguo Liu Hujun Bao Qunsheng Peng Baining Guo 

The Visual Computer | August 2006

[View Publication](http://dx.doi.org/10.1007/s00371-006-0052-0)

### 预处理

内部点：体素化网格，选取所有网格内部的体素顶点

表面点：在表面三角形等概率采样

随机选取内部点作为球心

1.

将点归给SOV增量最小的球（相同时选取距离最小）

加速：对每个球打表SOV下界，求出下界最小的SOV的实际值后可以删去下界更大的

2.

包围球调整

Powell's method

3.

突变

将SOV最大的球分裂，初始选取在最远的两个点；

删去被重叠比例最大的球

重复1，2





