#ifndef PeakFitter_h
#define PeakFitter_h 1

#include <iostream>
#include <vector>

#include <TFitResult.h>
#include <TFitResultPtr.h>

using namespace std;

class TH1D;

class PeakFitter {
public:
    PeakFitter(TH1D* inputH, string inputPeakType);
    ~PeakFitter();

    void   execute();
    void   test();

    void   setHistogram(TH1D* inputH)         { histogram = inputH; };
    void   setPeakType(string inputStr);
    void   setPeakNumb(int i);
    void   setFittingFuncStr(string inputStr) { fittingFuncStr = inputStr; };
    void   setIsZoomed(bool inputBool)        { isZoomed = inputBool; };
    void   setDoesOutput(bool inputBool)      { doesOutput = inputBool; };

    void   setOutputGraphFilename(string inputStr) {
	outputGraphFilename = inputStr; };
    void   setOutputGraphFolder(string inputStr) {
	outputGraphFolder = inputStr; };

    void   setRange(double low, double up) {
	upperRange = up; lowerRange = low; };
    void   setCPow(double start, double low, double up) {
	startCPow = start; upperCPow = up; lowerCPow = low; };
    void   setCExp(double start, double low, double up);
    void   setEPow(double start, double low, double up) {
	startEPow = start; upperEPow = up; lowerEPow = low; };
    void   setCGauss(double start, double low, double up, int i = 0) {
	startCGauss[i] = start; upperCGauss[i] = up; lowerCGauss[i] = low; };
    void   setMean(double start, double low, double up, int i = 0) {
	startMean[i] = start; upperMean[i] = up; lowerMean[i] = low; };
    void   setSTD(double start, double low, double up, int i = 0) {
	startSTD[i] = start; upperSTD[i] = up; lowerSTD[i] = low; };

    double getChi2()               { return chi2; };
    double getNdf()                { return ndf; };

    double getCPow()               { return cPow; };
    double getEPow()               { return ePow; };
    double getCGauss(int i = 0)    { return cGauss[i]; };
    double getMean(int i = 0)      { return mean[i]; };
    double getSTD(int i = 0)       { return std[i]; };

    double getCPowErr()            { return cPowErr; };
    double getEPowErr()            { return ePowErr; };
    double getCGaussErr(int i = 0) { return cGaussErr[i]; };
    double getMeanErr(int i = 0)   { return meanErr[i]; };
    double getSTDErr(int i = 0)    { return stdErr[i]; };

    void   fitPeak();

private:
    vector<double> vd;
    TH1D*  histogram;

    string fittingType;
    string peakType;
    int    peakNumb;
    string fittingFuncStr;
    bool   isZoomed;
    bool   doesOutput;

    string outputGraphFilename;
    string outputGraphFolder;

    TFitResultPtr fitptr;

    double chi2;
    int    ndf;

    double cPow;
    double ePow;
    vector<double> cGauss;
    vector<double> mean;
    vector<double> std;

    double cPowErr;
    double ePowErr;
    vector<double> cGaussErr;
    vector<double> meanErr;
    vector<double> stdErr;

    double upperRange;
    double lowerRange;

    double startCPow;
    double upperCPow;
    double lowerCPow;

    double startEPow;
    double upperEPow;
    double lowerEPow;

    vector<double> startCGauss;
    vector<double> upperCGauss;
    vector<double> lowerCGauss;

    vector<double> startMean;
    vector<double> upperMean;
    vector<double> lowerMean;

    vector<double> startSTD;
    vector<double> upperSTD;
    vector<double> lowerSTD;

    void setFittingParameters();
};

#endif
