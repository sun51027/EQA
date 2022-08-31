#include "GraphPrinter.h"

#include <TAxis.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TTree.h>

GraphPrinter::GraphPrinter() {
    infileName = "testTimeAna";
    outputGraphName = "testGraph";
    outputGraphFolder = "plotting";
    autoNaming = false;
    infile = nullptr;
    dataTree = nullptr;
}



GraphPrinter::~GraphPrinter() {
    if(dataTree != nullptr)
	delete dataTree;

    if(infile != nullptr) {
	infile->Close();
	delete infile;
    }
}



void GraphPrinter::execute() {
    if(dataTreeType == "fitting") {
	printTimeGraph("cGauss");
	printTimeGraph("mean");
    } else if(dataTreeType == "counting") {
	printTimeGraph("counts");
    }
}



void GraphPrinter::test() {
    setInfile();
    /*
    TList* listFirstLayer = infile->GetListOfKeys();
    listFirstLayer->Sort();

    TIter iterFL(listFirstLayer);
    TObject* objFL = nullptr;
    while((objFL = iterFL())) {
	cout << objFL->GetName() << endl;
    }

    printTimeGraph("cGauss");
    */
}



void GraphPrinter::setInfile() {
    setInfile(infileName);
}



void GraphPrinter::setInfile(string inputFilename) {
    infileName = inputFilename;
    if(infile != nullptr) {
	infile->Close();
	delete infile;
	infile = nullptr;
    }

    string infileFolder = "analyzedFile";
    string infilePath = infileFolder + "/" + infileName + ".root";
    cout << infilePath << endl;
    infile = new TFile(infilePath.c_str(), "READ");

    if(infile->GetListOfKeys()->FindObject("dataFitting"))
	dataTreeType = "fitting";
    else if(infile->GetListOfKeys()->FindObject("dataCounts"))
	dataTreeType = "counting";

    prepareDataTree(dataTreeType);
}



void GraphPrinter::setERList(string condition) {
    ERList.clear();

    ERList.push_back("peak029");
    ERList.push_back("peak060");
    ERList.push_back("peak146");
    ERList.push_back("sum029n060");

    if(condition == "counting") {
	ERList.push_back("0to25");
    }
}



