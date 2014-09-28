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
#include "XCS.h"

using namespace std;

xcs::xcs() :
	Original_XCS() {
	// TODO Auto-generated constructor stub
	//	this->iNbrIslands = 1;
	//	this->iIslandsNo = -1;
	//	this->iteratorTime = 0;
}

//xcs::~xcs() : ~Original_XCS() {
//	// TODO Auto-generated destructor stub
//	//restart();
//}

/* ########################## in a single step environment ########################## */

void xcs::startOneSingleStepExperimentEndWithPrediction(
		list<Classifier*> *TrainData, list<Classifier*> *TestData, int foldNo,
		int mixingNo, int islandNo, char * predictionFileName,
		char * predictionOverTrainFileName, char * bestPopFileName) {

	/* Start one experiment, trialCounter counts the number of learning problems (trials) */
	int exploit = 0;
	int TESTFREQUENCY = 200;
	int *correct = new int[TESTFREQUENCY];
	double *sysError = new double[TESTFREQUENCY];
	//	char portstring[256];
	//	sprintf(portstring, "Decision_%d.dec", this->iIslandsNo);
	unsigned long int trialCounter = 0;
	time_t startTime, endTime;
	time(&startTime);

	this->iteratorTime = mixingNo*Configuration::getMAX_NR_STEPS()*TrainData->size();

	for (unsigned int iSteps = 0; iSteps < Configuration::getMAX_NR_STEPS(); iSteps++) {
		for (list<Classifier *>::iterator i = TrainData->begin(); i
				!= TrainData->end(); i++, trialCounter++) {

			/* get the next problem instance */
			Classifier *cl = (*i);

			doOneSingleStepProblemExplore(cl, trialCounter);

			//doOneSingleStepProblemExploit(cl, trialCounter,
			//		&correct[trialCounter % (TESTFREQUENCY)],
			//		&sysError[trialCounter % (TESTFREQUENCY)], NULL);

			//			char stPopFileName[255];
			//			sprintf(stPopFileName,"pop%d.txt",trialCounter);
			//			dumpPopulation( stPopFileName);
		}
		//cout << " istep: "<< iSteps << " is done. with Size:"<< population->size() << endl;
	}

	if (predictionOverTrainFileName != NULL)
		DoClassificationOnDataSet(predictionOverTrainFileName, TrainData);

	if (predictionFileName != NULL)
		DoClassificationOnDataSet(predictionFileName, TestData);

	double MinAttSize, MaxAttSize, AvgAttSize;
	getAttibutesSize(MinAttSize, MaxAttSize, AvgAttSize);

	time(&endTime);
	double dif;

	dif = difftime(endTime, startTime);//check for function name

	/* start the experiments */
	FILE *outPutFile;

	/*Open files for statistics*/
	char portstring[256];
	sprintf(portstring, "tmpStat%d.res", this->iIslandsNo);
	outPutFile = (FILE *) fopen(portstring, "at");
	fprintf(
			outPutFile,
			"Time:%.2lf;PopSize:%d;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;Mem:%s",
			dif, population->size(), AvgAttSize, MinAttSize, MaxAttSize,
			get_CurrentProcessUsedMem());

	fclose(outPutFile);

	delete[] correct;
	delete[] sysError;
}

///*########################################### match set operations ##################################################*/
//

bool xcs::CheckActiveConditionsinASet(list<Classifier*> *mset, int maximum) {
	int indx;
	set<int> AllParticipatedCoditions;

	for (list<Classifier*>::const_iterator i = mset->begin(); i != mset->end(); i++) {
		map<int, xCondition *> *c = &((*i)->ConditionList);
		for (map<int, struct xCondition*>::const_iterator j = c->begin(); j
				!= c->end(); j++) {
			indx = (*j).first;
			AllParticipatedCoditions.insert(indx);
		}
		if (AllParticipatedCoditions.size() > maximum)
			return true;
	}
	if (AllParticipatedCoditions.size() > maximum)
		return true;

	return false;
}

