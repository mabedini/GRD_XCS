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
 

#include "Classifier.h"
//#include "quicklz.h"
//
//#if QLZ_STREAMING_BUFFER == 0
//    #error Define QLZ_STREAMING_BUFFER to a non-zero value for this demo
//#endif


map<int, ConditionTypes> Classifier::ConditionListTypes;
map<int, string> Classifier::AttributeNames;
map<int, double*> Classifier::ConditionListLowerBound;
map<int, double*> Classifier::ConditionListUpperBound;
map<int,map<int,string> > Classifier::attributesConvertMap;

xRealNormalizedCondition::xRealNormalizedCondition()
{
	center=0;
	range=0;	// range is always Positive value
}

xRealNormalizedCondition& xRealNormalizedCondition::operator=(const xRealNormalizedCondition &rhs)
{
	this->center = rhs.center;
	this->range = rhs.range;
	return *this;
}

// This function determines if this object has the same region that rhs  has?
// means this and rhs are the same
//int xRealNormalizedCondition::operator==(const xRealNormalizedCondition &rhs) const
bool xRealNormalizedCondition::isEqual(xCondition *r)
{
	xRealNormalizedCondition * rhs = (xRealNormalizedCondition * )r;
	if ((this->center == rhs->center )
			&& (this->range == rhs->range))
		return true;

	double minThis,maxThis,minRhs,maxRhs;
	minThis = this->getLowerValue();
	maxThis = this->getUpperValue();
	minRhs = rhs->getLowerValue();
	maxRhs = rhs->getUpperValue();

	if( minThis == minRhs  && maxThis == maxRhs)
		return true; // this  == rhs

	return false;
}


// This function determines if this object has some region that can be inside rhs region?
// means this is more specific than rhs
//int xRealNormalizedCondition::operator<(const xRealNormalizedCondition &rhs) const
bool xRealNormalizedCondition::isSpecificThan(xCondition *r)
{
	xRealNormalizedCondition * rhs = (xRealNormalizedCondition * )r;
	double minThis,maxThis,minRhs,maxRhs;
	minThis = this->getLowerValue();
	maxThis = this->getUpperValue();
	minRhs = rhs->getLowerValue();
	maxRhs = rhs->getUpperValue();

	if( minThis >= minRhs  && maxThis <= maxRhs)
		return true; // this can be subsumed by rhs
				  // means this has smaller region that can be covered by rhs region
	return false;
}

// This function determines if this object has some region that can be outside rhs region?
// means this is more general than rhs
//int xRealNormalizedCondition::operator>(const xRealNormalizedCondition &rhs) const
bool xRealNormalizedCondition::isGeneralThan(xCondition *r)
{
	xRealNormalizedCondition * rhs = (xRealNormalizedCondition * )r;

	double minThis,maxThis,minRhs,maxRhs;
	minThis = this->getLowerValue();
	maxThis = this->getUpperValue();
	minRhs = rhs->getLowerValue();
	maxRhs = rhs->getUpperValue();

	//if( ( minThis < minRhs  && maxThis >= maxRhs)
	//		||
	//		( minThis <= minRhs  && maxThis > maxRhs)
	//	)
	if(  minThis <= minRhs  && maxThis >= maxRhs )
		return true; // this can be subsumed by rhs
				  // means this has bigger region that can be covered by rhs region
	return false;
}

ConditionTypes xRealNormalizedCondition::GetConditionType()
{
	return RealNormalized;
}

xRealNormalizedCondition * xRealNormalizedCondition::Clone()
{
	xRealNormalizedCondition * target;
	assert(target = new xRealNormalizedCondition());
	target->center = this->center;
	target->range = this->range;
	return target;
}

double xRealNormalizedCondition::getLowerValue(void)
{
	double LowerV= (center - range);

	if ( LowerV < 0 )
		LowerV = 0;

	return LowerV;
}
double xRealNormalizedCondition::getUpperValue(void)
{
	double UpperV= (center + range);

	if ( UpperV > 1 )
		UpperV = 1;
	return UpperV;
}
void xRealNormalizedCondition::setCenterValue(double dCentre)
{
	center = dCentre;
}
void xRealNormalizedCondition::setRangeValue(double dRange)
{
	// range is always Positive value
	if (dRange < 0)
		range= -dRange;
	else
		range= dRange;
}
double xRealNormalizedCondition::getCenterValue(void)
{
	return center;
}

double xRealNormalizedCondition::getRangeValue(void)
{
	return range;
}

double xRealNormalizedCondition::getLowerBound(void)
{
	return 0;
}
double xRealNormalizedCondition::getUpperBound(void)
{
	return 1;
}

void xRealNormalizedCondition::createRandomRangeValues(void)
{
	this->range = frand()*Configuration::getRANGE_INI();
}

void xRealNormalizedCondition::createRandomValues(void)
{
	this->center = sfrand();
	createRandomRangeValues();
}

void xRealNormalizedCondition::AddRangeValues(double change)
{
	try{
		if (change >0)
		{
			double newvalue = this->range + change;
			if (newvalue > this->getUpperBound() ||  newvalue < this->getLowerBound())
				this->AddRangeValues( sfrand()*change);
			else
				this->range = newvalue;
		}else
		{
			double newvalue = this->range - change;
			if (newvalue > this->getUpperBound() ||  newvalue < this->getLowerBound())
				this->AddRangeValues( sfrand()*change);
			else
				this->range = newvalue;
		}
	}
	catch(...)
	{
		createRandomRangeValues();
	}
}

xRealNormalizedCondition::~xRealNormalizedCondition()
{
}


//double  xRealCondition ::Upperbound=1;
//double xRealCondition ::Lowerbound=0;

xRealCondition::xRealCondition(double *pDown,double *pUp)
{
	center=0;
	range=0;	// range is always Positive value
	pUpperbound= pUp ;
	pLowerbound= pDown;
}

double xRealCondition::getLowerValue(void)
{
	double LowerV= (center - range);

	if ( LowerV < *pLowerbound )
		LowerV = *pLowerbound;

	return LowerV;
}
double xRealCondition::getUpperValue(void)
{
	double UpperV= (center + range);

	if ( UpperV > *pUpperbound )
		UpperV = *pUpperbound;
	return UpperV;
}
void xRealCondition::setCenterValue(double dCentre)
{
	center = dCentre;
}
void xRealCondition::setRangeValue(double dRange)
{
	// range is always Positive value
	if (dRange < 0)
		range= -dRange;
	else
		range= dRange;
}
double xRealCondition::getCenterValue(void)
{
	return center;
}

double xRealCondition::getRangeValue(void)
{
	return range;
}

double xRealCondition::getLowerBound(void)
{
	return *pLowerbound;
}
double xRealCondition::getUpperBound(void)
{
	return *pUpperbound;
}

void xRealCondition::createRandomRangeValues(void)
{
	this->range = frand()*Configuration::getRANGE_INI();
}

