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
 

#include <iostream>
#include <pthread.h>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <time.h>
#include "CommIncludes.h"

enum WekaFilterCommandType {WekaGainRatio,WekaInformationGain, WekaSubsetEval, WekaSVMEval, WekaReliefF, WekaOneRule, PCA};


// for mutual access to IslandIndex
pthread_mutex_t islandsMutex = PTHREAD_MUTEX_INITIALIZER;

using namespace std;

void RunSingleIslandProcesses(int args, char* argv[], int iFold,
		int &iNbrNonRecognized, double &BestAUC);
void CreateListofIslandsProcesses(char *strAlgName, char *strParameters, const char* FRMode,
		int iFould, int &iNbrRecognized, double &BestAUC);

void startExperiments(int args, char* argv[]);
int readLastIslandsStat(double *BAC,int * NbrNonRecognized,double *time, int *populationSize, double *MaxBAC,double *MinAttSize,double *MaxAttSize, double *AvgAttSize, char* memSize, char * statFileName);

queue<int> RankFeatures(int fld, const char* FRMode);
queue <int> wekaRankFeatureSelection(WekaFilterCommandType filterType,const char * stDataFileName);
queue <int> ReadWekaRankFeatureOutputFile();



void RemovePopFiles() {
	system(" rm -f *.pop* ");
}

void RemoveStatisticsFiles() {
	system(" rm -f Eva*.txt ");
	system(" rm -f  *.txt ");
	system(" rm -f *.res ");
}

void CleanAllReportingFiles() {
	RemovePopFiles();
	RemoveStatisticsFiles();
	//system(" rm *.res* ");
	//system(" rm AFinalResult_*.Res -f ");
}

