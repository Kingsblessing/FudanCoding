#include<bits/stdc++.h>
using namespace std;

//基础数据结构
struct node{
    int node_id;//节点编号（1 ∼ N）
    int capacity;//该节点的最大可用 GPU 总数（正整数）
    int usedgpu;//该节点已使用的 GPU 数量
    node(int id,int cap,int ug=0):node_id(id),capacity(cap),usedgpu(ug){}
    ~node(){}
};
struct edge{
    int u;
    int v;//相连的两个节点编号（1 ∼ N）
    int past_cost;//迁移 “单个 GPU 占用” 所需的成本
    int bandwidth;//该链路的带宽容量（正整数）
    int rem_bandwidth;//该链路的剩余带宽容量
    string get_key() const{
        return u<v?to_string(u)+","+to_string(v):to_string(v)+","+to_string(u);
    }
    edge(int iu,int iv,int pc,int bw,int rbw):u(iu),v(iv),past_cost(pc),bandwidth(bw),rem_bandwidth(rbw){}
    ~edge(){}
};
struct task{
    int task_id;//任务编号（1 ∼ T）
    int started_node;//任务所在节点编号（1 ∼ N）
    int demand;//任务所需 GPU 数量（正整数）
    int end_node;//任务最终分配节点编号
    int mig_cost;//任务迁移成本
    vector<int> path;//迁移路径
    bool ismiged;//任务是否已迁移
    task(int id,int snode,int dem,int enode,int mc,vector<int> p,bool ism):task_id(id),started_node(snode),demand(dem),end_node(enode),mig_cost(mc),path(p),ismiged(ism){}
    ~task(){}
};

const int INF=1e9;
const int MAXNODE=51;
int N,M,T;
vector<node> nodes;
vector<edge> edges;
vector<task> tasks;
int distant[MAXNODE][MAXNODE];//最短路径成本矩阵
int prenode[MAXNODE][MAXNODE];//前驱节点矩阵
unordered_map<string,edge*> edgemap;//链路快速查找表

void basic_requirement();//基础要求
void advanced_requirement();//高级要求
void additional_requirement();//附加要求

int main()
{
    //读取输入数据并初始化数据结构
    cin>>N>>M>>T;
    //使用reserve代替resize，避免需要默认构造函数
    nodes.reserve(N);
    edges.reserve(M);
    tasks.reserve(T);
    
    for(int i=0;i<N;i++){
        int id;
        int cap;
        cin>>id>>cap;
        nodes.emplace_back(id, cap, 0);
    }
    for(int i=0;i<M;i++){
        int u,v,pc;
        int bw;
        cin>>u>>v>>pc>>bw;
        edges.emplace_back(u, v, pc, bw, bw);
        edgemap[edges.back().get_key()]=&edges.back();
    }
    for(int i=0;i<T;i++){
        int id,snode;
        int dem;
        cin>>id>>snode>>dem;
        tasks.emplace_back(id,snode,dem,-1,0,vector<int>(),false);
    }

    //初始化distant和prenode
    for(int i=1;i<=N;i++){
        for(int j=1;j<=N;j++){
            if(i==j){
                distant[i][j]=0;
                prenode[i][j]=i;
            }
            else{
                distant[i][j]=INF;
                prenode[i][j]=-1;
            }
        }
    }
    for(int i=0;i<M;i++){
        int iu=edges[i].u,iv=edges[i].v;
        int cost=edges[i].past_cost;
        if(cost<distant[iu][iv]){
            distant[iu][iv]=cost;
            distant[iv][iu]=cost;
            prenode[iu][iv]=iu;
            prenode[iv][iu]=iv;
        }
    }
    for(int k=1;k<=N;k++){
        for(int i=1;i<=N;i++){
            for(int j=1;j<=N;j++){
                if(distant[i][k]!=INF && distant[k][j]!=INF && distant[i][j]>distant[i][k]+distant[k][j]){
                    distant[i][j]=distant[i][k]+distant[k][j];
                    prenode[i][j]=prenode[k][j];
                }
            }
        }
    }
    //执行各项要求
    basic_requirement();
    advanced_requirement();
    additional_requirement();
    return 0;
}

void basic_requirement(){
    for(int i=0;i<N;i++) nodes[i].usedgpu=0;//重置usedgpu
    for(int i=0;i<T;i++){
        task &tsk=tasks[i];
        int minusedgpu=INF;
        int bestnode=-1;
        for(int j=0;j<N;j++){
            node &nd=nodes[j];
            if(nd.usedgpu+tsk.demand<=nd.capacity){
                if(nd.usedgpu<minusedgpu){
                    minusedgpu=nd.usedgpu;
                    bestnode=j;
                }
            }
        }
        tsk.end_node=nodes[bestnode].node_id;
        nodes[bestnode].usedgpu+=tsk.demand;
        tsk.mig_cost=0;
    }
    //输出结果
    cout<<"基础要求输出:"<<endl;
    for(int i=0;i<T;i++){
        task &tsk=tasks[i];
        cout<<tsk.task_id<<" "<<tsk.started_node<<" "<<tsk.end_node<<endl;
    }
    for(int i=0;i<N;i++){
        node &nd=nodes[i];
        cout<<nd.node_id<<" "<<nd.usedgpu<<endl;
    }
}