void xRealCondition::createRandomValues(void)
{
	this->center = sfrand();
	createRandomRangeValues();
}

void xRealCondition::AddRangeValues(double change)
{
	this->range += change;
	if (this->range <0)
		this->range = - this->range;

}


xRealCondition& xRealCondition::operator=(const xRealCondition &rhs)
{
	this->center = rhs.center;
	this->range = rhs.range;

	return *this;
}

// This function determines if this object has the same region that rhs  has?
// means this and rhs are the same
//int xRealNormalizedCondition::operator==(const xRealNormalizedCondition &rhs) const
bool xRealCondition::isEqual(xCondition *r)
{
	xRealCondition * rhs = (xRealCondition * )r;
	if ((this->center == rhs->center )
			&& (this->range == rhs->range))
		return true;

	double minThis,maxThis,minRhs,maxRhs;
	minThis = this->getLowerValue();
	maxThis = this->getUpperValue();
	minRhs = rhs->getLowerValue();
	maxRhs = rhs->getUpperValue();

	if( minThis == minRhs  && maxThis == maxRhs)
		return true; // this  == rhs

	return false;
}


// This function determines if this object has some region that can be inside rhs region?
// means this is more specific than rhs
bool xRealCondition::isSpecificThan(xCondition *r)
{
	xRealCondition * rhs = (xRealCondition * )r;
	exit(0);
	double minThis,maxThis,minRhs,maxRhs;
	minThis = this->getLowerValue();
	maxThis = this->getUpperValue();
	minRhs = rhs->getLowerValue();
	maxRhs = rhs->getUpperValue();

	if( minThis >= minRhs  && maxThis <= maxRhs)
		return true; // this can be subsumed by rhs
				  // means this has smaller region that can be covered by rhs region
	return false;
}

// This function determines if this object has some region that can be outside rhs region?
// means this is more generl than rhs
bool xRealCondition::isGeneralThan(xCondition *r)
{
	xRealCondition * rhs = (xRealCondition * )r;

	double minThis,maxThis,minRhs,maxRhs;
	minThis = this->getLowerValue();
	maxThis = this->getUpperValue();
	minRhs = rhs->getLowerValue();
	maxRhs = rhs->getUpperValue();

	if( ( minThis < minRhs  && maxThis >= maxRhs)
			||
			( minThis <= minRhs  && maxThis > maxRhs)
		)
		return true; // this can be subsumed by rhs
				  // means this has smaller region that can be covered by rhs region
	return false;
}

ConditionTypes xRealCondition::GetConditionType()
{
	return Real;
}

xRealCondition * xRealCondition::Clone()
{
	xRealCondition * target;
	assert(target = new xRealCondition(pLowerbound, pUpperbound));
	target->setCenterValue(this->center);
	target->setRangeValue(this->range);
	return target;
}

xRealCondition::~xRealCondition()
{
}

xNominalCondition::xNominalCondition()
{
	this->Category=NULL;
}
xNominalCondition::~xNominalCondition()
{
	if(this->Category!=NULL)
		delete[] this->Category;
	this->Category=NULL;
}


xNominalCondition& xNominalCondition::operator=(const xNominalCondition &rhs)
{
	this->CategoryLength = rhs.CategoryLength;
	this->Category = new bool[this->CategoryLength];
	for(unsigned int i=0;i<CategoryLength;i++)
		this->Category[i] = rhs.Category[i];
	return *this;
}

//int xNominalCondition::operator==(const xNominalCondition &rhs) const
bool xNominalCondition::isEqual(xCondition *r)
{
	xNominalCondition * rhs = (xNominalCondition * )r;

	for(unsigned int i=0;i<CategoryLength;i++)
	if (this->Category[i] != rhs->Category[i])
		return false;

	return true;
}


// this funciton determines if this have at least rhs categories or not?
// means this is more specific than rhs
//int xNominalCondition::operator<(const xNominalCondition &rhs) const
bool xNominalCondition::isSpecificThan(xCondition *r)
{
	xNominalCondition * rhs = (xNominalCondition * )r;

	exit(0);
	for(unsigned int i=0;i<CategoryLength;i++)
		if ( //((this->Category[i] ==false)  &&  (rhs.Category[i] == true))
			//||
			 ((this->Category[i] == true)  &&  (rhs->Category[i] ==false))
			)
			return false;
	return true;
}

// this funciton determines if this have all rhs categories or not?
// means this is more general than rhs
//int xNominalCondition::operator>(const xNominalCondition &rhs) const
bool xNominalCondition::isGeneralThan(xCondition *r)
{
	xNominalCondition * rhs = (xNominalCondition * )r;

	bool notEqual=false;
	for(unsigned int i=0;i<CategoryLength;i++)
	{
		if ((rhs->Category[i] == true) && (this->Category[i] ==false))
			return false;
		if (this->Category[i] != rhs->Category[i] )
			notEqual=true;
	}

	return notEqual;
}

ConditionTypes xNominalCondition::GetConditionType()
{
	return Nominal;
}

xNominalCondition * xNominalCondition::Clone()
{
	xNominalCondition * target;
	assert(target = new xNominalCondition());
	target->CategoryLength = this->CategoryLength;
	target->Category = new bool[this->CategoryLength];
	for(unsigned int i=0;i<this->CategoryLength;i++)
		target->Category[i] = this->Category[i];
	return target;
}

void xNominalCondition::SetCategoryBits(unsigned int index,bool value)
{
	Category[index]=value;
}


void xNominalCondition::SetCategoryByID(unsigned int ID)
{
	for(unsigned int i=0; i< CategoryLength; i++)
		if (i==ID)
			Category[i]=1;
		else
			Category[i]=0;
}


unsigned int xNominalCondition::getCategoryID()
{
	for(unsigned int i=0; i< CategoryLength; i++)
		if (Category[i]==1)
			return i;
}

xBinaryCondition::xBinaryCondition()
{
	value= false;
}
xBinaryCondition::~xBinaryCondition()
{
}


xBinaryCondition& xBinaryCondition::operator=(const xBinaryCondition &rhs)
{
	this->value = rhs.value;
	return *this;
}

//int xBinaryCondition::operator==(const xBinaryCondition &rhs) const
bool xBinaryCondition::isEqual(xCondition *rhs)
{
	return (this->value == ((xBinaryCondition*)rhs)->value);
}


//int xBinaryCondition::operator<(const xBinaryCondition &rhs) const
bool xBinaryCondition::isSpecificThan(xCondition *rhs)
{
	exit(0);
	return (this->value == ((xBinaryCondition*)rhs)->value);
}

// this funciton determines if this is not equal to rhs ?
//int xBinaryCondition::operator>(const xBinaryCondition &rhs) const
bool xBinaryCondition::isGeneralThan(xCondition *rhs)
{
	return (this->value == ((xBinaryCondition*)rhs)->value);
}

ConditionTypes xBinaryCondition::GetConditionType()
{
	return Binary;
}