int main(int args, char* argv[]) {

	/* randomize the pseudo-number generator */
	randomize();
	if (args < 2)  // With No parameter - should be warn the user
	{
		printf("please run with FDIRM FeatureRankingMethod(Info,SVM)");
		exit(0);
	}

	if (args > 3) { // With parameters ( when the program call itself )
		Configuration::readConfigurationsFromFile(argv[1]);
		startExperiments(args, argv);
	} else // when call by user
	{
		Configuration::readConfigurationsFromFile("Config.cfg");

		for (int attIndx = 0; attIndx
				< Configuration::getCONDITION_LENGTH(); attIndx++)
			Classifier::ConditionListTypes[attIndx]
					= (ConditionTypes) RealNormalized;

		RemovePopFiles();
		RemoveStatisticsFiles();

		if (Configuration::getNR_DIFFERENT_ISLANDS() <= 1) { // Sole island // I don't know why ?

			CleanAllReportingFiles();
			double bestAuc = 0;
			int nbrNonRecognized = 0;
			for (int fld = 0; fld < Configuration::getNR_EXPS(); fld++) // Fold corss validation
			{
				time_t startTime, endTime;
				time(&startTime);

				/* start the experiments */
				RunSingleIslandProcesses(args, argv, fld, nbrNonRecognized,
						bestAuc);

				// end of execution
				time(&endTime);

				// execution Statistics!
				double dif;
				double MinAttSize, MaxAttSize, AvgAttSize, time;
				double BAC,MaxBAC;
				int NbrNonRecognized, populationSize;

				dif = difftime(endTime, startTime);//check for function name

				FILE *outPutFile;
				/*Open files for statistics*/
				int line;
				char portstring[256];
				char memSize[64];

				sprintf(portstring, "Detailstat.res");

				line = readLastIslandsStat(&BAC,&NbrNonRecognized,&time, &populationSize, &MaxBAC, &MinAttSize,&MaxAttSize, &AvgAttSize, memSize, portstring);

				sprintf(portstring, "Finalstat.res");
				outPutFile = (FILE *) fopen(portstring, "at");

				fprintf( outPutFile,
						"BAC:%lf;withNonRecognized:%.2f;Time:%.2lf;PopSize:%.2lf;MaxBAC:%lf;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;islandExecTime:%.2lf,Mem:%s",
						bestAuc, nbrNonRecognized, dif, populationSize, MaxBAC, AvgAttSize, MinAttSize, MaxAttSize, time, "\n");

//				sprintf(portstring, "Finalstat_%d.res", fld);
//				outPutFile = (FILE *) fopen(portstring, "at");
//				fprintf(outPutFile, "BestAUC:%f;withNonRecognized:%d\n",
//						bestAuc, nbrNonRecognized);
				fclose(outPutFile);
			}

		} else // create a list of islands process
		{
			CleanAllReportingFiles();
			double bestAuc = 0;
			int nbrNonRecognized = 0;
			for (int fld = 0; fld < Configuration::getNR_EXPS(); fld++) // Fold corss validation
			{
				time_t startTime, endTime;
				time(&startTime);


				CreateListofIslandsProcesses(argv[0], argv[1], argv[2], fld,
						nbrNonRecognized, bestAuc);

				// end of execution
				time(&endTime);

				// execution Statistics!
				double dif;
				dif = difftime(endTime, startTime);//check for function name

				double MinAttSize, MaxAttSize, AvgAttSize, time;
				double BAC,MaxBAC;
				int NbrNonRecognized, populationSize;
				double SumMinAttSize=0, SumMaxAttSize=0, SumAvgAttSize=0, Sumtime=0;
				double SumBAC=0,SumMaxBAC=0;
				int SumNbrNonRecognized=0, SumpopulationSize=0;

				int line;
				char portstring[256];
				char memSize[64];
				for(unsigned int islandNo=0;islandNo< Configuration::getNR_DIFFERENT_ISLANDS(); islandNo++)
				{
					//int tStepId = Configuration::getNR_MIXING_DIFFERENT_POPULATION() * Configuration::getMAX_NR_STEPS() -100;
					//sprintf(portstring, "Detailstat_%d_%d.res", islandNo, tStepId );
					sprintf(portstring, "Detailstat_%d.res", islandNo);

					line = readLastIslandsStat(&BAC,&NbrNonRecognized,&time, &populationSize, &MaxBAC, &MinAttSize,&MaxAttSize, &AvgAttSize, memSize, portstring);

					SumBAC+=BAC;SumNbrNonRecognized+=NbrNonRecognized;
					Sumtime+=time;SumpopulationSize+=populationSize;SumMaxBAC+=MaxBAC;
					SumMinAttSize+=MinAttSize;SumMaxAttSize+=MaxAttSize;SumAvgAttSize+=AvgAttSize;
					//printf("Detail Stat has %d lines: sumMaxAtt=%f\n",line,SumMaxAttSize);

				}
				/* start the experiments */
				FILE *outPutFile;

				/*Open files for statistics*/
//				sprintf(portstring, "Finalstat_%d.res", fld);
				//to become compatible with GRDXCS stat file
				sprintf(portstring, "Finalstat.res");
				outPutFile = (FILE *) fopen(portstring, "at");
				double n=(double)Configuration::getNR_DIFFERENT_ISLANDS();
				fprintf( outPutFile,
						"BAC:%lf;withNonRecognized:%.2f;Time:%.2lf;PopSize:%.2lf;MaxBAC:%lf;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;islandExecTime:%.2lf,Mem:%s",
						bestAuc, SumNbrNonRecognized/n , dif, SumpopulationSize/n, SumMaxBAC/n, SumAvgAttSize/n, SumMinAttSize/n, SumMaxAttSize/n, Sumtime/n, "\n");

//				printf( "Fold:%d,BAC:%lf;withNonRecognized:%.2f;Time:%.2lf;PopSize:%.2lf;MaxBAC:%lf;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;islandExecTime:%.2lf,Mem:%s",
//						fld,bestAuc, SumNbrNonRecognized/n , dif, SumpopulationSize/n, SumMaxBAC/n, SumAvgAttSize/n, SumMinAttSize/n, SumMaxAttSize/n, Sumtime/n, "\n");

//				BAC, tmpNbrNonRecognized, dif, population->size(), MaxBAC,
//				AvgAttSize, MinAttSize, MaxAttSize,
//				get_CurrentProcessUsedMem());

//				fprintf(outPutFile, "BestAUC:%f;withNonRecognized:%d\n",
//						bestAuc, nbrNonRecognized);
				fclose(outPutFile);
			}
		} // if (Configuration::getNR_DIFFERENT_ISLANDS() <= 1)
	}// no params
	return 1;
}

void RunSingleIslandProcesses(int args, char* argv[], int iFold,
		int &iNbrNonRecognized, double &BestAUC) {

//	RemovePopFiles();
//	RemoveStatisticsFiles();

	system(" rm *.dec ");

	startExperiments(args, argv);

	double islandsAUC = 0;
	//ReadEvalFiles(foldcounter,OveralExp,&TestBestAUC,&TestWorstAUC,&AvgTestAUC,nbrTotalIslands);
	DecisionBasedOnVoting("TestPred_%d.txt", iFold, iNbrNonRecognized,
			islandsAUC);

	BestAUC = islandsAUC;
}

