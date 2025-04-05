#include "dataClasses.cpp"
#include<bits/stdc++.h>
using namespace std;

const double reliabilityBound =1;
const double g = 0.01;

class Node : public Nodes {
    public: 
        // Has pointer to the VM assigned
        double latestStartTime = numeric_limits<double>::max();
        double latestFinishTime = numeric_limits<double>::max();
        double earliestStartTime;
        double earliestFinishTime;
        double levelDeadline;
        double deadlineNode;

        friend ostream& operator<<(ostream& out, const Node& node) {
                out << "Node: "<< node .nodeIndex << endl;
                out << "Replicas : " << node.numOfReplicas << endl;
                out << "Worst Case Execution : " << node.worstCase << endl;
                out << "EST: "<< node._unchanged_est << " , LST: "<<node._unchanged_lst<< endl;
                out << "EFT: "<< node._unchanged_eft << " , LFT: "<<node._unchanged_lft<< endl;
                out << "Start Time : " << node.startTime << endl;
                out << "Finish Time : " << node.finishTime << endl;
                out << "Assigned Frequency: "<< node.assignedFreq<<endl;
                out << "Assigned VM: "<< node.assignedVM << endl;
                out << "Reliability : " << node.realibilty << endl;
                out << "Energy : " << node.cost << endl;
                out << "Level Deadline : "<< node.levelDeadline<<endl;
                out << "Deadline Node : "<< node.deadlineNode<<endl;  
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
        node.deadlineNode = node.levelDeadline;
        cout<<node.earliestStartTime<<" "<<node.latestFinishTime<<endl;
        node.deadlineNode = max(node.deadlineNode,node.earliestFinishTime);
        node.deadlineNode = min(node.deadlineNode,node.latestFinishTime);
    }
}

double getBestEnergy(Node &node, vector<VM>&VMs){
    VM bestVm;
    double bestEnergy = numeric_limits<double>::max();
    for(auto VM : VMs){
        for(auto freq : VM.freq){
                double energy = VM.getEnergy(node.worstCase,freq);
                if(energy<bestEnergy){
                    bestEnergy = energy;
                    bestVm = VM;
                }
        }
    }
    return bestEnergy;
}

double worstEenergy(Node &node, vector<VM>&VMs){
    double worstEnergy = 0;
    for(auto VM : VMs){
        for(auto freq : VM.freq){
            double energy = VM.getEnergy(node.worstCase,freq);
            worstEnergy = max(worstEnergy,energy);
        }
    }
    return worstEnergy;
}

double bestReliability(Node &node , vector<VM>&VMs){
    double bestRel = 0;
    for(auto VM : VMs){
        for(auto freq : VM.freq){
            double rel = VM.getReliability(freq,node.worstCase);
            if(rel>bestRel){
                bestRel = rel;
            }
        }
    }
    return bestRel;
}

double worstRealibility(Node &node, vector<VM>&VMs){
    double worstRel = numeric_limits<double>::max();
    for(auto VM : VMs){
        for(auto freq : VM.freq){
            double rel = VM.getReliability(freq,node.worstCase);
            worstRel = min(worstRel,rel);
        }
    }
    return worstRel;
}

void schedule(FullWorkflow & wf, vector<VM>&VMs){
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;
    for (int i = 0; i < wf.nodes.size(); i++){
        double deadline = wf.nodes[i].deadlineNode;
        pq.push({deadline,i});
    }
    double w_x = 1 + wf.nodes.size()*g;
    while (!pq.empty()){
        auto node = pq.top();
        pq.pop();
        int nodeIndex = node.second;
        Node &currNode = wf.nodes[nodeIndex];
        double bestEnergy = getBestEnergy(currNode,VMs);
        double worstEnergy = worstEenergy(currNode,VMs);
        double bestRel = bestReliability(currNode,VMs);
        double worstRel = worstRealibility(currNode,VMs);
        currNode.startTime = wf.arrivalTime;
        for(auto par : currNode.parent){
            currNode.startTime = max(currNode.startTime,wf.nodes[par.first].finishTime);
        }
        double curr_O = 0;
        VM assigned;
        double assignedFreq = 0;
        double slacktime = currNode.deadlineNode - currNode.startTime;
        for(auto VM : VMs){
            for(auto freq : VM.freq){
                double energy = VM.getEnergy(currNode.worstCase,freq);
                double rel = VM.getReliability(freq,currNode.worstCase);
                double O = (energy-worstEnergy)/(bestEnergy-worstEnergy) + w_x*(rel-worstRel)/(bestRel-worstRel);
                double tou = wf.nodes.size()*wf.realibiltyConstraint*(1-rel)/VM.wValue;
                if(O>curr_O && slacktime>=currNode.worstCase/(freq*VM.getCompPower()) && tou<reliabilityBound){
                    curr_O = O;
                    assigned = VM;
                    assignedFreq = freq;
                }
            }
        }
        if(curr_O==0){
            assigned = getVmWithBestCp(VMs);
            assignedFreq = *max_element(assigned.freq.begin(),assigned.freq.end());
        }
        currNode.assignedVM = assigned;
        currNode.assignedFreq = assignedFreq;
        currNode.cost = assigned.getEnergy(currNode.worstCase,assignedFreq);
        currNode.realibilty = assigned.getReliability(assignedFreq,currNode.worstCase);
        currNode.finishTime = currNode.startTime + currNode.worstCase/(assignedFreq*assigned.getCompPower());

        double tou = wf.nodes.size()*wf.realibiltyConstraint*(1-assigned.getReliability(assignedFreq,currNode.worstCase))/assigned.wValue;
        if(tou >reliabilityBound){
            currNode.realibilty = 1- pow(Subtract(1,currNode.realibilty),2);
            currNode.numOfReplicas++;
            currNode.cost = assigned.getEnergy(currNode.worstCase,assignedFreq)*2;
        }
    }
}


void printResults(string fileName, FullWorkflow&wf)
{
    string path = "./Results/sotaDeadline_"+fileName;
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

int main()
{
   vector<double> reliabilities = {1.5};
    for(auto rel : reliabilities){
    ofstream out("./Results/sota_WithDeadline"+ to_string(rel)+"_mu_"+to_string(mu)+".txt");
    out<<files.size()<<endl;
    for (auto fileName :files){
    // string fileName = "Epigenomics_24.txt";
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
        DeadlineDistrubution(wf, VMs);
        schedule(wf, VMs);
        wf.setReliabilityAndEnergy();
        // printResults(fileName, wf);
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
