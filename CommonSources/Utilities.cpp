/******************************************************
	A collection of XCS based classifiers for big data analysis
	Specifically designed for high-dimensional classification purpose	
/******************************************************
#
#		by Mani Abedini
#		
#		The department of computing and Information system
#		Engineering school
#		The University of Melbourne 
#		Australia
#		maniabedini@gmail.com
#		Last modified: July-2014
#		
#		This source code is based on Martin Butz's XCS implementation
#
# Copyright (c) 2009-2014, Mani Abedini
# All rights reserved.

# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************** */

#include "Utilities.h"
/* randomize the pseudo random generator */
void randomize(void) {
	int i;
	//  long int t = time( NULL );
	//    srand((unsigned)time( NULL ));

	for (i = 0; i < time(NULL) % 1000; rand(), i++)
		;
	_seed = (long) (time(NULL) % 10000 + 1);
}

void randomize(long int iTime) {

	if (iTime == 0)
		time(NULL);

	int i;
	for (i = 0; i < iTime % 1000; rand(), i++)
		;
}

double urand() {
	long lo, hi, test;

	hi = _seed / _QSeed;
	lo = _seed % _QSeed;
	test = _ASeed * lo - _RSeed * hi;

	if (test > 0)
		_seed = test;
	else
		_seed = test + _MSeed;

	return (double) (_seed) / _MSeed;
}

/* returns a floating-point random number generated according to a normal distribution with
 * mean 0 and standard deviation 1 */
double nrand() {

	float x1, x2, w;

	if (haveUniNum) {
		haveUniNum = 0;
		return uniNum;
	} else {
		do {
			x1 = 2.0 * urand() - 1.0;
			x2 = 2.0 * urand() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0);

		w = sqrt((-2.0 * log(w)) / w);
		uniNum = x1 * w;
		haveUniNum = 1;
		return x2 * w;
	}

}

// read Data from file
void ReadDataFromFile(char *dataFileName, list<Classifier *> *dataInFolds) {
	//	int *EachClassStat = new int [Configuration::getNUMBER_OF_ACTIONS()];

	char attName[255], attDesc[255], Line[255];

	list<Classifier *> RawData;

	FILE *dataFile;
	dataFile = fopen(dataFileName, "rb");
	if (dataFile == NULL) {
		fprintf(stderr, "Cannot open file");
		exit(0);
	}

	int attributeNumber = -1;
	// read header of file
	while (feof(dataFile) < 1) {
		fscanf(dataFile, "%s", Line);
		if (strncmp(Line, "@relation", 9) == 0) // file description
		{
			attributeNumber = -1;
		}

		if (strcmp(Line, "@attribute") == 0) // New attribute Definition
		{
			attributeNumber++;
			fscanf(dataFile, "%s", attName);
			fscanf(dataFile, "%s", attDesc);

			Classifier::AttributeNames[attributeNumber] = string(attName);
			if (attDesc[0] == '{') // found Nominal attribute
			{
				Classifier::ConditionListTypes[attributeNumber]
						= (ConditionTypes) Nominal;
				char *t1;
				map<int, string> convert;
				int counts = 0;
				int min = 0, max = 0;
				strcpy(Line, attDesc);
				for (t1 = strtok(attDesc, " {,}"); t1 != NULL; t1 = strtok(
						NULL, " {,}")) {
					convert[counts] = string(t1);
					counts++;
				}
				Classifier::attributesConvertMap[attributeNumber] = convert;
			} else // in other cases the attribute is double/float base
			{
				//Classifier::ConditionListTypes[attributeNumber]= (ConditionTypes)RealNormalized;
				Classifier::ConditionListTypes[attributeNumber]
						= (ConditionTypes) Real;
				Classifier::ConditionListLowerBound[attributeNumber]
						= new double;
				Classifier::ConditionListUpperBound[attributeNumber]
						= new double;
				//attributes[attributeNumber] = NULL;
			}
		}

		if (strcmp(Line, "@data") == 0) // header is finished
			break;
	}

	Configuration::setCONDITION_LENGTH(attributeNumber);
	if (Classifier::ConditionListTypes[attributeNumber]
			== (ConditionTypes) Nominal) { // Action Convertion Map
		map<int, string> cv = Classifier::attributesConvertMap[attributeNumber];
		Configuration::setNUMBER_OF_ACTIONS(cv.size());
	}

	/*
	 for(map<int,map<int,float>* >::iterator i= attributes.begin();i!=attributes.end();i++ )
	 {
	 if((*(*i))!=NULL)
	 for(map<int,float>::iterator j=(*(*i)).begin();j!=(*(*i)).end();j++)
	 cout << (*j).second << endl;
	 }
	 */

	int line = 0;
	while (feof(dataFile) < 1) {
		char *DataLine = new char[256 * (Configuration::getCONDITION_LENGTH()
				+ 1)];
		char *t1;
		line++;

		int act;
		int i = 0;

		Classifier *cl;
		assert(cl= new Classifier());

		cl->setNum(1);

		fscanf(dataFile, "%s\n", DataLine);
		for (t1 = strtok(DataLine, " {,}"); t1 != NULL; t1 = strtok(NULL,
				" {,}"), i++) {
			if (i < Configuration::getCONDITION_LENGTH()) // attributes
			{
				if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) Nominal) {
					xNominalCondition *cond;
					assert(cond = new xNominalCondition());
					map<int, string> convertMap =
							Classifier::attributesConvertMap[i];
					cond->CategoryLength = convertMap.size();
					cond->Category = new bool[cond->CategoryLength];
					for (map<int, string>::const_iterator it =
							convertMap.begin(); it != convertMap.end(); it++)
						if ((*it).second.compare(t1) == 0)
							cond->SetCategoryByID((*it).first);

					cl->ConditionList[i] = cond;

				} else if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) RealNormalized) {
					float fCond;
					xRealNormalizedCondition *cond;
					assert(cond = new xRealNormalizedCondition());
					double centre = atof(t1);
					cond->setCenterValue(centre);
					cond->setRangeValue(0);

					cl->ConditionList[i] = cond;
				} else if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) Real) {
					float fCond;
					xRealCondition *cond;
					assert(cond = new xRealCondition(Classifier::ConditionListLowerBound[i] , Classifier::ConditionListUpperBound[i]));
					double centre = atof(t1);
					cond->setCenterValue(centre);
					cond->setRangeValue(0);

					if (centre < *Classifier::ConditionListLowerBound[i])
						*Classifier::ConditionListLowerBound[i] = centre;
					if (centre > *Classifier::ConditionListUpperBound[i])
						*Classifier::ConditionListUpperBound[i] = centre;

					cl->ConditionList[i] = cond;
				}
			} else // the action part
			if (Classifier::attributesConvertMap.find(
					Configuration::getCONDITION_LENGTH())
					== Classifier::attributesConvertMap.end()) { // no conversion for action part
				int act;
				cl->setAct(atoi(t1));
				//cout << "Class Not Conv" << cl->act << endl;
			} else // Conversion for action part
			{
				map<int, string>
						convertMap =
								Classifier::attributesConvertMap[Configuration::getCONDITION_LENGTH()];
				for (map<int, string>::const_iterator it = convertMap.begin(); it
						!= convertMap.end(); it++) {
					//cout << t1 << " == " << (*it).second << ":" << (*it).second.compare(t1)<< endl;
					if ((*it).second.compare(t1) == 0) {
						//cout << t1 << " == " << (*it).second << ":" << (*it).second.compare(t1)<< endl;
						cl->setAct((*it).first);
					}
				}
				//cout << t1 << "Class Conv" << cl->act << endl;
			}

		}

		if (cl->ConditionList.size() > 0) {
			if (frand() > 0.5)
				RawData.push_back(cl);
			else
				RawData.push_front(cl);

			//			EachClassStat[cl->act]++;
			//			RawData.push_back(cl);
		} else
			delete cl;
		//AllData->push_back(cl);

		delete[] DataLine;
	}
	fclose(dataFile);

	int foldNo = 0;
	// n list is created due to number of folds
	// all instances be longed to each class is distributed in each fold equally
	for (int a = 0; a < Configuration::getNUMBER_OF_ACTIONS(); a++) {
		//int instInEachFold= (EachClassStat[a] / Configuration::getNR_EXPS());
		//int insertedInstInCurrentFold[Configuration::getNR_EXPS()];
		// Reset All Folds Counters
		//for (unsigned int j=0;j<Configuration::getNR_EXPS();j++)
		//insertedInstInCurrentFold[j]=0;
		for (list<Classifier *>::const_iterator i = RawData.begin(); i
				!= RawData.end(); i++) {
			if ((*i)->getAct() == a) {
				// Select A random Fold
				//foldNo =(int)( frand()*Configuration::getNR_EXPS());
				// If that fold is already filled Select next available fold
				//while (insertedInstInCurrentFold[foldNo] >= instInEachFold)
				//foldNo= (foldNo+1) %Configuration::getNR_EXPS();

				//if (frand() > 0.5)
				//DiffFolds[foldNo].push_back((*i));
				//else
				//DiffFolds[foldNo].push_front((*i));

				//insertedInstInCurrentFold[foldNo]++;

				dataInFolds[foldNo].push_back((*i));
				foldNo = (foldNo + 1) % Configuration::getNR_EXPS();
			}
		}
	}

	// merge the folds to build the dataList again this time normally distributed
	//	for(int f=0;f<Configuration::getNR_EXPS();f++)
	//	{
	//		for(list<Classifier *>::const_iterator i=DiffFolds[f].begin();i!=DiffFolds[f].end();i++)
	//		{
	//			AllData->push_back((*i));
	//		}
	//		DiffFolds[f].clear();
	//	}

	//	delete EachClassStat;

}