void startExperiments(int args, char* argv[]) {
	long int iTime = 0;
	int islandNbr = 0;
	int mixingNbr = 0;
	int foldCounter = 0;

	randomize();

	if (args > 1) {
		iTime = atol(argv[3]);
		islandNbr = atoi(argv[4]);
		mixingNbr = atoi(argv[5]);
		foldCounter = atoi(argv[13]);
		/* randomize the pseudo-number generator */
		//randomize(iTime);
	}

	// Set the attributes config before loading the popultaion from file
	for (int j = 0; j < Configuration::getCONDITION_LENGTH(); j++)
		Classifier::ConditionListTypes[j] = (ConditionTypes) RealNormalized;

	//	cout << "Used Mem start:" << get_CurrentProcessUsedMem() << endl;
	//  Original_XCS *xcsIns;
	//  XCS_Spec *xcsIns;
	//  xcsIns = new Original_XCS();
	xcs *xcsIns;

	xcsIns = new xcs();

	if (mixingNbr > 0 && args > 1) // load previous Population
	{
		xcsIns->loadpopulationForTrain(argv[6]);
	}
	//	list <Classifier *> TrainData;
	list<Classifier *> *DiffFolds =
			new list<Classifier *> [Configuration::getNR_EXPS()];

	//	ReadDataFromFile(argv[2],DiffFolds);
	ReadRealNormalizedRawDataFromFile(Configuration::getDATAFILE(), DiffFolds);

	xcsIns->MaxPopSize = Configuration::getMAX_POP_SIZE();

	xcsIns->iNbrIslands = Configuration::getNR_DIFFERENT_ISLANDS();
	xcsIns->iIslandsNo = islandNbr;

	queue<double> AttsWeight;

	// Extract assigned Attributes and put them into xcsIns->responsibleAtts
	if (args > 1) {
		set<int> Atts;
		char *t1;
		for (t1 = strtok(argv[9], " -"); t1 != NULL; t1 = strtok(NULL, " -")) {
			int attIndex = atoi(t1);
			Atts.insert(attIndex);

		}
		xcsIns->responsibleAtts = Atts;
	}

	list<Classifier *> TrainData, TestData;
	// test over test set (No cross validation)
	if (Configuration::getNR_EXPS() == 1) {
		for (int f = 0; f < Configuration::getNR_EXPS(); f++) {
			for (list<Classifier *>::const_iterator i = DiffFolds[f].begin(); i
					!= DiffFolds[f].end(); i++)
				TrainData.push_back((*i));
			//DiffFolds[f].clear();
		}
		// load test file
		ReadDataFromFile(Configuration::getTESTFILE(), DiffFolds);
		for (int f = 0; f < Configuration::getNR_EXPS(); f++) {
			for (list<Classifier *>::const_iterator i = DiffFolds[f].begin(); i
					!= DiffFolds[f].end(); i++)
				TestData.push_back((*i));
			//DiffFolds[f].clear();
		}
	} else // if Cross Validation is applying
	{
		//creating Train Data and test Data Out of All Data
		for (int f = 0; f < Configuration::getNR_EXPS(); f++) {
			for (list<Classifier *>::const_iterator i = DiffFolds[f].begin(); i
					!= DiffFolds[f].end(); i++) {
				if (f == foldCounter)
					TestData.push_back((*i));
				else
					TrainData.push_back((*i));
			}
			//DiffFolds[f].clear();
		}
	}

	// Start Learning and testing
	if (args > 1) // Multiple island experiments

		xcsIns->startProgressiveCheckingExperimentEndWithPrediction(&TrainData,
				&TestData, foldCounter, mixingNbr, islandNbr, argv[10], NULL,
				argv[12]);

//			xcsIns->startOneSingleStepExperimentEndWithPrediction(&TrainData,
//					&TestData, foldCounter, mixingNbr, islandNbr, argv[10],
//					NULL /*argv[11]*/, argv[12]);
	else
		// One Single island experiment
		xcsIns->startProgressiveCheckingExperimentEndWithPrediction(&TrainData,
				&TestData, foldCounter, mixingNbr, islandNbr, "TestPred_0.txt",
				NULL, NULL );
//			xcsIns->startOneSingleStepExperimentEndWithPrediction(&TrainData,
//					&TestData, foldCounter, mixingNbr, islandNbr, "TestPred_0.txt",
//					NULL /*argv[11]*/, NULL);

	if (args > 1) {
		xcsIns->dumpPopulation(argv[6]); // save the population
	}

	//	cout << "I:" << islandNbr << " M:" << mixingNbr << " F:" << foldCounter <<
	// " Used Mem:" << get_CurrentProcessUsedMem() <<
	// "executed time:" << dif << endl;


	FreeDataList(&TrainData);
	FreeDataList(&TestData);
	TrainData.clear();
	TestData.clear();

	delete[] DiffFolds;
	delete xcsIns;
}

