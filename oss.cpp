#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]){

    //Command Line Parsing
    string argNext = "";
    for(int i  = 1; i < argc; i++){
        string arg = argv[i];
        if(arg == "-h"){
            cout << "Help:" << endl;
            cout << "master [-h] [-s t] [-l f]" <<endl;
            cout << " -h : Help Options" <<endl;
            cout << " -s : Indicate how many maximum seconds before the system terminates" <<endl;
            cout << " -l : Specify a particular name for the log file" <<endl;
            cout << "Example run: ./oss -s 3 -l log.txt" <<endl;
            return 0;
        } else if(arg == "-s"){
            argNext = argv[i+1];
            cout << "-s " << argNext << endl;
        } else if(arg == "-l"){
            argNext = argv[i+1];
            cout << "-l " << argNext << endl;
        }
    }
}