void ReadBinaryRawDataFromFile(char *dataFileName,
		list<Classifier *> *RawDataList) {
	char attName[255], attDesc[255], Line[255];

	FILE *dataFile;
	dataFile = fopen(dataFileName, "rb");
	if (dataFile == NULL) {
		fprintf(stderr, "Cannot open file:%s\n", dataFileName);
		exit(0);
	}

	int attributeNumber = -1;
	// read header of file
	while (feof(dataFile) < 1) {
		fscanf(dataFile, "%s", Line);
		if (strncmp(Line, "@relation", 9) == 0) // file description
		{
			attributeNumber = -1;
		}

		if (strcmp(Line, "@attribute") == 0) // New attribute Definition
		{
			attributeNumber++;
			fscanf(dataFile, "%s", attName);
			fscanf(dataFile, "%s", attDesc);
			Classifier::ConditionListTypes[attributeNumber]
					= (ConditionTypes) Binary;
			Classifier::AttributeNames[attributeNumber] = string(attName);
		}

		if (strcmp(Line, "@data") == 0) // header is finished
			break;
	}

	Configuration::setCONDITION_LENGTH(attributeNumber);
	Configuration::setNUMBER_OF_ACTIONS(2);

	int line = 0;
	while (feof(dataFile) < 1) {
		char *DataLine = new char[256 * (Configuration::getCONDITION_LENGTH()
				+ 1)];
		char *t1;
		line++;

		int act;
		int i = 0;

		Classifier *cl;
		assert(cl= new Classifier());

		cl->setNum(1);

		fscanf(dataFile, "%s\n", DataLine);

		for (t1 = strtok(DataLine, " {,}"); t1 != NULL; t1 = strtok(NULL,
				" {,}"), i++) {
			if (i < Configuration::getCONDITION_LENGTH()) // attributes
			{
				xBinaryCondition *cond;
				assert(cond = new xBinaryCondition());
				cond->value = atoi(t1);
				cl->ConditionList[i] = cond;
			} else
				// the action part
				cl->setAct(atoi(t1));
		}

		if (cl->ConditionList.size() > 0) {
			if (frand() > 0.5)
				RawDataList->push_back(cl);
			else
				RawDataList->push_front(cl);

			//			//cl->dumpClassifier();
			//			RawDataList->push_back(cl);
		} else
			delete cl;

		delete[] DataLine;
	}
	fclose(dataFile);
}

int ReadRealNormalizedRawDataFromFile(const char *dataFileName, list<
		Classifier *> *dataInFolds) {
	char attTag[256], attName[255], attDesc[255], Line[255];
	list<Classifier *> RawData;

	FILE *dataFile;
	dataFile = fopen(dataFileName, "rb");
	if (dataFile == NULL) {
		fprintf(stderr, "Cannot open file");
		exit(0);
	}

	int attributeNumber = -1;
	// read header of file
	while (fgets(Line, sizeof Line, dataFile) != NULL) {
		//fscanf(dataFile, "%s", Line);

		int j = 0;
		for (j = 0; Line[j] != 0 && Line[j] != '\n'; j++)
			Line[j] = tolower(Line[j]);
		Line[j] = 0;

		if (strstr(Line, "@relation") != NULL) // file description
		{
			attributeNumber = -1;
		} else if (strstr(Line, "@attribute") != NULL) // New attribute Definition
		{
			attributeNumber++;
			sscanf(Line, "%s %s %s", attTag, attName, attDesc);

			Classifier::AttributeNames[attributeNumber] = string(attName);
			if (attDesc[0] == '{') // found Nominal attribute
			{
				Classifier::ConditionListTypes[attributeNumber]
						= (ConditionTypes) Nominal;
				char *t1;
				map<int, string> convert;
				int counts = 0;
				int min = 0, max = 0;
				strcpy(Line, attDesc);
				for (t1 = strtok(attDesc, " {,}"); t1 != NULL; t1 = strtok(
						NULL, " {,}")) {
					convert[counts] = string(t1);
					counts++;
				}
				Classifier::attributesConvertMap[attributeNumber] = convert;
			} else // in other cases the attribute is double/float base
			{
				Classifier::ConditionListTypes[attributeNumber]
						= (ConditionTypes) RealNormalized;
				//attributes[attributeNumber] = NULL;
			}
		} else if (strstr(Line, "@data") != 0) // header is finished
			break;
	}

	Configuration::setCONDITION_LENGTH(attributeNumber);

	if (Classifier::ConditionListTypes[attributeNumber]
			== (ConditionTypes) Nominal) { // Action Convertion Map
		map<int, string> cv = Classifier::attributesConvertMap[attributeNumber];
		Configuration::setNUMBER_OF_ACTIONS(cv.size());
	}

	int line = 0;
	int sizeOfLine = 256 * (attributeNumber + 1);
	char *DataLine = new char[sizeOfLine];

	while (fgets(DataLine, sizeOfLine, dataFile) != NULL) {
		char *cLastValue;
		line++;

		int act;
		int i = 0;
		char *t1;

		int j = 0;
		for (j = 0; DataLine[j] != 0 && DataLine[j] != '\n'; j++)
			DataLine[j] = tolower(DataLine[j]);
		DataLine[j] = 0;

		Classifier *cl;
		assert(cl= new Classifier());
		cl->setNum(1);

		for (t1 = strtok(DataLine, " {,}"); t1 != NULL; t1 = strtok(NULL,
				" {,}"), i++) {
			if (i < Configuration::getCONDITION_LENGTH()) // attributes
			{
				if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) Nominal) {
					xNominalCondition *cond;
					assert(cond = new xNominalCondition());
					map<int, string> convertMap =
							Classifier::attributesConvertMap[i];
					cond->CategoryLength = convertMap.size();
					cond->Category = new bool[cond->CategoryLength];
					for (map<int, string>::const_iterator it =
							convertMap.begin(); it != convertMap.end(); it++)
						if ((*it).second.compare(t1) == 0)
							cond->SetCategoryByID((*it).first);

					cl->ConditionList[i] = cond;

				} else if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) RealNormalized) {
					float fCond;
					xRealNormalizedCondition *cond;
					assert(cond = new xRealNormalizedCondition());
					double centre = atof(t1);
					cond->setCenterValue(centre);
					cond->setRangeValue(0);

					cl->ConditionList[i] = cond;
				} else if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) Real) {
					float fCond;
					xRealCondition *cond;
					assert(cond = new xRealCondition(Classifier::ConditionListLowerBound[i] , Classifier::ConditionListUpperBound[i]));
					double centre = atof(t1);
					cond->setCenterValue(centre);
					cond->setRangeValue(0);

					if (centre < *Classifier::ConditionListLowerBound[i])
						*Classifier::ConditionListLowerBound[i] = centre;
					if (centre > *Classifier::ConditionListUpperBound[i])
						*Classifier::ConditionListUpperBound[i] = centre;

					cl->ConditionList[i] = cond;
				}
			} else // the action part
			if (Classifier::attributesConvertMap.find(
					Configuration::getCONDITION_LENGTH())
					== Classifier::attributesConvertMap.end()) { // no conversion for action part
				int act;
				cl->setAct(atoi(t1));
				//cout << "Class Not Conv" << cl->act << endl;
			} else // Conversion for action part
			{
				map<int, string>
						convertMap =
								Classifier::attributesConvertMap[Configuration::getCONDITION_LENGTH()];
				for (map<int, string>::const_iterator it = convertMap.begin(); it
						!= convertMap.end(); it++) {
					if (string(t1).compare(0, (*it).second.length(),
							(*it).second) == 0) {
						cl->setAct((*it).first);
					}
				}
			}

		}

		if (cl->ConditionList.size() > 0) {
			// make different order for data set which may end with learning the test set in multiple COXCS
			//			if (frand() > 0.5)
			RawData.push_back(cl);
			//			else
			//				RawData.push_front(cl);
		} else
			delete cl;
	}
	fclose(dataFile);

	// Distributed data based on classes
	int foldNo = 0;
	// n list is created due to number of folds
	// all instances be longed to each class is distributed in each fold evenly
	for (int act = 0; act < Configuration::getNUMBER_OF_ACTIONS(); act++) {
		//    	map<int,string> cv = Classifier::attributesConvertMap[Configuration::getCONDITION_LENGTH()];
		//    	string act = cv[actIndex];
		for (list<Classifier *>::const_iterator clIterator = RawData.begin(); clIterator
				!= RawData.end(); clIterator++) {
			if ((*clIterator)->getAct() == act) {
				//				if (frand() > 0.5)
				dataInFolds[foldNo].push_back((*clIterator));
				//				else
				//					dataInFolds[foldNo].push_front((*clIterator));

				foldNo = (foldNo + 1) % Configuration::getNR_EXPS();
			}
		}
	}

	int NoRows = RawData.size();
	RawData.clear();
	delete[] DataLine;
	return NoRows;
}

