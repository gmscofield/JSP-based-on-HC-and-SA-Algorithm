#include <bits/stdc++.h>
using namespace std;

class JSP //将车间调度测试用例用类存储，方便调用
{
    int machine, element; //记录机器和元件数量
    int **cost;           //记录每个元件在不同机器上加工耗费的时间
    int *order;           //记录元件加工的顺序

public:
    JSP(int a = 0, int b = 0, int **c = NULL) : machine(a), element(b), cost(c)
    {
        order = new int[element];
        for (int i = 0; i < element; i++)
            order[i] = i;
    }
    void Swap();              //交换指定位置元件的加工顺序
    void randomsort();        //随机对元件加工顺序进行排序
    int *returnorder();       //返回元件加工的顺序
    int returnelement();      //返回元件个数
    int evaluate();           //计算加工所有零件耗费的总时长
    void changeorder(int *p); //改变元件的加工顺序
    void randomswap();        //随机改变元件的加工顺序
    int x = 0, y = 0;         //记录交换加工顺序元件的位置
};

void JSP::Swap()
{
    swap(order[x], order[y]);
    return;
}

void JSP::randomsort()
{
    for (int i = 0; i < element; i++)
    {
        int j = rand() % element;
        swap(order[i], order[j]);
    }
    return;
}

int *JSP::returnorder()
{
    return order;
}

int JSP::returnelement()
{
    return element;
}

int JSP::evaluate()
{
    int a = 0;
    /*
    int **time;
    time = new int *[machine];
    for (int i = 0; i < machine; i++)
    {
        time[i] = new int[element];
        for (int j = 0; j < element; j++)
            time[i][j] = 0;
    }
    for (int i = 0; i < element; i++)
    {
        a += cost[order[i]][0];
        time[0][i] = a;
    }
    for (int i = 1; i < machine; i++)
    {
        int tem = 0;
        for (int j = 0; j < element; j++)
        {
            if (j == 0)
                tem = time[i - 1][j];
            else
                tem = max(time[i - 1][j], time[i][j - 1]);
            time[i][j] = tem + cost[order[j]][i];
        }
    }
    return time[machine - 1][element - 1];
    */

    int *time; //记录每个工件在当前步骤机器上加工结束时的时间
    time = new int[element];
    for (int i = 0; i < element; i++) //用每个元件在第一台机器上加工的时间对time数组进行初始化
    {
        a += cost[order[i]][0];
        time[i] = a;
    }
    for (int i = 1; i < machine; i++)
    {
        int tem = 0;
        for (int j = 0; j < element; j++)
        {
            if (j == 0)
                tem = time[j];
            else
                tem = max(time[j], time[j - 1]); //比较已更新的上一元件该步结束时间和未更新的当前元件上步结束时间，取较大者更新当前元件加工时间
            time[j] = tem + cost[order[j]][i];
        }
    }
    return time[element - 1]; //最后元件在最后一台机器上加工的结束时间即为加工所需总时间
}

void JSP::changeorder(int *p)
{
    order = p;
    return;
}

void JSP::randomswap()
{
    x = rand() % element;
    y = rand() % element;
    swap(order[x], order[y]);
    return;
}

class HC //登山算法
{
    int best;       //最短加工时间
    int *bestorder; //最短加工时间对应的元件加工顺序
    JSP &instance;  //记录一个车间调度用例
    int iteration;  //最外层迭代次数
public:
    HC(JSP &a, int b) : instance(a), iteration(b)
    {
        best = 0;
        bestorder = NULL;
    }
    void run();             //登山算法
    int returnbest();       //返回最短加工时间
    int *returnbestorder(); //返回最短加工时间对应的元件加工顺序
};

void HC::run()
{
    int *order = NULL;
    for (int i = 0; i < rand(); i++)
        instance.randomsort();                      //随机化元件加工的初始顺序
    int newt = instance.evaluate(), t = 0x3f3f3f3f; //记录上次计算结果和本次计算结果
    int element = instance.returnelement();
    for (int k = 0; k < iteration; k++)
    {
        for (int i = 0; i < rand(); i++)
            instance.randomsort(); //随机化元件加工的初始顺序
        newt = instance.evaluate();
        element = instance.returnelement();
        while (newt < t) //结束条件：邻域内所有对换计算结果均不小于上次结果
        {
            best = newt;
            bestorder = instance.returnorder();
            t = newt;
            for (int i = 0; i < element - 1; i++)
            {
                for (int j = i + 1; j < element; j++)
                {
                    instance.x = i, instance.y = j;
                    instance.Swap(); //仅对换两个元件顺序的排列作为邻域，计算结果后与之前的结果进行比较，取较小值
                    // instance.randomswap();
                    newt = instance.evaluate();
                    if (newt < best)
                    {
                        best = newt;
                        bestorder = instance.returnorder(); //更新最优解以及对应的加工顺序
                    }
                    instance.Swap(); //将之前交换的元件加工顺序换回来
                }
            }
            newt = best;
            instance.changeorder(bestorder); //记录当前最小值及对应顺序
        }
    }
}

int HC::returnbest()
{
    return best;
}

int *HC::returnbestorder()
{
    return bestorder;
}

int main()
{
    clock_t startime, endtime;
    ifstream fin("flowshop-test-10-student.txt", ios::in); //读入数据文件
    ofstream out("result-HC.txt", ios::out);               //将结果写到txt文件里
    string s;
    srand(0);
    int iteration = 3000;
    int cnt = 11;
    int **cost;
    double timerecord[11];
    while (cnt--)
    {
        int machine, element;
        string line;
        int id, a;
        fin >> line;
        fin >> line >> id;
        fin >> element >> machine;
        cost = new int *[element];
        out << "Test case " << id << " : " << endl;
        for (int i = 0; i < element; i++)
        {
            cost[i] = new int[machine];
            for (int j = 0; j < machine; j++)
            {
                fin >> a;
                fin >> cost[i][j];
            }
        }
        JSP Instance(machine, element, cost); //将读入的数据封装成类对象
        HC DEAL(Instance, iteration);         //建立登山算法的类对象对数据进行处理
        // startime = clock();
        DEAL.run(); //使用登山算法计算该组数据的最小加工总时间和对应工件加工顺序
        // endtime = clock();
        out << "Time: " << DEAL.returnbest() << endl;
        out << "Order: [";
        int len = Instance.returnelement();
        int *bestorder = DEAL.returnbestorder();
        for (int i = 0; i < len; i++)
        {
            if (i == len - 1)
                out << bestorder[i];
            else
                out << bestorder[i] << ",";
        }
        out << ']' << endl;
        // timerecord[id] = double(endtime - startime) / CLOCKS_PER_SEC;
    }
    fin.close();
    out.close();
    // cout << "Running Time : " << endl;
    // for (int i = 0; i < 11; i++)
    // cout << "Test case" << i << " : " << timerecord[i] << "s" << endl;
    return 0;
}