/* return the corresponding int value of the best action in the prediction array*/
int xcs::ratingActionWinner(long double *predictionArray, int normalClass) {
	int ret = 0;
	long double max = 0.0, leastMax = 0.0;

	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		if (predictionArray[i] > max) {
			max = predictionArray[i];
			ret = i;
		}
	}

	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		if (predictionArray[i] > leastMax && predictionArray[i] < max) {
			leastMax = predictionArray[i];
		}
	}

	if (leastMax != 0 && max != 0) {
		long double diference = max - leastMax;
		if (diference / max > 0.5) {
			if (normalClass == ret)
				return 0;
			else
				return Configuration::getNBR_RATING_CATEGORY() - 1;
		} else if (diference / max > 0.1) {
			if (normalClass == ret)
				return 1;
			else
				return Configuration::getNBR_RATING_CATEGORY() - 2;
		} else {
			//cout << diference/max << endl ;
			return Configuration::getNBR_RATING_CATEGORY() / 2;
		}
	} else if (leastMax == 0 && max != 0) {
		if (normalClass == ret)
			return 0;
		else
			return 4;
	} else // leastMax==0 && max ==0
	{
		return 2;
	}
}
/* return the corresponding int value of the best action in the prediction array*/
int xcs::ratingBinaryActionWinner(long double *predictionArray, int normalClass) {
	int ret = 0;
	long double max = 0.0;

	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		if (predictionArray[i] > max) {
			max = predictionArray[i];
			ret = i;
		}
	}

	if (normalClass == ret)
		return 0;
	else
		return 2;
}

int xcs::uniformCrossOverAllAtts(Classifier **cl) {
	int ret = -1;
	set<int> MoveTo0, MoveTo1, DoMinMax;
	map<int, xCondition*>::const_iterator i;

	// Check if for this attribute do the Crossover or not
	for (i = cl[0]->ConditionList.begin(); i != cl[0]->ConditionList.end(); i++)
		if ((frand() < 0.5) && (responsibleAtts.empty()
				|| (responsibleAtts.find((*i).first) != responsibleAtts.end()))) // Is in responsible Atts
		{ // it could be also in cl[1]
			if (cl[1]->ConditionList.find((*i).first)
					!= cl[1]->ConditionList.end())
				DoMinMax.insert((*i).first); //Both Have the att
			else
				MoveTo1.insert((*i).first); // Cl0 only has the att
		}

	// check cl1 only for thoese features that not checked in cl0
	for (i = cl[1]->ConditionList.begin(); i != cl[1]->ConditionList.end(); i++)
		if (cl[0]->ConditionList.find((*i).first) == cl[0]->ConditionList.end()) // Not appeared in cl0
		{
			if ((frand() < 0.50) // Then take its chance
					&& (responsibleAtts.empty() || (responsibleAtts.find(
							(*i).first) != responsibleAtts.end()))) // Is in responsible Atts

				MoveTo0.insert((*i).first); // Cl0 only has the att
		}

	// Move from 1 to 0 (to Min child)
	//if (!MoveToMin.empty()) {
	//	for (set<int>::iterator it = MoveToMin.begin(); it != MoveToMin.end(); it++) {
	//		if (cl[0]->ConditionList[*it] != NULL)
	//			delete cl[0]->ConditionList[*it];
	//		cl[0]->ConditionList[*it]
	//				= (xCondition *) cl[1]->ConditionList[*it];
	//		cl[1]->ConditionList.erase(*it);
	//	}
	//	ret = 1;
	//}
	// Do MinMax Operation
	if (!DoMinMax.empty()) {
		for (set<int>::iterator it = DoMinMax.begin(); it != DoMinMax.end(); it++) {

			xCondition *tempMin = getMin(
					(xCondition *) cl[0]->ConditionList[*it],
					(xCondition *) cl[1]->ConditionList[*it]);
			xCondition *tempMax = getMax(
					(xCondition *) cl[0]->ConditionList[*it],
					(xCondition *) cl[1]->ConditionList[*it]);

			delete cl[0]->ConditionList[*it];
			delete cl[1]->ConditionList[*it];

			if (frand() < 0.5) {
				if (tempMin != NULL)
					cl[0]->ConditionList[*it] = (xCondition *) tempMin;
				else
					cl[0]->ConditionList.erase(*it);
				if (tempMax != NULL)
					cl[1]->ConditionList[*it] = (xCondition *) tempMax;
				else
					cl[1]->ConditionList.erase(*it);
			} else {
				if (tempMin != NULL)
					cl[1]->ConditionList[*it] = (xCondition *) tempMin;
				else
					cl[1]->ConditionList.erase(*it);
				if (tempMax != NULL)
					cl[0]->ConditionList[*it] = (xCondition *) tempMax;
				else
					cl[0]->ConditionList.erase(*it);
			}
		}
		ret = 1;
	}

	//Move from 1 to 0 (to Min child)
	if (!MoveTo0.empty()) {
		for (set<int>::iterator it = MoveTo0.begin(); it != MoveTo0.end(); it++) {
			if (cl[0]->ConditionList[*it] != NULL)
				delete cl[0]->ConditionList[*it];
			cl[0]->ConditionList[*it]
					= (xCondition *) cl[1]->ConditionList[*it];
			cl[1]->ConditionList.erase(*it);
		}
		ret = 1;
	}

	if (!MoveTo1.empty()) {
		for (set<int>::iterator it = MoveTo1.begin(); it != MoveTo1.end(); it++) {
			if (cl[1]->ConditionList[*it] != NULL)
				delete cl[1]->ConditionList[*it];
			cl[1]->ConditionList[*it]
					= (xCondition *) cl[0]->ConditionList[*it];
			cl[0]->ConditionList.erase(*it);
		}
		ret = 1;
	}

	return ret;

	return ret;
}