int ReadRealNormalizedRawDataCreateValidationDataSetsRandomlyFromFile(
		const char *dataFileName, list<Classifier *> *TrainData, list<
				Classifier *> *TestData, int TrainNum, int TestNum) {
	char attTag[256], attName[255], attDesc[255], Line[255];
	list<Classifier *> RawData;

	FILE *dataFile;
	dataFile = fopen(dataFileName, "rb");
	if (dataFile == NULL) {
		fprintf(stderr, "Cannot open file");
		exit(0);
	}

	int attributeNumber = -1;
	// read header of file
	while (fgets(Line, sizeof Line, dataFile) != NULL) {
		//fscanf(dataFile, "%s", Line);

		int j = 0;
		for (j = 0; Line[j] != 0 && Line[j] != '\n'; j++)
			Line[j] = tolower(Line[j]);
		Line[j] = 0;

		if (strstr(Line, "@relation") != NULL) // file description
		{
			attributeNumber = -1;
		} else if (strstr(Line, "@attribute") != NULL) // New attribute Definition
		{
			attributeNumber++;
			sscanf(Line, "%s %s %s", attTag, attName, attDesc);

			Classifier::AttributeNames[attributeNumber] = string(attName);
			if (attDesc[0] == '{') // found Nominal attribute
			{
				Classifier::ConditionListTypes[attributeNumber]
						= (ConditionTypes) Nominal;
				char *t1;
				map<int, string> convert;
				int counts = 0;
				int min = 0, max = 0;
				strcpy(Line, attDesc);
				for (t1 = strtok(attDesc, " {,}"); t1 != NULL; t1 = strtok(
						NULL, " {,}")) {
					convert[counts] = string(t1);
					counts++;
				}

				Classifier::attributesConvertMap[attributeNumber] = convert;

			} else // in other cases the attribute is double/float base
			{
				Classifier::ConditionListTypes[attributeNumber]
						= (ConditionTypes) RealNormalized;
				//attributes[attributeNumber] = NULL;
			}
		} else if (strstr(Line, "@data") != 0) // header is finished
			break;
	}

	Configuration::setCONDITION_LENGTH(attributeNumber);

	if (Classifier::ConditionListTypes[attributeNumber]
			== (ConditionTypes) Nominal) { // Action Convertion Map
		map<int, string> cv = Classifier::attributesConvertMap[attributeNumber];
		Configuration::setNUMBER_OF_ACTIONS(cv.size());
	}

	int line = 0;
	int sizeOfLine = 256 * (attributeNumber + 1);
	char *DataLine = new char[sizeOfLine];

	while (fgets(DataLine, sizeOfLine, dataFile) != NULL) {
		char *cLastValue;
		line++;

		int act;
		int i = 0;
		char *t1;

		int j = 0;
		for (j = 0; DataLine[j] != 0 && DataLine[j] != '\n'; j++)
			DataLine[j] = tolower(DataLine[j]);
		DataLine[j] = 0;

		Classifier *cl;
		assert(cl= new Classifier());
		cl->setNum(1);

		for (t1 = strtok(DataLine, " {,}"); t1 != NULL; t1 = strtok(NULL,
				" {,}"), i++) {
			if (i < Configuration::getCONDITION_LENGTH()) // attributes
			{
				if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) Nominal) {
					xNominalCondition *cond;
					assert(cond = new xNominalCondition());
					map<int, string> convertMap =
							Classifier::attributesConvertMap[i];
					cond->CategoryLength = convertMap.size();
					cond->Category = new bool[cond->CategoryLength];
					for (map<int, string>::const_iterator it =
							convertMap.begin(); it != convertMap.end(); it++)
						if ((*it).second.compare(t1) == 0)
							cond->SetCategoryByID((*it).first);

					cl->ConditionList[i] = cond;

				} else if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) RealNormalized) {
					float fCond;
					xRealNormalizedCondition *cond;
					assert(cond = new xRealNormalizedCondition());
					double centre = atof(t1);
					cond->setCenterValue(centre);
					cond->setRangeValue(0);

					cl->ConditionList[i] = cond;
				} else if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) Real) {
					float fCond;
					xRealCondition *cond;
					assert(cond = new xRealCondition(Classifier::ConditionListLowerBound[i] , Classifier::ConditionListUpperBound[i]));
					double centre = atof(t1);
					cond->setCenterValue(centre);
					cond->setRangeValue(0);

					if (centre < *Classifier::ConditionListLowerBound[i])
						*Classifier::ConditionListLowerBound[i] = centre;
					if (centre > *Classifier::ConditionListUpperBound[i])
						*Classifier::ConditionListUpperBound[i] = centre;

					cl->ConditionList[i] = cond;
				}
			} else // the action part
			if (Classifier::attributesConvertMap.find(
					Configuration::getCONDITION_LENGTH())
					== Classifier::attributesConvertMap.end()) { // no conversion for action part
				int act;
				cl->setAct(atoi(t1));
				//cout << "Class Not Conv" << cl->act << endl;
			} else // Conversion for action part
			{
				map<int, string>
						convertMap =
								Classifier::attributesConvertMap[Configuration::getCONDITION_LENGTH()];
				for (map<int, string>::const_iterator it = convertMap.begin(); it
						!= convertMap.end(); it++) {
					if (string(t1).compare(0, (*it).second.length(),
							(*it).second) == 0) {
						cl->setAct((*it).first);
					}
				}
			}

		}

		if (cl->ConditionList.size() > 0) {
			// make different order for data set which may end with learning the test set in multiple COXCS
			if (frand() > 0.5)
				RawData.push_back(cl);
			else
				RawData.push_front(cl);
		} else
			delete cl;
	}
	fclose(dataFile);

	// Distributed data based on classes
	int foldNo = 0;
	// n list is created due to number of folds
	// all instances be longed to each class is distributed in each fold evenly
	for (int act = 0; act < Configuration::getNUMBER_OF_ACTIONS(); act++) {

		int TrainCounter = TrainNum / Configuration::getNUMBER_OF_ACTIONS();
		int TestCounter = TestNum / Configuration::getNUMBER_OF_ACTIONS();
		for (list<Classifier *>::const_iterator clIterator = RawData.begin(); clIterator
				!= RawData.end(); clIterator++) {
			if ((*clIterator)->getAct() == act) {
				if (TrainCounter > 0) {
					if (frand() > 0.5)
						TrainData->push_back((*clIterator));
					else
						TrainData->push_front((*clIterator));
					TrainCounter--;
				} else if (TestCounter > 0) {
					if (frand() > 0.5)
						TestData->push_back((*clIterator));
					else
						TestData->push_front((*clIterator));
					TestCounter--;
				}
			}
		}
	}

	int NoRows = RawData.size();
	RawData.clear();
	delete[] DataLine;

	return NoRows;
}

int SaveRealNormalizedRawDataToARFFFile(const char *dataFileName, list<
		Classifier *> *dataList, set<int> iIncludedAttributes) {

	FILE *outEvalFile;

	outEvalFile = fopen(dataFileName, "wb");

	if (outEvalFile == NULL) {
		cout << "Error, file Not found" << endl;
		return 0;
	}

	//	for(set<int>::iterator t=iIncludedAttributes.begin(); t != iIncludedAttributes.end() ; t++)
	//		cout << *t << " , " ;

	// creating arff Header file
	fprintf(outEvalFile, "@relation CoXCS_saved_ARFF_DataSet\n");

	// iterates for all retrieved attributes
	for (map<int, string>::const_iterator att =
			Classifier::AttributeNames.begin(); att
			!= Classifier::AttributeNames.end(); att++) {
		unsigned int iAttIndex = att->first;
		// check if the attributes list is free or not/ if not check if the attribtes is selected to be saved or not
		if (iIncludedAttributes.empty() || iIncludedAttributes.find(iAttIndex)
				!= iIncludedAttributes.end()) {
			//				cout << "found " << iAttIndex << " : " << (iIncludedAttributes.find(iAttIndex)
			//						== iIncludedAttributes.end() ) << endl;
			//			continue;

			if (iAttIndex < Configuration::getCONDITION_LENGTH()) {
				if (Classifier::ConditionListTypes[iAttIndex]
						== (ConditionTypes) RealNormalized
						|| Classifier::ConditionListTypes[iAttIndex]
								== (ConditionTypes) Real)

					fprintf(outEvalFile, "@attribute %s %s\n",
							att->second.c_str(), "real");
				else if (Classifier::ConditionListTypes[iAttIndex]
						== (ConditionTypes) Binary)
					fprintf(outEvalFile, "@attribute %s %s\n",
							att->second.c_str(), "{0,1}");
			}
		}
		if (iAttIndex == Configuration::getCONDITION_LENGTH()) // for class attribute
		{

			map<int, string> convert =
					Classifier::attributesConvertMap[iAttIndex];

			fprintf(outEvalFile, "@attribute %s {", att->second.c_str());

			for (map<int, string>::const_iterator iNom = convert.begin(); iNom
					!= convert.end(); iNom++) {
				fprintf(outEvalFile, "%s", iNom->second.c_str());

				if (iNom->first < (convert.size() - 1))
					fprintf(outEvalFile, ",");

				fflush(outEvalFile);
			}
			fprintf(outEvalFile, "}\n");
		}
	}

	fprintf(outEvalFile, "@data\n");

	int iNoRows = 0;
	// iterates for all data lines in dataList
	for (list<Classifier *>::const_iterator clDataLine = dataList->begin(); clDataLine
			!= dataList->end(); clDataLine++) {
		for (map<int, xCondition *>::const_iterator xCond =
				(*clDataLine)->ConditionList.begin(); xCond
				!= (*clDataLine)->ConditionList.end(); xCond++) {
			int iCondIndex = xCond->first;
			if (iIncludedAttributes.empty() || iIncludedAttributes.find(
					iCondIndex) != iIncludedAttributes.end()) {

				if (Classifier::ConditionListTypes[iCondIndex]
						== (ConditionTypes) RealNormalized)
					fprintf(
							outEvalFile,
							"%.20f",
							((xRealNormalizedCondition *) xCond->second)->getCenterValue());
				else if (Classifier::ConditionListTypes[iCondIndex]
						== (ConditionTypes) Real)
					fprintf(outEvalFile, "%.20f",
							((xRealCondition*) xCond->second)->getCenterValue());

				else if (Classifier::ConditionListTypes[iCondIndex]
						== (ConditionTypes) Binary)
					fprintf(outEvalFile, "%d",
							((xBinaryCondition *) xCond->second)->value);

				fprintf(outEvalFile, ",");
			}
		}
		fprintf(outEvalFile, "%d\n", (*clDataLine)->getAct());
		iNoRows++;
	}

	fclose(outEvalFile);

	return iNoRows;
}

// Save Data to file
void SaveDataToFile(char *dataFileName, list<Classifier *> *AllData) {
	FILE *tabFile;

	/*Open files for statistics*/
	tabFile = (FILE *) fopen(dataFileName, "wt");
	if (tabFile == NULL) {
		fprintf(stderr, "Cannot open file");
		fprintf(stderr, dataFileName);
		return;
	}

	list<Classifier*>::const_iterator i;
	for (i = AllData->begin(); i != AllData->end(); i++) {
		map<int, xCondition*>::const_iterator j;
		for (j = (*i)->ConditionList.begin(); j != (*i)->ConditionList.end(); j++) {
			if (((xCondition*) (*j).second)->GetConditionType()
					== RealNormalized)
				fprintf(
						tabFile,
						"%f,",
						((xRealNormalizedCondition *) (*j).second)->getCenterValue());
			else if (((xCondition*) (*j).second)->GetConditionType() == Real)
				fprintf(tabFile, "%.100lf,",
						((xRealCondition *) (*j).second)->getCenterValue());
			else
				fprintf(tabFile, "%d,",
						((xNominalCondition*) (*j).second)->getCategoryID());
		}
		fprintf(tabFile, "%d\n", (*i)->getAct());
	}

	fclose(tabFile);
}

