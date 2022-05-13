#ifndef GraphPrinter_h
#define GraphPrinter_h 1

#include <iostream>
#include <map>
#include <vector>

class TAxis;
class TFile;
class TGraph;
class TMultiGraph;
class TTree;

using namespace std;

class GraphPrinter {
public:
    GraphPrinter();
    ~GraphPrinter();

    void execute();
    void test();

    void setInfile();
    void setInfile(string inputFilename);

    void setOutputGraphName(string inputStr) { outputGraphName = inputStr; };
    void setOutputGraphFolder(string inputStr) { outputGraphFolder = inputStr; };

    void setAutoNaming(bool inputBool) { autoNaming = inputBool; };

    void setERList(string condition);
    void setTermList(string condition);

    void prepareDataTree(string condition);

    void printTimeGraph(string term);

private:
    vector<string> ERList;
    vector<string> termList;
    map<string, double> termVal;

    string infileName;
    TFile* infile;
    TTree* dataTree;
    string dataTreeType;

    string outputGraphName;
    string outputGraphFolder;

    bool   autoNaming;

    int    entryNo;
    int    year;
    int    month;
    int    day;
    int    hour;
    int    minute;
    double second;

    void   printRawGraph(TGraph* dataG, string term, string ER);
    void   printGraphWithErrorBand(TGraph* dataG, double avg, double std, double bandWidth, string term, string ER);

    void   setGraphAtt(TGraph* inputG, string term, string ER);
    void   setMultiGAtt(TMultiGraph* inputMG, string term, string ER);
    void   setDateAxis(TAxis* inputAxis);
    void   setRangeUser(TAxis* inputAxis, string term, string ER);
    string makeERStr(string ER, string style);
    string makeTitle(string component, string term, string ER);
};

#endif
