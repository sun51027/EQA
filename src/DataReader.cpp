#include "DataReader.h"

DataReader::DataReader(string inputDatafileName) {
    datafileName = inputDatafileName;
    prepareDatafile();

    quantity = "Voltage";

    fileDT = new Calendar(datafileName);
    startDT = new Calendar(datafileName);
    endDT = new Calendar(datafileName);
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
	cout << "Processed ... " << entry << "/" << nentries << " events" << endl;

	tree->GetEntry(entry);

	double fillingValue = ch0;

	Calendar* eventDT = new Calendar(datafileName);
	eventDT->addDuration(0, 0, 0, 0, timestamp);

	if(*eventDT >= *startDT &&
	   *eventDT <= *endDT) {
	    if(quantity == "Voltage")
		inputH->Fill(fillingValue);
	    else if(quantity == "Energy")
		inputH->Fill(convertVolt2MeV(fillingValue));
	}

	delete eventDT;
    }
}



void DataReader::setDatafileName(string inputFilename) {
    datafileName = inputFilename;
    prepareDatafile();
}



void DataReader::setEndDT(string inputDTStr) {
    endDT->setDateTime(inputDTStr);
}



void DataReader::setStartDT(string inputDTStr) {
    startDT->setDateTime(inputDTStr);
}



void DataReader::prepareDatafile() {
    if(file != nullptr) {
	file->Close();
	delete file;
    }

    string datafilePath = "data/" + datafileName + ".root";
    file = new TFile(datafilePath.c_str(), "READ");
    file->GetObject("detector_A", tree);

    tree->SetBranchAddress("ch0", &ch0, &b_ch0);
    tree->SetBranchAddress("timestamp", &timestamp, &b_timestamp);
}



double DataReader::convertVolt2MeV(double inputV) {
    double p0 = 0.107648;
    double p1 = 1.034203;

    return (inputV - p0)/p1;
}
