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

#include "Original_XCS.h"

//#define TESTFREQUENCY 200

using namespace std;

Original_XCS::Original_XCS() {
	// TODO Auto-generated constructor stub
	this->population = new list<Classifier*> ();
	this->iNbrIslands = 1;
	this->iIslandsNo = -1;
	this->iteratorTime = 0;
}

Original_XCS::~Original_XCS() {
	// TODO Auto-generated destructor stub
	restart();
}

void Original_XCS::restart(void) {
	Classifier * cl;

	while (!population->empty()) {
		cl = population->front();
		population->pop_front();
		freeClassifier(cl);
	}

	population->clear();
}

long unsigned int Original_XCS::getIteratorTime() {
	return iteratorTime;
}
void Original_XCS::setIteratorTime(long unsigned int value) {
	if (value > iteratorTime)
		iteratorTime = value;
}

void Original_XCS::startOneSingleStepExperiment(char *statFileName,
		int mixingNo, list<Classifier *> TrainData) {
	long unsigned int trialCounter;
	int exploit = 1;
	int TESTFREQUENCY = 200;
	//int TESTFREQUENCY = Configuration::getMAX_NR_STEPS();
	int *correct = new int[TESTFREQUENCY];
	double *sysError = new double[TESTFREQUENCY];

	/* Start one experiment, trialCounter counts the number of learning problems (trials) */
//	trialCounter = 0;
	trialCounter = mixingNo*Configuration::getMAX_NR_STEPS();

//	for (trialCounter = 0; trialCounter < Configuration::getNR_EXPS(); trialCounter
//			+= exploit) {
//		//	  for(trialCounter=0; trialCounter < 100 ; trialCounter+=exploit) {
//
//		/* change from explore to exploit and backwards */
//		exploit = (exploit + 1) % 2;
//
//		Classifier *cl = new Classifier();
//		resetStateMultiplexer(cl);
//		if (!exploit)
//			/* learn from the problem instance */
//			doOneSingleStepProblemExplore(cl, trialCounter);
//		else {
//			/* test on the problem instance */
//			doOneSingleStepProblemExploit(cl, trialCounter,
//					&correct[trialCounter % (TESTFREQUENCY)],
//					&sysError[trialCounter % (TESTFREQUENCY)], NULL);
//		}
//
//		delete cl;
//	}

	char portstring[256];
	sprintf(portstring, "Decision_%d.dec", this->iIslandsNo);

	/* Start one experiment, trialCounter counts the number of learning problems (trials) */
	//trialCounter=0;
	for (list<Classifier *>::iterator i = TrainData.begin(); i
			!= TrainData.end(); trialCounter += exploit) {
		//	for (trialCounter = 0; trialCounter < Configuration::getMAX_NR_STEPS(); trialCounter
		//			+= exploit) {

		/* change from explore to exploit and backwards */
		exploit = (exploit + 1) % 2;

		/* get the next problem instance */
		Classifier *cl = (*i);
		//Classifier *cl = new Classifier();
		//resetStateMultiplexer(cl);

		if (!exploit)
			/* learn from the problem instance */
			doOneSingleStepProblemExplore(cl, trialCounter);
		else {
			/* test on the problem instance */
			doOneSingleStepProblemExploit(cl, trialCounter,
					&correct[trialCounter % (TESTFREQUENCY)],
					&sysError[trialCounter % (TESTFREQUENCY)], portstring);
		}

		//delete cl;
		i++;
		/* monitor performance in data file every 'testFrequency' steps */
//		if (trialCounter % TESTFREQUENCY == 0 && exploit && trialCounter > 0) {
//			writePerformance(statFileName, correct, sysError, trialCounter,
//					TESTFREQUENCY);
//		}

	}

//	trialCounter += mixingNo*Configuration::getMAX_NR_STEPS();
	/* check if final performance should be written */
//	if (trialCounter % TESTFREQUENCY == 0 && exploit && trialCounter > 0) {
		writePerformance(statFileName, correct, sysError, trialCounter,
				TESTFREQUENCY);
//	}

	delete[] correct;
	delete[] sysError;
}

void Original_XCS::doOneSingleStepProblemExplore(Classifier * trnClassifier,
		long unsigned int trialCntr) {
	int action = 0;
	float reward = 0.0, nonReward = 0.0;

	long double *predictionArray =
			new long double[Configuration::getNUMBER_OF_ACTIONS()];

	list<Classifier*> *mset, *aset, *nonAset;
	mset = new list<Classifier*> ();
	aset = new list<Classifier*> ();
	nonAset = new list<Classifier*> ();

	long unsigned int trialCounter = this->iteratorTime;

	// Unless it goes overflow
	if (trialCounter + trialCntr > trialCounter)
		trialCounter += trialCntr;

	/* get the match set */
	//cout << "No res Atts:" << responsibleAtts.size() ;
	getMatchSet(mset, &trnClassifier->ConditionList, trnClassifier->getAct(),
			trialCounter);

	/* get the Prediction array */
	getPredictionArray(mset, predictionArray);
	//cout << " mset size:" <<mset.size() << endl;

	/* Get the action, that wins in the prediction array */
	action = actionWinner(predictionArray);
	//action = trnClassifier->act;

	/* execute the action and get reward
	 * correct represents a boolean for the right or wrong action */
	int correct = 0;
	reward = doMPAction(trnClassifier, action, &correct);

	/* Get the action set according to the chosen action aw */
	getActionSet(action, mset, aset, nonAset);

	/* Give immediate reward */
	adjustActionSet(aset, 0, reward);
	//adjustActionSet(&nonAset,0,nonReward);

	discoveryComponent(aset, trialCounter, trnClassifier);

	/* Clean up */
	mset->clear();
	aset->clear();
	nonAset->clear();
	delete mset;
	delete aset;
	delete nonAset;
	delete[] predictionArray;
}

void Original_XCS::doOneSingleStepProblemExploit(Classifier * trnClassifier,
		long unsigned int trialCntr, int *correct, double *sysError,
		char * EvalFileName) {
	/* start the experiments */
	FILE *tabFile = NULL;

	/*Open files for statistics*/
	if (EvalFileName != NULL) {
		tabFile = (FILE *) fopen(EvalFileName, "at");

		if (tabFile == NULL) {
			fprintf(stderr, "Cannot open %s Out file", EvalFileName);
			return;
		}
	}

	int action = -1;
	long double reward = 0.0;
	long double *predictionArray =
			new long double[Configuration::getNUMBER_OF_ACTIONS()];
	long unsigned int trialCounter = trialCntr + this->iteratorTime;

	list<Classifier*> mset, aset, nonAset;

	/* get the match set */
	getMatchSet(&mset, &trnClassifier->ConditionList, trnClassifier->getAct(),
			trialCounter);

	if (mset.size() > 0) {
		/* get the Prediction array */
		getPredictionArray(&mset, predictionArray);
		//cout << " mset size:" <<mset.size() << endl;

		/* Get the action, that wins in the prediction array */
		action = deterministicActionWinner(predictionArray);
		//action=actionWinner(predictionArray);

		/* Get the action set according to the chosen action aw */
		//getActionSet(action, &mset , &aset, &nonAset);

		/* execute the action and get reward */
		reward = doMPAction(trnClassifier, action, correct);
		/* remember the system error */
		*sysError = (double) (abs((int) (reward - predictionArray[action])))
				/ Configuration::getPAYMENT_RANGE();
	} else {
		cout << " Not decided" << endl;
		*correct = 0;
		reward = 0;
		*sysError = 0;
	}

	if (tabFile != NULL) {
		fprintf(tabFile, "Sample #%d\t%d\t%d\n", 0,  action, trnClassifier->getAct());
		//fprintf(tabFile, "Prediction:%d;Actual:%d\n", action,
		//		trnClassifier->getAct());
		// Close the output file
		fclose(tabFile);
	}

	/* Clean up */
	mset.clear();
	aset.clear();
	nonAset.clear();
	delete[] predictionArray;
}

/**
 * Resets the state to a new problem instance.
 */
