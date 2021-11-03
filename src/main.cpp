#include <cctype>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "HistMaker.h"
#include "TimeAnalyzer.h"

using namespace std;

void main_test();
void main_help();

void main_makeHistogram1(); // for single file
void main_makeHistogram2();
void main_makeHistogram3();
void main_fittingAnalysis();
void main_countingAnalysis();

int anaType = 0;
string infileName = "";
string outfileName = "";
string outfileOpenState = "";

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
	    iarg++;
	}
	else if(arg == "-i" || arg == "--timeInterval") {
	    anaType = 1;
	    iarg++;
	}
	else if(arg == "-p" || arg == "--periods") {
	    anaType = 2;
	    iarg++;
	}
	else if(arg == "-fit" || arg == "--fitting") {
	    anaType = 3;
	    iarg++;
	}
	else if(arg == "-count" || arg == "--counting") {
	    anaType = 4;
	    iarg++;
	}
	else if(arg == "-if" || arg == "--inputFile") {
	    iarg++;
	    infileName = argv[iarg];
	    iarg++;
	}
	else if(arg == "-of" || arg == "--outputFile") {
	    iarg++;
	    outfileName = argv[iarg];
	    iarg++;
	}
	else if(arg == "-re" || arg == "--recreate") {
	    outfileOpenState = "RECREATE";
	    iarg++;
	}
	else if(arg == "-up" || arg == "--update") {
	    outfileOpenState = "UPDATE";
	    iarg++;
	}
    }

    switch(anaType) {
    case -2:
	main_test();
	break;
    case -1:
	main_help();
	break;
    case 0:
	main_makeHistogram1();
	break;
    case 3:
	main_fittingAnalysis();
	break;
    }

    return 0;
}

void main_makeHistogram1() { // for single file
    HistMaker* hm = new HistMaker();
    if(outfileName != "")
	hm->setOutfile(outfileName);

    if(outfileOpenState != "")
	hm->setOutfileOpenState(outfileOpenState);

    hm->setQuantity("Energy");
    hm->execute();

    delete hm;
}

void main_makeHistogram2() {
    HistMaker* hm = new HistMaker();

    delete hm;
}

void main_makeHistogram3() {
    HistMaker* hm = new HistMaker();

    delete hm;
}

void main_fittingAnalysis() {
    TimeAnalyzer* ta = new TimeAnalyzer();

    if(outfileOpenState != "")
	ta->setOutfileOpenState(outfileOpenState);

    if(infileName == "")
	ta->setInfile();
    else
	ta->setInfile(infileName);

    if(outfileName == "")
	ta->setOutfile();
    else
	ta->setOutfile(outfileName);

    ta->analyzeByFitting();

    delete ta;
}

void main_countingAnalysis() {
    TimeAnalyzer* ta = new TimeAnalyzer();

    if(outfileOpenState != "")
	ta->setOutfileOpenState(outfileOpenState);

    if(infileName == "")
	ta->setInfile();
    else
	ta->setInfile(infileName);

    if(outfileName == "")
	ta->setOutfile();
    else
	ta->setOutfile(outfileName);

    ta->analyzeByCounting();

    delete ta;
}

void main_test() {
    HistMaker* obj = new HistMaker();
    obj->test();
    delete obj;
}

void main_help() {
    std::cout << "main_help()" << std::endl;
}
