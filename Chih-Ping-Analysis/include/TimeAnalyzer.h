#ifndef TimeAnalyzer_h
#define TimeAnalyzer_h 1

#include <iostream>
#include <map>
#include <vector>

using namespace std;

class TFile;
class TTree;

class TimeAnalyzer {
public:
    TimeAnalyzer();
    ~TimeAnalyzer();

    void execute();
    void test();

    void setInfile();
    void setInfile(string inputFilename);

    void setOutfile();
    void setOutfile(string inputFilename);
    void setOutfileOpenState(string inputStr) { outfileOpenState = inputStr; };

    void setERList(string condition);
    void setTermList(string condition);

    void prepareDataTree(string condition);

    void analyzeByFitting();
    void analyzeByCounting();
    void normalizeFittingResult();
    void normalizeCountingResult();

private:
    vector<string> ERList; // list of energy range
    vector<string> termList;
    map<string, double> outVal; // output value

    int entryNo;
    int outyr;
    int outmon;
    int outday;
    int outhr;
    int outmin;
    double outsec;

    string infileName;
    TFile* infile;

    string outfileName;
    string outfileOpenState;
    TFile* outfile;
    TTree* dataTree;
    bool   isNewTree;
};

#endif
