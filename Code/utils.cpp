#include "dataClasses.cpp"
#include<bits/stdc++.h>
using namespace std;

class Node : public Nodes {
    public: 
        // Has pointer to the VM assigned
        double latestStartTime = numeric_limits<double>::max();
        double latestFinishTime = numeric_limits<double>::max();
        double earliestStartTime;
        double earliestFinishTime;
        double levelDeadline;
        double nodeDeadline;

        friend ostream& operator<<(ostream& out, const Node& node) {
                out << "Node: "<< node .nodeIndex << endl;
                out << "Replicas : " << node.numOfReplicas << endl;
                out << "Worst Case Execution : " << node.worstCase << endl;
                out << "EST: "<< node._unchanged_est << " , LST: "<<node._unchanged_lst<< endl;
                out << "EFT: "<< node._unchanged_eft << " , LFT: "<<node._unchanged_lft<< endl;
                out <<"Deadline: "<<node.nodeDeadline<<endl;
                out << "Start Time : " << node.startTime << endl;
                out << "Finish Time : " << node.finishTime << endl;
                out << "Assigned Frequency: "<< node.assignedFreq<<endl;
                out << "Assigned VM: "<< node.assignedVM << endl;
                out << "Reliability : " << node.realibilty << endl;
                out << "Energy : " << node.cost << endl;
                out << "-------------------------------------------------------" << endl;
                return out;
        }

        // Make earliest and latest starting and finishing time same as actual
        // starting and finishing time once scheduled
        void UpdateEarliestAndLatest()
        {
            latestFinishTime = finishTime;
            latestStartTime = startTime;
            earliestFinishTime = finishTime;
            earliestStartTime = startTime;
        }

        // Schedule a node by setting its parameters
        void SetNodeParams(VM &ass_vm, double new_freq, double new_st, bool replication=false)
        {
            assignedVM = ass_vm;
            assignedFreq = new_freq;
            cost = ass_vm.getEnergy(worstCase,assignedFreq);
            setStartAndFinishTime(new_st, new_freq, ass_vm);
            if(!replication)
            realibilty = assignedVM.getReliability(new_freq, worstCase);
            else {
                realibilty = 1- pow(Subtract(1,assignedVM.getReliability(new_freq, worstCase)),2);
                numOfReplicas++;
            }   
        }

        // Set the _unchanged parameters for printing the actual Earliest and latest times
        // as the publi variables will get modified at last
        void setUnchangedEarliestandLatestVars()
        {
            _unchanged_eft = earliestFinishTime;
            _unchanged_est = earliestStartTime;
            _unchanged_lft = latestFinishTime;
            _unchanged_lst = latestStartTime;
        }
    private:
        // Only for printing purposes
        double _unchanged_est;
        double _unchanged_lst;
        double _unchanged_eft;
        double _unchanged_lft;

};

class FullWorkflow {
    public:
        double arrivalTime = 0.0;
        vector<Node> nodes;
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
        bool isReliabilitySatisfied(int nodeId, double new_freq, VM &vm, bool replicate = false) {
            Node node = nodes[nodeId];
            double rem_rel = totalRel / node.realibilty;
            double new_rel = rem_rel * (!replicate ? vm.getReliability(new_freq,node.worstCase) : vm.getReliability(new_freq,node.worstCase) * (2- vm.getReliability(new_freq,node.worstCase)));
            return Subtract(new_rel,realibiltyConstraint)>=0.0;
        }

        void setPrintingEarliestAndLatestTimes()
        {
            for(auto &node : nodes)
            {
                node.setUnchangedEarliestandLatestVars();
            }
        }

        // Function to check if the DAG constraint is met
        bool checkDAGConstraint() {
            for (int idx = 0; idx < nodes.size(); idx++) {
                Node currentNode = nodes[idx];

                for (auto child : currentNode.children) {
                    int childIndex = child.first;
                    Node childNode = nodes[childIndex];

                    // Ensure that the start time of the child node is >= the finish time of all parent nodes
                    bool constraintMet = false;
                    for (auto par : currentNode.parent) {
                        Node parentNode = nodes[par.first];

                        if (childNode.startTime < parentNode.finishTime) {
                            cerr << "DAG constraint violated: Node " << childIndex << " starts before node " << par.first << " finishes.\n";
                            return false;
                        }
                    }
                }
            }
            return true;
        }
};

struct CompareEnergyFromNodeId {
    const FullWorkflow& wf; 

    CompareEnergyFromNodeId(const FullWorkflow& workflow) : wf(workflow) {}

