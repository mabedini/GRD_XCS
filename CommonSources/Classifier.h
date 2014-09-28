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

#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include <string>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <utility>
#include "Definitions.h"
#include <assert.h>
#include "Configuration.h"

using namespace std;

enum ConditionTypes { Nominal, RealNormalized , Binary , Real, BinaryNone, RealNone};


struct xCondition{
public:
//	virtual xCondition &operator=(const xCondition &rhs){return NULL;};
	virtual bool isEqual(xCondition *rhs){return false;};
	virtual bool isSpecificThan(xCondition *rhs){return false;}; // specific
	virtual bool isGeneralThan (xCondition *rhs){return false;}; // general
//	virtual int operator==(const xCondition &rhs) const{};
//	virtual int operator<(const xCondition &rhs) const{};  // specific
//	virtual int operator>(const xCondition &rhs) const{};  // general
	virtual ConditionTypes GetConditionType(){return Real;};
	virtual xCondition* Clone(){return NULL;};
	virtual ~xCondition(){};
	//virtual void Dump();
};

struct xNominalCondition : public xCondition{
public:
//	xNominalCondition(xNominalCondition &);
	xNominalCondition();
	virtual ~xNominalCondition();
	virtual xNominalCondition &operator=(const xNominalCondition &rhs);
	virtual bool isEqual(xCondition *rhs);
	virtual bool isSpecificThan(xCondition *rhs); // specific
	virtual bool isGeneralThan (xCondition *rhs); // general
//	int operator==(const xNominalCondition &rhs) const;
//	int operator<(const xNominalCondition &rhs) const; //specific
//	int operator>(const xNominalCondition &rhs) const; // general
	virtual ConditionTypes GetConditionType();
	virtual xNominalCondition* Clone();
	//virtual void Dump();
	void SetCategoryBits(unsigned int index,bool value);

	void SetCategoryByID(unsigned int );
	unsigned int getCategoryID();
public:
	unsigned int CategoryLength;
	bool *Category;
};

struct xBinaryCondition : public xCondition{
public:
	xBinaryCondition();
	virtual ~xBinaryCondition();
	virtual xBinaryCondition &operator=(const xBinaryCondition &rhs);
	virtual bool isEqual(xCondition *rhs);
	virtual bool isSpecificThan(xCondition *rhs); // specific
	virtual bool isGeneralThan (xCondition *rhs); // general
//	int operator==(const xBinaryCondition &rhs) const;
//	int operator<(const xBinaryCondition &rhs) const; //specific
//	int operator>(const xBinaryCondition &rhs) const; //general
	virtual ConditionTypes GetConditionType();
	virtual xBinaryCondition* Clone();
	//virtual void Dump();
public:
	bool value;
};

struct xRealNormalizedCondition : public xCondition{
public:
	xRealNormalizedCondition &operator=(const xRealNormalizedCondition &rhs);
	virtual bool isEqual(xCondition *rhs);
	virtual bool isSpecificThan(xCondition *rhs); // specific
	virtual bool isGeneralThan (xCondition *rhs); // general
	virtual ConditionTypes GetConditionType();
	virtual xRealNormalizedCondition* Clone();
	xRealNormalizedCondition();
	virtual ~xRealNormalizedCondition();

	double getLowerValue(void);
	double getUpperValue(void);
	void setCenterValue(double);
	void setRangeValue(double);
	double getCenterValue(void);
	double getRangeValue(void);
	double getLowerBound(void);
	double getUpperBound(void);
	void createRandomValues(void);
	void createRandomRangeValues(void);
	void AddRangeValues(double change);

	//virtual void Dump();
private:
	double center;
	double range;
};

struct xRealCondition : public xCondition{
public:
	xRealCondition &operator=(const xRealCondition &rhs);
	virtual bool isEqual(xCondition *rhs);
	virtual bool isSpecificThan(xCondition *rhs); // specific
	virtual bool isGeneralThan (xCondition *rhs); // general
	virtual ConditionTypes GetConditionType();
	virtual xRealCondition* Clone();
	virtual ~xRealCondition();
	xRealCondition(double *pDown,double *pUp);
	//xRealCondition();

