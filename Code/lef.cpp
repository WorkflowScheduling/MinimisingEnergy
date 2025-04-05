#include "utils.cpp"

// Uses LFT to extend
// Order is greatest energy first
int main()
{
    vector<double> reliabilities = {1.5};
    for(auto rel : reliabilities){
    ofstream out("./Results/navie1_WithDeadline"+ to_string(rel)+"_mu_"+to_string(mu)+".txt");
    out<<files.size()<<endl;
    for (auto fileName :files){
    // string fileName = "motif_small.txt";
        auto wf = parseFullWorkflow(fileName);
        wf.realibiltyConstraint = 0.95;

        auto p = isDAG(wf, wf.nodes.size());
        if(!p.first)
        {
            cout<<fileName<<": Not a DAG!"<<endl;
            exit(EXIT_FAILURE);
        }

        auto VMs = parseVms();

        wf.deadline = wf.deadlineCPWc/getVmWithBestCp(VMs).getCompPower();
        wf.deadline/=1.5;
        wf.deadline*=rel;

        assignEST(wf, p.second, VMs);
        assignLST(wf, p.second, VMs);

        wf.setPrintingEarliestAndLatestTimes();

        // Sort the nodes based on highest energy taken
        auto compareOnEnergy = [](const Node a,const Node b) {
            if(a.cost == b.cost){
                return a.realibilty > b.realibilty;
            }
            return a.cost > b.cost;
        };
        auto workflowNodes = wf.nodes;
        sort(workflowNodes.begin(), workflowNodes.end(), compareOnEnergy);

        schedule(wf, VMs, workflowNodes);
        
        wf.setReliabilityAndEnergy();

        if (Subtract(wf.totalRel,wf.realibiltyConstraint)<0){
            meetReliability(wf);
        }
        
        int max_indeg = 0;
        for(auto node : wf.nodes){
            max_indeg = max(max_indeg,(int)node.parent.size());
        }
        // printResults(fileName,wf);
        out<<fileName<<" "<<wf.nodes.size()<<" "<<max_indeg<<" "<<wf.deadline<<" "<<wf.realibiltyConstraint<<" ";
        out<<wf.nodes[wf.nodes.size()-1].finishTime<<" "<<wf.totalEnergy<<" "<<wf.totalRel<<endl;
        wf.checkDAGConstraint();
    }
    }
    return 0;
}
