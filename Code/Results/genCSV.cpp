#include <bits/stdc++.h>
using namespace std;

struct fileInput
{
    vector<string> filenames;
    vector<int> wfSizes;
    vector<int> max_indeg;
    vector<double> deadlines;
    vector<double> reliabilityConstraints;
    vector<double> makeSpan;
    vector<double> energy;
    vector<double> reliability;

    void setSize(int size)
    {
        filenames.resize(size);
        wfSizes.resize(size);
        max_indeg.resize(size);
        deadlines.resize(size);
        reliabilityConstraints.resize(size);
        makeSpan.resize(size);
        energy.resize(size);
        reliability.resize(size);
    }
};

fileInput parseFile(string filename)
{
    ifstream in(filename);
    fileInput input;
    int size;
    in >> size;
    input.setSize(size);
    for (int i = 0; i < size; i++)
    {
        in >> input.filenames[i];
        in >> input.wfSizes[i];
        in >> input.max_indeg[i];
        in >> input.deadlines[i];
        in >> input.reliabilityConstraints[i];
        in >> input.makeSpan[i];
        in >> input.energy[i];
        in >> input.reliability[i];
    }
    return input;
}

void GenEnergyWithDealine(){
        vector<double> reliabilities = {1.1,1.2,1.4,1.8,2.5};
    for(auto rel : reliabilities){
    ofstream out ("./Energy_With_Deadline"+to_string(rel)+".csv");
    auto bestCP = parseFile("./bestCP_WithDeadline" + to_string(rel) +".txt");
    auto ddnaive = parseFile("./ddnavie_WithDeadline"+ to_string(rel) +".txt");
    auto dynamic = parseFile("./dynamic_WithDeadline" + to_string(rel) +".txt");
    auto sb_lft = parseFile("./navie1_WithDeadline" + to_string(rel) +".txt");
    auto sota = parseFile("./sota_WithDeadline" + to_string(rel) +".txt");
    auto random = parseFile("./randomdd_WithDeadline" + to_string(rel) +".txt");
    out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,sota,bcpswa,random,sb_lft,sb_dd,dynamic"<<endl;

    int n = bestCP.filenames.size();

    for(int i =0;i<n;i++){
        out<<bestCP.filenames[i]<<",";
        out<<bestCP.wfSizes[i]<<",";
        out<<bestCP.max_indeg[i]<<",";
        out<<bestCP.deadlines[i]<<",";
        out<<bestCP.reliabilityConstraints[i]<<",";
        out<<sota.energy[i]<<",";
        out<<bestCP.energy[i]<<",";
        out<<random.energy[i]<<",";
        out<<sb_lft.energy[i]<<",";
        out<<ddnaive.energy[i]<<",";
        out<<dynamic.energy[i]<<endl;
    }
    }
}

void GenReliabilityWithDeadline(){
    vector<double> reliabilities = {1.1,1.2,1.4,1.8,2.5};
    for(auto rel : reliabilities){
    ofstream out ("./Reliability_With_Deadline"+ to_string(rel)+".csv");
    auto bestCP = parseFile("./bestCP_WithDeadline" + to_string(rel) +".txt");
    auto ddnaive = parseFile("./ddnavie_WithDeadline"+ to_string(rel) +".txt");
    auto dynamic = parseFile("./dynamic_WithDeadline" + to_string(rel) +".txt");
    auto sb_lft = parseFile("./navie1_WithDeadline" + to_string(rel) +".txt");
    auto sota = parseFile("./sota_WithDeadline" + to_string(rel) +".txt");
    auto random = parseFile("./randomdd_WithDeadline" + to_string(rel) +".txt");

    out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,sota,bcpswa,random,sb_lft,sb_dd,dynamic"<<endl;

    int n = bestCP.filenames.size();

    for(int i =0;i<n;i++){
        out<<bestCP.filenames[i]<<",";
        out<<bestCP.wfSizes[i]<<",";
        out<<bestCP.max_indeg[i]<<",";
        out<<bestCP.deadlines[i]<<",";
        out<<bestCP.reliabilityConstraints[i]<<",";
        out<<sota.reliability[i]<<",";
        out<<bestCP.reliability[i]<<",";
        out<<random.reliability[i]<<",";
        out<<sb_lft.reliability[i]<<",";
        out<<ddnaive.reliability[i]<<",";
        out<<dynamic.reliability[i]<<endl;
    }
    }
}