void *TrainOneIsland(void *ptr) {
	seperateAppOperator * x;
	x = (seperateAppOperator *) ptr;

	int indexIsland = 1;
	while (indexIsland > -1) {
		//lock islands Flags
		pthread_mutex_lock(&islandsMutex);
		indexIsland = -1;
		if (x->prtIslandFlags->size() > 0) {
			indexIsland = x->prtIslandFlags->back();
			x->prtIslandFlags->pop_back();
		}
		pthread_mutex_unlock(&islandsMutex);
		//unlock islands Flags

		if (indexIsland > -1) {
			string stAtts;
			char portstring[50];

			//			cout << "start Mix:" <<indexIsland <<endl;
			set<int> *Atts = x->responsibleAtts[indexIsland];
			for (set<int>::const_iterator i = Atts->begin(); i != Atts->end(); i++) {
				sprintf(portstring, "%d-", *i);
				stAtts.append(portstring);
			}

			//cout <<  x->minFeaturesWeight << " what?" <<stWeights.c_str() << endl;
			char *ctrCommand = new char[200 + stAtts.length()];

			sprintf(
					ctrCommand,
					"%s %s %s %ld %d %d PopIsland_%d.pop stat_%d.txt FinalStat_%d.txt %s TestPred_%d.txt Train_%d.txt BestPop_%d.pop %d ",
					x->AlgorithmName, "Config.cfg",
					Configuration::getDATAFILE(), x->nowTime, indexIsland,
					x->mixing, indexIsland, indexIsland, x->foldNo,
					stAtts.c_str(), indexIsland, indexIsland, indexIsland,
					x->foldNo);

//			cout << "island" << indexIsland << " No features:"
//					<< x->responsibleAtts[indexIsland]->size() << endl;
//			cout << ctrCommand << endl;

			system(ctrCommand);

			delete[] ctrCommand;
		}
	}
	return NULL;
}