/*########################################## discovery mechanism #################################*/

/* crosses the two received classifiers (right now: one-point crossover), return if the strings were changed */
//int xcs::crossover(Classifier **cl) {
//
//	//	return Original_XCS::crossover(cl);
//	int ret = 0;
//
//	if (!cl[0]->ConditionList.empty() && !cl[1]->ConditionList.empty()
//			&& !(cl[0]->isEqual(cl[1])))
//		// uniform Crossover
//		if (frand() < Configuration::getCROSSPX())
//			ret = uniformCrossOverAllAtts(cl);
//
//	/* return if the conditions of the classifiers changed */
//	return ret;
//}

xCondition * xcs::getMin(xCondition *pCond1, xCondition *pCond2) {
	// making Max
	if (pCond1->GetConditionType() == RealNormalized) {

		xRealNormalizedCondition *newCond;
		double minF, maxF;
		double Low1 = ((xRealNormalizedCondition*) pCond1)->getLowerValue();
		double Low2 = ((xRealNormalizedCondition*) pCond2)->getLowerValue();
		double Up1 = ((xRealNormalizedCondition*) pCond1)->getUpperValue();
		double Up2 = ((xRealNormalizedCondition*) pCond2)->getUpperValue();

		// finding min
		if (Low1 > Low2) // Max of mins
			minF = Low1;
		else
			minF = Low2;

		if (Up1 < Up2) // Min of maxs
			maxF = Up1;
		else
			maxF = Up2;

		if (minF < maxF) // there is common range between A and B
		{
			//assert(newCond = new xRealCondition());
			newCond = (xRealNormalizedCondition *) pCond1->Clone();
			//if (((double) maxF - minF) < Configuration::getRANGE_INI()) {
			//	// do nothing
			//	//				newCond->center = center1;
			//	//				newCond->range = range1;

			//} else
			{
				newCond->setCenterValue(minF + ((maxF - minF) / 2));
				newCond->setRangeValue((maxF - minF) / 2);
			}
			return newCond;
		} else // there is No common range between A and B
		{
			return NULL;

		}

	} else if (pCond1->GetConditionType() == Real) {
		xRealCondition *newCond;
		float minF, maxF;
		double Low1 = ((xRealCondition*) pCond1)->getLowerValue();
		double Low2 = ((xRealCondition*) pCond2)->getLowerValue();
		double Up1 = ((xRealCondition*) pCond1)->getUpperValue();
		double Up2 = ((xRealCondition*) pCond2)->getUpperValue();

		// finding min
		if (Low1 > Low2) // Max of mins
			minF = Low1;
		else
			minF = Low2;

		if (Up1 < Up2) // Min of maxs
			maxF = Up1;
		else
			maxF = Up2;

		if (minF < maxF) // there is common range between A and B
		{
			//assert(newCond = new xRealCondition());
			newCond = (xRealCondition *) pCond1->Clone();
			if (((double) maxF - minF) < Configuration::getRANGE_INI()) {
				// do nothing
				//				newCond->center = center1;
				//				newCond->range = range1;

			} else {
				newCond->setCenterValue(minF + ((maxF - minF) / 2));
				newCond->setRangeValue((maxF - minF) / 2);
			}
			return newCond;
		} else // there is No common range between A and B
		{
			return NULL;

		}
	} else if (pCond1->GetConditionType() == Nominal) {
		xCondition *newCond;
		newCond = pCond1->Clone();

		for (unsigned int i = 0; i
				< ((xNominalCondition*) pCond1)->CategoryLength; i++) {
			((xNominalCondition*) newCond)->Category[i]
					= ((xNominalCondition*) pCond1)->Category[i]
							& ((xNominalCondition*) pCond2)->Category[i];
		}

		// check if there is any true in Category[i] if not return NULL instead (Don't care)
		for (unsigned int i = 0; i
				< ((xNominalCondition*) pCond1)->CategoryLength; i++) {
			if (((xNominalCondition*) newCond)->Category[i] == true)
				return newCond;
		}

		return NULL;
	} else if (pCond1->GetConditionType() == Binary) {
		if (((xBinaryCondition*) pCond1)->value
				!= ((xBinaryCondition*) pCond2)->value)
			return NULL;
		else {
			xCondition *newCond;
			newCond = pCond1->Clone();
			return newCond;
		}
	}

	return NULL;

}

