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

#ifndef UTILITIESFUNCTIONS_H
#define UTILITIESFUNCTIONS_H

#include <list>
#include "Classifier.h"
#include "Definitions.h"
#include "Configuration.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <list>
#include <vector>
#include <queue>

#define _MSeed 2147483647     /* modulus of PMMLCG (default=2147483647=2^31 - 1)*/
#define _ASeed 16807          /* default=16807*/
#define ISLANDSPERSUBSECTION 1000

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

static long _QSeed = _MSeed / _ASeed; /* M / A */
static long _RSeed = _MSeed % _ASeed; /* M mod A */
static long _seed; /* a number between 1 and m-1 */
static int haveUniNum = 0;
static double uniNum = 0;

struct seperateAppOperator {
	//	int overalExp;
	int mixing;
	int nbrTotalIslands;
	vector<int> *prtIslandFlags;
	char * AlgorithmName;
	long int nowTime;
	set<int> **responsibleAtts;
	int foldNo;
	set<double> **featuresWeight;
	double minFeaturesWeight;
};

void randomize(void);
void randomize(long int iTime);
double urand();
void ReadDataFromFile(char *dataFileName, list<Classifier *> *dataInFolds);
void ReadBinaryRawDataFromFile(char *dataFileName,
		list<Classifier *> *RawDataList);
int ReadRealNormalizedRawDataFromFile(const char *dataFileName, list<
		Classifier *> *dataInFolds);
int ReadRealNormalizedRawDataCreateValidationDataSetsRandomlyFromFile(
		const char *dataFileName, list<Classifier *> *TrainData, list<
				Classifier *> *TestData, int TrainNum, int TestNum);
int ReadRealDataListFromFileForAnalysisOfFeatures(const char *dataFileName,
		list<Classifier *> *dataList);
void SaveDataToFile(char *dataFileName, list<Classifier *> *AllData);
int SaveRealNormalizedRawDataToARFFFile(const char *dataFileName, list<
		Classifier *> *dataList, set<int> iIncludedAttributes);

void FreeDataList(list<Classifier *> *DataList);

long double absLongDouble(long double value);
float absFloat(float value);

long int get_SystemFreeMem(void);
long int get_SystemUsedMem(void);
const char * get_CurrentProcessUsedMem(void);
bool CalculateAUC(double *AUC, double *SE, double *AVGAUC, double *AVGSE,
		int normalRated[], int AbNormalRated[]);
int
		calculateAUCByVoting(double *AUC, double *SE, double *AVGAUC,
				double *AVGSE, map<int, int *> &predictionMap,
				map<int, int> &ActionMap);
int calculateBACBinaryByVoting(map<int, int*> &predictionMap,
		map<int, int> &ActionMap, double &BAC);

void MigrateBtwnPops(vector<int> *MergingIslandsList);
void BestMigrateBtwnPops(vector<int> *MergingIslandsList);
void BestMigrateToAllPops(vector<int> *MergingIslandsList);
void MigrateAllBtwnPops(vector<int> *MergingIslandsList);
bool compare_classifiers_Acc(Classifier *first, Classifier *second);
void DecisionBasedOnVoting(char * strFormat, int iFold, int &NonRecognixed,
		double & BestAUC);
void MakeSamePops(vector<int> *MergingIslandsList);
void RandomPartition(set<int> **AttsDistributing, unsigned int mixCount);
void IterativePartition(set<int> **AttsDistributing, unsigned int mixCount);
void TRoundRobinPartition(set<int> **AttsDistributing, unsigned int mixCount, queue <int> lstFeatures);
void TIterativePartition(set<int> **AttsDistributing, unsigned int mixCount, queue <int> lstFeatures);


void ReadEvalFiles(int foldcounter, int OveralExp, double *outTestBestAUC,
		double *outTestWorstAUC, double *outAvgTestAUC,
		unsigned int nbrTotalIslands);
//double ReadPredictionFilesAndVote(int foldcounter, int OveralExp, unsigned int nbrTotalIslands);
double ReadPredictionFilesAndVote(int foldcounter, int OveralExp,
		unsigned int nbrTotalIslands, int test);
double ReconfigureAttributeAssignment(double *accuracies,
		list<int> *lastAddedAtts, set<int> **AttsDistributing);
float extractAUC(char * Line);

//#define _M 2147483647     /* modulus of PMMLCG (default=2147483647=2^31 - 1)*/
//#define _A 16807          /* default=16807*/
//
//double urand();
//double nrand();
//
//long setSeed(long newSeed);
//void randomizeAtr(void);
#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )

void corcol(double **data, int n, int m, double ** symmat);
void covcol(double **data, int n, int m, double **symmat);
void scpcol(double **data, int n, int m, double **symmat);
void erhand(char err_msg[]);
double *Ovector(int n);
double **Omatrix(int n, int m);
void free_vector(double *v, int n);
void free_matrix(double **mat, int n, int m);
void tred2(double **a, int n, double *d, double *e);
void tqli(double d[], double e[], int n, double **z);

double accessGetMatrix(double ** mat, int i, int j);
void accessSetMatrix(double ** mat, int i, int j, double value);

double **Omatrix(int n, int m);

int getNumberOfCPUs();

#endif
