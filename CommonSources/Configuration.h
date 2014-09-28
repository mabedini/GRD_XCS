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
#include <stdio.h>
#include <stdlib.h>

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

class Configuration {
public:

	private:
	    static unsigned int IS_MULTI_STEP;

	    static char TABOUTFILE[100]; /* the output file for the results */
	    static char POPULATIONOUTFILE[100];/* the classifiers output file for the results */
	    static char DATAFILE[100];
	    static char TESTFILE[100];

	    static unsigned int NR_EXPS;
	    static long unsigned int MAX_NR_STEPS;/* the number of problems to solve in one experiment */
	    static long unsigned int MAX_POP_SIZE ;/* the maximum size of the population */
	    static unsigned int MIN_PERDICTION_FOR_DELETE ;   // below min will delete
	    static unsigned int MIN_EXP_FOR_CHECKINGPREDICTION ;

	    static double EXPLORE_EXPLOITE_RATE ;/* the minimum size of the population */

	    static unsigned int NR_MIXING_DIFFERENT_POPULATION ;
	    static unsigned int NR_DIFFERENT_ISLANDS ;
	    static unsigned int NR_PARALLEL_RUNNING_ISLANDS;
	    static double PORTION_OF_MIGRATABLE_INDIVIDUAL ;

	    static unsigned int CONDITION_LENGTH ;//3226 // 33 //12533 //160
	    static unsigned int ACTION_LENGTH ;
	    static unsigned int NUMBER_OF_ACTIONS ;

	    static int PAYMENT_RANGE ;  /* 1000, 1800, 3400, 6600 */
	    static int NPAYMENT_RANGE ;/* 1000, 1800, 3400, 6600 */
	    static int PAYOFF_LANDSCAPE ;
	    static unsigned int NBR_RATING_CATEGORY;



	    static double BETA ;/* Learning Rate parameter */
	    static double GAMMA ;/* Discounting factor that discounts the maximum of the prediction array */
	    static unsigned int MEAN_LAST_GATIME ;//25 /* do a GA if the average of when a GA was done last time, is at least 25 steps ago */
	    static double MINIMUM_ERROR ;/* =epsilon 0 in STEWARTS "CLASSIFIER FITNESS BASED ON ACCURACY" */
	    static double FALL_OFF_RATE ;/* =ALPHA IN STEWARTS "CLASSIFIER FITNESS BASED ON ACCURACY" */

	    static double CROSSPX ;/* the probability to do crossover */
	    static double MUTPMUE ;/* probability of mutating one bit */
	    static double RANGE_INI ;/* initial range when creating a condition */

	    static double DELTA ; /* used to enforce the deletion of low fitness classifiers (= second deletion method) once they reached DELETION_EXPERIENCE */
	    static int DELETION_EXPERIENCE ;/*The experience required to use second deletion method */

	    static double PREDICTION_THRESHOLD ; /* =O| prediction for a loop in the match set ->covering occurs */

	    static double DONTCARE_PROBABILITY ; /* probability of choosing DONT_CARE in a newly generated classifier */

	    static double PRE_INI ; /* prediction initialization in new classifiers */
	    static double PREER_INI; /* prediction error initialization in new classifiers */
	    static double FIT_INI ; /* fitness initialization in new classifiers */

	    static double PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS; /* prediction error reduction in with the discovery component generated classifiers */
	    static double FITNESS_REDUCTION_IN_NEW_CLASSIFIERS ;/* fitness reduction in with the discovery component generated classifiers */
	    static double KAPPA_MULTIPLIER; /* decreases the accuracy value if classifier's prediction error is higher than MINIMUM_ERROR*PAYMENT_RANGE */

	    static double REDUCTION_THRESHOLD; /* experience threshold to reduce the used fitness of a classifier in the prediction array */
	    static double FITNESS_REDUCTION ; /* amount of reducing the fitness of a classifier in the prediction array */

	    static unsigned int SUBSUME_EXPERIENCE ; /* the experience required before can be subsumed */

	    static unsigned int INITIALIZE_POP ; /* Boolean to check if population should be initialized */

	    static double DECREASE_FITNESS_IN_M_NOT_A ; /* Boolean to decrease the fitness of classifiers that are not in the match set */
	    static double FITNESS_REDUCTION_IN_M_NOT_A ; /* reduction of the fitness if not in the action set but in the matchset */

