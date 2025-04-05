#include "dataClasses.cpp"
#include<bits/stdc++.h>
using namespace std;

class sotaNode : public Nodes {
    public: 
        // Has pointer to the VM assigned
        double avgexec;
        double priority;
        double prereliable;
        double maxreliable;

        friend ostream& operator<<(ostream& out, const sotaNode& node) {
                out << "Node: "<< node .nodeIndex << endl;
                out << "Replicas : " << node.numOfReplicas << endl;
                out << "Worst Case Execution : " << node.worstCase << endl;
                out << "Avg Exec: "<< node.avgexec << endl;
                out << "Priority: "<< node.priority <<endl;
                out << "PreReliablity: "<< node.prereliable << endl;
                out << "MaxReliablity: "<< node.maxreliable << endl;
                out << "Start Time : " << node.startTime << endl;
                out << "Finish Time : " << node.finishTime << endl;
                out << "Assigned Frequency: "<< node.assignedFreq<<endl;
                out << "Assigned VM: "<< node.assignedVM << endl;
                out << "Reliability : " << node.realibilty << endl;
                out << "Energy : " << node.cost << endl;
                out << "-------------------------------------------------------" << endl;
                return out;
        }

};

class SotaWorkflow {
    public:
        double arrivalTime = 0.0;
        vector<sotaNode> nodes;
        double realibiltyConstraint;
        double deadline;
        double totalRel = 0.0;
        double totalEnergy = 0.0;
        double deadlineCPWc;

        // Set the global workflow values
        void setReliabilityAndEnergy()
        {
            double rel = 1.0;
            double energy= 0.0;
            for(auto x : nodes)
            {
                rel *= x.realibilty;
                energy += x.cost;
            }
            totalEnergy = energy;
            totalRel = rel;
        }

        // Check if reliability is satisfied on scheduling on a new VM
        bool isReliabilitySatisfied(int nodeId, double new_freq, VM &vm) {
            sotaNode node = nodes[nodeId];
            double rem_rel = totalRel / node.realibilty;
            double new_rel = rem_rel * vm.getReliability(new_freq,node.worstCase);
            return new_rel >= realibiltyConstraint;
        }
};

SotaWorkflow parseFullWorkflow(string file){
    string path = "../Dataset/Parsed/"+file;
    ifstream cin(path.c_str());
    SotaWorkflow wf;
    int numOfNodes;
    cin>>numOfNodes;
    // To add a dummy start and end node
    numOfNodes+=2;
    double reliabilty;
    cin>>wf.realibiltyConstraint;
    cin>>wf.deadlineCPWc;
    wf.nodes.resize(numOfNodes);
    for(int i = 1 ; i < numOfNodes-1 ; i++){
        wf.nodes[i].nodeIndex = i;
        cin>>wf.nodes[i].worstCase;
        int edges;
        cin>>edges;
        long long int edge,weight;
        while (edges--)
        {
            cin>>edge>>weight;
            wf.nodes[i].children.push_back({edge+1,weight});
            wf.nodes[edge+1].parent.push_back({i,weight});
        }
    }

    // Dummy Nodes
    wf.nodes[0].nodeIndex = 0; wf.nodes[numOfNodes-1].nodeIndex = numOfNodes-1;
    wf.nodes[0].worstCase = 0; wf.nodes[numOfNodes-1].worstCase = 0;
    wf.nodes[0].startTime = wf.arrivalTime;
    wf.nodes[0].finishTime = wf.arrivalTime;
    wf.nodes[0].dummy = 1; wf.nodes[numOfNodes-1].dummy = 1;
    for(int i=1; i<numOfNodes-1; i++)
    {
        // Root Nodes
        if(wf.nodes[i].parent.size()==0){
            wf.nodes[0].children.push_back({i,0});
            wf.nodes[i].parent.push_back({0, 0});
        }
        // Leaf Nodes
        else if(wf.nodes[i].children.size()==0){
            wf.nodes[i].children.push_back({numOfNodes-1, 0});
            wf.nodes[numOfNodes-1].parent.push_back({i, 0});
        }
    }
    return wf;
}

void printResults(string fileName, SotaWorkflow&wf)
{
    string path = "./Results/sota_"+fileName;
    ofstream cout(path.c_str());

    for (auto node : wf.nodes){
        cout<<node;
    }

    cout<<"Total energy: "<<wf.totalEnergy<<endl;
    cout<<"Total Reliability: "<<wf.totalRel<<endl;
    cout<<"Reliability Constraint: "<<wf.realibiltyConstraint<<endl;
    cout<<"Makespan: "<<wf.nodes.back().finishTime<<endl;
    cout<<"Deadline Constraint: "<<wf.deadline<<endl;
}

void CalculateAvgExec(SotaWorkflow &wf, vector<VM> &VMs){
    for(auto &node : wf.nodes){
        double sum = 0;
        int count = 0;
        for(auto VM : VMs){
            for (auto freq : VM.freq){
                sum += node.worstCase/(freq*VM.getCompPower());
                count++;
            }
        }
        node.avgexec = sum/count;
    }
}

double CalcPriroityRecursive(SotaWorkflow &wf, int nodeIndex){
    double mx = 0;
    if (wf.nodes[nodeIndex].priority > 0) return wf.nodes[nodeIndex].priority;
    for(auto child : wf.nodes[nodeIndex].children){
        mx = max(mx,CalcPriroityRecursive(wf, child.first));
    }
    wf.nodes[nodeIndex].priority = mx + wf.nodes[nodeIndex].avgexec;
    return wf.nodes[nodeIndex].priority;
}

