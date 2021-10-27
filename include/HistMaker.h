#ifndef HistMaker_h
#define HistMaker_h 1

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

private:
    vector<string> datafileList;
    vector<string> startDTofTI; // start time of the time interval
    vector<string> endDTofTI; // end time of the time interval

    Int_t bin;
    Double_t min;
    Double_t max;
    string quantity;

    TFile* outfile;

    void makeHistogram();
    bool hasDatafileInInterval(string inputDTStr, string startDTStr,
			       string endDTStr, string nextDTStr);
    bool hasDatafileInInterval(string inputDTStr, Calendar* startDT,
			       Calendar* endDT, string nextDTStr);
    bool hasDatafileInInterval(Calendar* inputDT, Calendar* startDT,
			       Calendar* endDT, Calendar* nextDT);
};

#endif