void Original_XCS::resetStateMultiplexer(Classifier * cl) {
	/* generate a random state */

	int length = Configuration::getCONDITION_LENGTH();
	int tLength = 6;
	int lAddress = 2;
	while (tLength < Configuration::getCONDITION_LENGTH()) {
		lAddress++;
		tLength = lAddress + ((1 << lAddress));
	}
	set<int> PossiblePositions;
	for (int p = 0; p < (1 << lAddress); p++)
		PossiblePositions.insert(lAddress + p);

	bool dummyValue = ((((int) (frand() * 100)) % 2) == 1) ? true : false;
	for (unsigned int i = 0; i < Configuration::getCONDITION_LENGTH(); i++) {
		xBinaryCondition *x = new xBinaryCondition();
		if (i < lAddress || (PossiblePositions.find(i)
				!= PossiblePositions.end()))
			x->value = ((int) (frand() * 2)) == 1 ? true : false;
		else
			x->value = 0;// dummyValue;

		cl->ConditionList[i] = x;
	}

	cl->setAct(EvaluateClass(cl, length, lAddress));

}

/*########################################### match set operations ##################################################*/

/* Get MatchSet that matches state
 * If a classifier was deleted, record its address in killset to be able to update former actionsets
 * the iteration time 'itTime' is used when creating a new classifier */
void Original_XCS::getMatchSet(list<Classifier*> *mset,
		map<int, xCondition *> *state, int action, long unsigned int itTime) {
	long unsigned int setSize = 0;
	long unsigned int popSize = 0;
	int *coveredActions = new int[Configuration::getNUMBER_OF_ACTIONS()];
	int representedActions = 0;

	do {
		/* Delete classifier if population is too big and record it in killset */
		while (popSize > Configuration::getMAX_POP_SIZE()) {
			/* PL */
			deleteStochClassifier("DelClassifiers.txt", itTime);
			popSize--;
		}

		mset->clear();
		popSize = 0;
		setSize = 0;

		for (list<Classifier*>::const_iterator it = population->begin(); it
				!= population->end(); it++) {
			if (match(state, &(*it)->ConditionList)) {
				/* Calculate the mean prdiction prob. and the match set size to detect loops */
				setSize += ((*it)->getNum());
				mset->push_back((*it));
			}

			popSize += (*it)->getNum();
		}

		/* Create covering classifiers, if not all actions are covered */
		representedActions = nrActionsInSet(mset, coveredActions);

		if (representedActions < Configuration::getNUMBER_OF_ACTIONS())
			for (int NomAct = 0; NomAct < Configuration::getNUMBER_OF_ACTIONS(); NomAct++) {
				/* make sure that all actions are covered! */
				if (coveredActions[NomAct] == 0) {

					Classifier *coverCl;
					assert((coverCl= new Classifier()));

					if (responsibleAtts.empty()) {

						for (unsigned int j = 0; j
								< (unsigned int) Configuration::getCONDITION_LENGTH(); j++) {
							// if j is assigned to this island
							if (frand()
									> Configuration::getDONTCARE_PROBABILITY()) {

								coverCl->ConditionList[j]
										= ((xCondition *) (*state)[j])->Clone();
								if (((xCondition *) (*state)[j])->GetConditionType()
										== RealNormalized)
									((xRealNormalizedCondition *) coverCl->ConditionList[j])->createRandomRangeValues();
								if (((xCondition *) (*state)[j])->GetConditionType()
										== Real)
									((xRealCondition *) coverCl->ConditionList[j])->createRandomRangeValues();

							}
						}
					} else // there are some attributes that should be in concern
					{
						for (set<int>::iterator attIndex =
								this->responsibleAtts.begin(); attIndex
								!= this->responsibleAtts.end(); attIndex++) {
							if (frand()
									> Configuration::getDONTCARE_PROBABILITY()) {
								int idx = *attIndex;
								coverCl->ConditionList[idx]
										= ((xCondition *) (*state)[idx])->Clone();
								if (((xCondition *) (*state)[idx])->GetConditionType()
										== RealNormalized)
									((xRealNormalizedCondition *) coverCl->ConditionList[idx])->createRandomRangeValues();
								if (((xCondition *) (*state)[idx])->GetConditionType()
										== Real)
									((xRealCondition *) coverCl->ConditionList[idx])->createRandomRangeValues();
							}
						}
					}

					coverCl->setAct(NomAct);
					coverCl->setInitialVariables(itTime);

					/* set the parameters to the initial values */
					if (addClassifierToSet(coverCl, mset))
						//addNewClassifierToSet(coverCl, mset);
						addNewClassifierToSet(coverCl, population);

					popSize++;
					setSize++;
					coverCl->setActionSet(setSize);
				}
			}

	}
	// while (representedActions < Configuration::getNUMBER_OF_ACTIONS());
	while (mset->empty());

	delete[] coveredActions;
}

/* returns 1 if m matches c */
// state has 0 for R (train data) and only Cond are filled (Population)
bool Original_XCS::match(map<int, xCondition*> *state,
		map<int, xCondition*> *cnd) {
	for (map<int, xCondition *>::const_iterator i = cnd->begin(); i
			!= cnd->end(); i++) {
		if ((*i).second->GetConditionType() == RealNormalized) {
			int indx;
			float min, max, cnt, rng;
			xRealNormalizedCondition *cond, *target;
			indx = (*i).first;
			cond = (xRealNormalizedCondition*) ((*i).second);

			map<int, struct xCondition*>::const_iterator j;
			j = state->find(indx);
			if (j == state->end())
				return false; // impossible

			target = (xRealNormalizedCondition*) ((*j).second);
			if (!(target->getCenterValue() >= cond->getLowerValue()
					&& target->getCenterValue() <= cond->getUpperValue()))
				return false;
		} else if ((*i).second->GetConditionType() == Real) {
			int indx;
			float min, max, cnt, rng;
			xRealCondition *cond, *target;
			indx = (*i).first;
			cond = (xRealCondition*) ((*i).second);

			map<int, struct xCondition*>::const_iterator j;
			j = state->find(indx);
			if (j == state->end())
				return false; // impossible

			target = (xRealCondition*) ((*j).second);
			if (!(target->getCenterValue() >= cond->getLowerValue()
					&& target->getCenterValue() <= cond->getUpperValue()))
				return false;
		} else if ((*i).second->GetConditionType() == Nominal) {
			int indx;
			xNominalCondition *cond, *target;
			indx = (*i).first;
			cond = (xNominalCondition*) ((*i).second);

			map<int, struct xCondition*>::const_iterator j;
			j = state->find(indx);
			if (j == state->end())
				return true; // impossible

			target = (xNominalCondition*) ((*j).second);
			for (unsigned int t = 0; t < cond->CategoryLength; t++)
				if (target->Category[t] != 0 && cond->Category[t] == 0)
					return false;
		} else if ((*i).second->GetConditionType() == Binary) {
			int indx;
			xBinaryCondition *cond, *target;
			indx = (*i).first;
			cond = (xBinaryCondition*) ((*i).second);

			map<int, struct xCondition*>::const_iterator j;
			j = state->find(indx);
			if (j == state->end())
				return true; // impossible

			target = (xBinaryCondition*) ((*j).second);
			if (target->value != cond->value)
				return false;
			// else check others
		}

	}

	//	cout << ((xRealNormalizedCondition *) cnd)->center << ":" << ((xRealNormalizedCondition *) cnd)->range << "Matched ";
	//	cout << ((xRealNormalizedCondition *) state)->center << ":" << ((xRealNormalizedCondition *) state)->range << endl;

	return true;
}

/**
 * Returns the number of actions in the set and stores which actions are covered
 * in the array coveredActions.
 */
int Original_XCS::nrActionsInSet(list<Classifier*> *mset, int *coveredActions) {
	int nr = 0;
	for (unsigned int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		coveredActions[i] = 0;
	}

	for (list<Classifier*>::iterator it = mset->begin(); it != mset->end()
			&& nr < Configuration::getNUMBER_OF_ACTIONS(); it++) {
		if (coveredActions[((Classifier*) *it)->getAct()] == 0) {
			coveredActions[((Classifier*) *it)->getAct()] = 1;
			nr++;
		}
	}
	return nr;
}

/**
 * Multiplexer Function
 */
