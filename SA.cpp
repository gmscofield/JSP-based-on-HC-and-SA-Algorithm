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
    void randomswap();                //随机交换元件加工的顺序
    void backswap();                  //撤销之前交换元件加工顺序的操作
    int *returnorder();               //返回元件加工的顺序
    int returnelement();              //返回元件个数
    int evaluate();                   //计算加工所有零件耗费的总时长
    int position1 = 0, position2 = 0; //记录交换加工顺序元件的位置
};

void JSP::randomswap()
{
    position1 = rand() % element; //生成0~element-1之间的两个随机数表示交换顺序的元件的位置
    position2 = rand() % element;
    swap(order[position1], order[position2]);
    return;
}

void JSP::backswap()
{
    swap(order[position1], order[position2]);
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

class SA //模拟退火算法
{
    int iteration;                       //迭代的次数
    int best;                            //最短加工时间
    int *bestorder;                      //最短加工时间对应的元件加工顺序
    double temperature, threshold, rate; //记录初始温度，温度阈值和温度衰减率
    JSP &instance;                       //记录一个车间调度用例

public:
    SA(int a, double b, double c, double d, JSP &e) : iteration(a), temperature(b), threshold(c), rate(d), instance(e)
    {
        best = 0;
        bestorder = NULL;
    }
    void run();             //模拟退火算法
    int returnbest();       //返回最短加工时间
    int *returnbestorder(); //返回最短加工时间对应的元件加工顺序
};

void SA::run()
{
    int *order = NULL;
    int t = instance.evaluate();
    best = t;
    bestorder = instance.returnorder();
    // int cnt=0;
    // double t0=temperature;
    // int samecnt=0;
    while (temperature > threshold) //外循环结束条件：退火温度低于阈值
    {
        temperature *= rate; //以衰减率rate改变温度
        // temperature =t0/(1+cnt);
        // cnt++;
        // samecnt=0;
        for (int i = 0; i < iteration; i++)
        {
            instance.randomswap();
            int newt = instance.evaluate();
            if (newt < t)
            {
                t = newt;
                order = instance.returnorder();
                bestorder = order; //更新最优解以及对应的加工顺序
                // samecnt=0;
                if (t < best)
                    best = t;
            }
            else
            {
                double metropolis = exp((float)(-newt + t) / temperature);
                // double metropolis = temperature / pow(temperature * temperature + (newt - t), (instance.returnelement()+1)/2);
                // double metropolis = 1.0/pow(1+2.0/3.0*(newt-t)/temperature,1.5);
                if ((float)rand() / RAND_MAX <= metropolis) //非更优解被接受的概率：比较随机数与Metropolis准则的阈值
                {
                    // samecnt=0;
                    t = newt;
                    order = instance.returnorder();
                }
                else
                {
                    // samecnt++;
                    instance.backswap(); //将之前交换的元件加工顺序换回来
                    continue;
                }
            }
        }
    }
    // if(samecnt>1000) break;
}

int SA::returnbest()
{
    return best;
}

int *SA::returnbestorder()
{
    return bestorder;
}

int main()
{
    clock_t startime, endtime;
    ifstream fin("flowshop-test-10-student.txt", ios::in); //读入数据文件
    ofstream out("result.txt", ios::out);                  //将结果写到txt文件里
    srand(20220626);
    string s;
    int cnt = 11;
    double temperature = 1e5;
    double threshold = 1e-7;
    int iteration = 5000;
    double rate = 0.998;
    double timerecord[11];
    while (cnt--)
    {
        int machine, element;
        string line;
        int id, a;
        int **cost;
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
        JSP Instance(machine, element, cost);                       //将读入的数据封装成类对象
        SA DEAL(iteration, temperature, threshold, rate, Instance); //建立模拟退火算法的类对象对数据进行处理
        // startime = clock();
        DEAL.run(); //使用模拟退火算法计算该组数据的最小加工总时间和对应工件加工顺序
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
    fin >> s;
    fin.close();
    out.close();
    // cout << "Running Time : " << endl;
    // for (int i = 0; i < 11; i++)
    //    cout << "Test case" << i << " : " << timerecord[i] << "s" << endl;
    return 0;
}