xBinaryCondition * xBinaryCondition::Clone()
{
	xBinaryCondition * target;
	assert(target = new xBinaryCondition());
	target->value = this->value;
	return target;
}


Classifier::Classifier()
{
	setInitialVariables(0);
}

Classifier::~Classifier()
{
	map<int,xCondition*>::iterator i;
	for(i=ConditionList.begin();i!=ConditionList.end();i++)
		delete (*i).second;
	ConditionList.clear();
}


//Classifier& Classifier::operator=(const Classifier &rhs)
//{
//	map<int, xCondition *>::const_iterator i;
//	for(i= rhs.ConditionList.begin(); i!=rhs.ConditionList.end();i++){
//		this->ConditionList[(*i).first] = (*i).second->Clone();
//	}
//	this->setAct(rhs.getAct());
//	this->setPre(rhs.getPre());
//	this->setPreer(rhs.getPreer());
//	this->setAcc(rhs.getAcc());
//	this->setFit(rhs.getFit());
//	this->setNum(rhs.getNum());
//	this->setExp(rhs.getExp());
//	this->setActionSet(rhs.getActionSet());
//	this->setGaIterationTime(rhs.getGaIterationTime());
//	return *this;
//}


int Classifier::getAct(void){
	int value;
	if (_act >=0 && _act < Configuration::getNUMBER_OF_ACTIONS())
		value = _act;
	else
	{
		cout << "Error reading (out of bound) Action:"<< _act << endl;
		exit(0);
		value = frand() * Configuration::getNUMBER_OF_ACTIONS();
	}
	return value;
}
void Classifier::setAct(int value)
{
	if (value>=0 && value < Configuration::getNUMBER_OF_ACTIONS())
		_act = value;
	else
	{
		cout << "Error assigning (out of bound) Action:"<< value << endl;
		exit(0);
		_act = frand() * Configuration::getNUMBER_OF_ACTIONS();
	}
}
	long double Classifier::getPre(void)
	{
		return _pre;
	}
	void Classifier::setPre(long double value)
	{
		_pre = value;
	}
	long double Classifier::getPreer(void)
	{
		return _preer;
	}
	void Classifier::setPreer(long double value)
	{
		_preer = value;
	}

	long double Classifier::getAcc(void)
	{
		return _acc;
	}

	void Classifier::setAcc(long double value)
	{
		_acc = value;
	}


	long double Classifier::getFit(void)
	{
		return _fit;
	}

	void Classifier::setFit(long double value)
	{
		_fit = value;
	}

	long unsigned int Classifier::getNum(void)
	{
		return _num;
	}

	void Classifier::setNum(long unsigned int value)
	{
		_num = value;
	}

	long unsigned int Classifier::incNum(void)
	{
		if ( (_num + 1) > _num)
			_num ++;
		return _num;
	}
	long unsigned int Classifier::decNum(void)
	{
		if ( (_num - 1) < _num)
			_num--;
		return _num;
	}

	void Classifier::ResetProperties()
	{
		_exp = 1;
		_expPositive =1;
		_expNegative = 0;
	}

	long unsigned int Classifier::getExp(void)
	{
		return _exp;
	}

	void Classifier::setExp(long unsigned int value)
	{
		_exp = value;
	}

	long unsigned int Classifier::incExp(void)
	{
		if ( (_exp + 1) > _exp)
			_exp ++;
		return _exp;
	}

	long unsigned int Classifier::getExpPositive(void)
	{
		return _expPositive;
	}

	void Classifier::setExpPositive(long unsigned int value)
	{
		_expPositive = value;
	}

	long unsigned int Classifier::incExpPositive(void)
	{
		if ( (_expPositive + 1) > _expPositive)
			_expPositive ++;
		return _expPositive;
	}

	long unsigned int Classifier::getExpNegative(void)
	{
		return _expNegative;
	}

	void Classifier::setExpNegative(long unsigned int value)
	{
		_expNegative = value;
	}

	long unsigned int Classifier::incExpNegative(void)
	{
		if ( (_expNegative + 1) > _expNegative)
			_expNegative ++;
		return _expNegative;
	}

	long double Classifier::getActionSet(void)
	{
		return _actionSet;
	}

	void Classifier::setActionSet(long double value)
	{
		_actionSet = value;
	}

	long unsigned int Classifier::getGaIterationTime(void)
	{
		return _gaIterationTime;
	}

	void Classifier::setGaIterationTime(long unsigned int value)
	{
		_gaIterationTime = value;
	}

// this operator DOES check act part
//int Classifier::operator==(const Classifier &rhs)
bool Classifier::isEqual(Classifier *rhs)
{
	if (this->getAct() != rhs->getAct())
		return false;

	map<int,xCondition*>::const_iterator i,j;
	for(i= this->ConditionList.begin(); i!=this->ConditionList.end();i++)
	{
		j=rhs->ConditionList.find((*i).first);
		if( j == rhs->ConditionList.end() ) // Not found
			return 0;
		else								// found
			if ( !(*i).second->isEqual((*j).second) )
				return false;
	}

	for(i= rhs->ConditionList.begin(); i!=rhs->ConditionList.end();i++)
		if( this->ConditionList.find((*i).first) == this->ConditionList.end() ) // Not found
			// there is an instance that belonged to rhs and not in  this
			return false;

   return true;

//	//xCondition * left, * right;
//	map<int,xCondition*>::const_iterator i,j;
//
//	for(unsigned int index=0;index < Configuration::getCONDITION_LENGTH() ; index++ )
//	{
//		i= this->ConditionList.find(index);
//		j= rhs.ConditionList.find(index);
//		if( i != this->ConditionList.end() && j != rhs.ConditionList.end())
//		{
//			if (!((*(*i).second) == (*(*j).second)))
//				return 0;
//		}
//		else
//			return 0;
//
//	}
//
//   return 1;
}


// this operator doesn't check act part
// see if this can be more specific than rhs or not
bool Classifier::isSpecificThan(Classifier *rhs) // specific
//int Classifier::operator<(const Classifier &rhs)
{
	bool Equal=true;
	if (this->ConditionList.size() < rhs->ConditionList.size())
		return false; // Obviously this has more conditions, so it is not more general

	map<int,xCondition*>::const_iterator i,j;
	for(i= this->ConditionList.begin(); i!=this->ConditionList.end();i++)
	{
		j=rhs->ConditionList.find((*i).first);
		if( j != rhs->ConditionList.end() ) // found
		{
			if ( !(*i).second->isSpecificThan((*j).second) ) // it means Not more general
				return false;

			// it may more general or equal
			if (! (*i).second->isEqual((*j).second)) // it means Not equal
				Equal =false;
		}
	}

	if (!Equal)
	{
		for(i= rhs->ConditionList.begin(); i!=rhs->ConditionList.end();i++)
			if( this->ConditionList.find((*i).first) == this->ConditionList.end() ) // Not found
				// there is an instance that belonged to rhs and not in  this
				return false;
	}

	return !Equal;
}

