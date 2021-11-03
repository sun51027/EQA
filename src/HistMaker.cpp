#include "HistMaker.h"

#include <dirent.h>
#include <fstream>

#include <TFile.h>
#include <TH1.h>

#include "Calendar.h"
#include "DataReader.h"
#include "DocReader.h"

HistMaker::HistMaker() {
    prepareDatafileList();
    way2SetTimeInterval = "listFile";
    timeIntervalListFilename = "document/defaultTIList.txt";
    quantity = "Voltage";

    outfileName = "test";
    outfileOpenState = "RECREATE";
    outfile = nullptr;
}



HistMaker::~HistMaker() {
    if(outfile != nullptr) {
	outfile->Close();
	delete outfile;
    }
}



void HistMaker::execute() {
    setOutfile();
    prepareTimeIntervalList();
    makeHistogram();
}



void HistMaker::test() {
    prepareTimeIntervalList();

    for(unsigned int i = 0; i < startDTofTI.size(); i++) {
	cout << "\"" << startDTofTI[i] << " - " << endDTofTI[i] << "\"" << endl;
    }
}



void HistMaker::setOutfile() {
    setOutfile(outfileName);
}



void HistMaker::setOutfile(string inputFilename) {
    outfileName = inputFilename;
    if(outfile != nullptr) {
	outfile->Close();
	delete outfile;
    }

    string outfileFolder = "analyzedFile";
    string outfilePath = outfileFolder + "/" + outfileName + ".root";
    outfile = new TFile(outfilePath.c_str(), outfileOpenState.c_str());
}



void HistMaker::makeHistogram() {
    for(unsigned int iTI = 0; iTI < startDTofTI.size(); iTI++) {
	Calendar *startDT = new Calendar(startDTofTI[iTI]);
	Calendar* endDT = new Calendar(endDTofTI[iTI]);

	char htitle[35];
	sprintf(htitle, "%s - %s",
		startDT->getDateTime().c_str(),
		endDT->getDateTime().c_str());
	TH1D* h = new TH1D(startDT->getTime().c_str(), htitle, bin, min, max);

	string firstLayerDirName = "HistoCh0";
	if(outfile->GetDirectory(firstLayerDirName.c_str()) == nullptr)
	    outfile->mkdir(firstLayerDirName.c_str());

	char histPath[150];
	sprintf(histPath, "%s/%s", firstLayerDirName.c_str(), startDT->getDate().c_str());
	if(outfile->GetDirectory(histPath) == nullptr)
	    outfile->mkdir(histPath, histPath);

	cout << htitle << endl;

	bool willBeDealed = false;
	for(unsigned int iDF = 0; iDF < datafileList.size(); iDF++) {
	    string thisDF = datafileList[iDF];

	    if(iDF + 1 >= datafileList.size())
		willBeDealed = hasDatafileInInterval(thisDF, startDT, endDT, "null");
	    else
		willBeDealed = hasDatafileInInterval(thisDF, startDT, endDT, datafileList[iDF + 1]);

	    if(willBeDealed) {
		cout << thisDF << endl;
		DataReader* dr = new DataReader(thisDF);
		dr->setQuantity(quantity);
		dr->setStartDT(startDT->getDateTime());
		dr->setEndDT(endDT->getDateTime());
		dr->runFillingLoop(h);

		delete dr;
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



void HistMaker::prepareDatafileList() {
    DIR* dir;
    struct dirent * diread;
    string filename;

    if((dir = opendir("data/")) != nullptr) {
	while((diread = readdir(dir)) != nullptr) {
	    filename = diread->d_name;
	    if(filename != "." && filename != "..")
		datafileList.push_back(filename.substr(0, filename.find(".root")));
	}
	closedir(dir);
    } else {
	perror("opendir");
    }

    sort(datafileList.begin(), datafileList.end());
}



void HistMaker::prepareTimeIntervalList() {
    startDTofTI.clear();
    endDTofTI.clear();

    if(way2SetTimeInterval == "command") {
    } else if(way2SetTimeInterval == "listFile") {
	DocReader* docr = new DocReader(timeIntervalListFilename);

	string txtfileFormat = docr->readStrValue("FileFormat");
	if(txtfileFormat == "AssignedTimeIntervals") {
	    ifstream TIListFile;
	    TIListFile.open(timeIntervalListFilename.c_str());

	    string startline;
	    string endline;
	    string line;
	    while(TIListFile.is_open()) {
		if(TIListFile.eof()) break;

		getline(TIListFile, line);
		if(!line.empty()) {
		    if(line.find("-") == string::npos)
			continue;

		    startline = line.substr(0, line.find_first_of("-"));
		    endline = line.substr(line.find_first_of("-"));

		    startDTofTI.push_back(startline.substr(0, startline.find_last_not_of(" -") + 1));
		    endDTofTI.push_back(endline.substr(endline.find_first_not_of(" -")));
		}
	    }

	    TIListFile.close();
	} else if(txtfileFormat == "ContinuousPeriods") {
	    string startline = docr->readStrValue("StartTime");
	    int passedHour = docr->readIntValue("Hour");
	    int passedMin = docr->readIntValue("Minute");
	    double passedSec = docr->readDoubleValue("Second");
	    int numOfTI = docr->readIntValue("NumberOfTimeIntervals");

	    Calendar* dt = new Calendar(startline);
	    for(int i = 0; i < numOfTI; i++) {
		startDTofTI.push_back(dt->getDateTime());
		dt->addDuration(0, 0, passedHour, passedMin, passedSec);
		endDTofTI.push_back(dt->getDateTime());
	    }

	    delete dt;
	}

	delete docr;
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
	    if(*nextDT >= *startDT)
		output = true;
	    else
		output = false;
	} else
	    output = true;
    } else
	output = false;

    return output;
}