xCondition * xcs::getMax(xCondition *pCond1, xCondition *pCond2) {
	// making Max
	if (pCond1->GetConditionType() == RealNormalized) {
		xRealNormalizedCondition *newCond;
		float minF, maxF;
		double Low1 = ((xRealNormalizedCondition*) pCond1)->getLowerValue();
		double Low2 = ((xRealNormalizedCondition*) pCond2)->getLowerValue();
		double Up1 = ((xRealNormalizedCondition*) pCond1)->getUpperValue();
		double Up2 = ((xRealNormalizedCondition*) pCond2)->getUpperValue();

		// finding min
		if (Low1 <= Low2) // Min of mins
			minF = Low1;
		else
			minF = Low2;

		if (Up1 > Up2) // Max of maxs
			maxF = Up1;
		else
			maxF = Up2;

		if ((minF <= 0) && (maxF >= 1))
			// The Whole area is covered So make it Don't care
			return NULL;
		else {
			newCond = (xRealNormalizedCondition *) pCond1->Clone();
			//if (((double) maxF - minF) < Configuration::getRANGE_INI()) {
			//	// do nothing

			//} else
			{
				newCond->setCenterValue(minF + ((maxF - minF) / 2));
				newCond->setRangeValue((maxF - minF) / 2);
			}
			return newCond;
		}
	} else if (pCond1->GetConditionType() == Real) {
		xRealCondition *newCond;
		float minF, maxF;
		double Low1 = ((xRealCondition*) pCond1)->getLowerValue();
		double Low2 = ((xRealCondition*) pCond2)->getLowerValue();
		double Up1 = ((xRealCondition*) pCond1)->getUpperValue();
		double Up2 = ((xRealCondition*) pCond2)->getUpperValue();

		// finding min
		if (Low1 <= Low2) // Min of mins
			minF = Low1;
		else
			minF = Low2;

		if (Up1 > Up2) // Max of maxs
			maxF = Up1;
		else
			maxF = Up2;

		if ((minF <= ((xRealCondition*) pCond1)->getLowerBound()) && (maxF
				>= ((xRealCondition*) pCond1)->getUpperBound()))
			// The Whole area is covered So make it Don't care
			return NULL;
		else {
			newCond = (xRealCondition *) pCond1->Clone();
			if (((double) maxF - minF) < Configuration::getRANGE_INI()) {
				// do nothing

			} else {
				newCond->setCenterValue(minF + ((maxF - minF) / 2));
				newCond->setRangeValue((maxF - minF) / 2);
			}
			return newCond;
		}

	} else if (pCond1->GetConditionType() == Nominal) {
		xCondition *newCond;
		newCond = pCond1->Clone();

		for (unsigned int i = 0; i
				< ((xNominalCondition*) pCond1)->CategoryLength; i++) {
			((xNominalCondition*) newCond)->Category[i]
					= ((xNominalCondition*) pCond1)->Category[i]
							| ((xNominalCondition*) pCond2)->Category[i];
		}

		// check if there is any true in Category[i] if not return NULL instead (Don't care)
		for (unsigned int i = 0; i
				< ((xNominalCondition*) pCond1)->CategoryLength; i++) {
			if (((xNominalCondition*) newCond)->Category[i] == true)
				return newCond;
		}

		return NULL;
	} else if (pCond1->GetConditionType() == Binary) {
		if (((xBinaryCondition*) pCond1)->value
				!= ((xBinaryCondition*) pCond2)->value)
			return NULL;
		else {
			xCondition *newCond;
			newCond = pCond1->Clone();
			return newCond;
		}
	}

	return NULL;
}