// this operator check if <this> is more general than <rhs> or not
// it equality is not in concern
bool Classifier::isGeneralThan(Classifier *rhs) // general
//int Classifier::operator>(const Classifier &rhs)
{
	//bool Equal=true;
	if (this->ConditionList.size() > rhs->ConditionList.size())
		return false; // Obviously this has more conditions, so it is not more general

	map<int,xCondition*>::const_iterator i,j;
	for(i= this->ConditionList.begin(); i!=this->ConditionList.end();i++)
	{
		j=rhs->ConditionList.find((*i).first);
		if( j != rhs->ConditionList.end() ) // found
		{
			//if ( !(*i).second->isGeneralThan((*j).second) ) // it means Not more general
			//	return false;

			//// it may more general or equal
			//if (! (*i).second->isEqual((*j).second)) // it means Not equal
			//	Equal =false;

			if ( (*i).second->isGeneralThan((*j).second) ) // it means Not more general
				return true;
		}
		else
			return false;

	}

   return false;
}

// creates a matching classifier, deletes a classifier, if necessary, and adds the new classifier to the population
// * the created classifier matches state
void Classifier::createCoverMatch(map<int,xCondition*> *state,int action, int sIndex,int GeneNbr)
{
	// set all the parameters of a classifier

	createMatchingCondition(state,sIndex,GeneNbr);
	//setRandomAction();
	this->setAct(action);

	// reduce prediction error and fitness
	this->setPreer(this->getPreer() *0.25);
	this->setFit(this->getFit()* Configuration::getFITNESS_REDUCTION_IN_NEW_CLASSIFIERS());

}


/* creates a matching classifier, deletes a classifier, if necessary, and adds the new classifier to the population
 * the created classifier matches state
void Classifier::makeGeneralClassifier(Classifier *cl)
{
	//unsigned int maxActive= frand()*cl->ConditionList.size();
	// set all the parameters of a classifier
	for(map<int,xCondition*>::iterator i =cl->ConditionList.begin(); i != cl->ConditionList.end(); i++)
	{
		if(frand() > 0.9)
		{
			xCondition *cond;
			int index=(*i).first;
			xCondition* sCond=(*i).second;
			assert(cond = new xCondition());
			cond->center =sCond->center;
			cond->range =frand()* Configuration::getRANGE_INI();
			this->ConditionList[index] = cond;
			//maxActive--;
		}
		//if(maxActive<=0)
		//	break;
	}
	this->act = cl->act;
	preer*=0.25;
	fit*=Configuration::getFITNESS_REDUCTION_IN_NEW_CLASSIFIERS();
}

*/

/* creates a matching classifier, deletes a classifier, if necessary, and adds the new classifier to the population
 * the created classifier matches state */
void Classifier::createSingleGeneCoverMatch(map<int,xCondition*> *state,int action, int GeneNbr)
{
	/* set all the parameters of a classifier */
	createSingleGeneMatchingCondition(state,GeneNbr);
	//setRandomAction();
	this->setAct(action);

	/* reduce prediction error and fitness */
	this->setPreer(this->getPreer() *0.25 );
	this->setFit(this->getFit() * Configuration::getFITNESS_REDUCTION_IN_NEW_CLASSIFIERS());
}

/* creates a Non matching classifier, deletes a classifier, if necessary, and adds the new classifier to the population
 * the created classifier matches state */
void Classifier::createSingleGeneCoverNotMatch(map<int,xCondition*> *state,int action, int GeneNbr)
{
	/* set all the parameters of a classifier */
	createSingleGeneMatchingCondition(state,GeneNbr);
	//setRandomAction();
	this->setAct(action);

	/* reduce prediction error and fitness */
	this->setPreer(this->getPreer() *0.25);
	this->setFit(this->getFit()* Configuration::getFITNESS_REDUCTION_IN_NEW_CLASSIFIERS());
}



/* Creates a condition that matches mstring and chooses DON'T-CARE with probability DONTCARE_PROBABILITY */
void Classifier::createMultipleGeneCoverMatch(map<int,xCondition*> *state,int action, int numCare,int minIndex, int maxIndex)
{
	//unsigned int numCare=Configuration::getCONDITION_LENGTH();
	//unsigned int distance= Configuration::getCONDITION_LENGTH() -2;
	set <unsigned int> lstIncludedGenes;
	set <unsigned int>::iterator it;

	//numCare = (unsigned int)( ((double)iteration / (double)Configuration::getMAX_NR_STEPS()) * numCare*frand());
	if (numCare < 2)
		numCare = 2;

	if (numCare >= (int)Configuration::getCONDITION_LENGTH())
		numCare = (int)((double)Configuration::getCONDITION_LENGTH()*frand());

	// creating random index for cover rule with numCare active attribute
	while (numCare >0)
	{
		int length= maxIndex - minIndex;
		unsigned int index=(int)((double)length * frand())+minIndex;
		it = lstIncludedGenes.find(index);
		if (it  == lstIncludedGenes.end()) // Not found
		{
			lstIncludedGenes.insert(index);
			numCare--;
		}
	}

	map<int,xCondition*>::const_iterator i;
	for(it=lstIncludedGenes.begin();it!=lstIncludedGenes.end();it++)
	{
		i=state->find((*it));
		if (i  != state->end()) // found
		{
			this->ConditionList[(*i).first] =  (*i).second->Clone();
			if ((*i).second->GetConditionType() == RealNormalized)
			// this->ConditionList[GeneNbr]->range == 0 and should be assigned to different number
				((xRealNormalizedCondition*)this->ConditionList[(*i).first])->createRandomRangeValues();
			else if ((*i).second->GetConditionType() == Real)
			// this->ConditionList[GeneNbr]->range == 0 and should be assigned to different number
				((xRealCondition*)this->ConditionList[(*i).first])->createRandomRangeValues();
//				((xRealCondition*)this->ConditionList[(*i).first])->range  = frand()*(Configuration::getRANGE_INI());
/*
			else if ((*i).second->GetConditionType() == Nominal )// Nominal
			{
				for(unsigned int j=0;j<((xNominalCondition *)(*i).second)->CategoryLength;j++)
					if (!((xNominalCondition *)(*i).second)->Category[j] && frand() < Configuration::getMUTPMUE())
						((xNominalCondition *)(*i).second)->Category[j] = true;

			}
*/
		}
	}

	this->setAct(action);

	/* reduce prediction error and fitness */
	this->setPreer(this->getPreer() * 0.25);
	this->setFit(this->getFit() *Configuration::getFITNESS_REDUCTION_IN_NEW_CLASSIFIERS());
}