int ReadRealDataListFromFileForAnalysisOfFeatures(const char *dataFileName,
		list<Classifier *> *dataList) {

	char attTag[256], attName[255], attDesc[255], Line[255];

	dataList->clear();
	FILE *dataFile;
	dataFile = fopen(dataFileName, "rb");
	if (dataFile == NULL) {
		fprintf(stderr, "Cannot open file");
		exit(0);
	}

	int attributeNumber = -1;
	// read header of file
	while (fgets(Line, sizeof Line, dataFile) != NULL && (strstr(Line, "@data")
			!= 0))
		;

	int line = 0;
	int sizeOfLine = 256 * (attributeNumber + 1);
	char *DataLine = new char[sizeOfLine];

	while (fgets(DataLine, sizeOfLine, dataFile) != NULL) {
		char *cLastValue;
		line++;

		int act;
		int i = 0;
		char *t1;

		int j = 0;
		for (j = 0; DataLine[j] != 0 && DataLine[j] != '\n'; j++)
			DataLine[j] = tolower(DataLine[j]);
		DataLine[j] = 0;

		Classifier *cl;
		assert(cl= new Classifier());
		cl->setNum(1);

		for (t1 = strtok(DataLine, " {,}"); t1 != NULL; t1 = strtok(NULL,
				" {,}"), i++) {
			if (i < Configuration::getCONDITION_LENGTH()) // attributes
			{
				if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) Nominal) {
					xNominalCondition *cond;
					assert(cond = new xNominalCondition());
					map<int, string> convertMap =
							Classifier::attributesConvertMap[i];
					cond->CategoryLength = convertMap.size();
					cond->Category = new bool[cond->CategoryLength];
					for (map<int, string>::const_iterator it =
							convertMap.begin(); it != convertMap.end(); it++)
						if ((*it).second.compare(t1) == 0)
							cond->SetCategoryByID((*it).first);

					cl->ConditionList[i] = cond;

				} else if (Classifier::ConditionListTypes[i]
						== (ConditionTypes) RealNormalized) {
					//					float fCond;
					//					xRealNormalizedCondition *cond;
					//					assert(cond = new xRealNormalizedCondition());
					//					double centre = atof(t1);
					//					cond->setCenterValue(centre);
					//					cond->setRangeValue(0);
					//
					//					cl->ConditionList[i] = cond;
					//				} else if (Classifier::ConditionListTypes[i]
					//						== (ConditionTypes) Real) {
					float fCond;
					xRealCondition *cond;
					assert(cond = new xRealCondition(Classifier::ConditionListLowerBound[i] , Classifier::ConditionListUpperBound[i]));
					double centre = atof(t1);
					cond->setCenterValue(centre);
					cond->setRangeValue(0);

					if (centre < *Classifier::ConditionListLowerBound[i])
						*Classifier::ConditionListLowerBound[i] = centre;
					if (centre > *Classifier::ConditionListUpperBound[i])
						*Classifier::ConditionListUpperBound[i] = centre;

					cl->ConditionList[i] = cond;
				}
			} else // the action part
			if (Classifier::attributesConvertMap.find(
					Configuration::getCONDITION_LENGTH())
					== Classifier::attributesConvertMap.end()) { // no conversion for action part
				int act;
				cl->setAct(atoi(t1));
				//cout << "Class Not Conv" << cl->act << endl;
			} else // Conversion for action part
			{
				map<int, string>
						convertMap =
								Classifier::attributesConvertMap[Configuration::getCONDITION_LENGTH()];
				for (map<int, string>::const_iterator it = convertMap.begin(); it
						!= convertMap.end(); it++) {
					if (string(t1).compare(0, (*it).second.length(),
							(*it).second) == 0) {
						cl->setAct((*it).first);
					}
				}
			}

		}

		if (cl->ConditionList.size() > 0) {
			// make different order for data set which may end with learning the test set in multiple COXCS
			//			if (frand() > 0.5)
			dataList->push_back(cl);
			//			else
			//				RawData.push_front(cl);
		} else
			delete cl;
	}
	fclose(dataFile);

	return dataList->size();
}
void FreeDataList(list<Classifier *> *DataList) {
	list<Classifier*>::const_iterator i;
	for (i = DataList->begin(); i != DataList->end(); i++) {
		//		for (int j = 0; j < (*i)->ConditionList.size() ; j++)
		//			if ((*i)->ConditionList[j] != NULL )
		//				delete (*i)->ConditionList[j];

		delete (*i);
	}
}

/* get the abs value of value */
long double absLongDouble(long double value) {
	if (value >= 0.0)
		return value;
	else
		return value * (-1.0);
}

/* get the abs value of value */
float absFloat(float value) {
	if (value >= 0.0)
		return value;
	else
		return value * (-1.0);
}
//
//long int get_SystemFreeMem(void) {
//	long int retval = 0;
//	char tmp[256] = { 0x0 };
//	/* note= add a path to meminfo like /usr/bin/meminfo
//	 to match where meminfo lives on your system */
//	FILE *shellcommand = popen("free -m", "r");
//	while (fgets(tmp, sizeof(tmp), shellcommand) != NULL) {
//		if (memcmp(tmp, "Mem:", 4) == 0) {
//			int wordcount = 0;
//			char *delimiter = " ";
//			char *p = strtok(tmp, delimiter);
//			while (*p) {
//				if (wordcount == 3) {
//					retval = atol(p);
//					pclose(shellcommand);
//					return retval;
//				}
//				wordcount++;
//				p = strtok(NULL, delimiter);
//			}
//		}
//	}
//	pclose(shellcommand);
//	return retval;
//}
//
//long int get_SystemUsedMem(void) {
//	long int retval = 0;
//	char tmp[256] = { 0x0 };
//	/* note= add a path to meminfo like /usr/bin/meminfo
//	 to match where meminfo lives on your system */
//	FILE *shellcommand = popen("free -m", "r");
//	while (fgets(tmp, sizeof(tmp), shellcommand) != NULL) {
//		if (memcmp(tmp, "Mem:", 4) == 0) {
//			int wordcount = 0;
//			char *delimiter = " ";
//			char *p = strtok(tmp, delimiter);
//			while (*p) {
//				if (wordcount == 2) {
//					retval = atol(p);
//					pclose(shellcommand);
//					return retval;
//				}
//				wordcount++;
//				p = strtok(NULL, delimiter);
//			}
//		}
//	}
//	pclose(shellcommand);
//	return retval;
//}
//

const char * get_CurrentProcessUsedMem(void) {
	long int retval = 0;
	char tmp[85] = { 0x0 };
	pid_t pid = getpid();
	string data = "";

	/* note= add a path to meminfo like /usr/bin/meminfo
	 to match where meminfo lives on your system */

	sprintf(tmp, "pmap %ld", (long int) pid);

	FILE *shellcommand = popen(tmp, "r");
	while (fgets(tmp, sizeof(tmp), shellcommand) != NULL) {
		if (memcmp(tmp, " total", 6) == 0) {
			int wordcount = 0;
			char *delimiter = " ";
			char *p = strtok(tmp, delimiter);
			while (*p) {
				if (wordcount == 1) {
					data.append(p);
					break;
				}
				wordcount++;
				p = strtok(NULL, delimiter);
			}
		}
	}
	pclose(shellcommand);
	return data.c_str();
}

bool CalculateAUC(double *AUC, double *SE, double *AVGAUC, double *AVGSE,
		int normalRated[], int AbNormalRated[]) {
	double W = 0, Q1 = 0, Q2 = 0;
	int abNormalTotal = 0, normalTotal = 0;

	for (int i = 0; i < Configuration::getNBR_RATING_CATEGORY(); i++) {
		abNormalTotal += AbNormalRated[i];
		normalTotal += normalRated[i];
	}

	if (abNormalTotal <= 0 || normalTotal <= 0)
		return false; // AUC can not calculated due to devide by 0

	double R4, R2;
	double R1 = 0, R3 = 0;
	R2 = abNormalTotal;
	R4 = 0;
	for (int C = 0; C < Configuration::getNBR_RATING_CATEGORY(); C++) {
		R4 += R1;
		R1 = normalRated[C];
		R3 = AbNormalRated[C];
		R2 -= R3;

		W += (R1 * R2) + 0.5 * (R1 * R3); // ( 1 * 2)+ 0.5 ( 1 * 3)
		Q2 += R3 * (R4 * R4 + R4 * R1 + 1 / 3 * R1 * R1);
		Q1 += R1 * (R2 * R2 + R2 * R3 + 1 / 3 * R3 * R3);
	}
	W = W / (abNormalTotal * normalTotal);
	Q2 = Q2 / (abNormalTotal * normalTotal * normalTotal);
	Q1 = Q1 / (abNormalTotal * abNormalTotal * normalTotal);

	*AUC = (double) W;
	*SE
			= (double) sqrt((W * (1 - W) + (abNormalTotal - 1) * (Q1 - W * W)
					+ (normalTotal - 1) * (Q2 - W * W)) / (abNormalTotal
					* normalTotal));

	W = 0;
	R1 = 0, R3 = 0;
	R2 = abNormalTotal;
	R4 = 0;
	R4 += R1;
	R1 = normalRated[0] + normalRated[1];
	R3 = AbNormalRated[0];
	R2 -= R3;
	W += (R1 * R2) + 0.5 * (R1 * R3); // ( 1 * 2)+ 0.5 ( 1 * 3)
	R4 += R1;
	R1 = normalRated[2];
	R3 = AbNormalRated[2] + AbNormalRated[1];
	R2 -= R3;
	W += (R1 * R2) + 0.5 * (R1 * R3); // ( 1 * 2)+ 0.5 ( 1 * 3)

	double BestW = W / (abNormalTotal * normalTotal);

	W = 0;
	R1 = 0, R3 = 0;
	R2 = abNormalTotal;
	R4 = 0;
	R4 += R1;
	R1 = normalRated[0];
	R3 = AbNormalRated[0] + AbNormalRated[1];
	R2 -= R3;
	W += (R1 * R2) + 0.5 * (R1 * R3); // ( 1 * 2)+ 0.5 ( 1 * 3)
	R4 += R1;
	R1 = normalRated[2] + normalRated[1];
	R3 = AbNormalRated[2];
	R2 -= R3;
	W += (R1 * R2) + 0.5 * (R1 * R3); // ( 1 * 2)+ 0.5 ( 1 * 3)

	double WorstW = W / (abNormalTotal * normalTotal);

	*AVGSE = (BestW - WorstW) / 2.0;
	*AVGAUC = (BestW + WorstW) / 2.0;

	return true;
}

