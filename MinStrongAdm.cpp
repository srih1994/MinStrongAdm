/*

Copyright 2013-2017 Samer Nofal (Samer.Nofal@gju.edu.jo), Katie Atkinson (K.M.Atkinson@liverpool.ac.uk) and Paul E. Dunne (P.E.Dunne@liverpool.ac.uk)

The programs are free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The programs are distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License http://www.gnu.org/licenses for more details.

 */
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include<cstring>
#include<vector>
#include<math.h>
#include <sstream>
#include<exception>
#include<unordered_map>
using namespace std;
#include <queue>

class Arg{
    public:    
    vector<int> attacks;
    int attacks_count;
    vector<int> attackedBy;
    int attackedBy_count;
    string key;
    int id;
    Arg(int id,string key){
        this->key=key;
        this->id=id;
    }
    Arg(){}
};
char* inputFile; // contains the input argumentation framework in ASPARTIX/trivial format
unordered_map<string,int> arg_id; // a map of argument ids
vector<Arg> args; // set of arguments
char* label;//the grounded extension to be build
char* labelsa;
const char IN='1';
const char OUT='0';
const char UNDEC='3';
int *attackers_count; // undecided attackers count
int args_count; // total number of arguments in the framework
int* MMlab = NULL;
string target= "a111";
int* MMLabsa = NULL;
queue<int>unproc_in;

void readArgumentsASPARTIXFormat() { //reading an argumentation framework from an aspartix input file
    string inLine;
    ifstream infile;
    infile.open(inputFile);
    infile>>inLine;
    int id=0;
    while (!infile.eof() & inLine.find("arg") != string::npos) {
        string a = inLine.substr(4, inLine.find_last_of(")") - 4);
        arg_id[a]=id++;
        args.push_back(Arg(id,a));
        infile>>inLine;
    }
    while (!infile.eof()) {
        if (inLine.find("att") != string::npos) {
            string a=inLine.substr(4, inLine.find_last_of(",") - 4).c_str();
            string b=inLine.substr(inLine.find_last_of(",") + 1, inLine.find_last_of(")") - inLine.find_last_of(",") - 1).c_str();
            int a_id=arg_id[a];
            int b_id=arg_id[b];
            args[a_id].attacks.push_back(b_id);
            args[b_id].attackedBy.push_back(a_id);
        }
        infile>>inLine;
    }
    infile.close();
}

void print() { //printing labelling to screen in the format: {(a0,a1),(b0,b1),(c0,c1)} 
    cout << "LabC: ";
    cout << "{";
    cout << "(";
    bool InArg = true;
    int counterIn = 0;
    for (int i = 0; i < args.size(); i++) {
        if (label[i] == IN) {
            counterIn++;
            if (InArg) {
                cout << args[i].key << ":" << MMlab[i];
                InArg = false;
                
            } else
                cout << "," << args[i].key << ":"<< MMlab[i];
        }
    }
    cout <<")" <<endl;
    cout << "Count of in labelled arguments LabC: " << counterIn << endl;

    cout <<"}"<< endl;
    cout << "______________________________________________________________________________________________________________________" << endl;

}

void print2() { //printing labelling to screen in the format: {(a0,a1),(b0,b1),(c0,c1)} 
    cout << "LabP = ";
    cout << "{";
    cout << "(";
    bool InArg = true;
    int counterin = 0;
    for (int i = 0; i < args.size(); i++) {
        if (labelsa[i] == IN) {
            counterin++;
            if (InArg) {
                cout << args[i].key << ":" << MMlab[i];
                InArg = false;
            } else
                cout << "," << args[i].key << ":"<< MMlab[i];
        }
    }
    cout <<")" << endl;
    cout <<"}"<< endl;
    cout << "Count of in labelled arguments LabP: " << counterin << endl;

}

