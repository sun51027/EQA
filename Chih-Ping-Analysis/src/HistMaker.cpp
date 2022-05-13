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
    doRewriteTIListFile = false;
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
    makeHistWithWindow("peak029", 0.2677, 0.2977);
    makeHistWithWindow("peak060", 0.5511, 0.6648);
    makeHistWithWindow("peak146", 1.3918, 1.5057);
    makeHistWithWindow("0to25", 0., 2.5);

    if(doRewriteTIListFile)
	outputRecordTxt(timeIntervalListFilename);
}



void HistMaker::test() {
    timeIntervalListFilename = "document/repeatProduceDoc.txt";
    prepareDatafileList();
    prepareTimeIntervalList();
    /*
    for(unsigned int i = 0; i < datafileList.size(); i++) {
	cout << datafileList[i] << endl;
    }
    */
    for(unsigned int i = 0; i < startDTofTI.size(); i++) {
	cout << "\"" << startDTofTI[i] << " - " << endDTofTI[i] << "\"" << endl;
    }

    //outputRecordTxt();
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
	Calendar* startDT = new Calendar(startDTofTI[iTI]);
	Calendar* endDT = new Calendar(endDTofTI[iTI]);

	char htitle[35];
	sprintf(htitle, "%s - %s",
		startDT->getDateTime().c_str(),
		endDT->getDateTime().c_str());
	TH1D* h = new TH1D(startDT->getTime().c_str(), htitle, bin, min, max);

	string firstLayerDirName = "HistoCh0";
	if(outfile->GetDirectory(firstLayerDirName.c_str()) == nullptr)
	    outfile->mkdir(firstLayerDirName.c_str());

	char dateDirPath[150];
	sprintf(dateDirPath, "%s/%s", firstLayerDirName.c_str(), startDT->getDate().c_str());
	if(outfile->GetDirectory(dateDirPath) == nullptr)
	    outfile->mkdir(dateDirPath, dateDirPath);

	cout << htitle << endl;

	bool willBeDealed = false;
	for(unsigned int iDF = 0; iDF < datafileList.size(); iDF++) {
	    string thisDF = datafileList[iDF];
	    string thisDFDTStr = thisDF.substr(thisDF.find_first_of("/") + 1);
	    string nextDFDTStr = datafileList[iDF + 1].substr(datafileList[iDF + 1].find_first_of("/") + 1);

	    if(iDF + 1 >= datafileList.size())
		willBeDealed = hasDatafileInInterval(thisDFDTStr, startDT, endDT, "null");
	    else
		willBeDealed = hasDatafileInInterval(thisDFDTStr, startDT, endDT, nextDFDTStr);

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

	outfile->cd(dateDirPath);
	h->Write();

	delete h;
	delete endDT;
	delete startDT;
    }
}



void HistMaker::makeHistWithWindow(string energyRange, double low, double up) {
    for(unsigned int iTI = 0; iTI < startDTofTI.size(); iTI++) {
	Calendar* startDT = new Calendar(startDTofTI[iTI]);
	Calendar* endDT = new Calendar(endDTofTI[iTI]);

	char htitle[35];
	sprintf(htitle, "%s - %s",
		startDT->getDateTime().c_str(),
		endDT->getDateTime().c_str());
	TH1D* h = new TH1D(startDT->getTime().c_str(), htitle, bin, min, max);

	string firstLayerDirName = "window_" + energyRange;
	if(outfile->GetDirectory(firstLayerDirName.c_str()) == nullptr)
	    outfile->mkdir(firstLayerDirName.c_str());

	char dateDirPath[150];
	sprintf(dateDirPath, "%s/%s", firstLayerDirName.c_str(), startDT->getDate().c_str());

	if(outfile->GetDirectory(dateDirPath) == nullptr)
	    outfile->mkdir(dateDirPath, dateDirPath);

	cout << htitle << endl;

	bool willBeDealed = false;
	for(unsigned int iDF = 0; iDF < datafileList.size(); iDF++) {
	    string thisDF = datafileList[iDF];
	    string thisDFDTStr = thisDF.substr(thisDF.find_first_of("/") + 1);
	    string nextDFDTStr = datafileList[iDF + 1].substr(datafileList[iDF + 1].find_first_of("/") + 1);

	    if(iDF + 1 >= datafileList.size())
		willBeDealed = hasDatafileInInterval(thisDFDTStr, startDT, endDT, "null");
	    else
		willBeDealed = hasDatafileInInterval(thisDFDTStr, startDT, endDT, nextDFDTStr);

	    if(willBeDealed) {
		cout << thisDF << endl;
		DataReader* dr = new DataReader(thisDF);
		dr->setQuantity(quantity);
		dr->setStartDT(startDT->getDateTime());
		dr->setEndDT(endDT->getDateTime());
		dr->runFillingWithWindow(h, low, up);

		delete dr;
	    }
	}

	if(quantity == "Voltage")
	    h->SetXTitle("Voltage (V)");
	else if(quantity == "Energy")
	    h->SetXTitle("Energy (MeV)");

	h->SetYTitle("Entries");

	outfile->cd(dateDirPath);
	h->Write();

	delete h;
	delete endDT;
	delete startDT;
    }
}