/* Creates a condition that doesn't matches mstring and chooses DON'T-CARE with probability DONTCARE_PROBABILITY
void Classifier::createMultipleGeneCoverNotMatch(map<int,xCondition*> *state,int action, int iteration)
{
	unsigned int numCare=Configuration::getCONDITION_LENGTH();
	//unsigned int distance= Configuration::getCONDITION_LENGTH() -2;
	set <unsigned int> lstIncludedGenes;
	set <unsigned int>::iterator it;

	numCare = (unsigned int)( ((double)iteration / (double)Configuration::getMAX_NR_STEPS()) * numCare*frand());
	if (numCare < 2)
		numCare = 2;

	// creating random index for cover rule with numCare active attribute
	while (numCare >0)
	{
		unsigned int index=(int)(Configuration::getCONDITION_LENGTH() * frand());
		it = lstIncludedGenes.find(index);
		if (it  == lstIncludedGenes.end()) // Not found
		{
			lstIncludedGenes.insert(index);
			numCare--;
		}
	}

	map<int,xCondition*>::const_iterator i;
	for(it=lstIncludedGenes.begin();it!=lstIncludedGenes.end();it++)
	{
		i=state->find((*it));
		if (i  != state->end()) // found
		{
			xCondition *cond;
			assert(cond = new xCondition());

			if (frand() > 0.5)
				cond->center = ((*i).second->center -Configuration::getRANGE_INI() )* frand();
			else
				cond->center = (*i).second->center + ((1 - (*i).second->center -Configuration::getRANGE_INI())* frand());

			cond->range = (frand()* Configuration::getRANGE_INI() );
			ConditionList[(*i).first] =cond ;
		}
	}

	act=action;

	 //reduce prediction error and fitness
	preer*=0.25;
	fit*=Configuration::getFITNESS_REDUCTION_IN_NEW_CLASSIFIERS();
}

*/

/* Creates a condition that matches mstring and chooses DON'T-CARE with probability DONTCARE_PROBABILITY
void Classifier::createMatchingCondition(map<int,xCondition*> *state, int type)
{

	unsigned int numCare=Configuration::getCONDITION_LENGTH();
	unsigned int distance= Configuration::getCONDITION_LENGTH() -2;

  if(type >1)
	  numCare =Configuration::getCONDITION_LENGTH() -  (int)( ((double)distance /(double) (Configuration::getNR_DIFFERENT_ISLANDS()-1) )* (double)(type));

  map<int,xCondition*>::const_iterator i;
  for(i=state->begin();i!=state->end();i++)
  {
	if (numCare >= Configuration::getCONDITION_LENGTH() || (frand() > 0.5 && numCare>0 ) )
	{
		numCare --;
		xCondition *cond;
		assert(cond = new xCondition());

		cond->center = (*i).second->center;
		if (frand() > 0.5 )
			cond->range = (*i).second->range + (frand()* Configuration::getRANGE_INI());
		else
			cond->range = (*i).second->range - (frand()* Configuration::getRANGE_INI());
		ConditionList[(*i).first] =cond ;
	}
  }
}
*/

// Creates a condition that matches state and chooses DON'T-CARE with probability DONTCARE_PROBABILITY
void Classifier::createMatchingCondition(map<int,xCondition*> *state,int sIndex,int GeneNbr)
{
	map<int,xCondition*>::const_iterator i;
	int cout=0;
	for(i=state->find(sIndex);i!=state->end() && cout < GeneNbr;i++,cout++)
	{
		if( frand()  >  Configuration::getDONTCARE_PROBABILITY() ) {
			this->ConditionList[sIndex+cout] =  (*i).second->Clone();
			if ((*i).second->GetConditionType() == RealNormalized)
				// this->ConditionList[GeneNbr]->range == 0 and should be assigned to different number
				((xRealNormalizedCondition*)this->ConditionList[sIndex+cout])->createRandomRangeValues();
			else if ((*i).second->GetConditionType() == Real)
				// this->ConditionList[GeneNbr]->range == 0 and should be assigned to different number
				((xRealCondition*)this->ConditionList[sIndex+cout])->createRandomRangeValues();
		}
	}
}


/* Creates a condition that matches mstring and chooses DON'T-CARE with probability DONTCARE_PROBABILITY */
void Classifier::createSingleGeneMatchingCondition(map<int,xCondition*> *state,int GeneNbr)
{
	map<int,xCondition*>::const_iterator i;
	i = state->find(GeneNbr);
	if (i !=  state->end()) // this part should be involved
	{
		this->ConditionList[GeneNbr] =  (*i).second->Clone();
		if ((*i).second->GetConditionType() == RealNormalized)
			// this->ConditionList[GeneNbr]->range == 0 and should be assigned to different number
			((xRealNormalizedCondition*)this->ConditionList[GeneNbr])->createRandomRangeValues();
		else if ((*i).second->GetConditionType() == Real)
			// this->ConditionList[GeneNbr]->range == 0 and should be assigned to different number
			((xRealCondition*)this->ConditionList[GeneNbr])->createRandomRangeValues();

		/*
		else if ((*i).second->GetConditionType() == Nominal )// Nominal
		{
			for(unsigned int j=0;j<((xNominalCondition *)(*i).second)->CategoryLength;j++)
				if (!((xNominalCondition *)(*i).second)->Category[j] && frand()< Configuration::getMUTPMUE())
					((xNominalCondition *)(*i).second)->Category[j] = true;

		}
*/
	}
}

/* Creates a condition that matches mstring and chooses DON'T-CARE with probability DONTCARE_PROBABILITY
void Classifier::createSingleGeneNotMatchingCondition(map<int,xCondition*> *state,int GeneNbr)
{
	map<int,xCondition*>::const_iterator i;
	i = state->find(GeneNbr);
	if (i !=  state->end()) // this part should be involved
	{
		xCondition *cond;
		assert(cond = new xCondition());
		if (frand() > 0.5)
			cond->center = ((*i).second->center -Configuration::getRANGE_INI()) * frand() ;
		else
			cond->center = (*i).second->center+ ((1-(*i).second->center -Configuration::getRANGE_INI())* frand()) ;

		cond->range  = (frand()* Configuration::getRANGE_INI());
		this->ConditionList[GeneNbr] =cond ;
	}
}
*/

/* sets all initial varibles of a classifier list ->used by various funktions */
void Classifier::setInitialVariables(long unsigned int itTime)
{
	this->setPre(Configuration::getPRE_INI());
	this->setPreer(Configuration::getPREER_INI());
	this->setAcc(Configuration::getFIT_INI());
	this->setFit(Configuration::getFIT_INI());
	this->setNum(1);
	this->setExp(1);
	this->setExpPositive(1);
	this->setExpNegative(0);
	this->setActionSet(1.0);
	this->setGaIterationTime(itTime);
}

void Classifier::setRandomAction(void)
{
	int value = (int) (frand()* (double)Configuration::getNUMBER_OF_ACTIONS()) ;
	this->setAct( value );
}