void GraphPrinter::setTermList(string condition) {
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



void GraphPrinter::prepareDataTree(string condition) {
    if(dataTree != nullptr) {
	delete dataTree;
	dataTree = nullptr;
    }

    setERList(condition);
    setTermList(condition);

    if(condition == "fitting")
	infile->GetObject("dataFitting", dataTree);
    else if(condition == "counting")
	infile->GetObject("dataCounts", dataTree);

    dataTree->SetBranchAddress("entryNo", &entryNo);
    dataTree->SetBranchAddress("year", &year);
    dataTree->SetBranchAddress("month", &month);
    dataTree->SetBranchAddress("day", &day);
    dataTree->SetBranchAddress("hour", &hour);
    dataTree->SetBranchAddress("minute", &minute);
    dataTree->SetBranchAddress("second", &second);

    for(unsigned int iER = 0; iER < ERList.size(); iER++) {
	string thisER = ERList[iER];
	for(unsigned int iTerm = 0; iTerm < termList.size(); iTerm++) {
	    string thisTerm = termList[iTerm];
	    string termValName = thisER + "_" + thisTerm;

	    if(!((thisTerm == "mean" ||
		  thisTerm == "mean_Err" ||
		  thisTerm == "std" ||
		  thisTerm == "std_Err") &&
		 (thisER == "sum029n060"))) {
		termVal[termValName] = 0.;
		dataTree->SetBranchAddress(termValName.c_str(), &termVal[termValName]);
	    }
	}
    }
}



void GraphPrinter::printTimeGraph(string term) {
    map<string, double> validMean;
    map<string, double> validStd;
    map<string, int> validEntries;
    for(unsigned int iER = 0; iER < ERList.size(); iER++) {
	validMean[ERList[iER]] = 0.;
	validStd[ERList[iER]] = 0.;
	validEntries[ERList[iER]] = 0;
    }

    Long64_t nentries = dataTree->GetEntries();
    for(Long64_t entry = 0; entry < nentries; ++entry) {
	dataTree->GetEntry(entry);

	for(unsigned int iER = 0; iER < ERList.size(); iER++) {
	    string thisER = ERList[iER];
	    if(thisER == "sum029n060") {
		if(term == "mean" || term == "std") {
		    continue;
		}
	    }

	    string tag = thisER + "_" + term;
	    if(termVal[tag] != 0) {
		validMean[thisER] += termVal[tag];
		validStd[thisER] += termVal[tag]*termVal[tag];
		validEntries[thisER]++;
	    }
	}
    }

    for(unsigned int iER = 0; iER < ERList.size(); iER++) {
	string thisER = ERList[iER];
	validMean[thisER] /= (double)validEntries[thisER];
	validStd[thisER] -= ((double)validEntries[thisER])*validMean[thisER]*validMean[thisER];
	validStd[thisER] = sqrt(validStd[thisER]/(double)(validEntries[thisER] - 1));
    }

    map<string, TGraphErrors*> ge;
    for(unsigned int iER = 0; iER < ERList.size(); iER++)
	ge[ERList[iER]] = new TGraphErrors();

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	dataTree->GetEntry(entry);
	TDatime* tdt = new TDatime(year, month, day, hour, minute, (int)second);

	for(unsigned int iER = 0; iER < ERList.size(); iER++) {
	    string thisER = ERList[iER];
	    if(thisER == "sum029n060") {
		if(term == "mean" || term == "std") {
		    continue;
		}
	    }

	    TGraphErrors* thisGE = ge[thisER];

	    string tag = thisER + "_" + term;
	    const double width = 2.;
	    if(termVal[tag] <= validMean[thisER] + width*validStd[thisER] &&
	       termVal[tag] >= validMean[thisER] - width*validStd[thisER]) {
		thisGE->SetPoint(thisGE->GetN(), tdt->Convert(), termVal[tag]);
		tag += "_Err";
		thisGE->SetPointError(thisGE->GetN() - 1, 0., termVal[tag]);
	    }
	}

	delete tdt;
    }

    for(unsigned int iER = 0; iER < ERList.size(); iER++) {
	string thisER = ERList[iER];
	if(thisER == "sum029n060") {
	    if(term == "mean" || term == "std") {
		continue;
	    }
	}

	TGraphErrors* thisGE = ge[thisER];
	const double width = 2.;
	/*
	cout << "mean : " << validMean[thisER] << endl
	     << "std  : " << validStd[thisER] << endl
	     << "range: " << validMean[thisER] - width*validStd[thisER]
	     << " - " << validMean[thisER] + width*validStd[thisER] << endl;
	*/
	printRawGraph(thisGE, term, thisER);
    }

    for(map<string, TGraphErrors*>::iterator it = ge.begin(); it != ge.end(); ++it)
	delete it->second;
}



void GraphPrinter::printRawGraph(TGraph* dataG, string term, string ER) {
    TCanvas* cG = new TCanvas("cG", "cG", 1400, 800);

    cG->cd();
    setGraphAtt(dataG, term, ER);
    setRangeUser(dataG->GetYaxis(), term, ER);
    dataG->Draw("AP");

    string outputGraphPath = outputGraphFolder + "/" + term + "_" + ER + ".png";

    cG->Update();
    cG->Print(outputGraphPath.c_str());

    delete cG;
}



void GraphPrinter::printGraphWithErrorBand(TGraph* dataG, double avg, double std, double bandWidth, string term, string ER) {
}



void GraphPrinter::setGraphAtt(TGraph* inputG, string term, string ER) {
    string graphTitle = makeTitle("graphTitle", term, ER);
    string xAxisTitle = makeTitle("xAxis", term, ER);
    string yAxisTitle = makeTitle("yAxis", term, ER);

    if(ER != "")
	graphTitle = graphTitle + " (" + makeERStr(ER, "elementFirst") + ")";

    if(xAxisTitle == "Date") {
	setDateAxis(inputG->GetXaxis());
    }

    inputG->SetTitle(graphTitle.c_str());
    inputG->GetXaxis()->SetTitle(xAxisTitle.c_str());
    inputG->GetYaxis()->SetTitle(yAxisTitle.c_str());

    inputG->SetLineWidth(1);
    inputG->SetLineColor(kBlack);
    inputG->SetMarkerSize(1);
    inputG->SetMarkerStyle(kCircle);
    inputG->SetMarkerColor(kBlack);
}