double Original_XCS::doMPAction(Classifier *state, int pAction, int *correct) {
	double reward;
	if (state->getAct() == pAction) {
		*correct = 1;
		reward = Configuration::getPAYMENT_RANGE();
	} else {
		*correct = 0;
		reward = 0;
	}
	return reward;
}

int Original_XCS::ToDecimal(bool * binary, int length) {
	int sum = 0;
	for (int i = 0; i < length; i++) {
		int num = binary[i] ? 1 : 0;
		sum += (num << (length - 1 - i));
	}
	return sum;
}

bool Original_XCS::EvaluateClass(Classifier * cl, int length, int lAddress) {
	bool *tbinary = new bool[lAddress];
	bool c = 0;

	for (int i = 0; i < lAddress; i++)
		tbinary[i] = ((xBinaryCondition *) cl->ConditionList[i])->value;

	int point = ToDecimal(tbinary, lAddress);

	c
			= (bool) ((xBinaryCondition *) cl->ConditionList[lAddress + (point)])->value;
	delete[] tbinary;
	return c;
}

/**
 * Record and print to screen performance (averaged over the last testFrequency trials) and
 * other information about the current population status
 */
void Original_XCS::writePerformance(char *statFileName, int *correct,
		double *sysError, int counter, int windowFrequency) {
	double corr = 0., serr = 0., spec = 0., assest = 0., perr = 0., fit = 0.,
			exp = 0., attSize=0.0 , *specPos;
	unsigned int popsize = 0, popnum = 0;

	/* record population properties */
	list<Classifier *>::iterator it;
	for (it = population->begin(); it != population->end(); it++) {
		popnum += (*it)->getNum(); /* Just count the size of the population */
		spec += ((double) (*it)->getSpecificity()
				/ (double) Configuration::getCONDITION_LENGTH())
				* (double) (*it)->getNum();
		assest += (*it)->getActionSet() * (*it)->getNum();
		perr += (*it)->getPreer() * (*it)->getNum();
		fit += (*it)->getFit();
		exp += (double) (*it)->getExp() * (*it)->getNum();
		attSize += (*it)->ConditionList.size();

	}

	popsize = population->size();
	spec /= (double) popnum;
	assest /= (double) popnum;
	perr /= (double) popnum;
	fit /= (double) popnum;
	exp /= (double) popnum;
	attSize /= popsize;


	/* record average performance and error over the last 'testFrequency' trials */
	for (int i = 0; i < windowFrequency; i++) {
		corr += correct[i];
		serr += sysError[i];
	}
	corr /= windowFrequency;
	serr /= windowFrequency;

//	printf(
//			"Coun=%d Crr=%f Err=%f size=%d actSize=%f exp=%f Spec=%f perr=%f fit=%f\n",
//			counter, corr, serr, popsize, assest, exp, spec, perr, fit);

	/* start the experiments */
	FILE *statFile;

	statFile = (FILE *) fopen(statFileName, "at");

	if (statFile == NULL) {
		fprintf(stderr, "Cannot open Statistic Out file");
		return;
	}

	/* print to file the learning progress */
	fprintf(statFile, "%d %f %f %d %f %f %f %f %f %f\n", counter, corr, serr,
			popsize, assest, exp, spec, perr, fit, attSize);
	fclose(statFile);
}

void Original_XCS::MakeDontCare(Classifier * childs[2], Classifier * trnCls) {

	if (childs != NULL)
		for (int iChildIndex = 0; iChildIndex < 2; iChildIndex++) {
			if (childs[iChildIndex] != NULL) {
				// Make Don't Care
				for (map<int, xCondition*>::const_iterator attsIndex =
						childs[iChildIndex]->ConditionList.begin(); attsIndex
						!= childs[iChildIndex]->ConditionList.end(); attsIndex++) {
					int index = (*attsIndex).first;
					if (frand() < Configuration::getDONTCARE_PROBABILITY()) {
						childs[iChildIndex]->ConditionList.erase(index);
					}
				}
			}
		} // Child is not NULL
}

/*########################################## discovery mechanism #################################*/

/**
 * The discovery conmponent with the genetic algorithm
 * note: some classifiers in set could be deleted !
 */
void Original_XCS::discoveryComponent(list<Classifier*> *acSet,
		long unsigned int itTime, Classifier * trnClassifier) {

	Classifier *parents[2], *childs[2];

	long double fitsum = 0.0;
	unsigned long int meangait = 0, mssum = 0;
	/* if the classifier set is empty, return (due to deletion) */
	if (acSet->empty())
		return;

	/* get all sums that are needed to do the discovery */
	getDiscoversSums(acSet, fitsum, meangait, mssum, itTime);
	/* do not do a GA if the average number of time-steps in the set since the last GA
	 * is less or equal than MEAN_LAST_GATIME */
	if (((long double) (itTime) - (double) (meangait) / (long double) (mssum))
			< Configuration::getMEAN_LAST_GATIME())
		return;

	setTimeStamps(acSet, itTime);
	/* select two classifiers (roulette wheel selection) and copy them */
	if (!selectTwoClassifiers(parents, acSet, fitsum))
		return;

	//	childs = new Classifier*[2];
	// original Xover and Mutation
	for (int i = 0; i < 2; i++) {
		assert(childs[i] = new Classifier());
		childs[i]->Copy(parents[i]);
		childs[i]->setFit(parents[i]->getFit() / (double) parents[i]->getNum());
		childs[i]->setNum(1);
		childs[i]->setExp(1);

		childs[i]->setGaIterationTime(itTime);
		//childs[i]->setInitialVariables(itTime);

		//		childs[i]->ResetProperties();
		/* Reducing fitness of new classifiers by a given factor */
		childs[i]->setFit(childs[i]->getFit()
				* Configuration::getFITNESS_REDUCTION()); /* reduce fitness of new classifiers */
	}
	if (crossover(childs)) {
		/* do crossover on the two selected classifiers */
		/* if crossover got applied and changed classifiers, average their values */
		childs[0]->setPre((childs[0]->getPre() + childs[1]->getPre()) / 2.0);
		childs[0]->setFit((childs[0]->getFit() + childs[1]->getFit()) / 2.0);
		childs[0]->setPreer((childs[0]->getPreer() + childs[1]->getPreer())
				/ 2.0);
		childs[1]->setPre(childs[0]->getPre());
		childs[1]->setPreer(childs[0]->getPreer());
		childs[1]->setFit(childs[0]->getFit());
	}
	/* do mutation */
	for (int i = 0; i < 2; i++) {
		mutation(childs[i], 0, Configuration::getCONDITION_LENGTH(),
				trnClassifier);
	}

	// Change some attributes to Don't Care state
	// it was commented for multiplexer experiments but
	// not commented for GE experiments
	MakeDontCare(childs, trnClassifier);

	/* insert the new classifier and delete if necessary */
	insertDiscoveredClassifier(childs, parents, acSet, 20);

	/* Delete classifier if population is too big and record it in killset */
	for (unsigned int popSize = getPopSize(); popSize
			> Configuration::getMAX_POP_SIZE(); popSize--) {
		deleteStochClassifier("DelClassifiers.txt", itTime);
	}

	//delete childs;
}

/* Calculate all necessary sums in the set for the discovery component */
void Original_XCS::getDiscoversSums(list<Classifier*> *acSet, long double &sum,
		long unsigned int &meangait, long unsigned int &mssum,
		long unsigned int itTime) {
	sum = 0.;
	meangait = 0;
	mssum = 0;

	for (list<Classifier*>::iterator i = acSet->begin(); i != acSet->end(); i++) {
		sum += (*i)->getFit();
		//meangait+=(itTime-(*i)->gaIterationTime)*(*i)->num;
		meangait += ((*i)->getGaIterationTime()) * (*i)->getNum();
		mssum += (*i)->getNum();
	}
}


/* insert a discovered classifier into the population and respects the population size */
void Original_XCS::insertDiscoveredClassifier(Classifier **cl,
		Classifier **parents, list<Classifier*> *set, int thetaSub) {
	if (cl != NULL) {
		for (int i = 0; i < 2; i++) {
			if (cl[i] != NULL) {
				cl[i]->shrinkClassifier();
				if (cl[i]->ConditionList.empty()) {
					freeClassifier(cl[i]);
					cl[i] = NULL;
				} else {
					subsumeClassifier(cl[i], parents, set, thetaSub);
				}
			}
		}

	}
}