    bool operator()(const int& a, const int& b) {
        // (use > for min-heap)
        if(Subtract(wf.nodes[a].cost,wf.nodes[b].cost)==0)
        return wf.nodes[a].realibilty > wf.nodes[b].realibilty;
        return wf.nodes[a].cost > wf.nodes[b].cost;
    }
};

// To be done when a node is scheduled
// found_u is the u which has to be replicated in case of found = 3
void updateNodeParamsAndGlobalValues(int found, double curr_e, VM &new_vm, double new_f, int node_index, FullWorkflow &wf, priority_queue<int, vector<int>, CompareEnergyFromNodeId>&pq, int found_u) {
    double old_energy = wf.nodes[node_index].cost;
    double old_reliability = wf.nodes[node_index].realibilty;

    bool replicate = (found == 2);  // If found is 2, then replication
    double adjusted_energy_rep = new_vm.getEnergy(wf.nodes[node_index].worstCase, new_f);

    // Try to replicate a node that has minimum energy
    if(found==2)
    {
        // If the lowest energy node so far has lesser energy than node_index, replicate that instead
        if(!pq.empty() && wf.nodes[pq.top()].cost < adjusted_energy_rep && wf.isReliabilitySatisfied(pq.top(),wf.nodes[pq.top()].assignedFreq,wf.nodes[pq.top()].assignedVM,true))
        {
            replicate = false;
            cout<<"Found 2 but Node: "<<pq.top()<<" is replicated instead of Node: "<<node_index<<"->"<<wf.nodes[pq.top()].cost*2<<"< "<<wf.nodes[node_index].cost*2<<endl;
            wf.nodes[pq.top()].cost *= 2;
            wf.nodes[pq.top()].numOfReplicas++;
            old_reliability *= wf.nodes[pq.top()].realibilty;
            curr_e = curr_e - 2*adjusted_energy_rep + 2*wf.nodes[pq.top()].cost;
            wf.nodes[pq.top()].realibilty = wf.nodes[pq.top()].realibilty*(2-wf.nodes[pq.top()].realibilty);
            wf.totalRel *= wf.nodes[pq.top()].realibilty;
            pq.pop();
        }
    } else if(found == 3)
    {
        wf.nodes[found_u].cost *= 2;
        wf.nodes[found_u].numOfReplicas++;
        old_reliability *= wf.nodes[found_u].realibilty;
        wf.nodes[found_u].realibilty = wf.nodes[found_u].realibilty*(2-wf.nodes[found_u].realibilty);
        wf.totalRel *= wf.nodes[found_u].realibilty;
    }

    wf.nodes[node_index].SetNodeParams(new_vm, new_f, wf.nodes[node_index].startTime, replicate);

    // Update global values
    wf.totalEnergy = curr_e;
    wf.totalRel = (wf.totalRel / old_reliability);
    wf.totalRel *= wf.nodes[node_index].realibilty;

    if (found == 3) {
        cout << "Node: " << node_index << " is scheduled with type 3 (Replicate later)" << endl;
    }
}

// This gives out a top sort order and tells if the workflow is DAG
pair<bool, vector<int>> isDAG(FullWorkflow &wf, int numOfNodes) {
    vector<int> inDegree(numOfNodes, 0);
    vector<int>topSortOrder;
    topSortOrder.reserve(numOfNodes);


    for (int i = 0; i < numOfNodes; i++) {
        for (const auto &child : wf.nodes[i].children) {
            inDegree[child.first]++;
        }
    }

    queue<int> q;
    for (int i = 0; i < numOfNodes; i++) {
        if (inDegree[i] == 0) {
            q.push(i);
        }
    }

    int processedNodes = 0;

    // Process nodes in topological order
    while (!q.empty()) {
        int u = q.front();
        topSortOrder.push_back(u);
        q.pop();
        processedNodes++;

        for (const auto &child : wf.nodes[u].children) {
            int v = child.first;
            inDegree[v]--;
            if (inDegree[v] == 0) {
                q.push(v);
            }
        }
    }

    return {(processedNodes == numOfNodes), topSortOrder};
}


