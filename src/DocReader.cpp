#include "DocReader.h"

#include <fstream>

const string DocReader::conflictionStr = "Term name confliction!!!";
const string DocReader::notFoundStr = "The term was not found!!!";

DocReader::DocReader(string inputFilename) {
    loadTermList(inputFilename);
}



DocReader::~DocReader() {}



void DocReader::execute() {}



void DocReader::test() {
    cout << termList[0].valueStr << endl;
    string emptyStr = "";
    string nullStr;

    cout << "emptyStr: " << emptyStr.size() << endl
	 << "nullStr: " << nullStr.size() << endl;

    try {
	cout << readIntValue("test1") << endl;
    } catch(const char* message) {
	cout << message << endl;
    }
}



string DocReader::readStrValue(string term) {
    string returnStr = "";
    for(unsigned int i = 0; i < termList.size(); i++) {
	if(term == termList[i].termName) {
	    if(returnStr.size() == 0)
		returnStr = termList[i].valueStr;
	    else
		throw conflictionStr.c_str();
	}
    }

    if(returnStr.size() == 0)
	throw notFoundStr.c_str();

    return returnStr;
}



int DocReader::readIntValue(string term) {
    bool doesHaveReturnInt = false;
    int returnInt = 0;

    for(unsigned int i = 0; i < termList.size(); i++) {
	if(term == termList[i].termName) {
	    if(!doesHaveReturnInt) {
		returnInt = atoi(termList[i].valueStr.c_str());
		doesHaveReturnInt = true;
	    } else
		throw conflictionStr.c_str();
	}
    }

    if(!doesHaveReturnInt)
	throw notFoundStr.c_str();

    return returnInt;
}



double DocReader::readDoubleValue(string term) {
    bool doesHaveReturnDouble = false;
    double returnDouble = 0.;

    for(unsigned int i = 0; i < termList.size(); i++) {
	if(term == termList[i].termName) {
	    if(!doesHaveReturnDouble) {
		returnDouble = atof(termList[i].valueStr.c_str());
		doesHaveReturnDouble = true;
	    } else
		throw conflictionStr.c_str();
	}
    }

    if(!doesHaveReturnDouble)
	throw notFoundStr.c_str();

    return returnDouble;
}



bool DocReader::readBoolValue(string term) {
    bool doesHaveReturnBool = false;
    bool returnBool = false;

    for(unsigned int i = 0; i < termList.size(); i++) {
	if(term == termList[i].termName) {
	    if(!doesHaveReturnBool) {
		string boolStr = termList[i].valueStr;
		if(boolStr == "true" || boolStr == "\'TRUE\'")
		    returnBool = true;
		else if(boolStr == "false" || boolStr == "\'FALSE\'")
		    returnBool = false;
		else
		    throw "The input text cannot be identified!!!";
	    } else
		throw conflictionStr.c_str();
	}
    }

    if(!doesHaveReturnBool)
	throw notFoundStr.c_str();

    return returnBool;
}



void DocReader::loadTermList(string inputFilename) {
    ifstream termListFile;
    termListFile.open(inputFilename);

    string line;
    while(termListFile.is_open()) {
	if(termListFile.eof())
	    break;

	getline(termListFile, line);
	if(line.size() != 0 && line.find(":") != string::npos) {
	    Term aTerm;
	    aTerm.termName = line.substr(0, line.find_first_of(":"));
	    string tailOfLine = line.substr(line.find_first_of(":") + 1);
	    aTerm.valueStr = tailOfLine.substr(tailOfLine.find_first_not_of(" "));
	    termList.push_back(aTerm);
	}
    }

    termListFile.close();
}