void xcs::DoClassificationOnDataSet(char *evalFileName,
		list<Classifier*> *testData) {

	if (evalFileName == NULL)
		return;
	long double *predictionArray =
			new long double[Configuration::getNUMBER_OF_ACTIONS()];
	int PAction = -1, Action = -1;

	FILE *outEvalFile;
	outEvalFile = fopen(evalFileName, "wb");

	if (outEvalFile == NULL) {
		cout << "Error, couldn't open New File" << endl;
		return;
	}

	int index = 1;
	for (list<Classifier *>::const_iterator poplocal = testData->begin(); poplocal
			!= testData->end(); poplocal++, index++) {
		list<Classifier*> mset;
		Action = (*poplocal)->getAct();

		//Creeating the Matched Set
		for (list<Classifier*>::const_iterator i = population->begin(); i
				!= population->end(); i++) {
			// Calculate the mean prediction probability and the population size to detect loops
			if (match(&(*poplocal)->ConditionList, &(*i)->ConditionList)) {
				// add to match list
				mset.push_back((*i));
			}
		}

		if (!mset.empty()) {
			// get the Prediction array
			getPredictionArray(&mset, predictionArray);
			// Get the action, that wins in the prediction array
			PAction = deterministicActionWinner(predictionArray);

			mset.clear();
		} else
			// not recognized
			PAction = -1;

		fprintf(outEvalFile, "Sample #%d\t%d\t%d\n", index, PAction, Action);
	}

	fclose(outEvalFile);
	delete[] predictionArray;
}

