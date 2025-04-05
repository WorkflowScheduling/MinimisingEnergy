#include "utils.cpp"

void DeadlineDistrubution(FullWorkflow &wf, vector<VM> &Vms){
    auto bestVM = getVmWithBestCp(Vms);
    double omega = 0;
    double bestfeq = *max_element(bestVM.freq.begin(),bestVM.freq.end());
    for (auto node : wf.nodes){
        omega+=node.worstCase/(bestVM.getCompPower()*bestfeq);
    }
    double normalisedDeadline = (wf.deadline-wf.arrivalTime)/omega;
    vector<int> indeg(wf.nodes.size());
    queue<int> qu;
    for (int i = 0; i < wf.nodes.size(); i++){
        indeg[i] = wf.nodes[i].parent.size();
        if(indeg[i]==0){
            qu.push(i);
        }
    }
    double previous_dl =0;
    while(!qu.empty()){
        auto node = qu.front();
        qu.pop();
        vector<int> curr;
        curr.push_back(node);
        while (!qu.empty() ){
            auto node = qu.front();
            qu.pop();
            curr.push_back(node);
        }
        double omega_x = 0;
        for(auto node: curr){
            omega_x+=wf.nodes[node].worstCase/(bestVM.getCompPower()*bestfeq);
        }
        for(auto node : curr){
            wf.nodes[node].levelDeadline = previous_dl + omega_x*normalisedDeadline;
            for(auto child : wf.nodes[node].children){
                indeg[child.first]--;
                if(indeg[child.first]==0){
                    qu.push(child.first);
                }
            }
        }
        previous_dl+=omega_x*normalisedDeadline;
    }
    for (auto &node : wf.nodes){
        node.nodeDeadline = node.levelDeadline;
        node.nodeDeadline = max(node.nodeDeadline,node.earliestFinishTime);
        node.nodeDeadline = min(node.nodeDeadline,node.latestFinishTime);
    }
}

// Uses DD to schedule
// Order is topsort order
int main()
{
    vector<double> reliabilities = {1.5};
    for(auto rel : reliabilities){
    ofstream out("./Results/ddnavie_WithDeadline"+ to_string(rel)+"_mu_"+to_string(mu)+".txt");
    out<<files.size()<<endl;
    for (auto fileName :files){
    // string fileName = "Montage_25.txt";
        auto wf = parseFullWorkflow(fileName);
        wf.realibiltyConstraint = 0.95;
        wf.deadline = (wf.deadline/1.5)*rel;
        auto p = isDAG(wf, wf.nodes.size());
        if(!p.first)
        {
            cout<<fileName<<": Not a DAG!"<<endl;
            exit(EXIT_FAILURE);
        }

        auto VMs = parseVms();

        wf.deadline = wf.deadlineCPWc/getVmWithBestCp(VMs).getCompPower();
         wf.deadline = (wf.deadline/1.5)*rel;

        assignEST(wf, p.second, VMs);
        assignLST(wf, p.second, VMs);
        DeadlineDistrubution(wf,VMs);

        wf.setPrintingEarliestAndLatestTimes();

        unsigned seed = static_cast<unsigned>(time(nullptr));
        default_random_engine rng(seed);

        //auto workflowNodes = wf.nodes;
        //shuffle(workflowNodes.begin(), workflowNodes.end(), rng);
        //Order of traversal top to bottom
        //reverse(p.second.begin(), p.second.end());
        // for(int i=0; i<p.second.size(); i++)
        // {
        //     workflowNodes[i] = wf.nodes[p.second[i]];
        // }
        // Sort the nodes based on highest energy taken
        auto compareOnEnergy = [](const Node a,const Node b) {
            if(a.nodeDeadline == b.nodeDeadline){
                return a.cost > b.cost;
            }
            return a.nodeDeadline < b.nodeDeadline;
        };
        auto workflowNodes = wf.nodes;
        sort(workflowNodes.begin(), workflowNodes.end(), compareOnEnergy);
        schedule(wf, VMs, workflowNodes, true);
        
        wf.setReliabilityAndEnergy();

        if (Subtract(wf.totalRel,wf.realibiltyConstraint)<0){
            meetReliability(wf);
        }

        // printResults(fileName,wf,"ddnaive");
        int max_indeg = 0;
        for(auto node : wf.nodes){
            max_indeg = max(max_indeg,(int)node.parent.size());
        }
        out<<fileName<<" "<<wf.nodes.size()<<" "<<max_indeg<<" "<<wf.deadline<<" "<<wf.realibiltyConstraint<<" ";
        out<<wf.nodes[wf.nodes.size()-1].finishTime<<" "<<wf.totalEnergy<<" "<<wf.totalRel<<endl;
        wf.checkDAGConstraint();
    }
    }
    return 0;
}