// check if classifier this subsumes cl
// means this is more general than Cl
bool Classifier::subsumes (Classifier *cl,int thetaSub)
{
	  bool ret=false;
//	  ret= (this->act==cl->act && this->isSubsumer(thetaSub) && this->isGeneralThan(cl));
	  ret= (this->getAct()==cl->getAct() && this->isGeneralThan(cl));
	  return ret;
//  bool ret=false;
//  ret= //( this->exp >= Configuration::getSUBSUME_EXPERIENCE()) &&
//		  //(this->acc >= 0.99)		  &&
//		  (this->act == cl->act)
//		  && ( this->isSpecificThan(cl) )
//		  ;
//
//  return ret;
}

/**
 * Returns if the classifier satisfies the criteria for being a subsumer.
 */
int Classifier::isSubsumer(int thetaSub)
{
	return this->getExp() > thetaSub && this->getAcc() == 1.0;
}

// print a single classifier to the file fp
void Classifier::dumpClassifier()
{
	int condSize=this->ConditionList.size();
	if (condSize != 0)
	{
		//(*file) << "CondLength=" << condSize << ":";
		printf("CondLength=%c[%d;%dm%d%c[%dm:",SETTXTCOLOR, RED,BLINK,condSize,SETTXTCOLOR, RESETTXTCOLOR);
		for(map<int,xCondition *>::const_iterator i=this->ConditionList.begin();i!=this->ConditionList.end();i++)
		{
			int index = (*i).first;
			ConditionTypes cndType = ((xCondition *)(*i).second)->GetConditionType();
			if (cndType == (ConditionTypes)Nominal)
			{
				xNominalCondition *cond;
				cond = (xNominalCondition*) (*i).second;
				char *cCategory =new char[cond->CategoryLength+1];

				for(int k=0;k<cond->CategoryLength;k++)
				{
					cCategory[k]= cond->Category[k]==true ? '1' : '0';
				}
				cCategory[cond->CategoryLength]= '\0';
				printf("%d[%s],",index,cCategory);

				delete[] cCategory;
			} else if (cndType == (ConditionTypes)RealNormalized)
			{
				xRealNormalizedCondition *cond;
				cond = (xRealNormalizedCondition*) (*i).second;
				printf("%d(%.10lf,%.10lf),",index,cond->getCenterValue(),cond->getRangeValue());
			} else if (cndType == (ConditionTypes)Real)
			{
				xRealCondition *cond;
				cond = (xRealCondition*) (*i).second;
				printf("%d(%.10lf,%.10lf),",index,cond->getCenterValue(),cond->getRangeValue());
			} else if (cndType == (ConditionTypes)Binary)
			{
				xBinaryCondition *cond;
				cond = (xBinaryCondition*) (*i).second;
				char cCategory;
				cCategory = cond->value==true ? '1' : '0';
				printf("%c[%dm%d[%c]%c[%dm,", SETTXTCOLOR, CYAN, index,cCategory ,SETTXTCOLOR, RESETTXTCOLOR);
			}


		}

		printf("act:%c[%dm%d%c[%dm,", SETTXTCOLOR, GREEN, (int)this->getAct() ,SETTXTCOLOR, RESETTXTCOLOR);
		printf("pre:%.40Lf,",(long double)this->getPre());
		printf("preer:%.40Lf,",(long double)this->getPreer());
		printf("acc:%.40Lf,",(long double)this->getAcc());
		printf("fit:%.40Lf,",(long double)this->getFit());
		printf("num:%lu,",(long unsigned int)this->getNum());
		printf("exp:%lu,",(long unsigned int)this->getExp());
		printf("expPositive:%lu,",(long unsigned int)this->getExpPositive());
		printf("expNegative:%lu,",(long unsigned int)this->getExpNegative());
		printf("actionsest:%.40Lf,",(long double)this->getActionSet());
		printf("gaIterationTime:%lu\n",(long unsigned int)this->getGaIterationTime());

	}
}


/* print a single classifier to the file fp */
void Classifier::dumpClassifier(FILE * fp)
{
	int condSize=this->ConditionList.size();
	//if (condSize != 0)
	{
		//(*file) << "CondLength=" << condSize << ":";
		fprintf(fp,"CondLength=%d:",condSize);
		for(map<int,xCondition *>::const_iterator i=this->ConditionList.begin();i!=this->ConditionList.end();i++)
		{
			int index = (*i).first;
			ConditionTypes cndType = ((xCondition *)(*i).second)->GetConditionType();
			if (cndType == (ConditionTypes)Nominal)
			{
				xNominalCondition *cond;
				cond = (xNominalCondition*) (*i).second;
				char *cCategory = new char[cond->CategoryLength+1];

				for(int k=0;k<cond->CategoryLength;k++)
				{
					cCategory[k]= cond->Category[k]==true ? '1' : '0';
				}
				cCategory[cond->CategoryLength]= 0;
				fprintf(fp,"%d[%s],",index,cCategory);

				delete[] cCategory;

			} else if (cndType == (ConditionTypes)RealNormalized)
			{
				xRealNormalizedCondition *cond;
				cond = (xRealNormalizedCondition*) (*i).second;
				fprintf(fp,"%d(%.10lf,%.10lf),",index,cond->getCenterValue(),cond->getRangeValue());
			} else if (cndType == (ConditionTypes)Real)
			{
				xRealCondition *cond;
				cond = (xRealCondition*) (*i).second;
				printf("%d(%.10lf,%.10lf),",index,cond->getCenterValue(),cond->getRangeValue());
			} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)Binary)
			{
				xBinaryCondition *cond;
				cond = (xBinaryCondition*) (*i).second;
				char cValue = cond->value==true ? '1' : '0';
				fprintf(fp,"%d[%c],",index,cValue);
			}
		}

		fprintf(fp,"act:%d,",(int)this->getAct());
		fprintf(fp,"pre:%.20Lf,",(long double)this->getPre());
		fprintf(fp,"preer:%.20Lf,",(long double)this->getPreer());
		fprintf(fp,"acc:%.20Lf,",(long double)this->getAcc());
		fprintf(fp,"fit:%.20Lf,",(long double)this->getFit());
		fprintf(fp,"num:%lu,",(long unsigned int)this->getNum());
		fprintf(fp,"exp:%lu,",(long unsigned int)this->getExp());
		fprintf(fp,"expPositive:%lu,",(long unsigned int)this->getExpPositive());
		fprintf(fp,"expNegative:%lu,",(long unsigned int)this->getExpNegative());
		fprintf(fp,"actionsest:%.20Lf,",(long double)this->getActionSet());
		fprintf(fp,"gaIterationTime:%lu\n",(long unsigned int)this->getGaIterationTime());

	}
}