void xcs::EvaluateClassifiersWithData(FILE *tabFile,
		list<Classifier*> *EvaluateData, double *AUC, double *SE,
		double *AVGAUC, double *AVGSE) {
	list<Classifier *>::const_iterator poplocal;//, FirstPopLocal;
	//FirstPopLocal = EvaluateData->begin();
	list<Classifier*> mset;

	long double *predictionArray =
			new long double[Configuration::getNUMBER_OF_ACTIONS()];

	int action, PAction;

	int *TP = new int[Configuration::getNUMBER_OF_ACTIONS()];
	int *TN = new int[Configuration::getNUMBER_OF_ACTIONS()];
	int *FP = new int[Configuration::getNUMBER_OF_ACTIONS()];
	int *FN = new int[Configuration::getNUMBER_OF_ACTIONS()];
	int *NumNonRecognized = new int[Configuration::getNUMBER_OF_ACTIONS()];
	int *normalRated = new int[Configuration::getNBR_RATING_CATEGORY()];
	int *AbNormalRated = new int[Configuration::getNBR_RATING_CATEGORY()];

	// initializing variables
	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		TP[i] = 0;
		TN[i] = 0;
		FP[i] = 0;
		FN[i] = 0;
		NumNonRecognized[i] = 0;
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

		poplocal = EvaluateData->begin();
		for (int index = 0; index < EvaluateData->size(); index++) {
			action = (*poplocal) ->getAct();

			for (list<Classifier*>::const_iterator i = population->begin(); i
					!= population->end(); i++) {
				/* Calculate the mean prediction probability and the population size to detect loops */
				if (match(&(*poplocal)->ConditionList, &(*i)->ConditionList)) {
					/* Calculate the mean prdiction prob. and the match set size to detect loops */
					mset.push_back((*i));
				}
			}

			if (!mset.empty()) {
				/* get the Prediction array */
				getPredictionArray(&mset, predictionArray);
				/* Get the action, that wins in the prediction array */
				PAction = deterministicActionWinner(predictionArray);
				//fprintf(tabFile," Act:%d and PAct:%d, PA[0]:%Lf and PA[1]:%Lf\n",action,PAction,predictionArray[0],predictionArray[1]);

				if (NomClass == action) // action(Actual) is Positive
				{
					if (PAction == action)
						TP[NomClass]++; // The Number of predicted positive cases that are actually positive
					else
						FN[NomClass]++; // The Number of predicted Negative cases that are actually positive
				} else // NomClass != action // action(Actual) is Negative
				{
					if (PAction == action)
						TN[NomClass]++; // The Number of predicted Negative cases that are actually negative
					else
						FP[NomClass]++; // The Number of predicted positive cases that are actually negative
				}

				/* Get the action, that wins in the prediction array */
				int rate;
				if (Configuration::getNBR_RATING_CATEGORY() > 3)
					rate = ratingActionWinner(predictionArray, NomClass);
				else
					rate = ratingBinaryActionWinner(predictionArray, NomClass);

				if (NomClass == action)
					normalRated[rate]++;
				else
					AbNormalRated[rate]++;

			} else {
				NumNonRecognized[NomClass]++;
				int NonRecRate = Configuration::getNBR_RATING_CATEGORY() / 2;
				if (NomClass == action)
					normalRated[NonRecRate]++;
				else
					AbNormalRated[NonRecRate]++;
			}

			poplocal++;
			//if(poplocal == TrainData->end())
			//	poplocal = TrainData->begin();

			/* Clean up */
			mset.clear();
		}

		// calculate AUC
		CalculateAUC(&AUC[NomClass], &SE[NomClass], &AVGAUC[NomClass],
				&AVGSE[NomClass], normalRated, AbNormalRated);

		if (tabFile != NULL) {
			for (int j = 0; j < Configuration::getNBR_RATING_CATEGORY(); j++) {
				fprintf(tabFile, " Normal[%ld]:%d", j, normalRated[j]);
				fprintf(tabFile, " AbNormal[%ld]:%d \n", j, AbNormalRated[j]);
			}
			fprintf(tabFile, " AUC[%ld]:%4.2f \n", NomClass, AUC[NomClass]);
			fprintf(tabFile, " AVG AUC[%ld]:%4.2f \n", NomClass,
					AVGAUC[NomClass]);
		}
	}

	if (tabFile != NULL) {
		for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
			double TPR, FPR, Percision, FM;
			if (TP[i] > 0) {
				TPR = (double) TP[i] / ((double) TP[i] + (double) FN[i]);
				Percision = (double) TP[i] / ((double) TP[i] + (double) FP[i]);
				FM = (double) 2 * TPR * Percision / ((double) TPR
						+ (double) Percision);
			} else {
				TPR = 0;
				Percision = 0;
				FM = 0;
			}
			if (FP[i] > 0)
				FPR = (long double) FP[i] / ((long double) FP[i]
						+ (long double) TN[i]);
			else
				FPR = 0;

			fprintf(tabFile, " class[%2d]:", i);
			fprintf(tabFile, " TP:%4d", TP[i]);
			fprintf(tabFile, " TN:%4d", TN[i]);
			fprintf(tabFile, " FP:%4d", FP[i]);
			fprintf(tabFile, " FN:%4d", FN[i]);
			fprintf(tabFile, " TPR:%4.2f", TPR);
			fprintf(tabFile, " FPR:%4.2f", FPR);
			fprintf(tabFile, " Percision:%4.2f", Percision);
			fprintf(tabFile, " Recall:%4.2f", TPR);
			fprintf(tabFile, " F-Measure:%4.2f", FM);
			fprintf(tabFile, " Accuracy:%4.2f%%", (TP[i] + TN[i])
					/ EvaluateData->size() * 100);
			fprintf(tabFile, " AUC:%4.2f%%", AUC[i] * 100);
			fprintf(tabFile, " SE:%f%%", SE[i] * 100);
			fprintf(tabFile, " NonRecognized:%4d\n", NumNonRecognized[i]);
		}
	}

	fflush(tabFile);

	delete[] predictionArray;
	delete[] TP;
	delete[] TN;
	delete[] FP;
	delete[] FN;
	delete[] NumNonRecognized;
	delete[] normalRated;
	delete[] AbNormalRated;

}