void CreateListofIslandsProcesses(char *strAlgName, char *strParameters, const char* FRMode,
		int iFold, int &iNbrNonRecognized, double &BestAUC) {
	int MergingSubSections = (int) ceil(
			(double) Configuration::getNR_DIFFERENT_ISLANDS()
					/ ISLANDSPERSUBSECTION);

	if (MergingSubSections <= 0)
		MergingSubSections = 1;

	const int
			MaxNumberOfParralelThreads =
					Configuration::getNR_DIFFERENT_ISLANDS()
							> Configuration::getNR_PARALLEL_RUNNING_ISLANDS() ? Configuration::getNR_PARALLEL_RUNNING_ISLANDS()
							: Configuration::getNR_DIFFERENT_ISLANDS();

	seperateAppOperator *sp =
			new seperateAppOperator[MaxNumberOfParralelThreads];
	pthread_t *thread = new pthread_t[MaxNumberOfParralelThreads];

	vector<int> **RunningIslandsFlags = new vector<int> *[MergingSubSections];
	vector<int> **MergingIslandsFlags = new vector<int> *[MergingSubSections];
	set<int> **AttsDistributing =
			new set<int> *[Configuration::getNR_DIFFERENT_ISLANDS()];
	// used in Q part only
	set<double> **FeaturesWeight =
			new set<double> *[Configuration::getNR_DIFFERENT_ISLANDS()];

	//	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++)
	//			AttsDistributing[i] = new set<int> ();

	double lstBestTestAUC = 0;
	int lstiNbrNonRecognized = -1;
	double minWeightedFeature = -1;

	double *accuracies = new double[Configuration::getNR_DIFFERENT_ISLANDS()];

	for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++)
		accuracies[i] = 0;

	//Fixed feature allocation
	if (strchr(strParameters, 'F') != NULL || strchr(strParameters, 'f')
			!= NULL) {
		// New Feature ranking based Partitioning
		if (strchr(strParameters, 'T') != NULL || strchr(strParameters, 't') != NULL)
		{
			queue <int> lstFeatures = RankFeatures(iFold,FRMode);
			//Iterative feature allocation
			if (strchr(strParameters, 'I') != NULL || strchr(strParameters, 'i')
					!= NULL)
				TIterativePartition(AttsDistributing, 0,lstFeatures );
			//Random feature allocation
			if (strchr(strParameters, 'R') != NULL || strchr(strParameters, 'r')
					!= NULL)
				TRoundRobinPartition(AttsDistributing, 0,lstFeatures );
		}
		else{ // Old style Feature partitioning
				//Iterative feature allocation
				if (strchr(strParameters, 'I') != NULL || strchr(strParameters, 'i')
						!= NULL)
					IterativePartition(AttsDistributing, 0);
				//Random feature allocation
				if (strchr(strParameters, 'R') != NULL || strchr(strParameters, 'r')
						!= NULL)
					RandomPartition(AttsDistributing, 0);
		}

	}

	for (unsigned int mixCount = 0; mixCount
			< Configuration::getNR_MIXING_DIFFERENT_POPULATION(); mixCount++) {
		randomize();
		//system(" rm *.dec ");

		//CreateDataFile("Multiplexer.arff");

		//		cout << "start Mix:" << mixCount << " of "
		//				<< Configuration::getNR_MIXING_DIFFERENT_POPULATION() << endl;
		for (int k = 0; k < MergingSubSections; k++) {
			RunningIslandsFlags[k] = new vector<int> ();
			MergingIslandsFlags[k] = new vector<int> ();
		}
		//Configuration::getNR_DIFFERENT_ISLANDS()
		for (unsigned int i = 0; i < Configuration::getNR_DIFFERENT_ISLANDS(); i++) {
			int indexRand = 0;

			// for random running islands if number of islands are more than 50
			if (Configuration::getNR_DIFFERENT_ISLANDS() > ISLANDSPERSUBSECTION)
				indexRand = (int) (frand() * MergingSubSections);

			if (indexRand == MergingSubSections)
				indexRand--;

			RunningIslandsFlags[indexRand]->push_back(i);
			MergingIslandsFlags[indexRand]->push_back(i);
		}

		//Dynamic feature allocation
		if (strchr(strParameters, 'D') != NULL || strchr(strParameters, 'd')
				!= NULL) {
			// New Feature ranking based Partitioning
			if (strchr(strParameters, 'T') != NULL || strchr(strParameters, 't') != NULL)
			{
				queue <int> lstFeatures = RankFeatures(iFold,FRMode);
				//Iterative feature allocation
				if (strchr(strParameters, 'I') != NULL || strchr(strParameters, 'i')
						!= NULL)
					TIterativePartition(AttsDistributing, mixCount,lstFeatures );
				//Random feature allocation
				if (strchr(strParameters, 'R') != NULL || strchr(strParameters, 'r')
						!= NULL)
					TRoundRobinPartition(AttsDistributing, mixCount,lstFeatures );
			}
			else{ // Old style Feature partitioning
				//Iterative feature allocation
				if (strchr(strParameters, 'I') != NULL
						|| strchr(strParameters, 'i') != NULL)
					IterativePartition(AttsDistributing, mixCount);
				//Random feature allocation
				if (strchr(strParameters, 'R') != NULL
						|| strchr(strParameters, 'r') != NULL)
					RandomPartition(AttsDistributing, mixCount);
			}

		}

		long int nowTime = time(NULL);
		for (int k = 0; k < MergingSubSections; k++) {
			for (unsigned int i = 0; i < MaxNumberOfParralelThreads; i++) {
				sp[i].nowTime = nowTime;
				sp[i].mixing = mixCount;
				sp[i].prtIslandFlags = RunningIslandsFlags[k];
				sp[i].nbrTotalIslands
						= Configuration::getNR_DIFFERENT_ISLANDS();
				sp[i].AlgorithmName = strAlgName;
				sp[i].responsibleAtts = AttsDistributing;
				sp[i].foldNo = iFold;
				sp[i].minFeaturesWeight = minWeightedFeature;
				if (minWeightedFeature > 0)
					sp[i].featuresWeight = FeaturesWeight;
				else
					sp[i].featuresWeight = NULL;
			}

			/* Create independent threads each of which will execute function */
			for (unsigned int i = 0; i < MaxNumberOfParralelThreads; i++) {
				pthread_create(&thread[i], NULL, TrainOneIsland, (void*) &sp[i]);
			}

			for (unsigned int i = 0; i < MaxNumberOfParralelThreads; i++) {
				/* Wait till threads are complete before main continues. Unless we
				 * wait we run the risk of executing an exit which will terminate
				 * the process and all threads before the threads have completed.   */
				//cout << "Join training Island Thread:" << i << " for " << mixCount<< "th Mixing of "<< foldcounter <<"th Fold"<< endl;
				pthread_join(thread[i], NULL);
			}

			// Migrate
			if ((strchr(strParameters, 'M') != NULL || strchr(strParameters,
					'm') != NULL)
					&& Configuration::getPORTION_OF_MIGRATABLE_INDIVIDUAL() > 0
					&& Configuration::getNR_DIFFERENT_ISLANDS() > 1 // Multi Population
					&& mixCount
							< (Configuration::getNR_MIXING_DIFFERENT_POPULATION()
									- 1) // Last Mixing is ignored
			) {
				//MigrateBtwnPops(MergingIslandsFlags[k]);

				for (int attIndx = 0; attIndx
						< Configuration::getCONDITION_LENGTH(); attIndx++)
					Classifier::ConditionListTypes[attIndx]
							= (ConditionTypes) RealNormalized;

				//				cout << "Migration Start:" << endl;
				if (strchr(strParameters, 'B') != NULL || strchr(strParameters,
						'b') != NULL) { // Only Best Migrated
					if (strchr(strParameters, 'A') != NULL || strchr(
							strParameters, 'a') != NULL) { // To All pops
						BestMigrateToAllPops(MergingIslandsFlags[k]);
					} else
						BestMigrateBtwnPops(MergingIslandsFlags[k]);
				} else {
					if (strchr(strParameters, 'A') != NULL || strchr(
							strParameters, 'a') != NULL) { // To All pops
						MigrateAllBtwnPops(MergingIslandsFlags[k]);
					} else
						MigrateBtwnPops(MergingIslandsFlags[k]);
				}
				//MakeSamePops(MergingIslandsFlags[k]);
				//				cout << "Migration End." << endl;
				//remove Only BestPops
				//system(" rm Best*.pop ");
			}

			// Evaluate all islands and reconfigure atts assignments
			//double bestAccuracy=0;
			//bestAccuracy = ReconfigureAttributeAssignment(accuracies,lastAddedAtts,AttsDistributing);
			//cout << "BestAccurcy among islands:" << bestAccuracy << endl;

		} // for k
		// Decision and accuracy measure based on voting
		//DecisionBasedOnVoting("TestPred_%d.txt", iFold, iNbrNonRecignozed, BestAuc);

		// When all islands finish their internal learning
		double islandsAUC = 0;
		int tmpNbrNonRecognized;
		//ReadEvalFiles(foldcounter,OveralExp,&TestBestAUC,&TestWorstAUC,&AvgTestAUC,nbrTotalIslands);
		DecisionBasedOnVoting("TestPred_%d.txt", iFold, tmpNbrNonRecognized,
				islandsAUC);

		// Calculating Overal performance of learning through Mixing

		// Keep track of best AUC in all mixings
		if ((lstiNbrNonRecognized < 0) || (lstBestTestAUC <= islandsAUC
				&& lstiNbrNonRecognized >= tmpNbrNonRecognized))
		//if ((lstiNbrNonRecognized <0) || (lstBestTestAUC < islandsAUC) || (lstiNbrNonRecognized > tmpNbrNonRecognized && lstiNbrNonRecognized <= tmpNbrNonRecognized))
		{
			lstBestTestAUC = islandsAUC;
			lstiNbrNonRecognized = tmpNbrNonRecognized;
		}

		//		cout << "Last Train AUC:"<< AvgTrainAUC << "Last AUC:"<< AvgTestAUC << "Best AUC:" << lstTestAverageAUCperMixing << endl;
		for (int k = 0; k < MergingSubSections; k++) {
			delete RunningIslandsFlags[k];
			delete MergingIslandsFlags[k];
		}


// reading the average ACC over all islands
		double MinAttSize, MaxAttSize, AvgAttSize, time;
		double BAC,MaxBAC;
		int NbrNonRecognized, populationSize;
		double SumMinAttSize=0, SumMaxAttSize=0, SumAvgAttSize=0, Sumtime=0;
		double SumBAC=0,SumMaxBAC=0;
		int SumNbrNonRecognized=0, SumpopulationSize=0;

		char portstring[256];
		int line;
		char memSize[64];
		for(unsigned int islandNo=0;islandNo< Configuration::getNR_DIFFERENT_ISLANDS(); islandNo++)
		{
			sprintf(portstring, "Detailstat_%d.res", islandNo);

			line = readLastIslandsStat(&BAC,&NbrNonRecognized,&time, &populationSize, &MaxBAC, &MinAttSize,&MaxAttSize, &AvgAttSize, memSize, portstring);
			SumBAC+=BAC;SumNbrNonRecognized+=NbrNonRecognized;
			Sumtime+=time;SumpopulationSize+=populationSize;SumMaxBAC+=MaxBAC;
			SumMinAttSize+=MinAttSize;SumMaxAttSize+=MaxAttSize;SumAvgAttSize+=AvgAttSize;
		}

		double n=(double)Configuration::getNR_DIFFERENT_ISLANDS();
		printf( "BAC:%lf;withNonRecognized:%.2f;Time:%.2lf;PopSize:%.2lf;MaxBAC:%lf;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;islandExecTime:%.2lf,Mem:%s",
				SumBAC/n, SumNbrNonRecognized/n , -1.0, SumpopulationSize/n, SumMaxBAC/n, SumAvgAttSize/n, SumMinAttSize/n, SumMaxAttSize/n, Sumtime/n, "\n");


		// if it reached to AUC=1 No further execution
//		if (lstBestTestAUC == 1) {
//			if (mixCount < Configuration::getNR_MIXING_DIFFERENT_POPULATION())
//				mixCount++;
//			break;
//		}


		/* start the experiments */
		FILE *outPutFile;

		/*Open files for statistics*/
		sprintf(portstring, "DettailVoteStat_%d.res", iFold);
		outPutFile = (FILE *) fopen(portstring, "at");
		fprintf(outPutFile, "BestAUC:%f,NonRec:%d;CurrentAUC:%f,NonRec:%d,NonVotedAvgAuc:%f,NonVotedAvgBestAUC:%f\n",
				lstBestTestAUC, lstiNbrNonRecognized, islandsAUC,
				tmpNbrNonRecognized,SumBAC/n , SumMaxBAC/n);
		fclose(outPutFile);

	} // for mixing


	for (unsigned int t = 0; t < Configuration::getNR_DIFFERENT_ISLANDS(); t++)
		delete AttsDistributing[t];

	// used in Q part only
	if (minWeightedFeature > 0)
		for (unsigned int t = 0; t < Configuration::getNR_DIFFERENT_ISLANDS(); t++)
			delete FeaturesWeight[t];

	BestAUC = lstBestTestAUC;
	iNbrNonRecognized = lstiNbrNonRecognized;

	delete[] sp;
	delete[] thread;
	delete[] RunningIslandsFlags;
	delete[] MergingIslandsFlags;
	delete[] AttsDistributing;
	delete[] accuracies;
	delete[] FeaturesWeight;
}