/* print a single classifier to the file fp */
//void Classifier::dumpClassifierCompressed(FILE * fp)
//{
//	int stringSize = Configuration::getCONDITION_LENGTH() * 100;
//	  char strInLine[stringSize ];
//	  char strOutLine[stringSize ];
//	  char *scratch;
//
//	    // allocate and initially zero out the scratch buffer. After this, make sure it is
//	    // preserved across calls and never modified manually
//	    scratch = (char*) malloc(QLZ_SCRATCH_COMPRESS);
//		memset(scratch, 0, QLZ_SCRATCH_COMPRESS);
//
//	int condSize=this->ConditionList.size();
//	//if (condSize != 0)
//	{
//		//(*file) << "CondLength=" << condSize << ":";
////		fprintf(fp,"CondLength=%d:",condSize);
//		sprintf(strInLine, "CondLength=%d:",condSize);
//		for(map<int,xCondition *>::const_iterator i=this->ConditionList.begin();i!=this->ConditionList.end();i++)
//		{
//			int index = (*i).first;
//			if (Classifier::ConditionListTypes[index] == (ConditionTypes)Nominal)
//			{
//				xNominalCondition *cond;
//				cond = (xNominalCondition*) (*i).second;
//				char cCategory[cond->CategoryLength+1];
//
//				for(int k=0;k<cond->CategoryLength;k++)
//				{
//					cCategory[k]= cond->Category[k]==true ? '1' : '0';
//				}
//				cCategory[cond->CategoryLength]= 0;
//				//fprintf(fp,"%d[%s],",index,cCategory);
//				sprintf(strInLine + strlen(strInLine), "%d[%s],",index,cCategory);
//
//			} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)RealNormalized)
//			{
//				xRealNormalizedCondition *cond;
//				cond = (xRealNormalizedCondition*) (*i).second;
//				//fprintf(fp,"%d(%.20f,%.20f),",index,cond->center,cond->range);
//				sprintf(strInLine + strlen(strInLine), "%d(%.20f,%.20f),",index,cond->center,cond->range);
// 			} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)Real)
//			{
//				xRealCondition *cond;
//				cond = (xRealCondition*) (*i).second;
//				//fprintf("%d(%.100lf,%.100lf),",index,cond->getCenterValue(),cond->getRangeValue());
//				sprintf(strInLine + strlen(strInLine), "%d(%.20lf,%.20lf),",index,cond->getCenterValue(),cond->getRangeValue());
//			} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)Binary)
//			{
//				xBinaryCondition *cond;
//				cond = (xBinaryCondition*) (*i).second;
//				char cValue = cond->value==true ? '1' : '0';
//				//fprintf(fp,"%d[%c],",index,cValue);
//				sprintf(strInLine + strlen(strInLine), "%d[%c],",index,cValue);
//			}
//		}
//
//		sprintf(strInLine + strlen(strInLine), "act:%d,",(int)this->act);
//		sprintf(strInLine + strlen(strInLine), "pre:%.100Lf,",(long double)this->pre);
//		sprintf(strInLine + strlen(strInLine), "preer:%.100Lf,",(long double)this->preer);
//		sprintf(strInLine + strlen(strInLine), "acc:%.100Lf,",(long double)this->acc);
//		sprintf(strInLine + strlen(strInLine), "fit:%.100Lf,",(long double)this->fit);
//		sprintf(strInLine + strlen(strInLine), "num:%lu,",(long unsigned int)this->num);
//		sprintf(strInLine + strlen(strInLine), "exp:%lu,",(long unsigned int)this->exp);
//		sprintf(strInLine + strlen(strInLine), "expPositive:%lu,",(long unsigned int)this->expPositive);
//		sprintf(strInLine + strlen(strInLine), "expNegative:%lu,",(long unsigned int)this->expNegative);
//		sprintf(strInLine + strlen(strInLine), "actionsest:%.40Lf,",(long double)this->actionsest);
//		sprintf(strInLine + strlen(strInLine), "gaIterationTime:%lu\n",(long unsigned int)this->gaIterationTime);
//
////		fprintf(fp,"act:%d,",(int)this->act);
////		fprintf(fp,"pre:%.100Lf,",(long double)this->pre);
////		fprintf(fp,"preer:%.100Lf,",(long double)this->preer);
////		fprintf(fp,"acc:%.100Lf,",(long double)this->acc);
////		fprintf(fp,"fit:%.100Lf,",(long double)this->fit);
////		fprintf(fp,"num:%lu,",(long unsigned int)this->num);
////		fprintf(fp,"exp:%lu,",(long unsigned int)this->exp);
////		fprintf(fp,"expPositive:%lu,",(long unsigned int)this->expPositive);
////		fprintf(fp,"expNegative:%lu,",(long unsigned int)this->expNegative);
////		fprintf(fp,"actionsest:%.40Lf,",(long double)this->actionsest);
////		fprintf(fp,"gaIterationTime:%lu\n",(long unsigned int)this->gaIterationTime);
//
//	}
//
//    size_t d, c;
//	d = strlen(strInLine);
//    c = qlz_compress(strInLine, strOutLine, d, scratch);
//    printf("%u bytes compressed into %u\n", (unsigned int)d, (unsigned int)c);
//
//    // the buffer "compressed" now contains c bytes which we could have sent directly to a
//    // decompressing site for decompression
//    fwrite(strOutLine, c, 1, fp);
//
//}

// print a single classifier to the file fp
void Classifier::loadClassifier(FILE *fp)
{
	int index;
	double C,R;
	char cBinary;
	int ConditionLength;
	map<int,xCondition*>::iterator i;
	for(i=ConditionList.begin();i!=ConditionList.end();i++)
		delete (*i).second;
	ConditionList.clear();

	fscanf(fp,"CondLength=%d:",&ConditionLength);
	for(int j=0;j<ConditionLength;j++)
	{
		fscanf(fp,"%d",&index);
		if (Classifier::ConditionListTypes[index] == (ConditionTypes)BinaryNone) {
			char cValue;
			fscanf(fp,"[%c],",&cValue);
			continue;
		} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)RealNone) {
					char cValue;
					fscanf(fp,"(%lf,%lf),",&C,&R);
					continue;
				}
		else
		{
			if (Classifier::ConditionListTypes[index] == (ConditionTypes)Nominal)
			{
				xNominalCondition *cond;
				fscanf(fp,"[");
				assert(cond = new xNominalCondition());

				map<int,string> convertMap=Classifier::attributesConvertMap[j];
				cond->CategoryLength = convertMap.size();
				//cond->Category = new bool[cond->CategoryLength];
				for(int k=0;k<cond->CategoryLength;k++)
				{
					fscanf(fp,"%c",&cBinary);
					cond->SetCategoryBits(k, cBinary=='1' );
				}
				this->ConditionList[index]= cond;
				fscanf(fp,"],");

			} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)RealNormalized)
			{
				xRealNormalizedCondition *cond;
				fscanf(fp,"(%lf,%lf),",&C,&R);
				assert(cond = new xRealNormalizedCondition());
				cond->setCenterValue(C);
				cond->setRangeValue(R);
				this->ConditionList[index]=cond;
			} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)Real)
			{
				xRealCondition *cond;
				fscanf(fp,"(%lf,%lf),",&C,&R);
				assert(cond = new xRealCondition( Classifier::ConditionListLowerBound[index], Classifier::ConditionListUpperBound[index] ));
				cond->setCenterValue(C);
				cond->setRangeValue(R);
				this->ConditionList[index]=cond;
			} else if (Classifier::ConditionListTypes[index] == (ConditionTypes)Binary)
			{
				char cValue;
				xBinaryCondition *cond;
				fscanf(fp,"[%c],",&cValue);
				assert(cond = new xBinaryCondition());
				cond->value = cValue== '1' ? true : false;
				this->ConditionList[index]= cond;
			}
		}

	}


	long double f;
	int d;
	long unsigned int u;
	fscanf(fp,"act:%d,",&d);
	this->setAct(d);
	fscanf(fp,"pre:%Lf,",&f);
	this->setPre((long double )f);
	fscanf(fp,"preer:%Lf,",&f);
	this->setPreer( (long double )f);
	fscanf(fp,"acc:%Lf,",&f);
	this->setAcc((long double )f);
	fscanf(fp,"fit:%Lf,",&f);
	this->setFit((long double )f);
	fscanf(fp,"num:%lu,",&u);
	this->setNum((long unsigned int)u);
	fscanf(fp,"exp:%lu,",&u);
	this->setExp((long unsigned int)u);
	fscanf(fp,"expPositive:%lu,",&u);
	this->setExpPositive((long unsigned int)u);
	fscanf(fp,"expNegative:%lu,",&u);
	this->setExpNegative((long unsigned int)u);
	fscanf(fp,"actionsest:%Lf,",&f);
	this->setActionSet((long double )f);
	fscanf(fp,"gaIterationTime:%lu\n",&u);
	this->setGaIterationTime((long unsigned int)u);
}