void xcs::startProgressiveCheckingExperimentEndWithPrediction(
		list<Classifier*> *TrainData, list<Classifier*> *TestData, int foldNo,
		int mixingNo, int islandNo, char * predictionFileName,
		char * predictionOverTrainFileName, char * bestPopFileName) {

	/* Start one experiment, trialCounter counts the number of learning problems (trials) */
	int exploit = 0;
	int TESTFREQUENCY = 200;
	int *correct = new int[TESTFREQUENCY];
	double *sysError = new double[TESTFREQUENCY];
	double MaxBAC = 0;

	//char portstring[256];
	//sprintf(portstring, "TestPred_%d.txt", this->iIslandsNo);
	unsigned long int trialCounter = 0;
	this->iteratorTime = mixingNo*Configuration::getMAX_NR_STEPS()*TrainData->size();

	time_t startTime, endTime;
	time(&startTime);

//	trialCounter = mixingNo*Configuration::getNR_EXPS()* TrainData->size();

	for (unsigned int iSteps = 0; iSteps < Configuration::getMAX_NR_STEPS(); iSteps++) {
		for (list<Classifier *>::iterator i = TrainData->begin(); i
				!= TrainData->end(); i++, trialCounter++) {

			/* get the next problem instance */
			Classifier *cl = (*i);

			doOneSingleStepProblemExplore(cl, trialCounter);

			//cout << " istep: "<< iSteps << " is done. with Size:"<< population->size() << endl;
		}

		if ((iSteps % 10) == 0) {

			/*Open files for statistics*/
			char portstring[256]; // try to avoid override the previous mixing stat again
			int tStepId = iSteps
					+ (mixingNo * Configuration::getMAX_NR_STEPS());
			if (Configuration::getNR_DIFFERENT_ISLANDS() == 1) // for single Island model (GRD-XCS)
			{
				//sprintf(portstring, "Detailstat%d.res", iSteps );
				sprintf(portstring, "Detailstat.res");
				testAndReportBasedOntestDataFile(portstring, TestData,
						predictionFileName, startTime, MaxBAC);
			} else {
				// for CoXCS model and Dummmy, we don't need it just for calculating MaxBAC
				//sprintf(portstring, "Detailstat_%d_%d.res", islandNo, tStepId);
				sprintf(portstring, "Detailstat_%d.res", islandNo);
				testAndReportBasedOntestDataFile(portstring, TestData,
						predictionFileName, startTime, MaxBAC);
			}

		} // istep%100

	} // istep

	// end prediction report
	char portstring[256]; // try to avoid override the previous mixing stat again
	sprintf(portstring, "Detailstat_%d.res", islandNo);
	testAndReportBasedOntestDataFile(portstring, TestData, predictionFileName,
			startTime, MaxBAC);

	if (predictionOverTrainFileName != NULL)
		DoClassificationOnDataSet(predictionOverTrainFileName, TrainData);

	if (predictionFileName != NULL)
		DoClassificationOnDataSet(predictionFileName, TestData);

	delete[] correct;
	delete[] sysError;
}

