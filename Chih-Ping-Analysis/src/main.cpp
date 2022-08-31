#include <cctype>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "GraphPrinter.h"
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
void main_printGraph();

int anaType = 0;
bool autoNaming = false;
bool doRewriteTIListFile = false;
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
	else if(arg == "-g" || arg == "--printGraph") {
	    anaType = 5;
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
	else if(arg == "-TI" || arg == "--rewriteTI") {
	    doRewriteTIListFile = true;
	    iarg++;
	}
	else if(arg == "-an" || arg == "--autoNaming") {
	    autoNaming = true;
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
    case 4:
	main_countingAnalysis();
	break;
    case 5:
	main_printGraph();
	break;
    }

    return 0;
}

void main_makeHistogram1() {
	std:: cout << "Init histrogram maker:"<< std::endl;
    HistMaker* hm = new HistMaker();
    
    if(infileName != "") {
	hm->setTIListFilename(infileName);
    }

    if(outfileOpenState != "") {
	hm->setOutfileOpenState(outfileOpenState);
    }

    if(outfileName != "") {
	hm->setOutfile(outfileName);
    }

    hm->setDoRewriteTIListFile(doRewriteTIListFile);
	std:: cout << "histrogram maker after do rewirte"<< std::endl;
    hm->setQuantity("Energy");
	std:: cout << "histrogram maker execute start:"<< std::endl;
    hm->execute();
	std:: cout << "histrogram maker execute Finish:"<< std::endl;

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

void main_printGraph() {
    GraphPrinter* gp = new GraphPrinter();

    if(infileName != "")
	gp->setInfile(infileName);
    else
	gp->setInfile();

    if(outfileName != "")
	gp->setOutputGraphName(outfileName);

    gp->execute();

    delete gp;
}

void main_test() {
    //HistMaker* obj = new HistMaker();
    GraphPrinter* obj = new GraphPrinter();
    obj->setInfile("fitOutput");
    //obj->test();
    delete obj;
}

void main_help() {
    std::cout << "main_help()" << std::endl;
}