/**
 * Check if classifier cl1 subsumes cl2
 */
int Original_XCS::subsumes(Classifier *cl1, Classifier *cl2, int thetaSub) {
	int ret = 0;
	ret = (cl1->getAct() == cl2->getAct() && isSubsumer(cl1, thetaSub)
			&& cl1->isGeneralThan(cl2));
	return ret;
}

/**
 * Returns if the classifier satisfies the criteria for being a subsumer.
 */
int Original_XCS::isSubsumer(Classifier *cl, int thetaSub) {
	return cl->getExp() > thetaSub && cl->getAcc() == 1.0;
}

void Original_XCS::subsumeClassifier(Classifier *cl, Classifier **parents,
		list<Classifier*> *locset, int thetaSub) {
	/*Try first to subsume in parents */
	for (int i = 0; i < 2; i++) {
		if (parents[i] != NULL && subsumes(parents[i], cl, thetaSub)) {
			// if parent is more general than its child
			parents[i]->incNum();
			freeClassifier(cl);
			cl = NULL;
			return;
		}
	}

	if (subsumeClassifierToSet(cl, locset, thetaSub))
		return;

	addClassifierToSet(cl, population);
}

/**
 * Try to subsume in the specified set.
 */
int Original_XCS::subsumeClassifierToSet(Classifier *cl,
		list<Classifier*> *set, int thetaSub) {

	vector<Classifier*> subList;

	unsigned int spec = Configuration::getCONDITION_LENGTH();

	/*Try to subsume in the set*/
	for (list<Classifier*>::const_iterator i = set->begin(); i != set->end(); i++) {
		if (subsumes((*i), cl, thetaSub)) // means i is more general than cl
		{
			if (spec > (*i)->getSpecificity()) {
				subList.clear();
				spec = (*i)->getSpecificity();
			}
			subList.push_back((*i));
		}
	}

	/* if there were classifiers found to subsume, then choose randomly one and subsume */
	if (subList.size() > 0) {
		int numSub = (int) (subList.size() * frand());
		//if ( subList[numSub]->num < (Configuration::getMAX_POP_SIZE()/2) )
		subList[numSub]->incNum();
		freeClassifier(cl);
		subList.clear();
		return 1;
	}

	subList.clear();
	return 0;
}

void Original_XCS::freeClassifier(Classifier *cl) {
	//cl->~Classifier(); // Destruction
	delete cl;
}

/* Determines the prediction array out of the classifier set ms */
void Original_XCS::getPredictionArray(list<Classifier*> *ms, long double *pa) {
	long double *nr = new long double[Configuration::getNUMBER_OF_ACTIONS()];

	long double resetArray = 0;

	/* set the values to 0 (nr memorizes the sum of the fitnesses) */
	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		pa[i] = resetArray;
		nr[i] = resetArray;
	}

	//long double prod;
	/* Get the sum of the fitnesses and the predictions */
	for (list<Classifier*>::iterator i = ms->begin(); i != ms->end(); i++) {
		int act = (*i)->getAct();

		pa[act] += ((*i)->getPre() * (*i)->getFit());
		nr[act] += (*i)->getFit();
	}

	/* Divide the sum of the predictions by the sum of the fitnesses */
	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		if (nr[i] != 0) {
			pa[i] /= nr[i];
			if (pa[i] == 0) {
				/* Changed zero entry in prediction array to prevent infinite loops 05/25/2000 */
				pa[i] = 0.0000001;
			}
		} else
			pa[i] = 0;
	}

	delete[] nr;
}

/* choose the apropriate action */
int Original_XCS::actionWinner(long double *predictionArray) {
	int act;
	if (urand() <= Configuration::getEXPLORE_EXPLOITE_RATE())
		act = randomActionWinner(predictionArray);
	else
		act = deterministicActionWinner(predictionArray);

	return act;
}

/* choose randomly one of the possible actions */
int Original_XCS::randomActionWinner(long double *predictionArray) {
	return (int) (urand() * (double) Configuration::getNUMBER_OF_ACTIONS());
}

/* return the corresponding int value of the best action in the prediction array*/
int Original_XCS::deterministicActionWinner(long double *predictionArray) {
	int ret = -1;
	long double max = -1.0;

	for (unsigned i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		if (predictionArray[i] > max) {
			max = predictionArray[i];
			ret = i;
		} else if (predictionArray[i] == max) {
			ret = -1;
		}

	}
	return ret;
}

/*############################################## action set operations ##############################################*/

/* Get ActionSet out of the set ms (the match set) that only has classifiers that match action,
 * and  increase the experience of those classifiers*/

void Original_XCS::getActionSet(int action, list<Classifier*> *ms, list<
		Classifier*> *acSet, list<Classifier*> *acNonSet) {

	/* MatchSet must not be NULL (should never occur, due to covering) */
	assert(!ms->empty());

	/* insert all classifiers with action == 'action' to the new xClassifierSet */
	for (list<Classifier*>::iterator i = ms->begin(); i != ms->end(); i++) {
		if (action == (*i)->getAct()) {
			acSet->push_back(*i);
		} else {/*CLassifiers in M/A!*/
			acNonSet->push_back(*i);
		}
	}
}

/* This is the reinforcement component,
 * if maxP equals 0 then there was no previous action set (single-step env or a new trial) */
void Original_XCS::adjustActionSet(list<Classifier*> *acSet, float maxP,
		float reward) {
	long double P, newPred, newPredErr, newActSetSize;
	long int setSize = 0;

	for (list<Classifier*>::iterator i = acSet->begin(); i != acSet->end(); i++) {
		(*i)->incExp();
		if (reward > 0)
			(*i)->incExpPositive();
		else
			(*i)->incExpNegative();

		//if ((*i)->exp > 10000)
		//	(*i)->exp = 10000;
		//if ((*i)->expPositive > 10000)
		//	(*i)->expPositive = 10000;
		//if ((*i)->expNegative > 10000)
		//	(*i)->expNegative = 10000;

		setSize += (*i)->getNum();
	}

	/* Then update the prediction error and the prediction */
	P = (long double) reward + (long double) Configuration::getGAMMA()
			* (long double) maxP;

	for (list<Classifier*>::iterator i = acSet->begin(); i != acSet->end(); i++) {
		if ((long double) (*i)->getExp() < ((long double) 1.0
				/ Configuration::getBETA())) {
			// !first adjustments! -> simply calcualte the average
			(*i)->setPreer(((long double) (*i)->getPreer() * ((*i)->getExp()
					- 1.0) + absLongDouble(P - (*i)->getPre()))
					/ (long double) (*i)->getExp());
			(*i)->setPre(((long double) (*i)->getPre() * ((*i)->getExp() - 1.0)
					+ P) / (long double) (*i)->getExp());
			(*i)->setActionSet(((long double) (*i)->getActionSet()
					* ((long double) ((*i)->getExp() - 1.0)) + setSize)
					/ (long double) (*i)->getExp());
		} else {
			/* normal adjustment -> use widrow hoff delta rule */
			(*i)->setPreer((long double) (*i)->getPreer()
					+ (long double) (Configuration::getBETA()
							* ((absLongDouble(P - (*i)->getPre()))
									- (*i)->getPreer())));
			(*i)->setPre((long double) (*i)->getPre()
					+ (long double) (Configuration::getBETA() * (P
							- (*i)->getPre())));
			(*i)->setActionSet((long double) (*i)->getActionSet()
					+ (long double) (Configuration::getBETA() * (setSize
							- (*i)->getActionSet())));
		}
	}

	/* First update the fitness */
	updateFitness(acSet);
}

