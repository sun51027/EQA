#ifndef DataReader_h
#define DataReader_h 1

#include <iostream>
#include <vector>

using namespace std;

class Calendar;
class TBranch;
class TFile;
class TH1D;
class TTree;

class DataReader {
public:
    DataReader(string inputDatafileName);
    ~DataReader();

    void execute();
    void test();

    void runFillingLoop(TH1D* inputH);

    void setDatafileName(string inputFilename);
    void setEndDT(string inputDTStr);
    void setQuantity(string inputStr) { quantity = inputStr; };
    void setStartDT(string inputDTStr);

private:
    vector<double> vd;
    string    datafileName;
    
    TFile*    file;
    TTree*    tree;

    float     ch0;
    float     timestamp;

    TBranch*  b_ch0;
    TBranch*  b_timestamp;

    string    quantity;

    Calendar* fileDT;
    Calendar* startDT;
    Calendar* endDT;

    void prepareDatafile();

    double convertVolt2MeV(double inputV);
};

#endif