void CalculatePriority(SotaWorkflow &wf){
    int n = wf.nodes.size();
    for (int i =0;i<n;i++){
        CalcPriroityRecursive(wf, i);
    }
}

void CalcPreReliable(SotaWorkflow &wf, vector<VM> &VMs){
    double Rmax = 1;
    for(auto &node: wf.nodes){
        for (auto VM: VMs){
            node.maxreliable = max(node.maxreliable, VM.getReliability(*max_element(VM.freq.begin(),VM.freq.end()), node.worstCase));
        }
        Rmax*=node.maxreliable;
    }
    double val = pow(wf.realibiltyConstraint/Rmax, 1.0/wf.nodes.size());
    for(auto &node: wf.nodes){
        node.prereliable = node.maxreliable*val ;
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

void ScheduleSota(SotaWorkflow &wf, vector<VM> &VMs){
    sort(wf.nodes.begin(), wf.nodes.end(), [](sotaNode &a, sotaNode &b){
        return a.priority > b.priority;
    });
    double R_j_prev = 1;
    double R_j_assign =1;
    for (auto node : wf.nodes){
        R_j_prev*=node.prereliable;
    }
    int c = 0;
    double n_root_reliabilty = pow(wf.realibiltyConstraint,1.0/wf.nodes.size());
    VM bestVm = getVmWithBestCp(VMs);
    wf.totalEnergy=0;
    for (auto &node : wf.nodes){
        R_j_prev/=node.prereliable;
        double r_sub = wf.realibiltyConstraint/(R_j_assign*R_j_prev);
        if(c++>0 && R_j_assign>pow(n_root_reliabilty,c-1)){
            r_sub = min(r_sub,n_root_reliabilty);
        }
        VM assignedVM = bestVm;
        double assignedFreq = *max_element(assignedVM.freq.begin(),assignedVM.freq.end());
        double assignedEnergy = assignedVM.getEnergy(node.worstCase,assignedFreq);
        for(auto VM : VMs){
            for(auto freq : VM.freq){
                if(VM.getReliability(freq,node.worstCase)>=r_sub){
                    double energy = VM.getEnergy(node.worstCase,freq);
                    if(energy<assignedEnergy){
                        assignedEnergy = energy;
                        assignedFreq = freq;
                        assignedVM = VM;
                    }
                }
            }
        }
        node.assignedFreq = assignedFreq;
        node.assignedVM = assignedVM;
        node.cost = assignedEnergy;
        wf.totalEnergy+=node.cost;
        node.realibilty = assignedVM.getReliability(assignedFreq,node.worstCase);
        R_j_assign*=node.realibilty;
    }
    wf.totalRel = R_j_assign;
}

void CalFinishTimes(SotaWorkflow & wf){\
    sort(wf.nodes.begin(), wf.nodes.end(), [](sotaNode &a, sotaNode &b){
        return a.nodeIndex < b.nodeIndex;
    });
    queue<int> qu;
    vector<int> indeg(wf.nodes.size());
    for(int i =0;i<wf.nodes.size();i++){
        if(wf.nodes[i].parent.size()==0){
            qu.push(i);
            wf.nodes[i].startTime = wf.arrivalTime;
        }
        indeg[i]= wf.nodes[i].parent.size();
    }
    while(! qu.empty()){
        int indx = qu.front();
        qu.pop();
        for(auto par : wf.nodes[indx].parent){
            wf.nodes[indx].startTime = max(wf.nodes[indx].startTime,wf.nodes[par.first].finishTime);
        }
        wf.nodes[indx].finishTime = wf.nodes[indx].startTime + wf.nodes[indx].worstCase/(wf.nodes[indx].assignedFreq*wf.nodes[indx].assignedVM.getCompPower());
        for(auto child :wf.nodes[indx].children){
            indeg[child.first]--;
            if(indeg[child.first]==0){
                qu.push(child.first);
            }
        }
    }
}


int main(){
    vector<double> reliabilities = {0.95};
    for(auto rel : reliabilities){
    ofstream out("./Results/sota_WithOutDeadlineRel"+ to_string(rel)+"_mu_"+to_string(mu)+".txt");
    out<<files.size()<<endl;
    for (auto file :files){
    // string file = "Montage_25.txt";
        SotaWorkflow wf = parseFullWorkflow(file);
        vector<VM> VMs = parseVms();
        wf.deadline = wf.deadlineCPWc/getVmWithBestCp(VMs).getCompPower();
        wf.realibiltyConstraint = rel;
        CalculateAvgExec(wf, VMs);
        CalculatePriority(wf);
        CalcPreReliable(wf, VMs);
        ScheduleSota(wf, VMs);
        CalFinishTimes(wf);
        wf.setReliabilityAndEnergy();
        // printResults(file, wf);
        int max_indeg = 0;
        for(auto node : wf.nodes){
            max_indeg = max(max_indeg,(int)node.parent.size());
        }
        // printResults(fileName,wf);
        out<<file<<" "<<wf.nodes.size()<<" "<<max_indeg<<" "<<wf.deadline<<" "<<wf.realibiltyConstraint<<" ";
        out<<wf.nodes[wf.nodes.size()-1].finishTime<<" "<<wf.totalEnergy<<" "<<wf.totalRel<<endl;
    }
    }
}