/* update the fitnesses of a action set (the previous a.s. in multi-step envs or the current a.s. in single-step envs.) */
void Original_XCS::updateFitness(list<Classifier*> *acSet) {
	long double ksum, powerSub;

	/* If the action set got NULL (due to deletion) return */
	if (acSet->empty())
		return;

	/* Calculate the accuracies of all classifiers and get the sum of all accuracies */
	ksum = 0.;

	for (list<Classifier*>::iterator i = acSet->begin(); i != acSet->end(); i++) {
		if (absLongDouble((*i)->getPreer())
				<= Configuration::getMINIMUM_ERROR()
						* Configuration::getPAYMENT_RANGE())
			(*i)->setAcc(1.0);
		else {
			powerSub = (*i)->getPreer() / (Configuration::getMINIMUM_ERROR()
					* (long double) Configuration::getPAYMENT_RANGE());
			(*i)->setAcc(powerSub);
			for (int t = 2; t <= Configuration::getKAPPA_MULTIPLIER(); t++)
				(*i)->setAcc((*i)->getAcc() * powerSub);
			(*i)->setAcc(Configuration::getFALL_OFF_RATE() * (1.0
					/ ((*i)->getAcc())));
		}
		ksum += (*i)->getAcc() * (*i)->getNum();
	}

	/* Update the fitnesses of the classifiers in aset using the relative accuracy values */
	for (list<Classifier*>::iterator i = acSet->begin(); i != acSet->end(); i++) {
		/* update the values */
		(*i)->setFit((*i)->getFit() + (double) (Configuration::getBETA()
				* (((*i)->getAcc() * (*i)->getNum() / ksum) - (*i)->getFit())));
	}
}

/* adds a new Clasifier to the xClassifierSet */
bool Original_XCS::addNewClassifierToSet(Classifier *cl,
		list<Classifier*> *clSet) {
	if (cl->ConditionList.size() > 0)
		clSet->push_back(cl);

	return true;
}

/**
 * Sets the time steps of all classifiers in the set to itTime
 * (because a GA application is occurring in this set!).
 */
void Original_XCS::setTimeStamps(list<Classifier*> *acSet,
		long unsigned int itTime) {
	for (list<Classifier*>::iterator i = acSet->begin(); i != acSet->end(); i++)
		(*i)->setGaIterationTime(itTime);
}

/*####################################### adding classifiers to a set ###########################################*/

/* adds the classifier cl to the population, make sure that the same classifier does not exist, yet
 returns if inserted classifier was deleted */
bool Original_XCS::addClassifierToSet(Classifier *cl, list<Classifier*> *clSet) {
	/* Check if classifier exists already. if so, just increase the numeriosity and free the space of the new classifier */
	list<Classifier *>::iterator i;
	for (i = clSet->begin(); i != clSet->end(); i++) {
		//if( ((Classifier)(*(*i))) == ((Classifier)*cl) ) // this classifier already in set
		if ((*i)->isEqual(cl)) // this classifier already in set
		{
			(*i)->incNum();
			freeClassifier(cl);
			return false;
		}
	}

	return addNewClassifierToSet(cl, clSet);
}

/*########################################### deletion ##################################################################*/

void Original_XCS::deleteStochClassifier(char * filename,
		long unsigned int itTime) {
	long double sum = 0.0, voteSum = 0.0, meanf = 0.0, meane = 0.0, meanp = 0.0;
	long int size = 0;
	long double minFitness, minFitnessAcc;

	//	if (itTime <= this->iteratorTime + Configuration::getDELETION_EXPERIENCE())
	//		return;
	if (population->empty() || population->size() < 3)
		return;

	list<Classifier*>::iterator i;

	i = population->begin();

	for (i = population->begin(); i != population->end(); i++) {
		meanf += (*i)->getFit();
		meane += (*i)->getPreer() * (*i)->getNum();
		meanp += (*i)->getPre() * (*i)->getNum();
		size += (*i)->getNum();
	}
	if (size == 0)
		size = 1;

	meanf /= (long double) size;
	meane /= (long double) size;

	// get the delete proportion, which depends on the average fitness */

	for (i = population->begin(); i != population->end(); i++) {
		double inc = getDelProp((*i), meanf, meane) * (*i)->getNum();
		//if( (sum + inc) > 0.0)
		//printf("Sum overflow in deleteStochclassifier!!!\n");
		sum += inc;
	}

	/* choose the classifier that will be deleted */
	long double Choice = ((long double) urand()) * sum;

	/* look for the classifier */
	i = population->begin();
	voteSum = getDelProp((*i), meanf, meane) * (*i)->getNum();
	while (voteSum < Choice) {
		i++;
		if (i == population->end()) {
			i--;
			break;
		}
		voteSum += getDelProp((*i), meanf, meane) * (*i)->getNum();
	}

	/* delete the classifier */
	if ((*i)->getNum() > 1) {
		/* if the numeriosity is greater than one -> just decrease it */
		(*i)->decNum();
	} else {
		Classifier* d = (*i);
		population->erase(i);
		freeClassifier(d);
	}

	/* return the pointer to the deleted classifier, to be able to update other sets */
}

/* return the delete proportion of one classifier, meanf is the average fitness in the population */
float Original_XCS::getDelProp(Classifier *cl, long double meanf,
		long double meane) {
	long double ret = 1.0;

	// From Original Butz Version
	if (cl->getExp() >= Configuration::getDELETION_EXPERIENCE()
			&& (cl->getFit() / cl->getNum()) < Configuration::getDELTA()
					* meanf) {
		ret *= meanf / (cl->getFit() / (double) cl->getNum());
		ret *= cl->getActionSet();
	}

	return ret;
}

/* Set all the classifier values to the average values of the population, record the size of the population */
void Original_XCS::setInitialMeanVariables(Classifier *cl, long int itTime) {
	long double pre = 0., err = 0., fit = 0., acc = 0., actionsest = 0.;
	long int popSize = 0;

	if (population->empty()) {
		cl->setInitialVariables(itTime);
		return;
	}

	for (list<Classifier*>::const_iterator i = population->begin(); i
			!= population->end(); i++) {
		pre += (*i)->getPre() * (long double) (*i)->getNum();
		err += (*i)->getPreer() * (long double) (*i)->getNum();
		acc += (*i)->getAcc() * (long double) (*i)->getNum();
		fit += (*i)->getFit() * (long double) (*i)->getNum();
		actionsest += (*i)->getActionSet() * (long double) (*i)->getNum();
		popSize += (*i)->getNum();
	}
	cl->setPre(pre / popSize);
	cl->setPreer(err / popSize);
	cl->setAcc(acc / popSize);
	cl->setFit(fit / popSize);
	cl->setNum(1);
	cl->setExp(1);
	cl->setActionSet(actionsest / popSize);
	cl->setGaIterationTime(itTime);
}

long int Original_XCS::getPopSize(void) {
	long int popSize = 0;
	for (list<Classifier *>::const_iterator i = this->population->begin(); i
			!= this->population->end(); i++)
		popSize += (*i)->getNum();
	return popSize;
}

long int Original_XCS::getSetSize(list<Classifier*> *set) {
	long int popSize = 0;
	for (list<Classifier*>::const_iterator i = set->begin(); i != set->end(); i++)
		popSize += (*i)->getNum();
	return popSize;
}

void Original_XCS::getAttibutesSize(double & MinValue, double & MaxValue,
		double & AvgValue) {
	MinValue = -1;
	MaxValue = -1;
	AvgValue = -1;
	double AttSum = 0;

	for (list<Classifier *>::const_iterator i = this->population->begin(); i
			!= this->population->end(); i++) {
		double attNo = (*i)->ConditionList.size();
		AttSum += attNo;
		if (attNo > MaxValue)
			MaxValue = attNo;
		if (attNo < MaxValue || attNo == 0)
			MinValue = attNo;

	}

	AvgValue = AttSum / (double) this->population->size();
}

/*################################### concrete deletion of a classifier or a whole classifier set #################################*/

/* Frees the complete xClassifierSet with the corresponding xClassifiers */
void Original_XCS::freeClassifierSet(list<Classifier *> *AllData) {
	Classifier * cl;
	while (!AllData->empty()) {
		cl = AllData->front();
		freeClassifier(cl);
		AllData->pop_front();
	}
}

/*######################################## crossover & mutation ##############################################*/