	    static double DECREASE_FITNESS_IF_MUTATED ; /* Boolean to decrease the fitness if being mutated by FITNESS_REDUCTION_IN_NEW_CLASSIFIERS */

	    static long unsigned int TELETRANSPORTATION ; /* possible teletransportation in multi step environments (trial ends if counter exeeds this number) */

		// Tournoment Selection
	    static double TOURNAMENT_SIZE; /* default=0.4; percentage of action set that takes part in tournament; if set to zero, proportionate selection is used; if set greater than one, tournament with fixed size is used */
	    static double FORCE_DIFFERENT_IN_TOURNAMENT; /* default =0; the probability that the two participants must be different */
	    static double SELECT_TOLERANCE; /* default=.001; the tolerance with which classifier fitness/error is considered similar */
	    static unsigned int DO_GA_ERROR_BASED_SELECT; /* default=0; GA selection based on error */

		// For Binary Simulated Xover
	    static unsigned int RIGID ;
	    static double EPSILON ;
	    static unsigned int N_DISTRIBUTION_c;
	    static unsigned int N_DISTRIBUTION_m;
	    static double xbsINFINITY ;
	    static double REAL_MUTATION_SEED ;

	    void static ReadParameterFromFile(FILE *dataFile,char* format,void  *variable);

	public:
		Configuration();
		virtual ~Configuration();
    static unsigned int getIS_MULTI_STEP()
    {
        return IS_MULTI_STEP;
    }

    static char *getTABOUTFILE()
    {
        return TABOUTFILE;
    }

    static char *getPOPULATIONOUTFILE()
    {
        return POPULATIONOUTFILE;
    }

    static char *getDATAFILE()
    {
        return DATAFILE;
    }

    static char *getTESTFILE()
    {
        return TESTFILE;
    }

	static unsigned int getNR_EXPS()
	{
	 return NR_EXPS;
	}

    static long unsigned int getMAX_NR_STEPS()
    {
        return MAX_NR_STEPS;
    }


    static void setMAX_POP_SIZE(unsigned int MaxPopSize)
    {
    	MAX_POP_SIZE = MaxPopSize;
    }

    static long unsigned int getMAX_POP_SIZE()
    {
        return MAX_POP_SIZE;
    }

    static double getEXPLORE_EXPLOITE_RATE()
    {
        return EXPLORE_EXPLOITE_RATE;
    }

    static long unsigned int getMIN_PERDICTION_FOR_DELETE()
    {
        return MIN_PERDICTION_FOR_DELETE;
    }

    static long unsigned int getMIN_EXP_FOR_CHECKINGPREDICTION()
    {
        return MIN_EXP_FOR_CHECKINGPREDICTION;
    }

    static unsigned int getNR_MIXING_DIFFERENT_POPULATION()
    {
        return NR_MIXING_DIFFERENT_POPULATION;
    }

    static unsigned int getNR_DIFFERENT_ISLANDS()
    {
        return NR_DIFFERENT_ISLANDS;
    }

    static void setNR_DIFFERENT_ISLANDS(unsigned int nbrIslands)
    {
        NR_DIFFERENT_ISLANDS = nbrIslands;
    }

    static unsigned int getNR_PARALLEL_RUNNING_ISLANDS()
    {
        return NR_PARALLEL_RUNNING_ISLANDS;
    }


    static double getPORTION_OF_MIGRATABLE_INDIVIDUAL()
    {
        return PORTION_OF_MIGRATABLE_INDIVIDUAL;
    }


    static unsigned int getCONDITION_LENGTH()
    {
        return CONDITION_LENGTH;
    }

    static void setCONDITION_LENGTH(unsigned int cndLength)
    {
        CONDITION_LENGTH = cndLength;
    }

    static unsigned int getACTION_LENGTH()
    {
        return ACTION_LENGTH;
    }

    static void setNUMBER_OF_ACTIONS(unsigned int actNo)
    {
    	NUMBER_OF_ACTIONS= actNo;
    }

    static unsigned int getNUMBER_OF_ACTIONS()
    {
        return NUMBER_OF_ACTIONS;
    }


    static int getPAYMENT_RANGE()
    {
        return PAYMENT_RANGE;
    }


    static int getNPAYMENT_RANGE()
    {
        return NPAYMENT_RANGE;
    }