void HistMaker::prepareDatafileList() {
    datafileList.clear();
    string dataDirStr = "data/";
    DIR* dataDir;
    struct dirent* dataDirent;
    string dateDirStr;

    if((dataDir = opendir(dataDirStr.c_str())) != nullptr) {
	while((dataDirent = readdir(dataDir)) != nullptr) {
	    dateDirStr = dataDirent->d_name;
	    if(dateDirStr != "." && dateDirStr != "..") {
		string dateDirPath = dataDirStr + dateDirStr + "/";
		DIR* dateDir;
		struct dirent* dateDirent;
		string filename;

		if((dateDir = opendir(dateDirPath.c_str())) != nullptr) {
		    while((dateDirent = readdir(dateDir)) != nullptr) {
			filename = dateDirent->d_name;
			if(filename != "." && filename != "..") {
			    string filePath = dateDirStr + "/" + filename.substr(0, filename.find(".root"));
			    datafileList.push_back(filePath);
			}
		    }
		    closedir(dateDir);
		} else {
		    perror("opendir");
		}
	    }
	}
	closedir(dataDir);
    } else {
	perror("opendir");
    }

    sort(datafileList.begin(), datafileList.end());
}



void HistMaker::prepareDatafileList2() {
    datafileList.clear();
    DIR* dir;
    struct dirent * diread;
    string dateDir;
    string dataDir = "../bashTest/data/";

    if((dir = opendir(dataDir.c_str())) != nullptr) {
	while((diread = readdir(dir)) != nullptr) {
	    dateDir = diread->d_name;
	    if(dateDir != "." && dateDir != "..") {
		DIR* dir2;
		struct dirent * diread2;
		string filename;
		string dataDir2 = dataDir + dateDir + "/";

		if((dir2 = opendir(dataDir2.c_str())) != nullptr) {
		    while((diread2 = readdir(dir2)) != nullptr) {
			filename = diread2->d_name;
			if(filename != "." && filename != "..") {
			    datafileList.push_back(dateDir + "/" + filename);
			}
		    }
		    closedir(dir2);
		} else {
		    perror("opendir");
		}
	    }
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
	} else if(txtfileFormat == "ContinuousPeriodsCountTI") {
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
	} else if(txtfileFormat == "ContinuousPeriodsEndDT") {
	    string startline = docr->readStrValue("StartTime");
	    string endline = docr->readStrValue("EndTime");
	    int passedHour = docr->readIntValue("Hour");
	    int passedMin = docr->readIntValue("Minute");
	    double passedSec = docr->readDoubleValue("Second");

	    Calendar* dt = new Calendar(startline);
	    Calendar* endDT = new Calendar(endline);

	    while(*dt < *endDT) {
		startDTofTI.push_back(dt->getDateTime());
		dt->addDuration(0, 0, passedHour, passedMin, passedSec);
		if(*dt < *endDT)
		    endDTofTI.push_back(dt->getDateTime());
		else if(*dt >= *endDT)
		    endDTofTI.push_back(endDT->getDateTime());
	    }

	    delete endDT;
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



void HistMaker::outputRecordTxt() {
    outputRecordTxt("document/testStateRecord.txt");
}



void HistMaker::outputRecordTxt(string txtfile) {
    ofstream stateRecord;
    stateRecord.open(txtfile.c_str());
    if(stateRecord.is_open()) {
	stateRecord << "FileFormat: ContinuousPeriodsEndDT" << endl
		    << "StartTime: " << endDTofTI[endDTofTI.size() - 1] << endl
		    << "EndTime: 20220101000000" << endl
		    << "Hour: 2" << endl
		    << "Minute: 0" << endl
		    << "Second: 0" << endl;
    }
}