/* crosses the two received classifiers (right now: one-point crossover), return if the strings were changed */
int Original_XCS::crossover(Classifier **cl) {
	int ret = 0;

	if (!cl[0]->ConditionList.empty() && !cl[1]->ConditionList.empty()
			&& !(cl[0]->isEqual(cl[1])))
		if (frand() < Configuration::getCROSSPX()) {
			if (true) // uniform Crossover
			{
				set<int> MoveFrom0To1, MoveFrom1To0;
				map<int, xCondition*>::const_iterator i;
				for (i = cl[0]->ConditionList.begin(); i
						!= cl[0]->ConditionList.end(); i++)
					if (frand() > 0.5 && (responsibleAtts.empty()
							|| (responsibleAtts.find((*i).first)
									!= responsibleAtts.end()))) // interchange them
						MoveFrom0To1.insert((*i).first);

				for (i = cl[1]->ConditionList.begin(); i
						!= cl[1]->ConditionList.end(); i++)
					if (frand() > 0.5 && (responsibleAtts.empty()
							|| (responsibleAtts.find((*i).first)
									!= responsibleAtts.end()))) // interchange them
						if (MoveFrom0To1.find((*i).first) == MoveFrom0To1.end()) // already not candidate for interchange
							MoveFrom1To0.insert((*i).first);

				// replace from 0 to 1
				for (set<int>::iterator it = MoveFrom0To1.begin(); it
						!= MoveFrom0To1.end(); it++) {
					if (cl[1]->ConditionList.find(*it)
							!= cl[1]->ConditionList.end()) // swap
					{
						xCondition *temp =
								(xCondition *) cl[0]->ConditionList[*it];
						cl[0]->ConditionList[*it]
								= (xCondition *) cl[1]->ConditionList[*it];
						cl[1]->ConditionList[*it] = (xCondition *) temp;
					} else // just replace from 0 to 1
					{
						xCondition *temp =
								(xCondition *) cl[0]->ConditionList[*it];
						cl[0]->ConditionList.erase(*it);
						cl[1]->ConditionList[*it] = temp;
					}
					ret = 1;
				}

				// replace from 1 to 0
				for (set<int>::iterator it = MoveFrom1To0.begin(); it
						!= MoveFrom1To0.end(); it++) {
					if (cl[0]->ConditionList.find(*it)
							!= cl[0]->ConditionList.end()) // swap
					{
						xCondition *temp =
								(xCondition *) cl[1]->ConditionList[*it];
						cl[1]->ConditionList[*it]
								= (xCondition *) cl[0]->ConditionList[*it];
						cl[0]->ConditionList[*it] = (xCondition *) temp;
					} else // just replace from 1 to 0
					{
						xCondition *temp =
								(xCondition *) cl[1]->ConditionList[*it];
						cl[1]->ConditionList.erase(*it);
						cl[0]->ConditionList[*it] = temp;
					}
					ret = 1;
				}

			} else // one point cross over
			{
				/* crossover will be applied */
				/* get crossing site */
				int sep = (int) (frand()
						* (Configuration::getCONDITION_LENGTH() - 1));
				/* do the cross */
				map<int, xCondition*>::const_iterator i, j;

				for (int indx = sep; indx
						< Configuration::getCONDITION_LENGTH(); indx++) {
					i = cl[0]->ConditionList.find(indx);
					j = cl[1]->ConditionList.find(indx);

					if (i != cl[0]->ConditionList.end() && j
							!= cl[1]->ConditionList.end()) {
						xCondition * temp = (*i).second;
						cl[0]->ConditionList[indx] = cl[1]->ConditionList[indx];
						cl[1]->ConditionList[indx] = temp;
						ret = 1;
					} else if (i != cl[0]->ConditionList.end()) {
						cl[1]->ConditionList[indx] = cl[0]->ConditionList[indx];
						cl[0]->ConditionList.erase(indx);
						ret = 1;
					} else if (j != cl[1]->ConditionList.end()) {
						cl[0]->ConditionList[indx] = cl[1]->ConditionList[indx];
						cl[1]->ConditionList.erase(indx);
						ret = 1;
					}
				}
			}

		}
	/* return if the conditions of the classifiers changed */
	return ret;
}

/* mutate one classifier */
int Original_XCS::mutation(Classifier *cl, int minIndex, int maxIndex,
		Classifier * trnClassifier) {
	int mttr = 0;
	//if(frand()<Configuration::getMUTPMUE())
	//always

	if (!responsibleAtts.empty()) // There is only specific attributes assigned
		return mutation(cl, trnClassifier);

	// otherwise all attributes are mutated

	for (int index = minIndex; index < maxIndex; index++)
		if (frand() < Configuration::getMUTPMUE()) {
			map<int, xCondition *>::const_iterator i;
			//int mp = (int)( frand() * Configuration::getCONDITION_LENGTH());
			i = cl->ConditionList.find(index);

			if (i != cl->ConditionList.end()) // found index
			{
				//if (frand() > Configuration::getDONTCARE_PROBABILITY()) {
				if ((*i).second->GetConditionType() == RealNormalized) {

					if (frand() < 0.5)
						((xRealNormalizedCondition*) (*i).second)->AddRangeValues(
								sfrand() * Configuration::getRANGE_INI());
					else
						((xRealNormalizedCondition*) (*i).second)->AddRangeValues(
								-sfrand() * Configuration::getRANGE_INI());

				} else if ((*i).second->GetConditionType() == Real) {
					if (frand() < 0.5)
						((xRealCondition*) (*i).second)->AddRangeValues(
								sfrand() * Configuration::getRANGE_INI());
					else
						((xRealCondition*) (*i).second)->AddRangeValues(
								-sfrand() * Configuration::getRANGE_INI());

				} else if ((*i).second->GetConditionType() == Nominal) {
					int
							muIndex =
									(int) (((xNominalCondition*) (*i).second)->CategoryLength
											* frand());
					((xNominalCondition*) (*i).second)->Category[muIndex]
							= !((xNominalCondition*) (*i).second)->Category[muIndex];
				} else if ((*i).second->GetConditionType() == Binary) {
					if (frand() > 0.5)
						((xBinaryCondition*) (*i).second)->value
								= !((xBinaryCondition*) (*i).second)->value;
					else
						cl->ConditionList.erase(index);
				}

				//				} else // don't care
				//				{
				//					cl->ConditionList.erase(index);
				//				}

			}
//			} else { // Atts not empty and the index was not found in cl
//				//if (frand() > Configuration::getDONTCARE_PROBABILITY()) { // Don't care prob
//				cl->ConditionList[index]
//						= ((xCondition *) (trnClassifier->ConditionList[index]))->Clone();
//				if (((xCondition *) trnClassifier->ConditionList[index])->GetConditionType()
//						== RealNormalized)
//					((xRealNormalizedCondition *) cl->ConditionList[index])->createRandomRangeValues();
//				if (((xCondition *) trnClassifier->ConditionList[index])->GetConditionType()
//						== Real)
//					((xRealCondition *) cl->ConditionList[index])->createRandomRangeValues();
//				//}
//			}
			mttr = 1;
		} // No Mutation
	/* possibly mutate action */
	if (frand() < Configuration::getMUTPMUE()) {
		mttr = 1;
		mutateAction(cl);
	}

	/* return if the condition or action  of the classifier changed */
	return mttr;
}