void GenMakespanWithDeadline(){
    vector<double> reliabilities = {1.1,1.2,1.4,1.8,2.5};
    for(auto rel : reliabilities){
    ofstream out ("./Makespan_With_Deadline" + to_string(rel)+".csv");
    auto bestCP = parseFile("./bestCP_WithDeadline" + to_string(rel) +".txt");
    auto ddnaive = parseFile("./ddnavie_WithDeadline"+ to_string(rel) +".txt");
    auto dynamic = parseFile("./dynamic_WithDeadline" + to_string(rel) +".txt");
    auto sb_lft = parseFile("./navie1_WithDeadline" + to_string(rel) +".txt");
    auto sota = parseFile("./sota_WithDeadline" + to_string(rel) +".txt");
    auto random = parseFile("./randomdd_WithDeadline" + to_string(rel) +".txt");

    out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,sota,bcpswa,random,sb_lft,sb_dd,dynamic"<<endl;

    int n = bestCP.filenames.size();

    for(int i =0;i<n;i++){
        out<<bestCP.filenames[i]<<",";
        out<<bestCP.wfSizes[i]<<",";
        out<<bestCP.max_indeg[i]<<",";
        out<<bestCP.deadlines[i]<<",";
        out<<bestCP.reliabilityConstraints[i]<<",";
        out<<sota.makeSpan[i]<<",";
        out<<bestCP.makeSpan[i]<<",";
        out<<random.makeSpan[i]<<",";
        out<<sb_lft.makeSpan[i]<<",";
        out<<ddnaive.makeSpan[i]<<",";
        out<<dynamic.makeSpan[i]<<endl;
    }
    }
}