	double getLowerValue(void);
	double getUpperValue(void);
	void setCenterValue(double);
	void setRangeValue(double);
	double getCenterValue(void);
	double getRangeValue(void);
	double getLowerBound(void);
	double getUpperBound(void);
	void createRandomValues(void);
	void createRandomRangeValues(void);
	void AddRangeValues(double change);

private:
	double center;
	double range;
	double *pUpperbound;
	double *pLowerbound;

};

class Classifier {
public:
	map<int, xCondition *> ConditionList;
	static map<int, ConditionTypes> ConditionListTypes;
	static map<int, string> AttributeNames;
	static map<int, double*> ConditionListLowerBound;
	static map<int, double*> ConditionListUpperBound;
	static map<int,map<int,string> > attributesConvertMap;
	Classifier();
	virtual ~Classifier();
	void setInitialVariables(long unsigned int itTime);
	void createCoverMatch(map<int, xCondition *> *state,int action, int type);
	void createCoverMatch(map<int,xCondition*> *state,int action, int sIndex,int GeneNbr);
	void createSingleGeneCoverMatch(map<int,xCondition *> *state,int action, int GeneNbr);
	void createSingleGeneCoverNotMatch(map<int,xCondition*> *state,int action, int GeneNbr);
	void createMultipleGeneCoverMatch(map<int,xCondition*> *state,int action, int numCare,int minIndex, int maxIndex);
	void createMultipleGeneCoverNotMatch(map<int,xCondition*> *state,int action, int iteration);

	void dumpClassifier(FILE *fp);
	void loadClassifier(FILE *fp);
	void dumpClassifierCompressed(FILE *fp);
	void dumpClassifier(void);
	void copyClone(Classifier *clSource);
	void Copy(Classifier *clSource);
	void CopyExceptCondisionts(Classifier *clSource);
	void CopyProperties(Classifier *clSource);

	Classifier &operator=(const Classifier &rhs);
	bool isEqual(Classifier *rhs);
	bool isSpecificThan(Classifier *rhs); // specific
	bool isGeneralThan (Classifier *rhs); // general

//	int operator==(const Classifier &rhs);
//	int operator<(const Classifier &rhs); 	// specific
//	int operator>(const Classifier &rhs); 	// general
	bool subsumes (Classifier *cl,int thetaSub);
	int isSubsumer(int thetaSub);
	void shrinkClassifier(void);
	void resetConditionsList(void);
	void makeGeneralClassifier(Classifier *cl);
	int getSpecificity();

	int getAct(void);
	void setAct(int);
	long double getPre(void);
	void setPre(long double);
	long double getPreer(void);
	void setPreer(long double);
	long double getAcc(void);
	void setAcc(long double);
	long double getFit(void);
	void setFit(long double );
	long unsigned int getNum(void);
	void setNum(long unsigned int);
	long unsigned int getExp(void);
	void setExp(long unsigned int);
	void ResetProperties();
	long unsigned int getExpPositive(void);
	void setExpPositive(long unsigned int);
	long unsigned int getExpNegative(void);
	void setExpNegative(long unsigned int);
	long double getActionSet(void);
	void setActionSet(long double);
	long unsigned int getGaIterationTime(void);
	void setGaIterationTime(long unsigned int);

	long unsigned int incNum(void);
	long unsigned int decNum(void);
	long unsigned int incExp(void);
	long unsigned int incExpPositive(void);
	long unsigned int incExpNegative(void);

protected:
	int _act;
	long double _pre;
	long double _preer;
	long double _acc;
	long double _fit;
	long unsigned int _num;
	long unsigned int _exp;
	long unsigned int _expPositive;
	long unsigned int _expNegative;
	long double _actionSet;
	long unsigned int _gaIterationTime;

	//void createMatchingCondition(map<int,xCondition *> *state,int type);
	void createMatchingCondition(map<int,xCondition*> *state,int sIndex,int GeneNbr);
	void createSingleGeneMatchingCondition(map<int,xCondition *> *state,int GeneNbr);
	void createSingleGeneNotMatchingCondition(map<int,xCondition*> *state,int GeneNbr);

	void setRandomAction(void);
};

#endif
