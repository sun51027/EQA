#include "PeakFitter.h"

#include <TCanvas.h>
#include <TF1.h>
#include <TH1.h>
#include <TPaveText.h>

PeakFitter::PeakFitter(TH1D* inputH, string inputPeakType) {
    histogram = inputH;
    fittingType = "Minuit";
    peakType = "";
    peakNumb = 0;
    fittingFuncStr = "";
    isZoomed = false;
    doesOutput = true;

    outputGraphFilename = "default";
    outputGraphFolder = "plotting";

    setPeakType(inputPeakType);
}



PeakFitter::~PeakFitter() {}



void PeakFitter::execute() {}



void PeakFitter::test() {}



void PeakFitter::setPeakType(string inputStr) {
    peakType = inputStr;
    setFittingParameters();
}



void PeakFitter::setPeakNumb(int i) {
    if(peakType == "expo") {
	peakNumb = 0;
	fittingFuncStr = "expo";
    } else {
	peakNumb = i;
	if(peakNumb == 1)
	    fittingFuncStr = "expo(0) + gausn(2)";
	else if(peakNumb == 2)
	    fittingFuncStr = "expo(0) + gausn(2) + gausn(5)";
	else if(peakNumb == 3)
	    fittingFuncStr = "expo(0) + gausn(2) + gausn(5) + gausn(8)";
    }

    cGauss.resize(peakNumb);
    cGaussErr.resize(peakNumb);
    startCGauss.resize(peakNumb);
    upperCGauss.resize(peakNumb);
    lowerCGauss.resize(peakNumb);

    mean.resize(peakNumb);
    meanErr.resize(peakNumb);
    startMean.resize(peakNumb);
    upperMean.resize(peakNumb);
    lowerMean.resize(peakNumb);

    std.resize(peakNumb);
    stdErr.resize(peakNumb);
    startSTD.resize(peakNumb);
    upperSTD.resize(peakNumb);
    lowerSTD.resize(peakNumb);
}



void PeakFitter::fitPeak() {
    TCanvas* c = new TCanvas("c", "c", 1400, 800);

    TF1* peakFunc = new TF1("peakFunc", fittingFuncStr.c_str(), -0.5, 4.5);
    fitptr = nullptr;

    cout << "Range: " << lowerRange << ", " << upperRange << endl;
    peakFunc->SetRange(lowerRange, upperRange);
    peakFunc->SetParameter(0, startCPow);
    peakFunc->SetParameter(1, startEPow);

    peakFunc->SetParLimits(0, lowerCPow, upperCPow);
    peakFunc->SetParLimits(1, lowerEPow, upperEPow);

    for(int iPeak = 0; iPeak < peakNumb; iPeak++) {
	peakFunc->SetParameters(3*iPeak + 2, startCGauss[iPeak]);
	peakFunc->SetParameters(3*iPeak + 3, startMean[iPeak]);
	peakFunc->SetParameters(3*iPeak + 4, startSTD[iPeak]);

	peakFunc->SetParLimits(3*iPeak + 2, lowerCGauss[iPeak], upperCGauss[iPeak]);
	peakFunc->SetParLimits(3*iPeak + 3, lowerMean[iPeak], upperMean[iPeak]);
	peakFunc->SetParLimits(3*iPeak + 4, lowerSTD[iPeak], upperSTD[iPeak]);
    }

    if(isZoomed)
	histogram->GetXaxis()->SetRangeUser(0.9*lowerRange, 1.1*upperRange);

    fitptr = histogram->Fit(peakFunc, "S", "", lowerRange, upperRange);
    histogram->SetStats(kFALSE);

    chi2 = fitptr->Chi2();
    ndf = fitptr->Ndf();

    cPow = fitptr->Parameter(0);
    ePow = fitptr->Parameter(1);

    cPowErr = fitptr->ParError(0);
    ePowErr = fitptr->ParError(1);

    for(int iPeak = 0; iPeak < peakNumb; iPeak++) {
	cGauss[iPeak] = fitptr->Parameter(3*iPeak + 2);
	mean[iPeak] = fitptr->Parameter(3*iPeak + 3);
	std[iPeak] = fitptr->Parameter(3*iPeak + 4);

	cGaussErr[iPeak] = fitptr->ParError(3*iPeak + 2);
	meanErr[iPeak] = fitptr->ParError(3*iPeak + 3);
	stdErr[iPeak] = fitptr->ParError(3*iPeak + 4);
    }

    c->Update();
    double xleft = 0.75;
    double ydown = 0.4;
    TPaveText* pt = new TPaveText(xleft*gPad->GetUxmax()
				  + (1 - xleft)*gPad->GetUxmin(),
				  ydown*gPad->GetUymax()
				  + (1 - ydown)*gPad->GetUymin(),
				  gPad->GetUxmax(), gPad->GetUymax());

    char line[100];
    pt->AddText("Statistics");
    sprintf(line, "#chi^{2}: %f", chi2);
    pt->AddText(line);
    sprintf(line, "Ndf: %d", ndf);
    pt->AddText(line);
    sprintf(line, "cPow: %f", cPow);
    pt->AddText(line);
    sprintf(line, "ePow: %f", ePow);
    pt->AddText(line);
    for(int i = 0; i < peakNumb; i++) {
	sprintf(line, "cGauss %d: %f", i + 1, cGauss[i]);
	pt->AddText(line);
	sprintf(line, "Mean %d: %f", i + 1, mean[i]);
	pt->AddText(line);
	sprintf(line, "STD %d: %f", i + 1, std[i]);
	pt->AddText(line);
    }
    pt->Draw();

    c->Update();
    if(doesOutput) {
	string outputGraphPath = outputGraphFolder + "/" + outputGraphFilename + ".png";
	c->Print(outputGraphPath.c_str());
    }
    c->Close();

    delete pt;
    delete peakFunc;
    delete c;
}



void PeakFitter::setFittingParameters() {
    if(peakType == "peak029") {
	outputGraphFolder = "plotting/fitting/peak029";
	setPeakNumb(1);

	setRange(0.25, 0.35);
	setCPow(10., -100., 100.);
	setEPow(-3., -5., 0.);
	setCGauss(1000., 0., 1.0e8);
	setMean(0.29, 0.275, 0.3);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "peak060") {
	outputGraphFolder = "plotting/fitting/peak060";
	setPeakNumb(1);

	setRange(0.5, 0.7);
	setCPow(10., -100., 100.);
	setEPow(-3., -5., 0.);
	setCGauss(1000., 0., 1.0e8);
	setMean(0.6, 0.55, 0.65);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "peak146") {
	outputGraphFolder = "plotting/fitting/peak146";
	setPeakNumb(1);

	setRange(1.3, 1.6);
	setCPow(10., -100., 100.);
	setEPow(-3., -5., 0.);
	setCGauss(1000., 0., 1.0e8);
	setMean(1.45, 1.4, 1.5);
	setSTD(0.01, 0., 0.1);
    } else if(peakType == "expo") {
	setPeakNumb(0);

	setRange(0.4, 0.9);
	setCPow(10., -100., 100.);
	setEPow(-3., -1000., 0.);
    }
}
