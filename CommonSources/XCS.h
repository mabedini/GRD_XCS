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

#ifndef XCS_H_
#define XCS_H_
#include "Classifier.h"
#include <list>
#include <vector>
#include <set>
#include <map>
#include "Definitions.h"
#include "Utilities.h"
#include <math.h>
#include "Configuration.h"
#include "Original_XCS.h"
#include <sstream>

using namespace std;

class xcs : public Original_XCS {
public:
	xcs();
	void startOneSingleStepExperimentEndWithPrediction(list <Classifier*> *TrainData,list <Classifier*> *TestData,int foldNo, int mixingNo, int islandNo, char * predictionFileName, char * predictionOverTrainFileName, char * bestPopFileName);
	void startProgressiveCheckingExperimentEndWithPrediction(list <Classifier*> *TrainData,list <Classifier*> *TestData,int foldNo, int mixingNo, int islandNo, char * predictionFileName, char * predictionOverTrainFileName, char * bestPopFileName);
	void EvaluateClassifiersWithData(FILE *tabFile, list<Classifier*> *EvaluateData,double *AUC ,double *SE,double *AVGAUC ,double *AVGSE);
	void DoClassificationOnDataSet(char *evalFileName, list<Classifier*> *testData);
	bool addClassifierToPopulationWithChecking(Classifier *cl);

protected:
	bool CheckActiveConditionsinASet(list<Classifier*> *mset, int maximum);

	int ratingBinaryActionWinner(long double *predictionArray,int normalClass);
	int ratingActionWinner(long double *predictionArray,int normalClass);

//	int crossover(Classifier **cl);
	xCondition * getMin(xCondition *pCond1,xCondition *pCond2);
	xCondition * getMax(xCondition *pCond1,xCondition *pCond2);
	virtual int uniformCrossOverAllAtts(Classifier **cl);
//    virtual void MakeDontCare(Classifier * childs[2], Classifier * trnCls);

	void testAndReportBasedOntestDataFile( const char* reportFileName,
			list<Classifier*> *TestData, char * predictionFileName, time_t startTime, double &maxBAC);

};

#endif /* XCS_H_ */