int Original_XCS::mutation(Classifier *cl, Classifier * trnClassifier) {
	int mttr = 0;
	//if(frand()<Configuration::getMUTPMUE())
	//always

	for (set<int>::const_iterator attsIndex = responsibleAtts.begin(); attsIndex
			!= responsibleAtts.end(); attsIndex++) {
		int index = *attsIndex;
		if (frand() < Configuration::getMUTPMUE()) {
			map<int, xCondition *>::const_iterator i;
			//int mp = (int)( frand() * Configuration::getCONDITION_LENGTH());
			i = cl->ConditionList.find(index);

			if (i != cl->ConditionList.end()) // found index
			{
				//if (frand() > Configuration::getDONTCARE_PROBABILITY()) {
				if ((*i).second->GetConditionType() == RealNormalized) {
					if (frand() < 0.5)
						((xRealNormalizedCondition*) (*i).second)->AddRangeValues(
								sfrand() * Configuration::getRANGE_INI());
					else
						((xRealNormalizedCondition*) (*i).second)->AddRangeValues(
								-sfrand() * Configuration::getRANGE_INI());
				} else if ((*i).second->GetConditionType() == Real) {
					if (frand() < 0.5)
						((xRealCondition*) (*i).second)->AddRangeValues(
								sfrand() * Configuration::getRANGE_INI());
					else
						((xRealCondition*) (*i).second)->AddRangeValues(
								-sfrand() * Configuration::getRANGE_INI());
				} else if ((*i).second->GetConditionType() == Nominal) {
					int
							muIndex =
									(int) (((xNominalCondition*) (*i).second)->CategoryLength
											* frand());
					((xNominalCondition*) (*i).second)->Category[muIndex]
							= !((xNominalCondition*) (*i).second)->Category[muIndex];
				} else if ((*i).second->GetConditionType() == Binary) {
					if (frand() > 0.5)
						((xBinaryCondition*) (*i).second)->value
								= !((xBinaryCondition*) (*i).second)->value;
					else
						cl->ConditionList.erase(index);
				}

				mttr = 1;
				//} else // don't care
				//{
				//	cl->ConditionList.erase(index);
				//}

			} else { // Atts not empty and the index was not found in cl
				//if (frand() > Configuration::getDONTCARE_PROBABILITY()) { // Don't care prob
				cl->ConditionList[index]
						= ((xCondition *) (trnClassifier->ConditionList[index]))->Clone();
				if (((xCondition *) trnClassifier->ConditionList[index])->GetConditionType()
						== RealNormalized)
					((xRealNormalizedCondition *) cl->ConditionList[index])->createRandomRangeValues();
				if (((xCondition *) trnClassifier->ConditionList[index])->GetConditionType()
						== Real)
					((xRealCondition *) cl->ConditionList[index])->createRandomRangeValues();
				//}
			}
			mttr = 1;
		} // No Mutation
		/* possibly mutate action */
		if (frand() < Configuration::getMUTPMUE()) {
			mttr = 1;
			mutateAction(cl);
		}

	}

	/* return if the condition or action  of the classifier changed */
	return mttr;
}

/**
 * Mutates the action to another action uniformly randomly chosen.
 */
void Original_XCS::mutateAction(Classifier *cl) {
	int i;
	i = (frand() * Configuration::getNUMBER_OF_ACTIONS());
	if (i == 0)
		i++;

	cl ->setAct((cl -> getAct() + i) % Configuration::getNUMBER_OF_ACTIONS());
}

// print the classifiers in a xClassifierSet
void Original_XCS::dumpPopulation(const char *fileName) {

	FILE *dataFile;
	dataFile = fopen(fileName, "wb");
	if (dataFile == NULL) {
		cout << "Error, Couldn't Open Population file for dumping" << endl;
		return;
	}

	for (list<Classifier *>::const_iterator i = population->begin(); i
			!= population->end(); i++) {
		(*i)->dumpClassifier(dataFile);
		//(*i)->dumpClassifierCompressed(dataFile);
	}

	fclose(dataFile);

}

// print the classifiers in a xClassifierSet
//void Original_XCS::dumpPopulationCompressed(char *fileName)
//{
//	FILE *dataFile;
//
//	char tempFileName[500];
//	sprintf(tempFileName, "temp_%s",fileName);
//
//	dataFile=fopen(tempFileName, "wb");
//	if(dataFile==NULL)
//	{
//		cout << "Error, Couldn't Open Population file for dumping" << endl;
//		return;
//	}
//
//	for(list<Classifier *>::const_iterator i=population.begin();i!=population.end();i++)
//	{
//		(*i)->dumpClassifier(dataFile);
//		//(*i)->dumpClassifierCompressed(dataFile);
//	}
//
//	fclose(dataFile);
//
//    // now use the shorter way with the constructor to open the same file
//	ogzstream  out( fileName);
//    if ( ! out.good()) {
//        std::cerr << "ERROR: Opening file `" << fileName << "' failed.\n";
//    }
//    std::ifstream in(  tempFileName);
//    if ( ! in.good()) {
//        std::cerr << "ERROR: Opening file `" << tempFileName << "' failed.\n";
//    }
//    char c;
//    while ( in.get(c))
//    	out << c;
//    in.close();
//    out.close();
//    if ( ! in.eof()) {
//        std::cerr << "ERROR: Reading file `" << tempFileName<< "' failed.\n";
//    }
//    if ( ! out.good()) {
//        std::cerr << "ERROR: Writing file `" << fileName  << "' failed.\n";
//    }
//}


/* load the classifiers in a xClassifierSet */
void Original_XCS::loadpopulationForTrain(char *fileName) {
	try {

		FILE *dataFile;
		dataFile = fopen(fileName, "rb");
		if (dataFile == NULL) {
			cout << "Error, Population file is Not found 2" << endl;
			return;
		}

		long unsigned int MaxTime = 0;
		population->clear();
		Classifier * cl;
		while (feof(dataFile) < 1) {
			cl = new Classifier();
			cl->loadClassifier(dataFile);
			addNewClassifierToSet(cl, population);
			if (cl->getGaIterationTime() > MaxTime)
				MaxTime = cl->getGaIterationTime();
		}

		this->setIteratorTime(MaxTime);
		fclose(dataFile);
	} catch (...) {
		cout << "Error Reading saved population\n" << endl;
		exit(0);
	}

}

int Original_XCS::selectTwoClassifiers(Classifier **parents,
		list<Classifier*> *acSet, long double fitsum) {
	int i; /*,j, length;*/
	Classifier* clp;

	assert(!acSet->empty());

	int setsum = getSetSize(acSet);
	if (Configuration::getTOURNAMENT_SIZE <= 0)
		selectClassifierUsingRWS(parents, acSet, fitsum);
	else
		for (i = 0; i < 2; i++) {
			if (i == 0) {
				clp = selectClassifierUsingTournamentSelection(acSet, setsum,
						NULL);
			} else {
				clp = selectClassifierUsingTournamentSelection(acSet, setsum,
						parents[0]);
			}

			parents[i] = clp;
		}

	return 1;
}

/**
 * Selects a classifier from 'set' using tournament selection.
 * If 'notMe' is not the NULL pointer and forceDifferentInTournament is set to a value larger 0,
 * this classifier is not selected except if it is the only classifier.
 */