// Read the last line of each island statistics
int readLastIslandsStat(double *BAC,int * NbrNonRecognized,double *time, int *populationSize, double *MaxBAC,double *MinAttSize,double *MaxAttSize, double *AvgAttSize, char* memSize, char * statFileName)
{

	/* start the experiments */
	FILE *outPutFile;
	int line=0;
	/*Open files for statistics*/
	outPutFile = (FILE *) fopen(statFileName, "rt");

	if (outPutFile == NULL) {
		fprintf(stderr, "Cannot open file(%s)\n",statFileName);
		return -1;
	}

	while (	fscanf( outPutFile,
			"BAC:%lf;withNonRecognized:%d;Time:%lf;PopSize:%d;MaxBAC:%lf;AvgAttSize:%lf;MinAttSize:%lf;MaxAttSize:%lf;Mem:%s\n",
			BAC, NbrNonRecognized, time, populationSize, MaxBAC,
			AvgAttSize, MinAttSize, MaxAttSize, memSize)
			==9  )
	{
		line++;
	}

	fclose(outPutFile);

	return line;
}


queue<int> RankFeatures(int fld, const char* FRMode) {

	list<Classifier *> DiffFolds[Configuration::getNR_EXPS()];
	queue<int> features;

	// read all data file into folded Parts
	ReadRealNormalizedRawDataFromFile(Configuration::getDATAFILE(), DiffFolds);

	WekaFilterCommandType filter;
	filter= WekaInformationGain;

	//fld  Fold corss validation

	list<Classifier *> TrainData, TestData;

	// test over test set (No cross validation)
	//creating Train Data and test Data Out of All Data
	for (int f = 0; f < Configuration::getNR_EXPS(); f++) {
		for (list<Classifier *>::const_iterator i = DiffFolds[f].begin(); i
				!= DiffFolds[f].end(); i++) {
			if (f == fld)
				TestData.push_back((*i));
			else
				TrainData.push_back((*i));
		}
	}


	string tmpTrainDataSet("tmpTrainDataset.arff");
	set<int> workingFeatures;
	SaveRealNormalizedRawDataToARFFFile(tmpTrainDataSet.c_str(),
			&TrainData, workingFeatures);


	if (strstr(FRMode,"Info") != NULL)
		filter = WekaInformationGain;
	else if (strstr(FRMode,"Ratio") != NULL)
		filter = WekaGainRatio;
	else if (strstr(FRMode,"SVM") != NULL)
		filter = WekaSVMEval;
	else if (strstr(FRMode,"Sub") != NULL)
		filter = WekaSubsetEval;
	else if (strstr(FRMode,"One") != NULL)
		filter = WekaOneRule;
	else if (strstr(FRMode,"ReliefF") != NULL)
		filter = WekaReliefF;


	features=wekaRankFeatureSelection(filter,tmpTrainDataSet.c_str());
		//xcsIns->NoFeatureSelection();


		TrainData.clear();
		TestData.clear();

	for (int f = 0; f < Configuration::getNR_EXPS(); f++) {
		for (list<Classifier *>::const_iterator i = DiffFolds[f].begin(); i
				!= DiffFolds[f].end(); i++) {
			delete (*i);
		}
		DiffFolds[f].clear();
	}

	return features;
}