int calculateAUCByVoting(double *AUC, double *SE, double *AVGAUC,
		double *AVGSE, map<int, int*> &predictionMap, map<int, int> &ActionMap) {
	//	int nbrCategoryRates = Configuration::getNBR_RATING_CATEGORY();
	//int nbrCategoryRates = 5;
	int *normalRated = new int[Configuration::getNBR_RATING_CATEGORY()];
	int *AbNormalRated = new int[Configuration::getNBR_RATING_CATEGORY()];
	int rate = 0;
	int correct = 0;
	int notRecog = 0;
	// initializing variables
	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		AUC[i] = 0;
		SE[i] = 0;
		AVGAUC[i] = 0;
		AVGSE[i] = 0;
	}

	// for each actions, once it will be assumed that is positive and others are negative
	for (int NomClass = 0; NomClass < Configuration::getNUMBER_OF_ACTIONS(); NomClass++) {
		for (int i = 0; i < Configuration::getNBR_RATING_CATEGORY(); i++) {
			normalRated[i] = 0;
			AbNormalRated[i] = 0;
		}
		correct = 0;
		notRecog = 0;

		for (unsigned int idxSample = 0; idxSample < predictionMap.size(); idxSample++) {
			int ActVoted = 0, maxVoteValue = 0, NotRec = 0, prevMaxVoteValue =
					0, totalVotes = 0;
			for (int idxAct = 0; idxAct < Configuration::getNUMBER_OF_ACTIONS(); idxAct++) {
				totalVotes += predictionMap[idxSample][idxAct];
				if (maxVoteValue == predictionMap[idxSample][idxAct])
					NotRec = 1;
				else if (maxVoteValue < predictionMap[idxSample][idxAct]) {
					prevMaxVoteValue = maxVoteValue;
					maxVoteValue = predictionMap[idxSample][idxAct];
					ActVoted = idxAct;
					NotRec = 0;
				} else if (prevMaxVoteValue < predictionMap[idxSample][idxAct]) {
					prevMaxVoteValue = predictionMap[idxSample][idxAct];
				}
			}

			int action = ActionMap[idxSample];
			if (action == ActVoted)
				correct++;

			if (NotRec == 0) // recognize
			{
				if (NomClass == ActVoted) // Normal rate
				{
					if (maxVoteValue > (totalVotes / 80))
						rate = 0;
					else if (maxVoteValue > (totalVotes / 60))
						rate = 1;
					else
						rate = 2;
				} else {
					if (maxVoteValue > (totalVotes / 80))
						rate = 4;
					else if (maxVoteValue > (totalVotes / 60))
						rate = 3;
					else
						rate = 2;
				}

				if (NomClass == action)
					normalRated[rate]++;
				else
					AbNormalRated[rate]++;
			} else { // Not recognized
				int NonRecRate = Configuration::getNBR_RATING_CATEGORY() / 2;
				if (NomClass == action)
					normalRated[NonRecRate]++;
				else
					AbNormalRated[NonRecRate]++;

				notRecog++;
			}
		}
		//		cout << "Nr[0]:" << normalRated[0] << "Nr[1]:" << normalRated[1]
		//				<< "Nr[2]:" << normalRated[2] << "Nr[3]:" << normalRated[3]
		//				<< "Nr[4]:" << normalRated[4] << endl;
		//		cout << "abNr[0]:" << AbNormalRated[0] << "abNr[1]:"
		//				<< AbNormalRated[1] << "abNr[2]:" << AbNormalRated[2]
		//				<< "abNr[3]:" << AbNormalRated[3] << "abNr[4]:"
		//				<< AbNormalRated[4] << endl;
		CalculateAUC(&AUC[NomClass], &SE[NomClass], &AVGAUC[NomClass],
				&AVGSE[NomClass], normalRated, AbNormalRated);
	}
	//	cout << "C:" << correct << " NotRec:" << notRecog << " of "
	//			<< ActionMap.size() << " AUC:" << AUC[0] << endl;

	delete[] normalRated;
	delete[] AbNormalRated;
	return notRecog;
}

//long _Q = _M/_A;     /* M / A */
//long _R = _M%_A;     /* M mod A */
//long _seed;          /* a number between 1 and m-1 */
//int haveUniNum=0;
//double uniNum=0;
//
///* returns a floating-point random number generated according to uniform distribution from [0,1) */
//double urand()
//{
//  long lo,hi,test;
//
//  hi   = _seed / _Q;
//  lo   = _seed % _Q;
//  test = _A*lo - _R*hi;
//
//  if (test>0)
//    _seed = test;
//  else
//    _seed = test+_M;
//
//  return (double)(_seed)/_M;
//}
//
///* returns a floating-point random number generated according to a normal distribution with
// * mean 0 and standard deviation 1 */
//double nrand()
//{
//
//  float x1, x2, w;
//
//  if(haveUniNum){
//    haveUniNum=0;
//    return uniNum;
//  }else{
//    do {
//      x1 = 2.0 * urand() - 1.0;
//      x2 = 2.0 * urand() - 1.0;
//      w = x1 * x1 + x2 * x2;
//    } while ( w >= 1.0 );
//
//    w = sqrt( (-2.0 * log( w ) ) / w );
//    uniNum = x1 * w;
//    haveUniNum=1;
//    return x2 * w;
//  }
//
//}
//
///* sets the random seed */
//long  setSeed(long newSeed)
//{
//  return (_seed = newSeed);
//}
//
///**
// * randomizes the pseudo random generator
// */
//void randomizeAtr(void)
//{
//  setSeed((long)(time(NULL)%10000+1));
//}
//


/**  Correlation matrix: creation  ***********************************/
void corcol(double **data, int n, int m, double ** symmat)
/* Create m * m correlation matrix from given n * m data matrix. */
{
	double eps = 0.005;
	double x, *mean, *stddev;
	int i, j, j1, j2;

	/* Allocate storage for mean and std. dev. vectors */

	mean = Ovector(m);
	stddev = Ovector(m);

	/* Determine mean of column vectors of input data matrix */

	for (j = 1; j <= m; j++) {
		mean[j] = 0.0;
		for (i = 1; i <= n; i++) {
			mean[j] += data[i][j];
		}
		mean[j] /= (double) n;
	}

	printf("\nMeans of column vectors:\n");
	for (j = 1; j <= m; j++) {
		printf("%7.1f", mean[j]);
	}
	printf("\n");

	/* Determine standard deviations of column vectors of data matrix. */

	for (j = 1; j <= m; j++) {
		stddev[j] = 0.0;
		for (i = 1; i <= n; i++) {
			stddev[j] += ((data[i][j] - mean[j]) * (data[i][j] - mean[j]));
		}
		stddev[j] /= (double) n;
		stddev[j] = sqrt(stddev[j]);
		/* The following in an inelegant but usual way to handle
		 near-zero std. dev. values, which below would cause a zero-
		 divide. */
		if (stddev[j] <= eps)
			stddev[j] = 1.0;
	}

	printf("\nStandard deviations of columns:\n");
	for (j = 1; j <= m; j++) {
		printf("%7.1f", stddev[j]);
	}
	printf("\n");

	/* Center and reduce the column vectors. */

	for (i = 1; i <= n; i++) {
		for (j = 1; j <= m; j++) {
			data[i][j] -= mean[j];
			x = sqrt((double) n);
			x *= stddev[j];
			data[i][j] /= x;
		}
	}

	/* Calculate the m * m correlation matrix. */
	for (j1 = 1; j1 <= m - 1; j1++) {
		symmat[j1][j1] = 1.0;
		for (j2 = j1 + 1; j2 <= m; j2++) {
			symmat[j1][j2] = 0.0;
			for (i = 1; i <= n; i++) {
				symmat[j1][j2] += (data[i][j1] * data[i][j2]);
			}
			symmat[j2][j1] = symmat[j1][j2];
		}
	}
	symmat[m][m] = 1.0;

	return;

}

/**  Variance-covariance matrix: creation  *****************************/

void covcol(double **data, int n, int m, double **symmat)
/* Create m * m covariance matrix from given n * m data matrix. */
{
	double *mean;
	int i, j, j1, j2;

	/* Allocate storage for mean vector */

	mean = Ovector(m);

	/* Determine mean of column vectors of input data matrix */

	for (j = 1; j <= m; j++) {
		mean[j] = 0.0;
		for (i = 1; i <= n; i++) {
			mean[j] += data[i][j];
		}
		mean[j] /= (double) n;
	}

	printf("\nMeans of column vectors:\n");
	for (j = 1; j <= m; j++) {
		printf("%7.1f", mean[j]);
	}
	printf("\n");

	/* Center the column vectors. */

	for (i = 1; i <= n; i++) {
		for (j = 1; j <= m; j++) {
			data[i][j] -= mean[j];
		}
	}

	/* Calculate the m * m covariance matrix. */
	for (j1 = 1; j1 <= m; j1++) {
		for (j2 = j1; j2 <= m; j2++) {
			symmat[j1][j2] = 0.0;
			for (i = 1; i <= n; i++) {
				symmat[j1][j2] += data[i][j1] * data[i][j2];
			}
			symmat[j2][j1] = symmat[j1][j2];
		}
	}

	return;

}

/**  Sums-of-squares-and-cross-products matrix: creation  **************/

void scpcol(double **data, int n, int m, double **symmat)
/* Create m * m sums-of-cross-products matrix from n * m data matrix. */
{
	int i, j1, j2;

	/* Calculate the m * m sums-of-squares-and-cross-products matrix. */

	for (j1 = 1; j1 <= m; j1++) {
		for (j2 = j1; j2 <= m; j2++) {
			symmat[j1][j2] = 0.0;
			for (i = 1; i <= n; i++) {
				symmat[j1][j2] += data[i][j1] * data[i][j2];
			}
			symmat[j2][j1] = symmat[j1][j2];
		}
	}

	return;

}

/**  Error handler  **************************************************/

void erhand(char err_msg[])
/* Error handler */
{
	fprintf(stderr, "Run-time error:\n");
	fprintf(stderr, "%s\n", err_msg);
	fprintf(stderr, "Exiting to system.\n");
	exit(1);
}

/**  Allocation of vector storage  ***********************************/

double *Ovector(int n)
/* Allocates a double vector with range [1..n]. */
{

	double *v;

	v = (double *) malloc((unsigned) n * sizeof(double));
	if (!v)
		erhand("Allocation failure in vector().");
	return v - 1;

}

/**  Allocation of double matrix storage  *****************************/

double **Omatrix(int n, int m)
/* Allocate a double matrix with range [1..n][1..m]. */
{
	int i;
	double **mat;

	/* Allocate pointers to rows. */
	mat = (double **) malloc((unsigned) (n) * sizeof(double*));
	if (!mat) {
		erhand("Allocation failure 1 in matrix().");
		mat -= 1;
	}

	/* Allocate rows and set pointers to them. */
	for (i = 1; i <= n; i++) {
		mat[i] = (double *) malloc((unsigned) (m) * sizeof(double));
		if (!mat[i]) {
			erhand("Allocation failure 2 in matrix().");
			mat[i] -= 1;
		}
	}

	/* Return pointer to array of pointers to rows. */
	return mat;

}

/**  access of double matrix storage  *****************************/

double accessGetMatrix(double ** mat, int i, int j)
/* access a double matrix with range [1..n][1..m]. */
{
	double * row = mat[i];
	return (double) (*(row + j));
}

void accessSetMatrix(double ** mat, int i, int j, double value)
/* access a double matrix with range [1..n][1..m]. */
{
	double * row = mat[i];
	*(row + j) = value;
}

/**  Deallocate vector storage  *********************************/

void free_vector(double *v, int n)
/* Free a double vector allocated by vector(). */
{
	free((char*) (v + 1));
}

/**  Deallocate double matrix storage  ***************************/

void free_matrix(double **mat, int n, int m)
/* Free a double matrix allocated by matrix(). */
{
	int i;

	for (i = n; i >= 1; i--) {
		free((char*) (mat[i] + 1));
	}
	free((char*) (mat + 1));
}

/**  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */

