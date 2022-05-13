#include "DataReader.h"

#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

#include "Calendar.h"

DataReader::DataReader(string inputDatafileName) {
    file = nullptr;
    setDatafile(inputDatafileName);

    quantity = "Voltage";

    fileDT = new Calendar(actualDTStr);
    startDT = new Calendar(actualDTStr);
    endDT = new Calendar(actualDTStr);
}



DataReader::~DataReader() {
    file->Close();

    delete endDT;
    delete startDT;
    delete fileDT;

    delete file;
}



void DataReader::execute() {}



void DataReader::test() {}



void DataReader::runFillingLoop(TH1D* inputH) {
    Long64_t nentries = tree->GetEntries();

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	if(entry % 5000 == 0)
	    cout << "Processed ... " << entry << "/" << nentries << " events" << endl;

	tree->GetEntry(entry);

	double fillingValue = ch0;

	Calendar* eventDT = new Calendar(actualDTStr);
	eventDT->addDuration(0, 0, 0, 0, timestamp);

	if(*eventDT >= *startDT && *eventDT <= *endDT) {
	    if(quantity == "Voltage")
		inputH->Fill(fillingValue);
	    else if(quantity == "Energy")
		inputH->Fill(convertVolt2MeV(fillingValue));
	}

	delete eventDT;
    }
}



void DataReader::runFillingWithWindow(TH1D* inputH, double low, double up) {
    Long64_t nentries = tree->GetEntries();

    for(Long64_t entry = 0; entry < nentries; ++entry) {
	if(entry % 5000 == 0)
	    cout << "Processed ... " << entry << "/" << nentries << " events" << endl;

	tree->GetEntry(entry);

	double fillingValue = ch0;

	Calendar* eventDT = new Calendar(actualDTStr);
	eventDT->addDuration(0, 0, 0, 0, timestamp);

	if(*eventDT >= *startDT  && *eventDT <= *endDT) {
	    if(quantity == "Voltage") {
		if(fillingValue >= low && fillingValue <= up)
		    inputH->Fill(fillingValue);
	    } else if(quantity == "Energy") {
		fillingValue = convertVolt2MeV(fillingValue);
		if(fillingValue >= low && fillingValue <= up)
		    inputH->Fill(fillingValue);
	    }
	}

	delete eventDT;
    }
}



void DataReader::setDatafile(string inputFilename) {
    datafileName = inputFilename;
    if(file != nullptr) {
	file->Close();
	delete file;
    }

    string datafilePath = "data/" + datafileName + ".root";
    file = new TFile(datafilePath.c_str(), "READ");
    file->GetObject("detector_A", tree);

    tree->SetBranchAddress("ch0", &ch0, &b_ch0);
    tree->SetBranchAddress("timestamp", &timestamp, &b_timestamp);

    actualDTStr = datafileName.substr(datafileName.find_first_of("/") + 1);
}



void DataReader::setEndDT(string inputDTStr) {
    endDT->setDateTime(inputDTStr);
}



void DataReader::setStartDT(string inputDTStr) {
    startDT->setDateTime(inputDTStr);
}



double DataReader::convertVolt2MeV(double inputV) {
    double p0 = 0.112214;
    double p1 = 1.;

    return (inputV - p0)/p1;
}