void Classifier::copyClone(Classifier *clSource)
{
	for(map<int,xCondition*>::iterator i=this->ConditionList.begin();i!=this->ConditionList.end();i++)
		delete (*i).second;

	this->ConditionList.clear();

	for(map<int,xCondition*>::const_iterator i= clSource->ConditionList.begin(); i!=clSource->ConditionList.end();i++)
	{
		xCondition *x;
		int index = (*i).first;
		this->ConditionList[index] = ((xCondition *)(*i).second)->Clone();
	}
	this->setAct(clSource->getAct());
}

void Classifier::Copy(Classifier *clSource)
{
	this->copyClone(clSource);
	this->CopyProperties(clSource);
}

void Classifier::CopyProperties(Classifier *clSource)
{
	this->setPre(clSource->getPre());
	this->setPreer(clSource->getPreer());
	this->setAcc(clSource->getAcc());
	this->setFit(clSource->getFit());
	this->setNum(clSource->getNum());
	this->setExp(clSource->getExp());
	this->setExpPositive(clSource->getExpPositive());
	this->setExpNegative(clSource->getExpNegative());
	this->setActionSet(clSource->getActionSet());
	this->setGaIterationTime(clSource->getGaIterationTime());
}

void Classifier::CopyExceptCondisionts(Classifier *clSource)
{
	//for(map<int,xCondition*>::iterator i=this->ConditionList.begin();i!=this->ConditionList.end();i++)
		//delete (*i).second;

	this->ConditionList.clear();

	this->setAct(clSource->getAct());
	this->CopyProperties(clSource);
}

/*

void Classifier::copyCloneExceptCondisionts(Classifier *clSource)
{
	for(map<int,xCondition*>::iterator i=this->ConditionList.begin();i!=this->ConditionList.end();i++)
		delete (*i).second;

	this->ConditionList.clear();

	this->act = clSource->act;
	this->pre = clSource->pre;
	this->preer = clSource->preer;
	this->acc = clSource->acc;
	this->fit = clSource->fit;
	this->num = clSource->num;
	this->exp = clSource->exp;
	this->expPositive = clSource->expPositive;
	this->expNegative = clSource->expNegative;
	this->actionsest = clSource->actionsest;
	this->gaIterationTime = clSource->gaIterationTime;
}
*/

void Classifier::shrinkClassifier(void)
{
	double min,max;
	if (this->ConditionList.empty())
		return;

	map<int,xCondition *>::iterator i;
	list<int> deletableIndex;
	for(i=this->ConditionList.begin();i!=this->ConditionList.end();i++)
	{
		if ((*i).second->GetConditionType() == RealNormalized)
		{
			int index=(*i).first;
			xRealNormalizedCondition * cond;
			cond = ((xRealNormalizedCondition*)(*i).second);
			// if a condition is cover al range, delete it because it is don't care
			if(cond->getLowerValue() <=0 && cond->getUpperValue() >= 1){
				deletableIndex.push_back(index);
			}
		}
		else if ((*i).second->GetConditionType() == Real)
		{
			int index=(*i).first;
			xRealCondition * cond;
			cond = ((xRealCondition*)(*i).second);

//			min = cond->center + cond->range <= cond->center - cond->range  ? cond->center + cond->range  : cond->center - cond->range ;
//			max = cond->center + cond->range >= cond->center - cond->range  ? cond->center + cond->range  : cond->center - cond->range ;
//			min = min < *cond->pLowerbound ? *cond->pLowerbound : min;
//			min = min > *cond->pUpperbound ? *cond->pUpperbound : min;
//			max = max < *cond->pLowerbound ? *cond->pLowerbound : max;
//			max = max > *cond->pUpperbound ? *cond->pUpperbound : max;
			// if a condition is cover al range, delete it because it is don't care
//			if(min <= *cond->pLowerbound && max >= *cond->pUpperbound){
				//this->ConditionList.erase(i);
				//delete cond;
				//delete ((xCondition *)(*i).second);
			if (  cond->getLowerValue() <= cond->getLowerBound()
					&&
				 cond->getUpperValue() >= cond->getUpperBound()
				 )
				deletableIndex.push_back(index);
		}
		else if ((*i).second->GetConditionType() == Nominal)
		{
			int index=(*i).first;
			bool ContCare=true;
			for(unsigned int t=0;t< ((xNominalCondition*)(*i).second)->CategoryLength ; t++)
				if (((xNominalCondition*)(*i).second)->Category[t]==false)
				{
					ContCare = false;
					break;  // Not ALL True
				}
			if (!ContCare)
			{
				ContCare=true;
				for(unsigned int t=0;t< ((xNominalCondition*)(*i).second)->CategoryLength ; t++)
					if (((xNominalCondition*)(*i).second)->Category[t]==true)
					{
						ContCare = false;
						break;  // Not ALL False
					}
			}
			if (ContCare)
				deletableIndex.push_back(index);
		}else if ((*i).second->GetConditionType() == Binary)
			{
			// Do Nothing
			}


	}

	for(list<int>::const_iterator j= deletableIndex.begin();j != deletableIndex.end();j++ )
	{
		xCondition * cond;
		cond = this->ConditionList[*j];
		delete (cond);
		this->ConditionList.erase((*j));
	}

	deletableIndex.clear();

}

/**
 * Calculates the specificity of the classifier
 */
int Classifier::getSpecificity()
{
  return this->ConditionList.size();
}