void tred2(double **a, int n, double *d, double *e)
/* Householder reduction of matrix a to tridiagonal form.
 Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
 Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
 Springer-Verlag, 1976, pp. 489-494.
 W H Press et al., Numerical Recipes in C, Cambridge U P,
 1988, pp. 373-374.  */
{
	int l, k, j, i;
	double scale, hh, h, g, f;

	for (i = n; i >= 2; i--) {
		l = i - 1;
		h = scale = 0.0;
		if (l > 1) {
			for (k = 1; k <= l; k++)
				scale += fabs(a[i][k]);
			if (scale == 0.0)
				e[i] = a[i][l];
			else {
				for (k = 1; k <= l; k++) {
					a[i][k] /= scale;
					h += a[i][k] * a[i][k];
				}
				f = a[i][l];
				g = f > 0 ? -sqrt(h) : sqrt(h);
				e[i] = scale * g;
				h -= f * g;
				a[i][l] = f - g;
				f = 0.0;
				for (j = 1; j <= l; j++) {
					a[j][i] = a[i][j] / h;
					g = 0.0;
					for (k = 1; k <= j; k++)
						g += a[j][k] * a[i][k];
					for (k = j + 1; k <= l; k++)
						g += a[k][j] * a[i][k];
					e[j] = g / h;
					f += e[j] * a[i][j];
				}
				hh = f / (h + h);
				for (j = 1; j <= l; j++) {
					f = a[i][j];
					e[j] = g = e[j] - hh * f;
					for (k = 1; k <= j; k++)
						a[j][k] -= (f * e[k] + g * a[i][k]);
				}
			}
		} else
			e[i] = a[i][l];
		d[i] = h;
	}
	d[1] = 0.0;
	e[1] = 0.0;
	for (i = 1; i <= n; i++) {
		l = i - 1;
		if (d[i]) {
			for (j = 1; j <= l; j++) {
				g = 0.0;
				for (k = 1; k <= l; k++)
					g += a[i][k] * a[k][j];
				for (k = 1; k <= l; k++)
					a[k][j] -= g * a[k][i];
			}
		}
		d[i] = a[i][i];
		a[i][i] = 1.0;
		for (j = 1; j <= l; j++)
			a[j][i] = a[i][j] = 0.0;
	}
}

/**  Tridiagonal QL algorithm -- Implicit  **********************/

void tqli(double d[], double e[], int n, double **z) {
	int m, l, iter, i, k;
	double s, r, p, g, f, dd, c, b;

	for (i = 2; i <= n; i++)
		e[i - 1] = e[i];
	e[n] = 0.0;
	for (l = 1; l <= n; l++) {
		iter = 0;
		do {
			for (m = l; m <= n - 1; m++) {
				dd = fabs(d[m]) + fabs(d[m + 1]);
				if (fabs(e[m]) + dd == dd)
					break;
			}
			if (m != l) {
				if (iter++ == 30)
					erhand("No convergence in TLQI.");
				g = (d[l + 1] - d[l]) / (2.0 * e[l]);
				r = sqrt((g * g) + 1.0);
				g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
				s = c = 1.0;
				p = 0.0;
				for (i = m - 1; i >= l; i--) {
					f = s * e[i];
					b = c * e[i];
					if (fabs(f) >= fabs(g)) {
						c = g / f;
						r = sqrt((c * c) + 1.0);
						e[i + 1] = f * r;
						c *= (s = 1.0 / r);
					} else {
						s = f / g;
						r = sqrt((s * s) + 1.0);
						e[i + 1] = g * r;
						s *= (c = 1.0 / r);
					}
					g = d[i + 1] - p;
					r = (d[i] - g) * s + 2.0 * c * b;
					p = s * r;
					d[i + 1] = g + p;
					g = c * r - b;
					for (k = 1; k <= n; k++) {
						f = z[k][i + 1];
						z[k][i + 1] = s * z[k][i] + c * f;
						z[k][i] = c * z[k][i] - s * f;
					}
				}
				d[l] = d[l] - p;
				e[l] = g;
				e[m] = 0.0;
			}
		} while (m != l);
	}
}

int calculateBACBinaryByVoting(map<int, int*> &predictionMap,
		map<int, int> &ActionMap, double &BAC) {

	// initializing variables
	int NotRecognized = 0;
	int TP = 0, TN = 0, FP = 0, FN = 0;
	int pos = 0, neg = 0;

	for (unsigned int idxSample = 0; idxSample < predictionMap.size(); idxSample++) {
		int ActVoted = -1;
		int action = ActionMap[idxSample];

		if (predictionMap[idxSample][0] == predictionMap[idxSample][1]) {
			NotRecognized++;
			//			if (frand() > 0.5)
			//				FN++;
			//			else
			//				FP++;
		} else {
			if (predictionMap[idxSample][0] > predictionMap[idxSample][1])
				ActVoted = 0;
			else
				ActVoted = 1;

			cout << predictionMap[idxSample][0] << " : " << predictionMap[idxSample][1] << endl;
		}

		if (action == 0) {
			if (ActVoted == 0)
				TN++;
			else if (ActVoted == 1)
				FP++;

		} else // action ==1
		{
			if (ActVoted == 1)
				TP++;
			else if (ActVoted == 0)
				FN++;
		}

		if (action == 0) {
			neg++;
		} else // action ==1
		{
			pos++;
		}

	}

	//	double Sensiticity = (double)TP/(TP+FN);
	//	double Specificity = (double)TN/(TN+FP);
	double Sensiticity = (double) TP / (pos);
	double Specificity = (double) TN / (neg);

	//BAC = 0;
	BAC = (Sensiticity + Specificity)/2.0;

//	if ((TP + FN) > 0 && (TN + FP) > 0) {
//		BAC += 0.5 * Specificity * (1 - Sensiticity);
//		BAC += 0.5 * Sensiticity * (1 - Specificity);
//		BAC += Sensiticity * Specificity;
//	}

	cout <<" T:" << TP << " TN:"<< TN << " SE:" << Sensiticity << " SP:" << Specificity << endl;

	return NotRecognized;
}

int getNumberOfCPUs() {
	long nprocs = -1;
	long nprocs_max = -1;
#ifdef _WIN32
#ifndef _SC_NPROCESSORS_ONLN
	SYSTEM_INFO info;
	GetSystemInfo(&info);
#define sysconf(a) info.dwNumberOfProcessors
#define _SC_NPROCESSORS_ONLN
#endif
#endif
#ifdef _SC_NPROCESSORS_ONLN
	nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	if (nprocs < 1) {
		fprintf(stderr, "Could not determine number of CPUs online:\n%s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
	if (nprocs_max < 1) {
		fprintf(stderr, "Could not determine number of CPUs configured:\n%s\n",
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("%ld of %ld processors online\n", nprocs, nprocs_max);
	exit(EXIT_SUCCESS);
#else
	fprintf(stderr, "Could not determine number of CPUs");
	exit (EXIT_FAILURE);
#endif

	return nprocs;
}

void MigrateBtwnPops(vector<int> *MergingIslandsList) {

	system(" rm -f *.popImprt ");
	system(" rm -f *.popMerge ");

	if (MergingIslandsList->size() < 1)
		return;

	char portstring[256];
	//string classifier;
	char *classifier = new char[Configuration::getCONDITION_LENGTH() * 256];

	//for(int i=0;i<Configuration::getNR_DIFFERENT_ISLANDS();i++)
	for (int i = 0; i < MergingIslandsList->size(); i++) {
		vector<string> pop;

		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];

		//sprintf(portstring, "BestPopIsland_%d_Exp%d_TotalExp%d.pop", sourcePopInx , expCounter , overalExp);
		sprintf(portstring, "PopIsland_%d.pop", sourcePopInx);
		FILE *sourceFile;
		sourceFile = fopen(portstring, "rb");
		if (sourceFile == NULL) {
			sprintf(portstring, "PopIsland_%d.pop", sourcePopInx);
			sourceFile = fopen(portstring, "rb");
			if (sourceFile == NULL) {
				cout << "Error 1, file Not found" << portstring << endl;
				continue;
			}
		}

		while (feof(sourceFile) < 1) {
			fscanf(sourceFile, "%s\n", classifier);
			//string news=new string();
			pop.push_back(string(classifier));
		}
		fclose(sourceFile);

		//cout << " read done:"<< portstring <<endl;
		int popSize = pop.size();
		int nbrMigrationIndividual = (int) (popSize
				* Configuration::getPORTION_OF_MIGRATABLE_INDIVIDUAL());
		for (int j = 0; j < nbrMigrationIndividual && pop.size() > 0; j++) {
			//char *classifier;
			int selectedInx;
			popSize = pop.size();
			selectedInx = (int) (frand() * popSize);
			string classifier = pop[selectedInx];
			int desPopInx = 0;
			if (MergingIslandsList->size() < 4) {
				desPopInx = (i + 1) % MergingIslandsList->size();
				desPopInx = ((vector<int> ) *MergingIslandsList)[desPopInx];
			} else {
				do {
					//desPopInx= (int)( frand()*Configuration::getNR_DIFFERENT_ISLANDS());
					desPopInx = (int) (frand() * MergingIslandsList->size());
					desPopInx = ((vector<int> ) *MergingIslandsList)[desPopInx];
				} while (desPopInx == sourcePopInx);
			}

			sprintf(portstring, "PopIsland_%d.popImprt", desPopInx);
			//sprintf(portstring, "PopIsland%d.txt", desPopInx);
			FILE *destFile;
			destFile = fopen(portstring, "ab");
			if (destFile == NULL) {
				cout << "Error 2, file Not found" << portstring << endl;
				continue;
			}
			fprintf(destFile, "%s\n", classifier.c_str());
			fclose(destFile);
			pop.erase(pop.begin() + selectedInx);
		}

		//for(vector<string>::iterator i=pop.begin();i!=pop.end();i++)
		//delete (*i);
		pop.clear();
	}

	for (int i = 0; i < MergingIslandsList->size(); i++) {
		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];
		sprintf(
				portstring,
				"cat PopIsland_%d.pop PopIsland_%d.popImprt > PopIsland_%d.popMerge",
				sourcePopInx, sourcePopInx, sourcePopInx);
		system(portstring);
		sprintf(portstring, "rm PopIsland_%d.pop", sourcePopInx);
		system(portstring);
		sprintf(portstring, "mv PopIsland_%d.popMerge PopIsland_%d.pop",
				sourcePopInx, sourcePopInx);
		//sprintf(portstring, "mv PopIsland_%d.popImprt PopIsland_%d.pop", sourcePopInx, sourcePopInx);
		system(portstring);
	}

	delete[] classifier;

}

void BestMigrateBtwnPops(vector<int> *MergingIslandsList) {
	system(" rm -f *.popImprt ");
	system(" rm -f *.popMerge ");

	if (MergingIslandsList->size() < 1)
		return;

	char portstring[256];

	for (int i = 0; i < MergingIslandsList->size(); i++) {
		list<Classifier*> pop;

		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];

		//sprintf(portstring, "BestPopIsland_%d_Exp%d_TotalExp%d.pop", sourcePopInx , expCounter , overalExp);
		sprintf(portstring, "PopIsland_%d.pop", sourcePopInx);
		FILE *sourceFile;
		sourceFile = fopen(portstring, "rb");
		if (sourceFile == NULL) {
			sprintf(portstring, "PopIsland_%d.pop", sourcePopInx);
			sourceFile = fopen(portstring, "rb");
			if (sourceFile == NULL) {
				cout << "Error 1, file Not found" << portstring << endl;
				continue;
			}
		}

		while (feof(sourceFile) < 1) {
			Classifier *cl = new Classifier();
			cl->loadClassifier(sourceFile);

			pop.push_back(cl);
		}
		fclose(sourceFile);

		int nbrOfMigratingCl = pop.size()
				* Configuration::getPORTION_OF_MIGRATABLE_INDIVIDUAL();

		// sort the classifier list (population)
		pop.sort(compare_classifiers_Acc);

		list<Classifier*>::iterator iter;
		for (iter = pop.begin(); iter != pop.end() && nbrOfMigratingCl > 0; iter++, nbrOfMigratingCl--) {
			int desPopInx = 0;

			if (MergingIslandsList->size() < 4) {
				desPopInx = (i + 1) % MergingIslandsList->size();
				desPopInx = ((vector<int> ) *MergingIslandsList)[desPopInx];
			} else {
				do {
					desPopInx = (int) (frand() * MergingIslandsList->size());
					desPopInx = ((vector<int> ) *MergingIslandsList)[desPopInx];
				} while (desPopInx == sourcePopInx);
			}

			sprintf(portstring, "PopIsland_%d.popImprt", desPopInx);
			FILE *destFile;
			destFile = fopen(portstring, "ab");
			if (destFile == NULL) {
				cout << "Error 2, file Not found" << portstring << endl;
				continue;
			}

			((Classifier*) *iter)->setNum(1);
			//			((Classifier*) *iter)->setFit(((Classifier*) *iter)->getFit()
			//					* Configuration::getFITNESS_REDUCTION()); /* reduce fitness of new classifiers */

			((Classifier*) *iter)->dumpClassifier(destFile);
			fclose(destFile);
		}

		for (iter = pop.begin(); iter != pop.end(); iter++)
			delete *iter;
		pop.clear();
	}

	for (int i = 0; i < MergingIslandsList->size(); i++) {
		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];
		sprintf(portstring, "cat PopIsland_%d.popImprt  >> PopIsland_%d.pop ",
				sourcePopInx, sourcePopInx, sourcePopInx);
		system(portstring);

		//		sprintf(portstring, "cat PopIsland_%d.pop PopIsland_%d.popImprt > PopIsland_%d.popMerge", sourcePopInx, sourcePopInx, sourcePopInx);
		//		system(portstring);
		//		sprintf(portstring, "rm PopIsland_%d.pop", sourcePopInx);
		//		system(portstring);
		////		sprintf(portstring, "mv PopIsland_%d.popMerge PopIsland_%d.pop", sourcePopInx, sourcePopInx);
		//		sprintf(portstring, "mv PopIsland_%d.popImprt PopIsland_%d.pop", sourcePopInx, sourcePopInx);
		//		system(portstring);
	}

}

