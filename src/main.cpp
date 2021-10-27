#include <cctype>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

void main_test();
void main_help();

int anaType = 0;

int main(int argc, char** argv) {
    string arg_string;
    vector<string> arg_list;
    for(int i = 0; i < argc; ++i) {
	arg_string = argv[i];
	arg_list.push_back(arg_string);
    }

    int iarg = 1;
    while(iarg < argc) {
	string arg = arg_list[iarg];

	if(arg == "-h" || arg == "--help") {
	    anaType = -1;
	    break;
	}
	else if(arg == "-t" || arg == "--test") {
	    anaType = -2;
	    break;
	}
	else if(arg == "-a" || arg == "--singleFile") {
	    anaType = 0;
	}
	else if(arg == "-r" || arg == "--timeRange") {
	    anaType = 1;
	}
	else if(arg == "-p" || arg == "--period") {
	    anaType = 2;
	}
    }

    switch(anaType) {
    case -2:
	main_test();
	break;
    case -1:
	main_help();
	break;	
    }

    return 0;
}

void main_test() {
    std::cout << "main_test()" << std::endl;
}

void main_help() {
    std::cout << "main_help()" << std::endl;
}
