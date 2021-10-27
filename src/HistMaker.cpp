#include "HistMaker.h"

#include <TH1.h>

#include "Calendar.h"

HistMaker::HistMaker() {}



HistMaker::~HistMaker() {}



void HistMaker::execute() {}



void HistMaker::test() {}



void HistMaker::makeHistogram() {
    //initialize

    for(unsigned int iTI = 0; iTI < startDTofTI.size(); iTI++) {
	Calendar *startDT = new Calendar(startDTofTI[iTI]);
	Calendar* endDT = new Calendar(endDTofTI[iTI]);

	char htitle[35];
	sprintf(htitle, "%s - $s",
		startDT->getDateTime().c_str(),
		endDT->getDateTime().c_str());
	TH1D* h = new TH1D(startDT->getTime().c_str(), htitle, bin, min, max);

	string firstFolderName = "HistoCh0";
	if(outfile->GetDirectory(firstFolderName.c_str()) == nullptr)
	    outfile->mkdir(folderName.c_str());

	char histPath[150];
	sprintf(histPath, "%s/%s", firstFolderName.c_str(), startDT->getDate().c_str());
	if(outfile->GetDirectory(histPath) == nullptr)
	    outfile->mkdir(histPath, histPath);

	cout << htitle << endl;

	bool willBeDealed = false;
	for(unsigned int iDF = 0; iDF < datafileList.size(); iDF++) {
	    string thisDF = datafileList[iDF];

	    if(iDF + 1 >= datafileList.size())
		willBeDealed = hasDataInInterval(thisDF, startDT, endDT, "null");
	    else
		willBeDealed = hasDataInInterval(thisDF, startDT, endDT, datafileList[iDF + 1]);

	    if(willBeDealed) {
		cout << thisDF << endl;

		//DataReader* dr
	    }
	}

	if(quantity == "Voltage")
	    h->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    h->SetXTitle("Energy (MeV)");

	h->SetYTitle("Entries");

	outfile->cd(histPath);
	h->Write();

	delete h;
	delete endDT;
	delete startDT;
    }
}



bool HistMaker::hasDatafileInInterval(string inputDTStr, string startDTStr, string endDTStr, string nextDTStr) {
    bool output = false;
    Calendar* inputDT = new Calendar(inputDTStr);
    Calendar* startDT = new Calendar(startDTStr);
    Calendar* endDT = new Calendar(endDTStr);
    Calendar* nextDT = nullptr;

    if(nextDTStr != "null")
	nextDT = new Calendar(nextDTStr);

    output = hasDatafileInInterval(inputDT, startDT, endDT, nextDT);

    if(nextDT != nullptr)
	delete nextDT;

    delete endDT;
    delete startDT;
    delete inputDT;

    return output;
}



bool HistMaker::hasDatafileInInterval(string inputDTStr, Calendar* startDT, Calendar* endDT, string nextDTStr) {
    bool output = false;
    Calendar* inputDT = new Calendar(inputDTStr);
    Calendar* nextDT = nullptr;

    if(nextDTStr != "null")
	nextDT = new Calendar(nextDTStr);

    output = hasDatafileInInterval(inputDT, startDT, endDT, nextDT);

    if(nextDT != nullptr)
	delete nextDT;

    delete inputDT;

    return output;
}



bool HistMaker::hasDatafileInInterval(Calendar* inputDT, Calendar* startDT, Calendar* endDT, Calendar* nextDT) {
    bool output = false;
    if(*inputDT >= *startDT && *inputDT < *endDT)
	output = true;
    else if(*inputDT < *startDT) {
	if(nextDT != nullptr) {
	    if(*nextDT > *startDT)
		output = true;
	    else
		output = false;
	} else
	    output = true;
    } else
	output = false;

    return output;
}