// comparison, two classifiers based on acc values
bool compare_classifiers_Acc(Classifier *first, Classifier *second) {
	// It could be Multi Objective based Migration
	bool ret = false;

	if ((first->getFit() * first->getAcc()) > (second->getFit()
			* second->getAcc()))
		ret = true;
	else if ((first->getFit() * first->getAcc()) == (second->getFit()
			* second->getAcc())) {
		if (first->ConditionList.size() > second->ConditionList.size())
			ret = true;
		//			else if (first->ConditionList.size() == second->ConditionList.size() )
		//			{
		//				if (first->getActionSet() >  second->getActionSet() )
		//					ret = true;
		//				else
		ret = false;
		//			}
	} else
		ret = false;

	//	if (first->acc > second->acc)
	//		ret = true;
	//	else if (first->acc == second->acc)
	//	{
	//		if (first->ConditionList.size() > second->ConditionList.size() )
	//			ret = true;
	//		else if (first->ConditionList.size() == second->ConditionList.size() )
	//		{
	//			if (first->actionsest >  second->actionsest  )
	//				ret = true;
	//			else
	//				ret = ((int)(frand()*100))%2 == 0 ;
	//		}
	//	}

	//	if (first->getExp() > 1 && second->getExp() > 1) {
	//		if (first->getExpNegative() == 0 && second->getExpNegative() == 0) {
	//			if (first->getExpPositive() > second->getExpPositive())
	//				return true;
	//			else if (first->getExpPositive() == second->getExpPositive()) {
	//				if (first->getNum() > second->getNum())
	//					return true;
	//			} else
	//				return false;
	//		} else if (first->getExpNegative() != 0 && second->getExpNegative() != 0) { // both have more than one Exp and both have negative exp as well
	//			if (first->getAcc() > second->getAcc())
	//				return true;
	//			else if (first->getAcc() < second->getAcc())
	//				return false;
	//
	//			if (first->getExpPositive() > second->getExpPositive())
	//				return true;
	//			else if (first->getExpPositive() < second->getExpPositive())
	//				return false;
	//
	//			if (first->getNum() > second->getNum())
	//				return true;
	//			else if (first->getNum() < second->getNum())
	//				return false;
	//
	//			if (first->ConditionList.size() > second->ConditionList.size())
	//				return true;
	//			else if (first->ConditionList.size() < second->ConditionList.size())
	//				return false;
	//
	//			if (first->getActionSet() > second->getActionSet() )
	//				return true;
	//			else if (first->getActionSet() < second->getActionSet() )
	//				return false;
	//
	//		} else // both have more than one Exp but one of them has negative exp as well
	//		{
	//			if (first->getExpNegative() == 0)
	//				return true;
	//			else
	//				return false;
	//		}
	//	} else if (first->getExp() == 1 && second->getExp() == 1) { // both have one Exp
	//		// Do nothing based on random
	//	} else { // One has one expe but other has more than one
	//		if (first->getExp() > second->getExp())
	//			return true;
	//		else
	//			return false;
	//	}

	return ret;
	//return ((int) (frand() * 100)) % 2 == 0;
}

void DecisionBasedOnVoting(char * strFormat, int iFold, int &iNbrNonRecognized,
		double &BestAUC) {
	int nbrData = 0;
	// recognizing number of data available
	FILE *vFile;

	/*Open files for statistics*/
	char strFileName[256];
	int tmpNonRec;

	map<int, int*> votes;
	map<int, int> ActualClass;

	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++) {
		/* start the experiments */
		FILE *dataFile;

		/*Open files for statistics*/
		char portstring[256];
		sprintf(portstring, strFormat, i);
		dataFile = (FILE *) fopen(portstring, "rt");

		if (dataFile == NULL) {
			fprintf(stderr, "Cannot open Decision file(%s)\n", portstring);
			return;
		}

		unsigned int iSampleNo = 0;
		for (iSampleNo = 0; !feof(dataFile); iSampleNo++) {
			int action, pAction, LineNo;
			fscanf(dataFile, "Sample #%d\t%d\t%d\n", &LineNo, &pAction, &action);

			if (i == 0) // create the Votes container
			{
				votes[iSampleNo]
						= new int[Configuration::getNUMBER_OF_ACTIONS()];
				for (unsigned int iAction = 0; iAction
						< Configuration::getNUMBER_OF_ACTIONS(); iAction++)
					votes[iSampleNo][iAction] = 0;

				ActualClass[iSampleNo] = action;
			}

			if (pAction >= 0) // if -1 means not recognized
				votes[iSampleNo][pAction]++;

		}
		fclose(dataFile);

		iNbrNonRecognized = iSampleNo;
	}

	//	double AUC[Configuration::getNUMBER_OF_ACTIONS()],SE[Configuration::getNUMBER_OF_ACTIONS()];
	//	double AVGAUC[Configuration::getNUMBER_OF_ACTIONS()],AVGSE[Configuration::getNUMBER_OF_ACTIONS()];
	//	tmpNonRec= calculateAUCByVoting(AUC,SE,AVGAUC,AVGSE, votes, ActualClass);
	//
	//	BestAUC=0;
	//	for(unsigned int c=0;c<Configuration::getNUMBER_OF_ACTIONS();c++)
	//		if (BestAUC <= AUC[c] && tmpNonRec <= iNbrNonRecognized)
	//		{
	//			BestAUC = AUC[c];
	//			iNbrNonRecognized = tmpNonRec;
	//		}
	//

	cout << "Fold:" <<iFold << endl;
	iNbrNonRecognized = calculateBACBinaryByVoting(votes, ActualClass, BestAUC);

	for (map<int, int*>::iterator itr = votes.begin(); itr != votes.end(); itr++)
		delete[] itr->second;
	return;
}

double ReconfigureAttributeAssignment(double *accuracies,
		list<int> *lastAddedAtts, set<int> **AttsDistributing) {
	double corr;
	double bestCorr = 0;

	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++) {
		/* start the experiments */
		FILE *dataFile;

		/*Open files for statistics*/
		char portstring[256];
		sprintf(portstring, "Decision_%d.dec", i);
		dataFile = (FILE *) fopen(portstring, "rt");
		if (dataFile == NULL) {
			fprintf(stderr, "Cannot open Decision file");
			fprintf(stderr, portstring);
		}

		corr = 0;
		for (unsigned int j = 0; j < Configuration::getMAX_NR_STEPS() && feof(
				dataFile) < 1; j++) {
			int action, pAction;
			fscanf(dataFile, "Prediction:%d;Actual:%d\n", &pAction, &action);

			if (pAction >= 0 && (action == pAction))
				corr++;
		}
		fclose(dataFile);
		double currentAccuracy = corr / Configuration::getMAX_NR_STEPS();
		if (accuracies[i] > currentAccuracy) { // UnImprovement
			// Undo Previous Action
			for (list<int>::iterator itr = lastAddedAtts[i].begin(); itr
					!= lastAddedAtts[i].end(); itr++) {
				int addedAtt = *itr;
				if (addedAtt < 0)
					AttsDistributing[i]->insert(-addedAtt);
				else
					AttsDistributing[i]->erase(addedAtt);
			}
		} else // Improve so take next action
		{
			int rndAttIndex = (int) (frand()
					* Configuration::getCONDITION_LENGTH());
			if (frand() < 0.5) {
				AttsDistributing[i]->erase(rndAttIndex);
				lastAddedAtts[i].push_back(-rndAttIndex);
			} else {
				AttsDistributing[i]->insert(rndAttIndex);
				lastAddedAtts[i].push_back(rndAttIndex);
			}

		}

		if (bestCorr < currentAccuracy)
			bestCorr = currentAccuracy;

		accuracies[i] = currentAccuracy;
	}
	return bestCorr;
}

