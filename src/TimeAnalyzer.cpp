#include "TimeAnalyzer.h"

#include <TFile.h>
#include <TH1.h>
#include <TMath.h>
#include <TTree.h>

#include "Calendar.h"
#include "PeakFitter.h"

TimeAnalyzer::TimeAnalyzer() {
    infileName = "test";
    outfileName = "testTimeAna";
    outfileOpenState = "RECREATE";
    infile = nullptr;
    outfile = nullptr;
    dataTree = nullptr;
    isNewTree = false;
}



TimeAnalyzer::~TimeAnalyzer() {
    if(dataTree != nullptr)
	delete dataTree;

    if(infile != nullptr) {
	infile->Close();
	delete infile;
    }

    if(outfile != nullptr) {
	outfile->Close();
	delete outfile;
    }
}



void TimeAnalyzer::execute() {
    analyzeByFitting();
}



void TimeAnalyzer::test() {}



void TimeAnalyzer::setInfile() {
    setInfile(infileName);
}



void TimeAnalyzer::setInfile(string inputFilename) {
    infileName = inputFilename;
    if(infile != nullptr) {
	infile->Close();
	delete infile;
	infile = nullptr;
    }

    string infileFolder = "analyzedFile";
    string infilePath = infileFolder + "/" + infileName + ".root";
    infile = new TFile(infilePath.c_str(), "READ");
}



void TimeAnalyzer::setOutfile() {
    setOutfile(outfileName);
}



void TimeAnalyzer::setOutfile(string inputFilename) {
    outfileName = inputFilename;
    if(outfile != nullptr) {
	outfile->Close();
	delete outfile;
	outfile = nullptr;
    }

    string outfileFolder = "analyzedFile";
    string outfilePath = outfileFolder + "/" + outfileName + ".root";
    outfile = new TFile(outfilePath.c_str(), outfileOpenState.c_str());
}



void TimeAnalyzer::setERList(string condition) {
    ERList.clear();

    ERList.push_back("peak029");
    ERList.push_back("peak060");
    ERList.push_back("peak146");
    ERList.push_back("sum029n060");

    if(condition == "counting") {
	ERList.push_back("0to25");
    }
}



void TimeAnalyzer::setTermList(string condition) {
    termList.clear();

    if(condition == "fitting") {
	termList.push_back("cGauss");
	termList.push_back("cGauss_Err");
	termList.push_back("mean");
	termList.push_back("mean_Err");
	termList.push_back("std");
	termList.push_back("std_Err");
    } else if(condition == "counting") {
	termList.push_back("counts");
	termList.push_back("counts_Err");
    }
}



void TimeAnalyzer::prepareDataTree(string condition) {
    if(dataTree != nullptr && isNewTree) {
	delete dataTree;
	dataTree = nullptr;
    }

    setERList(condition);
    setTermList(condition);

    if(condition == "fitting")
	dataTree = new TTree("dataFitting", "dataTree");
    else if(condition == "counting")
	dataTree = new TTree("dataCounts", "dataTree");

    dataTree->Branch("entryNo", &entryNo, "entryNo/I");
    dataTree->Branch("year", &outyr, "year/I");
    dataTree->Branch("month", &outmon, "month/I");
    dataTree->Branch("day", &outday, "day/I");
    dataTree->Branch("hour", &outhr, "hour/I");
    dataTree->Branch("minute", &outmin, "minute/I");
    dataTree->Branch("second", &outsec, "second/D");

    for(unsigned int iER = 0; iER < ERList.size(); iER++) {
	string thisER = ERList[iER];
	for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
	    string thisTerm = termList[iTerm];
	    string outValName = thisER + "_" + thisTerm;

	    if(!((thisTerm == "mean" ||
		  thisTerm == "mean_Err" ||
		  thisTerm == "std" ||
		  thisTerm == "std_Err") &&
		 (thisER == "sum029n060"))) {
		outVal[outValName] = 0.;

		string outValFormat = outValName + "/D";
		dataTree->Branch(outValName.c_str(), &outVal[outValName], outValFormat.c_str());
	    }
	}
    }

    isNewTree = true;
}



