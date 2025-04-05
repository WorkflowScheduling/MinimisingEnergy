#include<bits/stdc++.h>
// #include <filesystem>
using namespace std;

const double EPSILON = 1e-9; 
const int mu = 3;
const vector<string> files = {"CyberShake_100.txt","CyberShake_50.txt","Epigenomics_100.txt","Epigenomics_24.txt","Epigenomics_46.txt","Inspiral_100.txt","Inspiral_1000.txt","Inspiral_30.txt","Inspiral_50.txt","Montage_100.txt","Montage_1000.txt","Montage_25.txt","Montage_50.txt","Sipht_100.txt","Sipht_1000.txt","Sipht_30.txt","Sipht_60.txt","avianflu_large.txt","avianflu_medium.txt","avianflu_small.txt","cadsr.txt","floodplain.txt","gene2life.txt","glimmer.txt","leadadas.txt","leaddm..txt","leaddm.txt","leadmm..txt","leadmm.txt","mcstas.txt","mememast.txt","molsci.txt","motif_large.txt","motif_medium.txt","motif_small.txt","psload_large.txt","psload_medium.txt","psload_small.txt","psmerge_medium.txt","psmerge_small.txt","scoop_large..txt","scoop_large.txt","scoop_medium.txt","scoop_small.txt"};


bool Equal(double a, double b, double epsilon = EPSILON) {
    return fabs(a - b) < epsilon;
}

// Subtract a - b with precision
double Subtract(double a, double b, double epsilon = EPSILON)
{
    if(Equal(a,b)) return 0.0;
    else return a-b;
}

class VM{
    public:
        string cloudName;
        string VMtype;
        vector<double> freq;
        double pStatic;
        double pDynamic;
        double roValue;
        double wValue;

        // Get computational power of VM
        // TODO: Find a better function
        double getCompPower() {
            return sqrt(pStatic + pDynamic);
        }

        // Get Energy expended at a frequency
        double getEnergy(double wc, double freq) {
            double power = pStatic + pDynamic * pow(freq,mu);
            double time = wc/(freq * getCompPower());
            return power*time;
        }

        // Get reliability
        double getReliability(double freq, double wc) {
            return exp(-failure_rate(freq) * (wc / (getCompPower() * freq)));
        }

        // Get the frequencies that minimizes the energy
        vector<double> getCriticalFreqs() {
            vector<double> minFreqs;
            double minEnergy = getEnergy(1.0, freq[0]);
            minFreqs.push_back(freq[0]);

            for (int i = 1; i < freq.size(); ++i) {
                double currentEnergy = getEnergy(1.0, freq[i]);
                if (currentEnergy < minEnergy) {
                    minFreqs.clear();
                    minEnergy = currentEnergy;
                    minFreqs.push_back(freq[i]);
                } else if (Equal(currentEnergy, minEnergy)) {
                    minFreqs.push_back(freq[i]);
                }
            }
            return minFreqs;
        }

        friend ostream& operator<<(ostream& out, const VM& vm) {
            out << "Cloud Provider: "<< vm.cloudName << ", ";
            out << "r_0: "<<vm.roValue<< ", ";
            out << "w : "<<vm.wValue<< ", ";
            out << "alpha: "<<vm.pStatic<<", ";
            out << "beta: "<<vm.pDynamic;
            return out;
        }

    protected:
        // Assuming normalised frequency
        double failure_rate(double freq) {
            return roValue * pow(10, (wValue * (1.0 - freq)));
        };
};

vector<string> splitByChar(string s, char ch){
    vector<string> ans;
    string temp;
    for(auto x :s){
        if(x==ch){
            ans.push_back(temp);
            temp = "";
        }
        else {
            temp.push_back(x);
        }
    }
    ans.push_back(temp);
    return ans;
}

vector<VM> parseVms(){
    vector<VM> parsed;
    ifstream cin("../Dataset/System-Model/system_model_data.csv");
    string temp;
    getline(cin,temp);
    while (getline(cin,temp))
    {
        auto commaSep = splitByChar(temp,',');
        VM vm;
        vm.cloudName = commaSep[0];
        vm.VMtype = commaSep[1];
        auto frequencies = splitByChar(commaSep[3],' ');
        for( auto x : frequencies){
            vm.freq.push_back(stod(x));
        }
        vm.pStatic = stod(commaSep[4]);
        vm.pDynamic= stod(commaSep[5]);
        vm.roValue = stod(commaSep[6]);
        vm.wValue = stod(commaSep[7]);
        parsed.push_back(vm);
    }
    return parsed;
}


class Nodes{
    public:
        VM assignedVM;
        double assignedFreq;
        int nodeIndex = 0;
        vector<pair<int, long long int>> children;
        vector<pair<int, long long int>> parent;
        double worstCase;
        double startTime;
        double finishTime;
        double realibilty;
        double cost;
        int numOfReplicas =1;
        // If this node is a dummy node or not
        bool dummy = 0;

        friend ostream& operator<<(ostream& out, const Nodes& node) {
            out << "Node: "<< node .nodeIndex << endl;
            out << "Replicas : " << node.numOfReplicas << endl;
            out << "Worst Case Execution : " << node.worstCase << endl;
            out << "Start Time : " << node.startTime << endl;
            out << "Finish Time : " << node.finishTime << endl;
            out << "Reliability : " << node.realibilty << endl;
            out << "Cost : " << node.cost << endl;
            out << "-------------------------------------------------------" << endl;
            return out;
        }

        // Get exponentially distributed RV given 
        double get_actual_time(unsigned seed=0) {
            if(seed==0)
            seed = chrono::system_clock::now().time_since_epoch().count();
            mt19937 gen(seed);
            double mean = 0.75 * worstCase;                     // Mean as 75% of worst-case time
            double lambda = 1.0 / mean;        
            exponential_distribution<> exp_dist(lambda);

            double min_time = worstCase / 3.0;
            double max_time = worstCase;
            double task_time;

            do {
                task_time = exp_dist(gen);              
            } while (task_time < min_time || task_time > max_time); 

            return task_time;
        }

        // Set the starting time and finishing time based on VM scheduled
        void setStartAndFinishTime(double st, double f, VM &vm){
            startTime = st;
            double et = getExecutionTimeonVM(vm,f);
            finishTime = st + et;
        }

        // Get the execution time on a VM given a frequency
        double getExecutionTimeonVM(VM &vm, double f){
            if(dummy) return 0.0;
            double t = worstCase/(f*vm.getCompPower());
            return t;
        }
        
};

class Workflow{
    public:
        double arrivalTime = 0.0;
        vector<Nodes> nodes;
        double realibiltyConstraint;
        double deadline;
        double totalEnergy = 0.0;
        double totalRel = 0;
        double deadlineCPWc;
        double totalRealibility = 1;
        void setReliabilityAndEnergy()
        {
            // Set the global workflow values
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
};

Workflow parseWorkflow(string file){
    string path = "../Dataset/Parsed/"+file;
    ifstream cin(path.c_str());
    Workflow wf;
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

// int main(){
//     auto x = parseVms();
//     auto y = parseWorkflow("cadsr.txt");
//     int c = 0;
//     for (auto it: y){
//         it.prettyPrint(c++);
//     }
// }