FullWorkflow parseFullWorkflow(string file){
    string path = "../Dataset/Parsed/"+file;
    ifstream cin(path.c_str());
    FullWorkflow wf;
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

// Gets the VM with Best CP
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

// Assigns Earliest Start time and Earliest Finish Time to the workflow
void assignEST(FullWorkflow& wf, vector<int>&topsort, vector<VM>&VMs)
{
    VM bestVm;
    int numOfNodes= topsort.size();
    bestVm =  getVmWithBestCp(VMs);

    // First one is the dummy node
    wf.nodes[topsort[0]].earliestStartTime = wf.arrivalTime;
    double maxfreq = *max_element(bestVm.freq.begin(),bestVm.freq.end());
    double execTime = wf.nodes[topsort[0]].worstCase/(maxfreq* bestVm.getCompPower());
    wf.nodes[topsort[0]].earliestFinishTime = wf.nodes[topsort[0]].earliestStartTime + execTime;
    wf.nodes[topsort[0]].assignedVM = bestVm;
    wf.nodes[topsort[0]].assignedFreq = maxfreq;
    wf.nodes[topsort[0]].cost = bestVm.getEnergy(wf.nodes[topsort[0]].worstCase,maxfreq);
    wf.nodes[topsort[0]].realibilty = bestVm.getReliability(maxfreq, wf.nodes[topsort[0]].worstCase);
    wf.nodes[topsort[0]].setStartAndFinishTime(wf.nodes[topsort[0]].earliestStartTime, maxfreq, bestVm);

    for(int i=0; i<numOfNodes; i++)
    {
        for(auto child : wf.nodes[topsort[i]].children)
        {
            wf.nodes[child.first].earliestStartTime = max(wf.nodes[child.first].earliestStartTime, wf.nodes[topsort[i]].earliestFinishTime);
            double maxfreq = *max_element(bestVm.freq.begin(),bestVm.freq.end());
            double execTime = wf.nodes[child.first].worstCase/(maxfreq* bestVm.getCompPower());
            wf.nodes[child.first].earliestFinishTime = wf.nodes[child.first].earliestStartTime + execTime;
            wf.nodes[child.first].assignedVM = bestVm;
            wf.nodes[child.first].assignedFreq = maxfreq;
            wf.nodes[child.first].cost = bestVm.getEnergy(wf.nodes[child.first].worstCase,maxfreq);
            wf.nodes[child.first].realibilty = bestVm.getReliability(maxfreq, wf.nodes[child.first].worstCase);
            wf.nodes[child.first].setStartAndFinishTime(wf.nodes[child.first].earliestStartTime, maxfreq, bestVm);
        }
    }

    wf.setReliabilityAndEnergy();
}


// Assign Latest Start Time and Latest Finish Time to the Workflow
// Here LFT should include the datatransfer time to its children
void assignLST(FullWorkflow& wf, vector<int>&topSort, vector<VM>&VMs)
{
    VM bestVm = getVmWithBestCp(VMs);
    int numOfNodes= wf.nodes.size();
    vector<bool> visited(numOfNodes,false);

    // The final node is always the sink node
    wf.nodes[topSort.back()].latestFinishTime = wf.deadline;
    wf.nodes[topSort.back()].latestStartTime = wf.nodes[topSort.back()].latestFinishTime;


    for(int i=numOfNodes-1; i>=0; i--)
    {
        int indx = topSort[i];
        double maxfreq = *max_element(bestVm.freq.begin(),bestVm.freq.end());
        double execTime = wf.nodes[indx].worstCase/(maxfreq* bestVm.getCompPower());

        for(auto child : wf.nodes[indx].children)
        {
            wf.nodes[indx].latestFinishTime = min(wf.nodes[indx].latestFinishTime, wf.nodes[child.first].latestStartTime);
        }
        wf.nodes[indx].latestStartTime = Subtract(wf.nodes[indx].latestFinishTime, execTime);
    }
}

// Schedules according to an order of workflowNodes specified
void schedule(FullWorkflow & wf, vector<VM>&VMs, vector<Node>&workflowNodes, bool use_deadline_distribution=false, bool takeRandomDecision=false){
    // To store the nodes that are already scheduled so far
    priority_queue<int, vector<int>, CompareEnergyFromNodeId>pq{CompareEnergyFromNodeId(wf)};
    srand(static_cast<unsigned int>(std::time(0)));

    for(auto node : workflowNodes){
        int node_index = node.nodeIndex;

        if (Subtract(wf.nodes[node_index].earliestFinishTime,wf.nodes[node_index].latestFinishTime) > 0) {
            cerr << "Warning: latestFinishTime (" << wf.nodes[node_index].latestFinishTime 
                    << ") is less than earliestFinishTime (" << wf.nodes[node_index].earliestFinishTime 
                    << ") for node " << node_index << endl;
        }

        if (Subtract((use_deadline_distribution ? wf.nodes[node_index].nodeDeadline : wf.nodes[node_index].latestFinishTime), wf.nodes[node_index].startTime)<=0) {
            cerr << "Warning: " << (use_deadline_distribution ? "node_deadline" : "latestFinishTime") <<"(" << (use_deadline_distribution ? wf.nodes[node_index].nodeDeadline : wf.nodes[node_index].latestFinishTime) 
            << ") is not greater than startTime (" << wf.nodes[node_index].startTime 
            << ") for node " << node_index << ",skipping it."<<endl;
            continue;
        }

        if(takeRandomDecision)
        {
            use_deadline_distribution = rand() % 2;
        }

        // Get the minimum frequency *cp needed to stretch the duration
        double min_cpf = wf.nodes[node_index].worstCase/ Subtract((use_deadline_distribution ? wf.nodes[node_index].nodeDeadline : wf.nodes[node_index].latestFinishTime),wf.nodes[node_index].startTime);
        // found -> 1 : No replications and best solution found
        // found -> 2 : Replication and best solution found 
        // found -> 3 : Not Replicated, but replicate some other previous node (not applying for now)
        int found = 0;
        int found_u = 0;
        double new_f;
        double curr_e = wf.totalEnergy;
        VM new_vm = wf.nodes[node_index].assignedVM;
        // Find the smallest VM such that its freq is >= min_f
        for (auto &vm : VMs){
            double max_cpf_poss = vm.getCompPower() * vm.freq.back();
            // If the max_cpf possible is less than the minimum cpf required, then skip this vm
            if(max_cpf_poss - min_cpf < 0.0) continue;
            for(auto f : vm.freq){
                if(Subtract(vm.getCompPower() * f , min_cpf) >= 0.0 && Subtract(vm.getEnergy(wf.nodes[node_index].worstCase, f) , wf.nodes[node_index].cost) < 0.0){
                    if(wf.isReliabilitySatisfied(node_index, f, vm)){
                        if(Subtract(wf.totalEnergy - wf.nodes[node_index].cost + vm.getEnergy(wf.nodes[node_index].worstCase, f) , curr_e) < 0.0){
                            found = 1;
                            curr_e = Subtract(wf.totalEnergy , wf.nodes[node_index].cost) + vm.getEnergy(wf.nodes[node_index].worstCase, f);
                            new_f = f;
                            new_vm = vm;
                        }
                    } 
                    // Reliability is not satisfied but can be replicated
                    else{
                        double rep_energy = wf.totalEnergy - wf.nodes[node_index].cost + 2 * vm.getEnergy(wf.nodes[node_index].worstCase, f);
                        if(Subtract(rep_energy, curr_e) < 0.0 && wf.isReliabilitySatisfied(node_index,f,vm,true)){
                            found = 2;
                            curr_e = rep_energy;
                            new_f = f;
                            new_vm = vm;
                        } 
                        // Replication gives worse - found 3 not applying for now
                        // else if(Subtract(wf.totalEnergy - wf.nodes[node_index].cost + vm.getEnergy(wf.nodes[node_index].worstCase, f) , curr_e) < 0.0){
                        //     if(!pq.empty())
                        //     {
                        //         int u = pq.top();
                        //         found_u = u;
                        //         if(!wf.isReliabilitySatisfied(u, wf.nodes[u].assignedFreq, wf.nodes[u].assignedVM, true)) continue;
                        //         double new_energy = wf.totalEnergy - wf.nodes[node_index].cost + vm.getEnergy(wf.nodes[u].worstCase,wf.nodes[u].assignedFreq) + vm.getEnergy(wf.nodes[node_index].worstCase,f);
                        //         if(Subtract(new_energy,curr_e) < 0){
                        //             found = 3;
                        //             curr_e = new_energy;
                        //             new_f;
                        //             new_vm = vm;
                        //         }
                        //         pq.pop();
                        //     }
                        // }
                    }
                }
            }
        }

        if(found != 0)
        {
            updateNodeParamsAndGlobalValues(found, curr_e, new_vm, new_f,node_index, wf,pq, found_u);

            // Push into priority queue if not replicated
            if(wf.nodes[node_index].numOfReplicas==1 && !wf.nodes[node_index].dummy)
            pq.push(node_index);

            // If deadline isnt achieved yet, push the job to deadline so that there is more gap for predecessors
            // if(use_deadline_distribution && !Equal(wf.nodes[node_index].nodeDeadline,wf.nodes[node_index].latestFinishTime))
            // {
            //     double new_st = wf.nodes[node_index].startTime + (wf.nodes[node_index].nodeDeadline-wf.nodes[node_index].finishTime);
            //     wf.nodes[node_index].setStartAndFinishTime(new_st, wf.nodes[node_index].assignedFreq, wf.nodes[node_index].assignedVM);
            // }

            // Update the LST and EFT to be same as this starting and finishing time
            wf.nodes[node_index].UpdateEarliestAndLatest();

            // Update the starting and ending times for other nodes
            queue<int>q;

            // Updating EST, EFT, ST, FT for the successsors
            for(auto child : wf.nodes[node_index].children)
            {
                // Only if the current node index's finish time becomes greater than the 
                // child's earliest starting time, there will be a change
                if(Subtract(wf.nodes[node_index].earliestFinishTime , wf.nodes[child.first].earliestStartTime) > 0.0 && Subtract(wf.nodes[node_index].finishTime , wf.nodes[child.first].startTime) > 0.0)
                q.push(child.first);
            }
        
            while(!q.empty())
            {
                int idx = q.front();
                q.pop();

                for(auto par : wf.nodes[idx].parent){
                    wf.nodes[idx].startTime = max(wf.nodes[idx].startTime,wf.nodes[par.first].finishTime);
                    wf.nodes[idx].earliestStartTime = max(wf.nodes[idx].earliestStartTime,wf.nodes[par.first].finishTime);
                }

                wf.nodes[idx].finishTime = wf.nodes[idx].startTime + wf.nodes[idx].getExecutionTimeonVM(wf.nodes[idx].assignedVM,wf.nodes[idx].assignedFreq);
                wf.nodes[idx].earliestFinishTime = wf.nodes[idx].finishTime;
                // Edge case for nodeDeadline
                wf.nodes[idx].nodeDeadline = max(wf.nodes[idx].nodeDeadline, wf.nodes[idx].earliestFinishTime);

                for(auto child :wf.nodes[idx].children){
                    if(Subtract(wf.nodes[idx].earliestFinishTime , wf.nodes[child.first].earliestStartTime) > 0 && Subtract(wf.nodes[idx].finishTime, wf.nodes[child.first].startTime) > 0.0){
                        q.push(child.first);
                    }
                }
            }

            // Update LFT, LST for predecessors
            q.push(node_index);

            while(!q.empty())
            {
                int idx = q.front();
                q.pop();

                wf.nodes[idx].latestStartTime = Subtract(wf.nodes[idx].latestFinishTime, wf.nodes[idx].getExecutionTimeonVM(wf.nodes[idx].assignedVM,wf.nodes[idx].assignedFreq));

                for(auto par : wf.nodes[idx].parent){
                    // If only we are minimizing the latestStartTime more than what is already there in finish time
                    if(Subtract(wf.nodes[par.first].latestFinishTime ,wf.nodes[idx].latestStartTime) > 0.0)
                    {
                        wf.nodes[par.first].latestFinishTime = min(wf.nodes[par.first].latestFinishTime ,wf.nodes[idx].latestStartTime);
                        wf.nodes[par.first].latestStartTime = Subtract(wf.nodes[par.first].latestFinishTime,wf.nodes[par.first].getExecutionTimeonVM(wf.nodes[par.first].assignedVM,wf.nodes[par.first].assignedFreq));
                        q.push(par.first);
                        // Edge case for nodeDeadline
                        wf.nodes[par.first].nodeDeadline = min(wf.nodes[par.first].nodeDeadline, wf.nodes[par.first].latestFinishTime);
                    }
                }
            }
        }
    }
}

void meetReliability(FullWorkflow &workflow){
    priority_queue<pair<double,int>> pq;
    int n = (workflow).nodes.size();
    for (int i =0;i<n;i++){
        if(!workflow.nodes[i].dummy && workflow.nodes[i].numOfReplicas==1)
        pq.push({-(workflow).nodes[i].cost,i});
    }
    while((workflow).totalRel<(workflow).realibiltyConstraint && !pq.empty()){
        auto it = pq.top();
        pq.pop();
        (workflow).totalRel/=(workflow).nodes[it.second].realibilty;
        (workflow).nodes[it.second].numOfReplicas++;
        double oldRealibilty = (workflow).nodes[it.second].realibilty;
        (workflow).nodes[it.second].realibilty = oldRealibilty*(Subtract(2,oldRealibilty));
        (workflow).totalEnergy+= (workflow).nodes[it.second].cost;
        (workflow).nodes[it.second].cost*=2;
        (workflow).totalRel*=(workflow).nodes[it.second].realibilty;
    }
}

void printResults(string fileName, FullWorkflow&wf, string algoName="naive")
{
    string path = "./Results/"+algoName+"_"+fileName;
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