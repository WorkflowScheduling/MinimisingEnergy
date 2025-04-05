#include "utils.cpp"

// Uses LFT to extend
// Order is greatest energy first

void SwapNodes(vector<Node> &wf, int node1, int node2){
    Node temp = wf[node1];
    wf[node1] = wf[node2];
    wf[node2] = temp;
}

void GenerateNeighbour(vector<Node> &wf){
    int n = wf.size();
    int node1 = rand()%n;
    int node2 = rand()%n;
    while(node1 == node2){
        node2 = rand()%n;
    }
    SwapNodes(wf, node1, node2);
}

int main()
{
    for (auto fileName :files){
        ofstream out("./Results/simulated_annaleling_"+ fileName +".txt");
    // string fileName = "motif_small.txt";
        FullWorkflow bestworkflow = parseFullWorkflow(fileName);  
        auto wf = parseFullWorkflow(fileName);
        wf.realibiltyConstraint = 0.95;
        if (wf.nodes.size() <= 1){
            continue;
        }
        auto p = isDAG(wf, wf.nodes.size());
        if(!p.first)
        {
            cout<<fileName<<": Not a DAG!"<<endl;
            exit(EXIT_FAILURE);
        }

        auto VMs = parseVms();

        wf.deadline = wf.deadlineCPWc/getVmWithBestCp(VMs).getCompPower();

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
        wf.checkDAGConstraint();
        bestworkflow = wf;
        for (int i = 0; i < 1000; i++)
        {
            assignEST(wf, p.second, VMs);
            assignLST(wf, p.second, VMs);

            wf.setPrintingEarliestAndLatestTimes();
            GenerateNeighbour(workflowNodes);

            schedule(wf, VMs, workflowNodes);
            wf.setReliabilityAndEnergy();
            if (Subtract(wf.totalRel,wf.realibiltyConstraint)<0){
                meetReliability(wf);
            }
            if (wf.totalEnergy < bestworkflow.totalEnergy)
            {
                bestworkflow = wf;
            }
            out<<i<<","<<wf.totalEnergy<<endl;
        }
        

    }
    return 0;
}