queue <int> wekaRankFeatureSelection(WekaFilterCommandType filterType,
		const char * stDataFileName) {
	char portstring[2024];
	char *FilterCommand;

	switch (filterType) {
	case WekaGainRatio:
		FilterCommand
				= "weka.attributeSelection.GainRatioAttributeEval -s \"weka.attributeSelection.Ranker -T -1.7976931348623157E308 -N -1\" ";
		break;
	case WekaInformationGain:
		FilterCommand
				= "weka.attributeSelection.InfoGainAttributeEval -s \"weka.attributeSelection.Ranker -T -1.7976931348623157E308 -N -1\" ";
		break;
	case WekaSubsetEval:
		FilterCommand
				= "weka.attributeSelection.CfsSubsetEval -s \"weka.attributeSelection.BestFirst -D 1 -N 5\" ";
		break;
	case WekaSVMEval:
		FilterCommand
				= "weka.attributeSelection.SVMAttributeEval -s \"weka.attributeSelection.Ranker -T -1.7976931348623157E308 -N -1\" -X 1 -Y 0 -Z 0 -P 1.0E-25 -T 1.0E-10 -C 1.0 -N 0 ";
		break;
	case WekaReliefF:
		FilterCommand
				= "weka.attributeSelection.ReliefFAttributeEval -s \"weka.attributeSelection.Ranker -T -1.7976931348623157E308 -N -1\" -M -1 -D 1 -K 10 ";
		break;
	case WekaOneRule:
		FilterCommand
				= "weka.attributeSelection.OneRAttributeEval -s \"weka.attributeSelection.Ranker -T -1.7976931348623157E308 -N -1\" -S 1 -F 10 -B 6 ";
		break;

	}

	sprintf(
			portstring,
			"java -Xmx2048M  -cp %s %s -i %s -c last > AttributeRankOutput.txt",
			WEKAPATH, FilterCommand, stDataFileName);
	system(portstring);
	return ReadWekaRankFeatureOutputFile();
}