void GraphPrinter::setMultiGAtt(TMultiGraph* inputMG, string term, string ER) {
    string graphTitle = makeTitle("graphTitle", term, ER);
    string xAxisTitle = makeTitle("xAxis", term, ER);
    string yAxisTitle = makeTitle("yAxis", term, ER);

    if(ER != "")
	graphTitle = graphTitle + " (" + makeERStr(ER, "elementFirst") + ")";

    if(xAxisTitle == "Date") {
	setDateAxis(inputMG->GetXaxis());
    }

    inputMG->SetTitle(graphTitle.c_str());
    //inputMG->GetHistogram()->SetTitle(graphTitle.c_str());
    inputMG->GetXaxis()->SetTitle(xAxisTitle.c_str());
    inputMG->GetYaxis()->SetTitle(yAxisTitle.c_str());
}



void GraphPrinter::setDateAxis(TAxis* inputAxis) {
    inputAxis->SetTimeDisplay(kTRUE);
    inputAxis->SetTimeFormat("%m\/%d");
}



void GraphPrinter::setRangeUser(TAxis* inputAxis, string term, string ER) {
    double upper = -1.;
    double lower = 0.;
    if(term == "cGauss") {
	if(ER == "peak029") {
	    
	} else if(ER == "peak060") {
	    upper = 100.;
	    lower = 0.;
	} else if(ER == "peak146") {
	    upper = 120.;
	    lower = 20.;
	}
    } else if(term == "mean") {
	if(ER == "peak029") {
	    upper = 0.31;
	    lower = 0.25;
	} else if(ER == "peak060") {
	    upper = 0.65;
	    lower = 0.55;
	} else if(ER == "peak146") {
	    upper = 1.5;
	    lower = 1.35;
	}
    }

    if(upper > lower)
	inputAxis->SetRangeUser(lower, upper);
}



string GraphPrinter::makeERStr(string ER, string style) {
    string returnStr = "";
    if(ER == "full") {
	returnStr = "Full Spectrum";
    } else if(ER == "0to25") {
	returnStr = "0 to 2.5 MeV";
    } else if(style == "elementFirst") {
	if(ER == "peak029")
	    returnStr = "Pb-214, 0.295 MeV";
	else if(ER == "peak060")
	    returnStr = "Bi-214, 0.609 MeV";
	else if(ER == "peak146")
	    returnStr = "K-40, 1.460 MeV";
	else if(ER == "sum029n060")
	    returnStr = "Pb-214 & Bi-214, 0.295 & 0.609 MeV";
    } else if(style == "energyFirst") {
	if(ER == "peak029")
	    returnStr = "0.295 MeV (Pb-214)";
	else if(ER == "peak060")
	    returnStr = "0.609 MeV (Bi-214)";
	else if(ER == "peak146")
	    returnStr = "1.460 MeV (K-40)";
	else if(ER == "sum029n060")
	    returnStr = "0.295 & 0.609 MeV (Pb-214 & Bi-214)";
    } else {
	cout << "Unknown Energy Range!!! Return original input type name." << endl;
	returnStr = ER;
    }

    return returnStr;
}



string GraphPrinter::makeTitle(string component, string term, string ER) {
    string returnStr = "";
    if(term == "cGauss") {
	if(component == "graphTitle")
	    returnStr = "Coefficient of Gaussian Term";
	else if(component == "xAxis")
	    returnStr = "Date";
	else if(component == "yAxis")
	    returnStr = "Coefficient";
    } else if(term == "mean") {
	if(component == "graphTitle")
	    returnStr = "Gaussian Mean";
	else if(component == "xAxis")
	    returnStr = "Date";
	else if(component == "yAxis")
	    returnStr = "Mean (MeV)";
    } else if(term == "std") {
	if(component == "graphTitle")
	    returnStr = "Gaussian STD";
	else if(component == "xAxis")
	    returnStr = "Date";
	else if(component == "yAxis")
	    returnStr = "STD (MeV)";
    } else if(term == "counts") {
	if(component == "graphTitle")
	    returnStr = "Total Counts in an Energy Range";
	else if(component == "xAxis")
	    returnStr = "Date";
	else if(component == "yAxis")
	    returnStr = "Event Counts";
    }

    return returnStr;
}
