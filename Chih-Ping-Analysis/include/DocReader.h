#ifndef DocReader_h
#define DocReader_h 1

#include <iostream>
#include <vector>

using namespace std;

struct Term {
    string termName;
    string valueStr;
};

class DocReader {
public:
    DocReader(string inputFilename);
    ~DocReader();

    void   execute();
    void   test();

    string readStrValue(string term);
    int    readIntValue(string term);
    double readDoubleValue(string term);
    bool   readBoolValue(string term);

private:
    vector<Term> termList;

    static const string conflictionStr;
    static const string notFoundStr;

    void loadTermList(string inputFilename);
};

#endif