    static unsigned int getPAYOFF_LANDSCAPE()
    {
        return PAYOFF_LANDSCAPE;
    }

    static unsigned int getNBR_RATING_CATEGORY()
    {
    	return NBR_RATING_CATEGORY;
    }

   static double getBETA()
    {
        return BETA;
    }

    static double getGAMMA()
    {
        return GAMMA;
    }

    static unsigned int getMEAN_LAST_GATIME()
    {
        return MEAN_LAST_GATIME;
    }


    static double getMINIMUM_ERROR()
    {
        return MINIMUM_ERROR;
    }


    static double getFALL_OFF_RATE()
    {
        return FALL_OFF_RATE;
    }


    static double getCROSSPX()
    {
        return CROSSPX;
    }


    static double getMUTPMUE()
    {
        return MUTPMUE;
    }

    static double getRANGE_INI()
    {
        return RANGE_INI;
    }

    static void setRANGE_INI(double  range)
    {
    	RANGE_INI= range;
    }

    static unsigned int getRIGID()
    {
        return RIGID;
    }


    static double getEPSILON()
    {
        return EPSILON;
    }

    static unsigned int getN_DISTRIBUTION_c()
    {
        return N_DISTRIBUTION_c;
    }


    static unsigned int getN_DISTRIBUTION_m()
    {
        return N_DISTRIBUTION_m;
    }

    static double getXbsINFINITY()
    {
        return xbsINFINITY;
    }

    static double getREAL_MUTATION_SEED()
    {
        return REAL_MUTATION_SEED;
    }

    static double getDELTA()
    {
        return DELTA;
    }

    static unsigned int getDELETION_EXPERIENCE()
    {
        return DELETION_EXPERIENCE;
    }

     static double getPREDICTION_THRESHOLD()
    {
        return PREDICTION_THRESHOLD;
    }

    static double getDONTCARE_PROBABILITY()
    {
        return DONTCARE_PROBABILITY;
    }


    static double getPRE_INI()
    {
        return PRE_INI;
    }

    static double getPREER_INI()
    {
        return PREER_INI;
    }


    static double getFIT_INI()
    {
        return FIT_INI;
    }

    static double getPRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS()
    {
        return PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS;
    }


    static double getFITNESS_REDUCTION_IN_NEW_CLASSIFIERS()
    {
        return FITNESS_REDUCTION_IN_NEW_CLASSIFIERS;
    }


    static double getKAPPA_MULTIPLIER()
    {
        return KAPPA_MULTIPLIER;
    }

   static double getREDUCTION_THRESHOLD()
    {
        return REDUCTION_THRESHOLD;
    }


    static double getFITNESS_REDUCTION()
    {
        return FITNESS_REDUCTION;
    }

    static unsigned int getSUBSUME_EXPERIENCE()
    {
        return SUBSUME_EXPERIENCE;
    }


    static unsigned int getINITIALIZE_POP()
    {
        return INITIALIZE_POP;
    }

    static double getDECREASE_FITNESS_IN_M_NOT_A()
    {
        return DECREASE_FITNESS_IN_M_NOT_A;
    }

    static double getFITNESS_REDUCTION_IN_M_NOT_A()
    {
        return FITNESS_REDUCTION_IN_M_NOT_A;
    }

    static double getDECREASE_FITNESS_IF_MUTATED()
    {
        return DECREASE_FITNESS_IF_MUTATED;
    }

     static long unsigned int getTELETRANSPORTATION()
    {
        return TELETRANSPORTATION;
    }

     static double getTOURNAMENT_SIZE()
     {
         return TOURNAMENT_SIZE;
     }

     static double getFORCE_DIFFERENT_IN_TOURNAMENT()
     {
         return FORCE_DIFFERENT_IN_TOURNAMENT;
     }

     static double getSELECT_TOLERANCE()
     {
         return SELECT_TOLERANCE;
     }

      static long unsigned int getDO_GA_ERROR_BASED_SELECT()
     {
         return DO_GA_ERROR_BASED_SELECT;
     }

    void static readConfigurationsFromFile(char *configFileName);
    void static saveConfigurationsToFile(char *configFileName);
    void static saveDefaultConfigurationsToFile(char *configFileName);
};

#endif /* CONFIGURATION_H_ */