void buildTheGroundedExtension(queue<int> mustIN_args) {
    while (mustIN_args.empty()==false) {
        int i=mustIN_args.front();
        mustIN_args.pop();
        for (int j = 0; j < args[i].attacks_count; j++){
            if(label[args[i].attacks[j]] != OUT && label[args[i].attacks[j]] != OUT){
                label[args[i].attacks[j]] = OUT;
                MMlab[args[i].attacks[j]] = MMlab[i] + 1;
                for (int k= 0; k < args[args[i].attacks[j]].attacks_count; k++){
                    attackers_count[args[args[i].attacks[j]].attacks[k]]--;
                    if(attackers_count[args[args[i].attacks[j]].attacks[k]]==0 && label[args[args[i].attacks[j]].attacks[k]]==UNDEC){
                        label[args[args[i].attacks[j]].attacks[k]] = IN;
                        MMlab[args[args[i].attacks[j]].attacks[k]] = MMlab[args[i].attacks[j]] + 1;
                        mustIN_args.push(args[args[i].attacks[j]].attacks[k]);

                        if(args[args[args[i].attacks[j]].attacks[k]].key == target){

                            print();

                        }
                    }
                }
            }                    
        }  
    }
}




void buildStrongAdm(queue<int> unproc_in){
    while(unproc_in.empty() == false){
        int i = unproc_in.front();
        unproc_in.pop();
        
        for (int j = 0; j < args[i].attackedBy_count; j++){
            if(label[args[i].attackedBy[j]] == OUT){
                labelsa[args[i].attackedBy[j]] = OUT;

                for (int k= 0; k < args[args[i].attackedBy[j]].attackedBy_count; k++) {

                    if(label[args[args[i].attackedBy[j]].attackedBy[k]]==IN && labelsa[args[args[i].attackedBy[j]].attackedBy[k]]!=IN ){

                        int min = MMlab[args[i].attackedBy[j]] - 1;

                        if(MMlab[args[args[i].attackedBy[j]].attackedBy[k]] == min){

                            labelsa[args[args[i].attackedBy[j]].attackedBy[k]] = IN;
                            unproc_in.push(args[args[i].attackedBy[j]].attackedBy[k]);
                            break;

                        }

                    
                    }
                }

            }

        }


    }
}

int main(int argc, char* arguments[]) {

    clock_t start_time = clock();

    inputFile =arguments[1];//"/home/samer/Documents/research/on\ reasoning\ and\ guessing\ of\ abstract\ argument\ labels/experiments/C/4/admbuster_1000000.apx";// arguments[1];
    readArgumentsASPARTIXFormat();
    args_count=args.size();    
    
    label = new char[args_count]; // I for representing labels: I[i]='3' for UNDEC, I[i]='0' for OUT, I[i]='1' for IN    
    labelsa = new char[args_count];
    attackers_count=new int[args_count]; // dynamically change
    MMlab = new int[args_count];
    MMLabsa = new int[args_count];

    //initialization
    for (int i = 0; i < args_count; i++) {    
        label[i]=UNDEC;
        labelsa[i] = UNDEC;
        args[i].attacks_count=args[i].attacks.size();
        args[i].attackedBy_count=args[i].attackedBy.size();
        attackers_count[i]=args[i].attackedBy_count;                   
    }
    queue<int>mustIN_args;
    
    // first round of propagation
    for (int i = 0; i < args_count; i++) {        
        if(label[i]==UNDEC && attackers_count[i]==0){ 
            label[i] = IN;
            MMlab[i] = 1;
            mustIN_args.push(i); 
        }
               
    }

    for (int i = 0; i < args_count; i++) {

         if(label[i] == IN){

             if(args[i].key == target){
                print();
             }
         }

    

    }  

    labelsa[arg_id[target]] = IN;
    unproc_in.push(arg_id[target]);

    
    buildTheGroundedExtension(mustIN_args);
    buildStrongAdm(unproc_in);
    print();
    print2();

    clock_t end_time1 = clock();
	clock_t result = end_time1 - start_time;
	cout << "time taken by the program: " <<result<<" milliseconds"<<endl;
	cout<<result/CLOCKS_PER_SEC<<" seconds"<<endl;

    
    return 0;    
}