void TimeAnalyzer::analyzeByFitting() {
    prepareDataTree("fitting");

    TDirectory* firstLayerDir = infile->GetDirectory("HistoCh0");
    TList* listDate = firstLayerDir->GetListOfKeys();
    listDate->Sort();

    int ientry = 0;
    TIter iterDate(listDate);
    TObject* objDate = nullptr;
    while((objDate = iterDate())) {
	TDirectory* dirDate = firstLayerDir->GetDirectory(objDate->GetName());
	TList* listTime = dirDate->GetListOfKeys();

	TIter iterTime(listTime);
	TObject* objTime = nullptr;
	while((objTime = iterTime())) {
	    Calendar* thisDT = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));
	    TH1D* thisH = (TH1D*)dirDate->Get(objTime->GetName());

	    entryNo = ientry;
	    outyr = thisDT->getYear();
	    outmon = thisDT->getMonth();
	    outday = thisDT->getMDay();
	    outhr = thisDT->getHour();
	    outmin = thisDT->getMinute();
	    outsec = thisDT->getSecond();

	    for(unsigned int iER = 0; iER < ERList.size(); iER++) {
		string thisER = ERList[iER];
		if(thisER == "sum029n060")
		    continue;

		PeakFitter* pf = new PeakFitter(thisH, thisER);
		pf->setOutputGraphFilename(thisDT->getDateTime() + "_" + thisER);

		pf->fitPeak();

		string tag = thisER + "_cGauss";
		outVal[tag] = pf->getCGauss();
		tag = thisER + "_cGauss_Err";
		outVal[tag] = pf->getCGaussErr();
		tag = thisER + "_mean";
		outVal[tag] = pf->getMean();
		tag = thisER = "_mean_Err";
		outVal[tag] = pf->getMeanErr();
		tag = thisER = "_std";
		outVal[tag] = pf->getSTD();
		tag = thisER = "_std_Err";
		outVal[tag] = pf->getSTDErr();
	    }

	    outVal["sum029n060_cGauss"] = outVal["peak029_cGauss"] + outVal["peak060_cGauss"];
	    outVal["sum029n060_cGauss_Err"] = TMath::Sqrt(outVal["peak029_cGauss_Err"]*outVal["peak029_cGauss_Err"] + outVal["peak060_cGauss_Err"]*outVal["peak060_cGauss_Err"]);

	    dataTree->Fill();

	    delete thisDT;
	    ientry++;
	}
    }

    dataTree->Write();
}



void TimeAnalyzer::analyzeByCounting() {
    prepareDataTree("counting");

    TDirectory* firstLayerDir = infile->GetDirectory("HistoCh0");
    TList* listDate = firstLayerDir->GetListOfKeys();
    listDate->Sort();

    int ientry = 0;
    TIter iterDate(listDate);
    TObject* objDate = nullptr;
    while((objDate = iterDate())) {
	TDirectory* dirDate = firstLayerDir->GetDirectory(objDate->GetName());
	TList* listTime = dirDate->GetListOfKeys();

	TIter iterTime(listTime);
	TObject* objTime = nullptr;
	while((objTime = iterTime())) {
	    Calendar* thisDT = new Calendar((string)(objDate->GetName()) + (string)(objTime->GetName()));

	    entryNo = ientry;
	    outyr = thisDT->getYear();
	    outmon = thisDT->getMonth();
	    outday = thisDT->getMDay();
	    outhr = thisDT->getHour();
	    outmin = thisDT->getMinute();
	    outsec = thisDT->getSecond();

	    for(unsigned int iER = 0; iER < ERList.size(); iER++) {
		string thisER = ERList[iER];
		if(thisER == "sum029n060")
		    continue;

		string histPath = "window_" + thisER + "/" + (string)(objDate->GetName()) + "/" + (string)(objTime->GetName());
		TH1D* thisH = (TH1D*)infile->Get(histPath.c_str());

		string tag = thisER + "_counts";
		outVal[tag] = thisH->GetEntries();
		tag = thisER + "_counts_Err";
		outVal[tag] = TMath::Sqrt(thisH->GetEntries());
	    }

	    outVal["sum029n060_counts"] = outVal["peak029_counts"] + outVal["peak060_counts"];
	    outVal["sum029n060_counts_Err"] = TMath::Sqrt(outVal["peak29_counts_Err"]*outVal["peak029_counts_Err"] + outVal["peak060_counts_Err"]*outVal["peak060_counts_Err"]);

	    dataTree->Fill();

	    delete thisDT;
	    ientry++;
	    
	}
    }

    dataTree->Write();
}