queue <int> ReadWekaRankFeatureOutputFile() {
	FILE *outputFile;
	char Line[22500], attIndexs[22500];
	queue <int> lstFeatures;

	/*Open files for ranked features*/

	outputFile = (FILE *) fopen("AttributeRankOutput.txt", "rt");

	if (outputFile == NULL) {
		fprintf(stderr, "Cannot open AttributeRankOutput");
		exit(0);
	}

//	for (unsigned int i = 0; i < Configuration::getCONDITION_LENGTH(); i++)
//		//FeaturesWieght[i] = (double) 1.0 / ( Configuration::getCONDITION_LENGTH()*2);
//		FeaturesWieght[i] = (double) (MIN_PROB);

	int counts = 0;

	while (!feof(outputFile)) {
		fgets(Line, sizeof Line, outputFile);
		//cout << Line << endl;
		if (strstr(Line, "Selected attributes:") != NULL) // feature Ranked List
		{
			sscanf(Line, "Selected attributes: %s", attIndexs);
			char *t1;

			//			cout << attIndexs << endl;

			int min = 0, max = 0;

			for (t1 = strtok(attIndexs, " ,:"); t1 != NULL; t1 = strtok(NULL,
					" ,:")) {

				int index = atoi(t1);
				index--; // c index type
				lstFeatures.push(index);

				counts++;
			}

		}
	}

	//cout << "Finish" << endl;
	if (counts == 0) {
		fprintf(stderr, "AttributeRankOutput is empty");
		exit(0);
	}
	//	Selected attributes:
	fclose(outputFile);

	return lstFeatures;
}


//void testReadingFile()
//{
//	int fld=0;
//	double MinAttSize, MaxAttSize, AvgAttSize, time;
//	double BAC,MaxBAC;
//	int NbrNonRecognized, populationSize;
//	double SumMinAttSize=0, SumMaxAttSize=0, SumAvgAttSize=0, Sumtime=0;
//	double SumBAC=0,SumMaxBAC=0;
//	int SumNbrNonRecognized=0, SumpopulationSize=0;
//
//	int line=0;
//	char portstring[256];
//	char memSize[64];
//	for(unsigned int islandNo=0;islandNo< Configuration::getNR_DIFFERENT_ISLANDS(); islandNo++)
//	{
//		int tStepId = Configuration::getNR_MIXING_DIFFERENT_POPULATION() * Configuration::getMAX_NR_STEPS() -100;
//		sprintf(portstring, "Detailstat_%d_%d.res", islandNo, tStepId );
//
//		line = readLastIslandsStat(&BAC,&NbrNonRecognized,&time, &populationSize, &MaxBAC, &MinAttSize,&MaxAttSize, &AvgAttSize, memSize, portstring);
//
//		SumBAC+=BAC;SumNbrNonRecognized+=NbrNonRecognized;
//		Sumtime+=time;SumpopulationSize+=populationSize;SumMaxBAC+=MaxBAC;
//		SumMinAttSize+=MinAttSize;SumMaxAttSize+=MaxAttSize;SumAvgAttSize+=AvgAttSize;
//
//	}
//
//	double n=(double)Configuration::getNR_DIFFERENT_ISLANDS();
//	printf( "BAC:%lf;withNonRecognized:%d;Time:%.2lf;PopSize:%f;MaxBAC:%lf;AvgAttSize:%.2lf;MinAttSize:%.2lf;MaxAttSize:%.2lf;Mem:0K\n",
//			BAC, SumNbrNonRecognized/n , 0, SumpopulationSize/n, SumMaxBAC/n, SumAvgAttSize/n, SumMinAttSize/n, SumMaxAttSize/n);
//
//}
