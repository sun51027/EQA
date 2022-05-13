#ifndef HistMaker_h
#define HistMaker_h 1

#include <TROOT.h>

#include <iostream>
#include <vector>

using namespace std;

class Calendar;
class TFile;

class HistMaker {
public:
    HistMaker();
    ~HistMaker();

    void execute();
    void test();

    void setTIListFilename(string inputStr) { timeIntervalListFilename = inputStr; };

    void setOutfile();
    void setOutfile(string inputFilename);
    void setOutfileOpenState(string inputStr) { outfileOpenState = inputStr; };
    void setDoRewriteTIListFile(bool inputB) { doRewriteTIListFile = inputB; };
    void setQuantity(string inputStr) { quantity = inputStr; };

private:
    vector<string> datafileList;
    vector<string> startDTofTI; // start time of the time interval
    vector<string> endDTofTI; // end time of the time interval

    Int_t    bin = 867;
    Double_t min = -0.5;
    Double_t max = 4.5;

    string   way2SetTimeInterval;
    string   timeIntervalListFilename;
    bool     doRewriteTIListFile;
    string   quantity;

    string   outfileName;
    string   outfileOpenState;
    TFile*   outfile;

    void     makeHistogram();
    void     makeHistWithWindow(string energyRange, double low, double up);

    void     prepareDatafileList();
    void     prepareDatafileList2();
    void     prepareTimeIntervalList();
    bool     hasDatafileInInterval(string inputDTStr, string startDTStr,
				   string endDTStr, string nextDTStr);
    bool     hasDatafileInInterval(string inputDTStr, Calendar* startDT,
				   Calendar* endDT, string nextDTStr);
    bool     hasDatafileInInterval(Calendar* inputDT, Calendar* startDT,
				   Calendar* endDT, Calendar* nextDT);

    void     outputRecordTxt();
    void     outputRecordTxt(string txtfile);
};

#endif