void advanced_requirement(){
    for(int i=0;i<N;i++) nodes[i].usedgpu=0;
    for(int i=0;i<T;i++){
        tasks[i].end_node=-1;
        tasks[i].mig_cost=0;
        tasks[i].ismiged=false;
        tasks[i].path.clear();
    }//重置usedgpu与tasks
    for(int i=0;i<T;i++){
        task &tsk=tasks[i];
        int start=tsk.started_node;
        int mincost=INF;
        int bestnode=-1;
        //优先尝试留在原节点
        for(int j=0;j<N;j++){
            node &nd=nodes[j];
            if(nd.node_id==start){
                if(nd.usedgpu+tsk.demand<=nd.capacity){
                    bestnode=j;
                    mincost=0;
                    break;
                }
            }
        }
        //遍历所有可行节点
        if(mincost!=0){
            for(int j=0;j<N;j++){
                node &nd=nodes[j];
                if(nd.node_id!=start){
                    if(nd.usedgpu+tsk.demand<=nd.capacity){
                        int cost=distant[start][nd.node_id]*tsk.demand;
                        if(cost<mincost){
                            mincost=cost;
                            bestnode=j;
                        }
                    }
                }
            }
        }
        tsk.end_node=nodes[bestnode].node_id;
        tsk.mig_cost=mincost;
        nodes[bestnode].usedgpu+=tsk.demand;
        //记录最短路径
        vector<int> path;
        int cur=tsk.end_node;
        while(cur!=start){
            path.push_back(cur);
            cur=prenode[start][cur];
        }
        path.push_back(start);
        reverse(path.begin(),path.end());
        tsk.path=path;
    }

    int totalcost=0;
    for(int i=0;i<T;i++) totalcost+=tasks[i].mig_cost;

    //输出结果
    cout<<"高级要求输出:"<<endl;
    for(int i=0;i<T;i++){
        task &tsk=tasks[i];
        cout<<tsk.task_id<<" "<<tsk.started_node<<" "<<tsk.end_node<<" "<<tsk.mig_cost<<endl;
    }
    for(int i=0;i<N;i++){
        node &nd=nodes[i];
        cout<<nd.node_id<<" "<<nd.usedgpu<<endl;
    }
    cout<<totalcost<<endl;
}

void additional_requirement(){
    for(int i=0;i<M;i++) edges[i].rem_bandwidth=edges[i].bandwidth;
    for(int i=0;i<T;i++) tasks[i].ismiged=false;//重置
    int count=0;//轮次计数
    vector<vector<string>> roundlog;//迁移日志
    while(1){
        bool allmiged=true;
        for(int i=0;i<T;i++){
            if(!tasks[i].ismiged){
                allmiged=false;
                break;
            }
        }
        if(allmiged) break;

        count++;
        roundlog.emplace_back();
        for(int i=0;i<M;i++) edges[i].rem_bandwidth=edges[i].bandwidth;
        for(int i=0;i<T;i++){
            task &tsk=tasks[i];
            if(tsk.ismiged) continue;
            vector<int> &path=tsk.path;
            if(path.size()<2){
                tsk.ismiged=true;
                roundlog.back().push_back(to_string(count)+" "+to_string(tsk.task_id)+" "+to_string(tsk.started_node)+" "+to_string(tsk.end_node));
                continue;
            }
            //检查路径上所有链路是否有剩余带宽
            bool canmiged=true;
            vector<edge*> usededges;
            int pathsize=path.size();
            for(int j=0;j<pathsize-1;j++){
                int u=path[j];
                int v=path[j+1];
                string edgekey=u<v?to_string(u)+","+to_string(v):to_string(v)+","+to_string(u);
                edge *ed=edgemap[edgekey];
                if(ed->rem_bandwidth<=0){
                    canmiged=false;
                    break;
                }
                usededges.push_back(ed);
            }
            if(canmiged){
                for(auto ed:usededges){
                    ed->rem_bandwidth--;
                }
                tsk.ismiged=true;
                roundlog.back().push_back(to_string(count)+" "+to_string(tsk.task_id)+" "+to_string(tsk.started_node)+" "+to_string(tsk.end_node));
            }
        }
    }
    int totalcost=0;
    for(int i=0;i<T;i++) totalcost+=tasks[i].mig_cost;

    //输出结果
    cout<<"附加要求输出:"<<endl;
    //任务最终分配结果
    for(int i=0;i<T;i++){
        task &tsk=tasks[i];
        cout<<tsk.task_id<<" "<<tsk.started_node<<" "<<tsk.end_node<<" "<<tsk.mig_cost<<endl;
    }
    //各节点最终负载
    for(int i=0;i<N;i++){
        node &nd=nodes[i];
        cout<<nd.node_id<<" "<<nd.usedgpu<<endl;
    }
    //总迁移成本和轮次
    cout<<totalcost<<endl;
    cout<<count<<endl;
    //迁移日志
    for(auto &round:roundlog){
        for(auto &logentry:round){
            cout<<logentry<<endl;
        }
    }
}