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

#ifndef Original_XCS_H_
#define Original_XCS_H_
#include "Configuration.h"
#include "Classifier.h"
#include "Definitions.h"
#include "Utilities.h"
#include <list>
#include <vector>
#include <set>
#include <map>
#include <math.h>
#include <stdlib.h>

using namespace std;

class Original_XCS {
public:
	long int MaxPopSize;
	int iNbrIslands;
	int iIslandsNo;
	set<int> responsibleAtts;
    unsigned long int getIteratorTime();
    void setIteratorTime(unsigned long int value);
    Original_XCS();
    virtual ~Original_XCS();
    void restart(void);
    virtual void startOneSingleStepExperiment(char *statFileName, int mixingNo, list<Classifier*> TrainData);
    bool EvaluateClass(Classifier *cl, int length, int lAddress);
    void resetStateMultiplexer(Classifier *cl);
    void dumpPopulation(const char *fileName);
    void dumpPopulationCompressed(char *fileName);
    void loadpopulationForTrain(char *fileName);
    //void loadpopulation(char *fileName);
    void dumpClassifierSet(list<Classifier*> *set);
    void dumpClassifierSet(list<Classifier*> *set, FILE *dumpFile);
    Classifier *getClassifer(long int index);
    long int getPopSize(void);
    long int getSetSize(list<Classifier*> *set);
    set<int> findFixedLengthAttributeRules(int iLength);
protected:
    unsigned long int iteratorTime;
    list<Classifier*> *population;
//    virtual void MakeDontCare(Classifier * childs[2]);
    virtual void MakeDontCare(Classifier * childs[2], Classifier * trnCls);
    virtual void doOneSingleStepProblemExplore(Classifier *trnClassifier, unsigned long int trialCounter);
    virtual void doOneSingleStepProblemExploit(Classifier *trnClassifier, unsigned long int trialCounter, int *correct, double *sysError, char *EvalFileName);
    int nrActionsInSet(list<Classifier*> *mset, int *coveredActions);
    virtual void getMatchSet(list<Classifier*> *mset, map<int,xCondition*> *state, int action, unsigned long int itTime);
    double doMPAction(Classifier *state, int pAction, int *correct);
    int ToDecimal(bool *binary, int length);
    void writePerformance(char *statFileName, int *correct, double *sysError, int counter, int TESTFREQUENCY);
    virtual void discoveryComponent(list<Classifier*> *acSet, unsigned long int itTime, Classifier *trnClassifier);
    int subsumes(Classifier *cl1, Classifier *cl2, int thetaSub);
    int isSubsumer(Classifier *cl, int thetaSub);
    void subsumeClassifier(Classifier *cl, Classifier **parents, list<Classifier*> *locset, int thetaSub);
    int subsumeClassifierToSet(Classifier *cl, list<Classifier*> *set, int thetaSub);
    virtual void insertDiscoveredClassifier(Classifier **cl, Classifier **parents, list<Classifier*> *set, int thetaSub);
    virtual bool CheckPredictionArrays(list<Classifier*> *mset);
    bool match(map<int,xCondition*> *m, map<int,xCondition*> *c);
    virtual float getDelProp(Classifier *cl, long double meanf, long double meane);
    void setInitialMeanVariables(Classifier *cl, long int itTime);
    bool addNewClassifierToSet(Classifier *cl, list<Classifier*> *clSet);
    bool addClassifierToSet(Classifier *cl, list<Classifier*> *clSet);
    virtual void deleteStochClassifier(char *filename, unsigned long int itTime);
    void freeClassifierSet(list<Classifier*> *AllData);
    void getPredictionArray(list<Classifier*> *ms, long double *pa);
    int actionWinner(long double *predictionArray);
    int randomActionWinner(long double *predictionArray);
    virtual int deterministicActionWinner(long double *predictionArray);
    int rouletteWheelActionWinner(long double *predictionArray);
    void updateFitness(list<Classifier*> *acSet);
    void adjustActionSet(list<Classifier*> *acSet, float maxP, float reward);
    void getActionSet(int action, list<Classifier*> *ms, list<Classifier*> *acSet, list<Classifier*> *acNonSet);
    void getDiscoversSums(list<Classifier*> *acSet, long double & sum, unsigned long int & meangait, unsigned long int & mssum, unsigned long int itTime);
    void setTimeStamps(list<Classifier*> *acSet, unsigned long int itTime);
	virtual int selectTwoClassifiers(Classifier **parents, list<Classifier*> *acSet,long double fitsum);
	Classifier * selectClassifierUsingRWS(list<Classifier*> *set,long double fitsum);
	int selectClassifierUsingRWS(Classifier **parents, list<Classifier*> *acSet,long double fitsum);
	unsigned int randPickClassifiers(vector<long double> &fitList,long double sumFit);
	Classifier * selectClassifierUsingTournamentSelection(list<Classifier*> *SelectionSet, int setsum, Classifier *NotMe);

//	virtual void discoveryComponent(list<Classifier*> *acSet,long unsigned int itTime, int islandNo);
	void sbxCalculateBtwNbr(float &p1, float &p2, float min, float max);
	virtual int crossover(Classifier **cl);
	virtual int mutation(Classifier *cl,int minIndex, int maxIndex, Classifier * trnClassifier);
	virtual int mutation(Classifier *cl,Classifier * trnClassifier);
	virtual void mutateAction(Classifier *cl);
	void freeClassifier(Classifier *cl);
	float get_beta(float u);
	void maintainPopulationSize(FILE *tabFile);
	void insertMoreGeneralClassifier(Classifier *cl);

	void getAttibutesSize(double & MinValue, double & MaxValue ,double & AvgValue);
};

#endif /* XCS_H_ */