Classifier * Original_XCS::selectClassifierUsingTournamentSelection(list<
		Classifier*> *SelectionSet, int setsum, Classifier *NotMe) {

	set<Classifier*> winnerSet;
	Classifier* winner = NULL;
	long double fitness = -1, value;
	int i, j, size = 0;
	int * sel = new int[setsum];

	assert(!SelectionSet->empty());/* there must be at least one classifier in the set */

	/* Assume picking the same guy is allowed */
	NotMe = NULL;

	//	  if(NotMeSet != NULL) {
	//	    if(urand() < xcs->forceDifferentInTournament)
	//	      setsum -= notMe->num;
	//	    else
	//	      notMe=0; /* picking the same guy is allowed */
	//	  }

	if (setsum <= 0 || SelectionSet->size() == 1) /* only one classifier in set */
		return SelectionSet->front();

	//	  if( tournamentSize >1) {/* tournament with fixed size */
	//	    for(i=0; i< xcs->tournamentSize; i++) { /* (with replacement) */
	//	      sel[(int)(urand()*setsum)]=1;
	//	    }
	//	    if(i-xcs->tournamentSize != 0 && urand() > i-xcs->tournamentSize)
	//	      /* possible probabilistic selection of the last guy */
	//	      sel[(int)(urand()*setsum)]=1;
	//
	//	    for(setp=set, i=0; setp!=NULL; setp=setp->next) {
	//	      if(setp->cl != notMe) {
	//		if(fitness < setp->cl->fit/setp->cl->num) {
	//		  for(j=0; j<setp->cl->num; j++) {
	//		    if(sel[i+j]) {
	//		      freeSet(&winnerSet);
	//		      addClassifierToPointerSet(setp->cl, &winnerSet);
	//		      fitness = setp->cl->fit/setp->cl->num;
	//		      break; /* go to next classifier since this one is already a winner*/
	//		    }
	//		  }
	//		}
	//		i += setp->cl->num;
	//	      }
	//	    }
	//	    free(sel);
	//	    assert(winnerSet!=NULL);
	//	    size=1;
	//	  }else
	{
		/* tournament selection with the tournament size approx. equal to tournamentSize*setsum */
		while (winnerSet.empty()) {
			size = 0;
			for (list<Classifier*>::iterator itr = SelectionSet->begin(); itr
					!= SelectionSet->end(); itr++)
				if (*itr != NotMe) {
					/* do not reselect the same classifier -> this only applies if forcedDifferentInTournament is set!*/
					value = (*itr)->getPreer();
					if (winnerSet.empty()
							|| (!Configuration::getDO_GA_ERROR_BASED_SELECT()
									&& fitness
											- Configuration::getSELECT_TOLERANCE()
											<= (*itr)->getFit()
													/ (*itr)->getNum())
							|| (Configuration::getDO_GA_ERROR_BASED_SELECT()
									&& fitness
											+ Configuration::getSELECT_TOLERANCE()
													* Configuration::getPAYMENT_RANGE()
											>= value)) {
						/* if his fitness is worse then do not bother */
						for (i = 0; i < (*itr)->getNum(); i++) {
							if (urand() < Configuration::getTOURNAMENT_SIZE()) {
								/* this classifier is a selection candidate and
								 * his fitness/error is higher/lower or similar to the other classifier */
								if (winnerSet.empty()) {
									/* the first guy in the tournament */
									winnerSet.insert(*itr);
									if (Configuration::getDO_GA_ERROR_BASED_SELECT())
										fitness = value;
									else
										fitness = (*itr)->getFit()
												/ (*itr)->getNum();
								} else {
									/* another guy in the tournament */
									if ((!Configuration::getDO_GA_ERROR_BASED_SELECT()
											&& fitness
													+ Configuration::getSELECT_TOLERANCE()
													> (*itr)->getFit()
															/ (*itr)->getNum())
											|| (Configuration::getDO_GA_ERROR_BASED_SELECT()
													&& fitness
															- Configuration::getSELECT_TOLERANCE()
																	* Configuration::getPAYMENT_RANGE()
															< value)) {
										/* both classifiers in tournament have a similar fitness/error */
										winnerSet.insert(*itr);
									} else {
										/* new classifier in tournament is clearly better */
										winnerSet.clear();
										winnerSet.insert(*itr);
										if (Configuration::getDO_GA_ERROR_BASED_SELECT())
											fitness = value;
										else
											fitness = (*itr)->getFit()
													/ (*itr)->getNum();

									}
								}
								break; /* go to next classifier since this one is already a winner*/
							}
						}
					}
				} // if( *itr != NotMe)

		} // while(winnerSet.empty())
	}

	/* choose one of the equally best winners at random */
	size = (int) (urand() * winnerSet.size());

	set<Classifier *>::const_iterator citr;
	for (citr = winnerSet.begin(); citr != winnerSet.end(); citr++) {
		if (size == 0)
			break;
		size--;
	}
	winner = *citr;

	winnerSet.clear();
	delete[] sel;
	return winner;
}

/* select classifier for the discovery mechanism using roulette wheel selection */
Classifier * Original_XCS::selectClassifierUsingRWS(list<Classifier*> *set,
		long double fitsum) {
	long double choicep;

	choicep = frand() * fitsum;
	list<Classifier*>::const_iterator i;
	i = set->begin();

	fitsum = (*i)->getFit();
	while (choicep > fitsum && i != set->end()) {
		i++;
		fitsum += (*i)->getFit();
	}
	assert((*i)!=NULL);
	return *i;
}

/* select two classifiers and copy them for applying the GA */
int Original_XCS::selectClassifierUsingRWS(Classifier **parents, list<
		Classifier*> *acSet, long double fitsum) {
	vector<Classifier*> Nomenies;
	vector<long double> NomeniesFit;
	long double CumFit = 0.0;
	long double fit;

	for (list<Classifier*>::const_iterator i = acSet->begin(); i
			!= acSet->end(); i++) {
		// it works but lets try another approach
		fit = (long double) (*i)->getPre() * (long double) (*i)->getFit() /* * ((long double)((*i)->expPositive+1) / ((long double)(*i)->exp + (long double)(*i)->expNegative) + 1.0) * (*i)->ConditionList.size() */;
		//fit=(long double)(*i)->pre *(long double)(*i)->fit ;

		//fit*=1/(long double)(*i)->num;
		//fit=(long double)(*i)->pre * ((long double)(*i)->expPositive / (long double)(*i)->exp) * (*i)->ConditionList.size();
		// 2 fit=((long double)(*i)->fit) * ((long double)(*i)->expPositive / (long double)(*i)->exp) * (*i)->ConditionList.size()* (*i)->ConditionList.size();

		Nomenies.push_back((*i));
		NomeniesFit.push_back(fit);
		CumFit += fit;
	}

	if (!Nomenies.empty()) {
		unsigned int Index1 = randPickClassifiers(NomeniesFit, CumFit);
		unsigned int Index2 = randPickClassifiers(NomeniesFit, CumFit);
		parents[0] = Nomenies[Index1];
		parents[1] = Nomenies[Index2];
	} else
		return 0;

	Nomenies.clear();
	NomeniesFit.clear();

	return 1;

}

unsigned int Original_XCS::randPickClassifiers(vector<long double> &fitList,
		long double sumFit) {
	long double randFit = sumFit * frand();
	long double tempSumFit = 0.0;
	for (unsigned int i = 0; i < fitList.size(); i++) {
		tempSumFit += fitList[i];
		if (tempSumFit >= randFit) {
			return i;
		}
	}

	return (unsigned int) ((fitList.size() - 1) * frand());
}

bool Original_XCS::CheckPredictionArrays(list<Classifier*> *mset) {
	long double *nr = new long double[Configuration::getNUMBER_OF_ACTIONS()];
	long double *pa = new long double[Configuration::getNUMBER_OF_ACTIONS()];

	int resetArray = 0;

	if (mset->empty())
		return true;

	/* set the values to 0 (nr memorizes the sum of the fitnesses) */
	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		pa[i] = 0;
		nr[i] = 0;
	}

	//long double prod;
	/* Get the sum of the fitnesses and the predictions */
	for (list<Classifier*>::iterator i = mset->begin(); i != mset->end(); i++) {
		int act = (*i)->getAct();
		pa[act] += 1.0 / (((*i)->getPre() * (*i)->getFit())
				* ((*i)->getExpPositive() / (*i)->getExp()));
		nr[act] += (*i)->getFit() * ((*i)->getExpPositive() / (*i)->getExp());
	}

	/* Divide the sum of the predictions by the sum of the fitnesses */
	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		if (nr[i] != 0) {
			pa[i] = (1 / pa[i]);
			pa[i] /= nr[i];
		} else
			pa[i] = 0;
	}

	for (int i = 0; i < Configuration::getNUMBER_OF_ACTIONS(); i++) {
		for (int j = i + 1; j < Configuration::getNUMBER_OF_ACTIONS(); j++) {
			if (pa[i] >= pa[j]) {
				if (pa[j] > 0) {
					if (pa[i] / pa[j] < 2)
						return true;
				}//else
				//return true;

			} else if (pa[i] > 0) {
				if (pa[j] / pa[i] < 2)
					return true;
			}//else
			//return true;

		}
	}

	delete[] nr;
	delete[] pa;
	return false;
}


set<int> Original_XCS::findFixedLengthAttributeRules(int iLength) {
	set<int> iFeatures;
	for (list<Classifier *>::const_iterator i = this->population->begin(); i
			!= this->population->end(); i++) {
		double attNo = (*i)->ConditionList.size();
		if (attNo == iLength)
		{
			for (map<int, xCondition *>::const_iterator cnd = (*i)->ConditionList.begin(); cnd
					!= (*i)->ConditionList.end(); cnd++) {
				int indexF = cnd->first ;
				iFeatures.insert(indexF);
			}

		}
	}

	return iFeatures;
}