void MakeSamePops(vector<int> *MergingIslandsList) {

	system(" rm -f *.popImprt ");
	system(" rm -f *.popMerge ");

	if (MergingIslandsList->size() < 1)
		return;

	char portstring[256];

	for (unsigned int i = 0; i < MergingIslandsList->size(); i++) {
		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];
		sprintf(portstring, "cp PopIsland_%d.pop PopIsland_%d.popImprt",
				sourcePopInx, sourcePopInx);
		system(portstring);
	}

	for (unsigned int i = 0; i < MergingIslandsList->size(); i++) {
		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];

		for (unsigned int j = 0; j < MergingIslandsList->size(); j++) {
			int desPopInx = ((vector<int> ) *MergingIslandsList)[j];
			if (sourcePopInx != desPopInx) // Not same population
			{
				sprintf(
						portstring,
						"cat PopIsland_%d.pop PopIsland_%d.popImprt > PopIsland_%d.popMerge",
						sourcePopInx, desPopInx, desPopInx);
				system(portstring);
				sprintf(portstring, "rm PopIsland_%d.popImprt", desPopInx);
				system(portstring);
				sprintf(portstring,
						"mv PopIsland_%d.popMerge PopIsland_%d.popImprt",
						desPopInx, desPopInx);
				system(portstring);
			}
		}
	}

	for (unsigned int i = 0; i < MergingIslandsList->size(); i++) {
		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];
		sprintf(portstring, "rm PopIsland_%d.pop", sourcePopInx);
		system(portstring);
		sprintf(portstring, "mv PopIsland_%d.popImprt PopIsland_%d.pop",
				sourcePopInx, sourcePopInx);
		system(portstring);
	}
}

void MigrateAllBtwnPops(vector<int> *MergingIslandsList) {

	system(" rm -f *.popImprt ");
	system(" rm -f *.popMerge ");

	if (MergingIslandsList->size() < 1)
		return;

	char portstring[256];
	char *classifier = new char[Configuration::getCONDITION_LENGTH() * 256];

	for (unsigned int i = 0; i < MergingIslandsList->size(); i++) {

		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];

		sprintf(portstring, "PopIsland_%d.pop", sourcePopInx);
		FILE *sourceFile;
		sourceFile = fopen(portstring, "rb");
		if (sourceFile == NULL) {
			cout << "Error 1, file Not found" << portstring << endl;
			continue;
		}

		int desPopInx = 0;
		while (feof(sourceFile) < 1) {
			fscanf(sourceFile, "%s\n", classifier);

			desPopInx = (int) (frand() * MergingIslandsList->size());
			desPopInx = ((vector<int> ) *MergingIslandsList)[desPopInx];

			sprintf(portstring, "PopIsland_%d.popImprt", desPopInx);
			FILE *destFile;
			destFile = fopen(portstring, "ab");
			if (destFile == NULL) {
				cout << "Error 2, file Not found" << portstring << endl;
				continue;
			}
			fprintf(destFile, "%s\n", classifier);
			fclose(destFile);

		}
		fclose(sourceFile);
	}

	for (unsigned int i = 0; i < MergingIslandsList->size(); i++) {
		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];
		sprintf(portstring, "rm PopIsland_%d.pop", sourcePopInx);
		system(portstring);
		sprintf(portstring, "mv PopIsland_%d.popImprt PopIsland_%d.pop",
				sourcePopInx, sourcePopInx);
		system(portstring);
	}

	delete[] classifier;
}

void BestMigrateToAllPops(vector<int> *MergingIslandsList) {
	system(" rm -f *.popImprt ");
	system(" rm -f *.popMerge ");

	if (MergingIslandsList->size() < 1)
		return;

	char portstring[256];
	//string classifier;
	char *classifier = new char[Configuration::getCONDITION_LENGTH() * 256];

	//for(int i=0;i<Configuration::getNR_DIFFERENT_ISLANDS();i++)
	for (int i = 0; i < MergingIslandsList->size(); i++) {
		list<Classifier*> pop;

		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];

		//sprintf(portstring, "BestPopIsland_%d_Exp%d_TotalExp%d.pop", sourcePopInx , expCounter , overalExp);
		sprintf(portstring, "PopIsland_%d.pop", sourcePopInx);
		FILE *sourceFile;
		sourceFile = fopen(portstring, "rb");
		if (sourceFile == NULL) {
			sprintf(portstring, "PopIsland_%d.pop", sourcePopInx);
			sourceFile = fopen(portstring, "rb");
			if (sourceFile == NULL) {
				cout << "Error 1, file Not found" << portstring << endl;
				continue;
			}
		}

		while (feof(sourceFile) < 1) {
			Classifier *cl = new Classifier();
			cl->loadClassifier(sourceFile);
			pop.push_back(cl);
		}
		fclose(sourceFile);

		// sort the classifier list (population)
		pop.sort(compare_classifiers_Acc);

		// open all destination files iteratively
		for (unsigned int desPopIt = 0; desPopIt < MergingIslandsList->size(); desPopIt++) {
			int desPopInx = ((vector<int> ) *MergingIslandsList)[desPopIt];

			sprintf(portstring, "PopIsland_%d.popImprt", desPopInx);
			FILE *destFile;
			destFile = fopen(portstring, "ab");
			if (destFile == NULL) {
				cout << "Error 2, file Not found" << portstring << endl;
				continue;
			}

			int nbrOfMigratingCl = pop.size()
					* Configuration::getPORTION_OF_MIGRATABLE_INDIVIDUAL();
			list<Classifier*>::iterator iter;
			for (iter = pop.begin(); iter != pop.end() && nbrOfMigratingCl > 0; iter++) {
				((Classifier*) *iter)->dumpClassifier(destFile);
				//nbrOfMigratingCl-= ((Classifier*) *iter)->num ;
				nbrOfMigratingCl--;
			}

			//cout << portstring << " Finished" << endl;
			fclose(destFile);
		}

		for (list<Classifier*>::iterator iter = pop.begin(); iter != pop.end(); iter++)
			delete *iter;
		pop.clear();
	}

	for (int i = 0; i < MergingIslandsList->size(); i++) {
		int sourcePopInx = ((vector<int> ) *MergingIslandsList)[i];
		sprintf(portstring, "rm PopIsland_%d.pop", sourcePopInx);
		system(portstring);
		sprintf(portstring, "mv PopIsland_%d.popImprt PopIsland_%d.pop",
				sourcePopInx, sourcePopInx);
		system(portstring);
	}

	delete[] classifier;
}

void RandomPartition(set<int> **AttsDistributing, unsigned int mixCount) {
	int length = Configuration::getCONDITION_LENGTH();

	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++) {
		if (mixCount != 0)
			delete AttsDistributing[i];
		AttsDistributing[i] = new set<int> ();
	}
	// distribute atts through islands
	for (int attsIndex = 0; attsIndex < Configuration::getCONDITION_LENGTH(); attsIndex++) {
		int islandIndex = (int) (frand()
				* Configuration::getNR_DIFFERENT_ISLANDS());
		AttsDistributing[islandIndex]->insert(attsIndex);
	}
}

void IterativePartition(set<int> **AttsDistributing, unsigned int mixCount) {

	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++) {
		if (mixCount > 0)
			delete AttsDistributing[i];
		AttsDistributing[i] = new set<int> ();
	}

	int length = Configuration::getCONDITION_LENGTH()
			/ Configuration::getNR_DIFFERENT_ISLANDS();
	int eachIslandlength = length;
	for (unsigned int islandIndex = 0; islandIndex
			< Configuration::getNR_DIFFERENT_ISLANDS(); islandIndex++) {
		if (islandIndex == (Configuration::getNR_DIFFERENT_ISLANDS() - 1)) // last island
			length = Configuration::getCONDITION_LENGTH() - eachIslandlength
					* (Configuration::getNR_DIFFERENT_ISLANDS() - 1);

		for (int attsIndex = 0; attsIndex < length; attsIndex++)
			AttsDistributing[islandIndex]->insert(islandIndex
					* eachIslandlength + attsIndex);
	}

}

void TIterativePartition(set<int> **AttsDistributing, unsigned int mixCount,
		queue<int> lstFeatures) {

	int islandActualIndex, sratIndexActualIsland;
	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++) {
		if (mixCount > 0)
			delete AttsDistributing[i];
		AttsDistributing[i] = new set<int> ();
	}

	sratIndexActualIsland = frand() * Configuration::getNR_DIFFERENT_ISLANDS();
	int length = Configuration::getCONDITION_LENGTH()
			/ Configuration::getNR_DIFFERENT_ISLANDS();
	int eachIslandlength = length;
	for (unsigned int islandIndex = 0; islandIndex
			< Configuration::getNR_DIFFERENT_ISLANDS() && !lstFeatures.empty(); islandIndex++) {
		if (islandIndex == (Configuration::getNR_DIFFERENT_ISLANDS() - 1)) // last island
			length = Configuration::getCONDITION_LENGTH() - eachIslandlength
					* (Configuration::getNR_DIFFERENT_ISLANDS() - 1);

		for (int attsIndex = 0; attsIndex < length && !lstFeatures.empty(); attsIndex++) {
			islandActualIndex = (sratIndexActualIsland + islandIndex)
					% Configuration::getNR_DIFFERENT_ISLANDS();
			AttsDistributing[islandActualIndex]->insert(
					(int) lstFeatures.front());
			lstFeatures.pop();
		}
	}

}

void TRoundRobinPartition(set<int> **AttsDistributing, unsigned int mixCount,
		queue<int> lstFeatures) {

	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++) {
		if (mixCount > 0)
			delete AttsDistributing[i];
		AttsDistributing[i] = new set<int> ();
	}

	unsigned int islandIndex = frand()
			* Configuration::getNR_DIFFERENT_ISLANDS();

	for (int attsIndex = 0; attsIndex < Configuration::getCONDITION_LENGTH()
			&& !lstFeatures.empty(); attsIndex++) {
		AttsDistributing[islandIndex]->insert((int) lstFeatures.front());
		lstFeatures.pop();
		islandIndex = (islandIndex + 1)
				% Configuration::getNR_DIFFERENT_ISLANDS();
	}

}