void xcs::testAndReportBasedOntestDataFile(const char* reportFileName, list<
		Classifier*> *TestData, char * predictionFileName, time_t startTime,
		double &maxBAC) {

	time_t endTime;

	DoClassificationOnDataSet(predictionFileName, TestData);

	double islandsAUC = 0;
	int tmpNbrNonRecognized = 0;

	int nbrData = 0;
	// recognizing number of data available
	FILE *vFile;

	/*Open files for statistics*/
	char strFileName[256];
	int tmpNonRec;
	int TP = 0, TN = 0, FP = 0, FN = 0;
	int Positive = 0, Negative = 0;
	;
	double BAC = 0;

	/* start the experiments */
	FILE *dataFile;

	/*Open files for statistics*/

	dataFile = (FILE *) fopen(predictionFileName, "rt");

	if (dataFile == NULL) {
		fprintf(stderr,
				"Cannot open Decision file(%s) for progressive check\n",
				predictionFileName);
		return;
	}

	unsigned int iSampleNo = 0;
	for (iSampleNo = 0; !feof(dataFile); iSampleNo++) {
		int action, pAction, LineNo;
		fscanf(dataFile, "Sample #%d\t%d\t%d\n", &LineNo, &pAction, &action);

		if (pAction >= 0) // if -1 means not recognized
		{
			if (action == 0) {
				if (pAction == 0)
					TN++;
				else if (pAction == 1)
					FP++;
			} else // action ==1
			{
				if (pAction == 1)
					TP++;
				else if (pAction == 0)
					FN++;
			}

		} else {
			tmpNbrNonRecognized++;
			//					if (frand() > 0.5)
			//						FP++;
			//					else
			//						FN++;
		}

		if (action == 0)
			Negative++;
		else
			// action ==1
			Positive++;

	}
	fclose(dataFile);

	//			double Sensiticity = (double) TP / (TP + FN);
	//			double Specificity = (double) TN / (TN + FP);
	double Sensiticity = (double) TP / (Positive);
	double Specificity = (double) TN / (Negative);

	//	BAC = 0;
	BAC = (Sensiticity + Specificity) / 2.0;

	//	if ((TP + FN) > 0 && (TN + FP) > 0) {
	//		BAC += 0.5 * Specificity * (1 - Sensiticity);
	//		BAC += 0.5 * Sensiticity * (1 - Specificity);
	//		BAC += Sensiticity * Specificity;
	//	}

	// end of execution
	time(&endTime);

	// execution Statistics!
	double dif;
	dif = difftime(endTime, startTime);//check for function name

	// Setting max values
	if (maxBAC <= BAC) {
		maxBAC = BAC;
		// DumpPopulation
		//				   stringstream ssFileName;//create a stringstream
		//				   ssFileName << iSteps <<".pop";//add the iterationNo as File Name
		//
		//				dumpPopulation(ssFileName.str().c_str());
	}

	/*Open files for statistics*/

	if (reportFileName != NULL) {
		/* start the experiments */
		FILE *outPutFile;

		double MinAttSize, MaxAttSize, AvgAttSize;
		getAttibutesSize(MinAttSize, MaxAttSize, AvgAttSize);

		outPutFile = (FILE *) fopen(reportFileName, "at");
		fprintf(
				outPutFile,
				"BAC:%lf;withNonRecognized:%d;Time:%.2lf;PopSize:%d;MaxBAC:%lf;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;Mem:%s",
				BAC, tmpNbrNonRecognized, dif, population->size(), maxBAC,
				AvgAttSize, MinAttSize, MaxAttSize, get_CurrentProcessUsedMem());
		//			printf(
		//					"BAC:%lf;withNonRecognized:%d;Time:%.2lf;PopSize:%d;MaxBAC:%lf;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;Mem:%s",
		//					BAC, tmpNbrNonRecognized, dif, population->size(), MaxBAC,
		//					AvgAttSize, MinAttSize, MaxAttSize,
		//					get_CurrentProcessUsedMem());
		//					"BAC:%lf;withNonRecognized:%d;Time:%.2lf;PopSize:%d;Mem:%s", BAC,
		//					tmpNbrNonRecognized, dif, population->size() , get_CurrentProcessUsedMem());
		fclose(outPutFile);
	}
}
