#include "dataClasses.cpp"
#include<bits/stdc++.h>
using namespace std;

Workflow ScheduleWorkflow(string file,vector<VM>& VMs,double rel){
    auto workflow = parseWorkflow(file);
    workflow.realibiltyConstraint = rel;
    int numOfNodes= workflow.nodes.size();
    VM bestVm;
    vector<bool> visited(numOfNodes,false);
    double currCp =0;
    for (auto v :VMs){
        double cp =*max_element(v.freq.begin(),v.freq.end())* v.getCompPower();
        if (cp>currCp){
            bestVm = v;
            currCp = cp;
        }
    }
    //BFS
    queue<int> qu;
    vector<int> indeg(numOfNodes);
    //All root Nodes
    for(int i =0;i<numOfNodes;i++){
        if(workflow.nodes[i].parent.size()==0){
            qu.push(i);
            workflow.nodes[i].startTime = workflow.arrivalTime;
            visited[i]= true;
        }
        indeg[i]= workflow.nodes[i].parent.size();
    }
    while(! qu.empty()){
        int indx = qu.front();
        qu.pop();
        //Dag Constraints
        for(auto par : workflow.nodes[indx].parent){
            workflow.nodes[indx].startTime = max(workflow.nodes[indx].startTime,workflow.nodes[par.first].finishTime);
        }
        double maxfreq = *max_element(bestVm.freq.begin(),bestVm.freq.end());
        double execTime = workflow.nodes[indx].worstCase/(maxfreq* bestVm.getCompPower());
        workflow.nodes[indx].finishTime = workflow.nodes[indx].startTime + execTime;
        workflow.nodes[indx].realibilty = bestVm.getReliability(maxfreq,workflow.nodes[indx].worstCase);
        workflow.nodes[indx].cost = bestVm.getEnergy(workflow.nodes[indx].worstCase,maxfreq);
        workflow.nodes[indx].assignedVM = bestVm;
        workflow.totalRealibility*=workflow.nodes[indx].realibilty;
        for(auto child :workflow.nodes[indx].children){
            indeg[child.first]--;
            if(indeg[child.first]==0){
                qu.push(child.first);
            }
        }
    }
    return workflow;
}

void PrintResults(string file, vector<Nodes>& workflow){
    string path = "./Results/bestCP_"+file;
    ofstream cout(path.c_str());
    double totalCost =0;
    double reliabilty = 1;
    for(int i =0;i<workflow.size();i++){
        auto node = workflow[i];
        totalCost+=node.cost;
        reliabilty*=workflow[i].realibilty;
        cout<<node;
    }
    cout<<"Total Energy :"<<totalCost<<endl;
    cout<<"Max Time :"<<workflow.back().finishTime<<endl;
    cout<<"Total Reliabilty :"<<reliabilty<<endl;
}

void meetReliability(Workflow &workflow){
    priority_queue<pair<double,int>> pq;
    int n = (workflow).nodes.size();
    for (int i =0;i<n;i++){
        if(!workflow.nodes[i].dummy && workflow.nodes[i].numOfReplicas==1)
        pq.push({-(workflow).nodes[i].cost,i});
    }
    while((workflow).totalRealibility<(workflow).realibiltyConstraint && !pq.empty()){
        auto it = pq.top();
        pq.pop();
        (workflow).totalRealibility/=(workflow).nodes[it.second].realibilty;
        (workflow).nodes[it.second].numOfReplicas++;
        double oldRealibilty = (workflow).nodes[it.second].realibilty;
        (workflow).nodes[it.second].realibilty = oldRealibilty*(2-oldRealibilty);
        workflow.totalEnergy+=(workflow).nodes[it.second].cost;
        (workflow).nodes[it.second].cost*=2;
        (workflow).totalRealibility*=(workflow).nodes[it.second].realibilty;
    }
}

 VM getVmWithBestCp(vector<VM>&VMs){
    VM bestVm;
    double currCp =0;
    for (auto v :VMs){
        double cp =*max_element(v.freq.begin(),v.freq.end())* v.getCompPower();
        if (cp>currCp){
            bestVm = v;
            currCp = cp;
        }
    }
    return bestVm;
}

int main(){
    auto VMs = parseVms();
    vector<double> deadlineFactor = {1.5};
    for(auto deadline : deadlineFactor){
    ofstream out("./Results/bestCP_WithDeadline"+to_string(deadline)+"_mu_"+to_string(mu)+".txt");
    out<<files.size()<<endl;
    for (auto file :files){
        auto scheduledWorkflow = ScheduleWorkflow(file,VMs,0.95);
        if(scheduledWorkflow.totalRealibility<scheduledWorkflow.realibiltyConstraint){
            meetReliability(scheduledWorkflow);
        }
        scheduledWorkflow.totalEnergy = 0;
        scheduledWorkflow.totalRealibility = 1;
        int max_indeg = 0;
        for(int i =0;i<scheduledWorkflow.nodes.size();i++){
            auto node = scheduledWorkflow.nodes[i];
            scheduledWorkflow.totalEnergy+=node.cost;
            scheduledWorkflow.totalRealibility*=scheduledWorkflow.nodes[i].realibilty;
            max_indeg = max(max_indeg,(int)scheduledWorkflow.nodes[i].parent.size());
        }
        auto vm = getVmWithBestCp(VMs);
        scheduledWorkflow.deadline = scheduledWorkflow.deadlineCPWc/(vm.getCompPower()*(*max_element(vm.freq.begin(),vm.freq.end())));
        out<<file<<" "<<scheduledWorkflow.nodes.size()<<" "<<max_indeg<<" "<<scheduledWorkflow.deadline<<" "<<scheduledWorkflow.realibiltyConstraint<<" ";
        out<<scheduledWorkflow.nodes[scheduledWorkflow.nodes.size()-1].finishTime<<" "<<scheduledWorkflow.totalEnergy<<" "<<scheduledWorkflow.totalRealibility<<endl;
    }
    }
    string file = "cadsr.txt";
    
    // PrintResults(file,scheduledWorkflow.nodes);
}