void GenEnegryWithOutDeadline(){
    vector<double> reliabilities = {0.9,0.95,0.98,0.99,0.999};
    for(auto rel : reliabilities){
    ofstream out ("./Energy_WithOut_DeadlineRel"+to_string(rel)+".csv");
    auto bestCP = parseFile("./bestCP_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto ddnaive = parseFile("./ddnavie_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto dynamic = parseFile("./dynamic_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto sb_lft = parseFile("./navie1_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto sota = parseFile("./sota_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto random = parseFile("./randomdd_WithOutDeadlineRel"+to_string(rel)+".txt");

    out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,sota,bcpswa,random,sb_lft,sb_dd,dynamic"<<endl;

    int n = bestCP.filenames.size();

    for(int i =0;i<n;i++){
        out<<bestCP.filenames[i]<<",";
        out<<bestCP.wfSizes[i]<<",";
        out<<bestCP.max_indeg[i]<<",";
        out<<bestCP.deadlines[i]<<",";
        out<<bestCP.reliabilityConstraints[i]<<",";
        out<<sota.energy[i]<<",";
        out<<bestCP.energy[i]<<",";
        out<<random.energy[i]<<",";
        out<<sb_lft.energy[i]<<",";
        out<<ddnaive.energy[i]<<",";
        out<<dynamic.energy[i]<<endl;
    }
    }
}

void GenRealiabilityWithOutDeadline(){
    vector<double> reliabilities = {0.9,0.95,0.98,0.99,0.999};
    for(auto rel : reliabilities){
    ofstream out ("./Reliability_WithOut_DeadlineRel"+to_string(rel)+".csv");
   auto bestCP = parseFile("./bestCP_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto ddnaive = parseFile("./ddnavie_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto dynamic = parseFile("./dynamic_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto sb_lft = parseFile("./navie1_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto sota = parseFile("./sota_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto random = parseFile("./randomdd_WithOutDeadlineRel"+to_string(rel)+".txt");

    out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,sota,bcpswa,random,sb_lft,sb_dd,dynamic"<<endl;

    int n = bestCP.filenames.size();

    for(int i =0;i<n;i++){
        out<<bestCP.filenames[i]<<",";
        out<<bestCP.wfSizes[i]<<",";
        out<<bestCP.max_indeg[i]<<",";
        out<<bestCP.deadlines[i]<<",";
        out<<bestCP.reliabilityConstraints[i]<<",";
        out<<sota.reliability[i]<<",";
        out<<bestCP.reliability[i]<<",";
        out<<random.reliability[i]<<",";
        out<<sb_lft.reliability[i]<<",";
        out<<ddnaive.reliability[i]<<",";
        out<<dynamic.reliability[i]<<endl;
    }
    }
}

void GenMakeSpanWithOutDeadline(){
    vector<double> reliabilities = {0.9,0.95,0.98,0.99,0.999};
    for(auto rel : reliabilities){
    ofstream out ("./Makespan_WithOut_DeadlineRel"+to_string(rel)+".csv");
    auto bestCP = parseFile("./bestCP_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto ddnaive = parseFile("./ddnavie_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto dynamic = parseFile("./dynamic_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto sb_lft = parseFile("./navie1_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto sota = parseFile("./sota_WithOutDeadlineRel"+to_string(rel)+".txt");
    auto random = parseFile("./randomdd_WithOutDeadlineRel"+to_string(rel)+".txt");

    out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,sota,bcpswa,random,sb_lft,sb_dd,dynamic"<<endl;

    int n = bestCP.filenames.size();

    for(int i =0;i<n;i++){
        out<<bestCP.filenames[i]<<",";
        out<<bestCP.wfSizes[i]<<",";
        out<<bestCP.max_indeg[i]<<",";
        out<<bestCP.deadlines[i]<<",";
        out<<bestCP.reliabilityConstraints[i]<<",";
        out<<sota.makeSpan[i]<<",";
        out<<bestCP.makeSpan[i]<<",";
        out<<random.makeSpan[i]<<",";
        out<<sb_lft.makeSpan[i]<<",";
        out<<ddnaive.makeSpan[i]<<",";
        out<<dynamic.makeSpan[i]<<endl;
    }
    }
}


void GenDynamicUpdated(){
    vector<double> slacks = {0,0.1,0.15,0.2,0.25,0.3,0.4,0.5,0.6,0.7,0.9,1.1,1.5,2.0,2.5,3.0,3.5,4.0,4.5,5.0,10,20,50,100};
    for(auto rel : slacks){
        ofstream out ("./dynamic_upd_df_1.5_"+to_string(rel)+".csv");
        auto random = parseFile("./dynamic_upd_WithSlack"+to_string(rel)+".txt");
    
        out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,thresh,dynamic_up"<<endl;
    
        int n = random.filenames.size();
    
        for(int i =0;i<n;i++){
            out<<random.filenames[i]<<",";
            out<<random.wfSizes[i]<<",";
            out<<random.max_indeg[i]<<",";
            out<<random.deadlines[i]<<",";
            out<<random.reliabilityConstraints[i]<<",";
            out<<to_string(rel)<<",";
            out<<random.energy[i]<<endl;
        }
        }
}

void GenEnergyWithMu(){
    vector<int> mus = {3,4,5,6};
    for(auto rel : mus){
    cout<<"./bestCP_WithDeadline1.500000_mu_"+to_string(rel)+".txt"<<endl;
    ofstream out ("./Energy_WithDeadline1.500000_mu_"+to_string(rel)+".csv");
    auto bestCP = parseFile("./bestCP_WithDeadline1.500000_mu_"+to_string(rel)+".txt");
    auto ddnaive = parseFile("./ddnavie_WithDeadline1.500000_mu_"+to_string(rel)+".txt");
    auto dynamic = parseFile("./dynamic_WithDeadline1.500000_mu_"+to_string(rel)+".txt");
    auto sb_lft = parseFile("./navie1_WithDeadline1.500000_mu_"+to_string(rel)+".txt");
    auto sota = parseFile("./sota_WithDeadline1.500000_mu_"+to_string(rel)+".txt");
    auto random = parseFile("./randomdd_WithDeadline1.500000_mu_"+to_string(rel)+".txt");

    out<<"wf_name,num_nodes,max_indeg,deadline_const,rel_const,sota,bcpswa,random,sb_lft,sb_dd,dynamic"<<endl;

    int n = bestCP.filenames.size();

    for(int i =0;i<n;i++){
        out<<bestCP.filenames[i]<<",";
        out<<bestCP.wfSizes[i]<<",";
        out<<bestCP.max_indeg[i]<<",";
        out<<bestCP.deadlines[i]<<",";
        out<<bestCP.reliabilityConstraints[i]<<",";
        out<<sota.energy[i]<<",";
        out<<bestCP.energy[i]<<",";
        out<<random.energy[i]<<",";
        out<<sb_lft.energy[i]<<",";
        out<<ddnaive.energy[i]<<",";
        out<<dynamic.energy[i]<<endl;
    }
    }
}

int main(){
    // GenEnergyWithDealine();
    // GenReliabilityWithDeadline();
    // GenMakespanWithDeadline();
    // GenEnergyWithMu();
    GenDynamicUpdated();
    // GenEnegryWithOutDeadline();
    // GenRealiabilityWithOutDeadline();
    // GenMakeSpanWithOutDeadline();
}