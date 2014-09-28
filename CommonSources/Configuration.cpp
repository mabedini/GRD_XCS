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
#include "Configuration.h"

	unsigned int Configuration::IS_MULTI_STEP;
    char Configuration::TABOUTFILE[100]; /* the output file for the results */
    char Configuration::POPULATIONOUTFILE[100];/* the classifiers output file for the results */
    char Configuration::DATAFILE[100];
    char Configuration::TESTFILE[100];

    unsigned int Configuration::NR_EXPS;
    long unsigned int Configuration::MAX_NR_STEPS;/* the number of problems to solve in one experiment */
    long unsigned int Configuration::MAX_POP_SIZE ;/* the maximum size of the population */
    double Configuration::EXPLORE_EXPLOITE_RATE ;/* the minimum size of the population */

    unsigned int Configuration::NR_MIXING_DIFFERENT_POPULATION ;
    unsigned int Configuration::NR_DIFFERENT_ISLANDS ;
    unsigned int Configuration::NR_PARALLEL_RUNNING_ISLANDS;
    double Configuration::PORTION_OF_MIGRATABLE_INDIVIDUAL ;

    unsigned int Configuration::CONDITION_LENGTH ;//3226 // 33 //12533 //160
    unsigned int Configuration::ACTION_LENGTH ;
    unsigned int Configuration::NUMBER_OF_ACTIONS ;

    int Configuration::PAYMENT_RANGE ;  /* 1000, 1800, 3400, 6600 */
    int Configuration::NPAYMENT_RANGE ;/* 1000, 1800, 3400, 6600 */
    unsigned int Configuration::MIN_PERDICTION_FOR_DELETE;
    unsigned int Configuration::MIN_EXP_FOR_CHECKINGPREDICTION ;

    int Configuration::PAYOFF_LANDSCAPE ;
    unsigned int Configuration::NBR_RATING_CATEGORY;


    double Configuration::BETA;/* Learning Rate parameter */
    double Configuration::GAMMA ;/* Discounting factor that discounts the maximum of the prediction array */
    unsigned int Configuration::MEAN_LAST_GATIME ;//25 /* do a GA if the average of when a GA was done last time, is at least 25 steps ago */
    double Configuration::MINIMUM_ERROR ;/* =epsilon 0 in STEWARTS "CLASSIFIER FITNESS BASED ON ACCURACY" */
    double Configuration::FALL_OFF_RATE ;/* =ALPHA IN STEWARTS "CLASSIFIER FITNESS BASED ON ACCURACY" */

    double Configuration::CROSSPX ;/* the probability to do crossover */
    double Configuration::MUTPMUE ;/* probability of mutating one bit */
    double Configuration::RANGE_INI ;/* probability of mutating one bit */

    double Configuration::DELTA ; /* used to enforce the deletion of low fitness classifiers (= second deletion method) once they reached DELETION_EXPERIENCE */
    int Configuration:: DELETION_EXPERIENCE ;/*The experience required to use second deletion method */

    double Configuration::PREDICTION_THRESHOLD ; /* =O| prediction for a loop in the match set ->covering occurs */

    double Configuration::DONTCARE_PROBABILITY ; /* probability of choosing DONT_CARE in a newly generated classifier */

    double Configuration::PRE_INI ; /* prediction initialization in new classifiers */
    double Configuration::PREER_INI; /* prediction error initialization in new classifiers */
    double Configuration::FIT_INI ; /* fitness initialization in new classifiers */

    double Configuration::PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS; /* prediction error reduction in with the discovery component generated classifiers */
    double Configuration::FITNESS_REDUCTION_IN_NEW_CLASSIFIERS ;/* fitness reduction in with the discovery component generated classifiers */
    double Configuration::KAPPA_MULTIPLIER; /* decreases the accuracy value if classifier's prediction error is higher than MINIMUM_ERROR*PAYMENT_RANGE */

    double Configuration::REDUCTION_THRESHOLD; /* experience threshold to reduce the used fitness of a classifier in the prediction array */
    double Configuration::FITNESS_REDUCTION ; /* amount of reducing the fitness of a classifier in the prediction array */

    unsigned int Configuration:: SUBSUME_EXPERIENCE ; /* the experience required before can be subsumed */

    unsigned int Configuration:: INITIALIZE_POP ; /* Boolean to check if population should be initialized */

    double Configuration::DECREASE_FITNESS_IN_M_NOT_A ; /* Boolean to decrease the fitness of classifiers that are not in the match set */
    double Configuration::FITNESS_REDUCTION_IN_M_NOT_A ; /* reduction of the fitness if not in the action set but in the matchset */

    double Configuration::DECREASE_FITNESS_IF_MUTATED ; /* Boolean to decrease the fitness if being mutated by FITNESS_REDUCTION_IN_NEW_CLASSIFIERS */

    long unsigned int Configuration::TELETRANSPORTATION ; /* possible teletransportation in multi step environments (trial ends if counter exeeds this number) */

	// Tournoment Selection
	double Configuration::TOURNAMENT_SIZE; /* default=0.4; percentage of action set that takes part in tournament; if set to zero, proportionate selection is used; if set greater than one, tournament with fixed size is used */
	double Configuration::FORCE_DIFFERENT_IN_TOURNAMENT; /* default =0; the probability that the two participants must be different */
	double Configuration::SELECT_TOLERANCE; /* default=.001; the tolerance with which classifier fitness/error is considered similar */
	unsigned int Configuration::DO_GA_ERROR_BASED_SELECT; /* default=0; GA selection based on error */

	// For Binary Simulated Xover
    unsigned int Configuration::RIGID ;
    double Configuration::EPSILON ;
    unsigned int Configuration:: N_DISTRIBUTION_c;
    unsigned int Configuration::N_DISTRIBUTION_m;
    double Configuration::xbsINFINITY ;
    double Configuration::REAL_MUTATION_SEED ;


Configuration::Configuration() {
	// TODO Auto-generated constructor stub

}

Configuration::~Configuration() {
	// TODO Auto-generated destructor stub
}


void Configuration::readConfigurationsFromFile(char *configFileName)
{
   /*Open files for statistics*/
   FILE *tabFile;
   tabFile = (FILE*)(fopen(configFileName, "rt"));
   if (tabFile== NULL)
			{
			  Configuration::saveDefaultConfigurationsToFile(configFileName);
			  //fprintf(stderr, "Cannot open config file");
			  //fprintf(stderr,configFileName);
			  //return;
			}

   ReadParameterFromFile(tabFile,(char*)"IS_MULTI_STEP:%u\n", &IS_MULTI_STEP);
   ReadParameterFromFile(tabFile,(char*)"TABOUTFILE:%s\n", TABOUTFILE);
   ReadParameterFromFile(tabFile,(char*)"POPULATIONOUTFILE:%s\n", POPULATIONOUTFILE);
   ReadParameterFromFile(tabFile,(char*)"DATAFILE:%s\n", DATAFILE);
   ReadParameterFromFile(tabFile,(char*)"TESTFILE:%s\n", TESTFILE);
   ReadParameterFromFile(tabFile,(char*)"NR_EXPS:%u\n", &NR_EXPS);
   ReadParameterFromFile(tabFile,(char*)"MAX_NR_STEPS:%lu\n", &MAX_NR_STEPS);
   ReadParameterFromFile(tabFile,(char*)"MAX_POP_SIZE:%lu\n", &MAX_POP_SIZE);
   ReadParameterFromFile(tabFile,(char*)"EXPLORE_EXPLOITE_RATE:%lf\n", &EXPLORE_EXPLOITE_RATE);
   ReadParameterFromFile(tabFile,(char*)"MIN_PERDICTION_FOR_DELETE:%u\n", &MIN_PERDICTION_FOR_DELETE);
   ReadParameterFromFile(tabFile,(char*)"MIN_EXP_FOR_CHECKINGPREDICTION:%u\n", &MIN_EXP_FOR_CHECKINGPREDICTION);

   ReadParameterFromFile(tabFile,(char*)"NR_MIXING_DIFFERENT_POPULATION:%u\n", &NR_MIXING_DIFFERENT_POPULATION);
   ReadParameterFromFile(tabFile,(char*)"NR_DIFFERENT_ISLANDS:%u\n", &NR_DIFFERENT_ISLANDS);
   ReadParameterFromFile(tabFile,(char*)"NR_PARALLEL_RUNNING_ISLANDS:%u\n", &NR_PARALLEL_RUNNING_ISLANDS);
   ReadParameterFromFile(tabFile,(char*)"PORTION_OF_MIGRATABLE_INDIVIDUAL:%lf\n", &PORTION_OF_MIGRATABLE_INDIVIDUAL);
   ReadParameterFromFile(tabFile,(char*)"CONDITION_LENGTH:%u\n", &CONDITION_LENGTH);
   ReadParameterFromFile(tabFile,(char*)"ACTION_LENGTH:%u\n", &ACTION_LENGTH);
   ReadParameterFromFile(tabFile,(char*)"NUMBER_OF_ACTIONS:%u\n", &NUMBER_OF_ACTIONS);
   ReadParameterFromFile(tabFile,(char*)"PAYMENT_RANGE:%d\n", &PAYMENT_RANGE);
   ReadParameterFromFile(tabFile,(char*)"NPAYMENT_RANGE:%d\n", &NPAYMENT_RANGE);
   ReadParameterFromFile(tabFile,(char*)"PAYOFF_LANDSCAPE:%d\n", &PAYOFF_LANDSCAPE);
   ReadParameterFromFile(tabFile,(char*)"NBR_RATING_CATEGORY:%u\n", &NBR_RATING_CATEGORY);

   ReadParameterFromFile(tabFile,(char*)"BETA:%lf\n", &BETA);
   ReadParameterFromFile(tabFile,(char*)"GAMMA:%lf\n", &GAMMA);
   ReadParameterFromFile(tabFile,(char*)"MEAN_LAST_GATIME:%u\n", &MEAN_LAST_GATIME);
   ReadParameterFromFile(tabFile,(char*)"MINIMUM_ERROR:%lf\n", &MINIMUM_ERROR);
   ReadParameterFromFile(tabFile,(char*)"FALL_OFF_RATE:%lf\n", &FALL_OFF_RATE);
   ReadParameterFromFile(tabFile,(char*)"CROSSPX:%lf\n", &CROSSPX);
   ReadParameterFromFile(tabFile,(char*)"MUTPMUE:%lf\n", &MUTPMUE);
   ReadParameterFromFile(tabFile,(char*)"RANGE_INI:%lf\n", &RANGE_INI);\

   ReadParameterFromFile(tabFile,(char*)"DELTA:%lf\n", &DELTA);
   ReadParameterFromFile(tabFile,(char*)"DELETION_EXPERIENCE:%u\n", &DELETION_EXPERIENCE);
   ReadParameterFromFile(tabFile,(char*)"PREDICTION_THRESHOLD:%lf\n", &PREDICTION_THRESHOLD);
   ReadParameterFromFile(tabFile,(char*)"DONTCARE_PROBABILITY:%lf\n", &DONTCARE_PROBABILITY);
   ReadParameterFromFile(tabFile,(char*)"PRE_INI:%lf\n", &PRE_INI);
   ReadParameterFromFile(tabFile,(char*)"PREER_INI:%lf\n", &PREER_INI);
   ReadParameterFromFile(tabFile,(char*)"FIT_INI:%lf\n", &FIT_INI);
   ReadParameterFromFile(tabFile,(char*)"PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS:%lf\n", &PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS);
   ReadParameterFromFile(tabFile,(char*)"FITNESS_REDUCTION_IN_NEW_CLASSIFIERS:%lf\n", &FITNESS_REDUCTION_IN_NEW_CLASSIFIERS);
   ReadParameterFromFile(tabFile,(char*)"KAPPA_MULTIPLIER:%lf\n", &KAPPA_MULTIPLIER);
   ReadParameterFromFile(tabFile,(char*)"REDUCTION_THRESHOLD:%lf\n", &REDUCTION_THRESHOLD);
   ReadParameterFromFile(tabFile,(char*)"FITNESS_REDUCTION:%lf\n", &FITNESS_REDUCTION);
   ReadParameterFromFile(tabFile,(char*)"SUBSUME_EXPERIENCE:%u\n", &SUBSUME_EXPERIENCE);
   ReadParameterFromFile(tabFile,(char*)"INITIALIZE_POP:%u\n", &INITIALIZE_POP);

   ReadParameterFromFile(tabFile,(char*)"DECREASE_FITNESS_IN_M_NOT_A:%lf\n", &DECREASE_FITNESS_IN_M_NOT_A);
   ReadParameterFromFile(tabFile,(char*)"FITNESS_REDUCTION_IN_M_NOT_A:%lf\n", &FITNESS_REDUCTION_IN_M_NOT_A);
   ReadParameterFromFile(tabFile,(char*)"DECREASE_FITNESS_IF_MUTATED:%lf\n", &DECREASE_FITNESS_IF_MUTATED);
   ReadParameterFromFile(tabFile,(char*)"TELETRANSPORTATION:%lu\n", &TELETRANSPORTATION);

   ReadParameterFromFile(tabFile,(char*)"TOURNAMENT_SIZE:%lf\n", &TOURNAMENT_SIZE); /* default=0.4; percentage of action set that takes part in tournament; if set to zero, proportionate selection is used; if set greater than one, tournament with fixed size is used */
   ReadParameterFromFile(tabFile,(char*)"FORCE_DIFFERENT_IN_TOURNAMENT:%lf\n", &FORCE_DIFFERENT_IN_TOURNAMENT); /* default =0; the probability that the two participants must be different */
   ReadParameterFromFile(tabFile,(char*)"SELECT_TOLERANCE:%lf\n", &SELECT_TOLERANCE); /* default=.001; the tolerance with which classifier fitness/error is considered similar */
   ReadParameterFromFile(tabFile,(char*)"DO_GA_ERROR_BASED_SELECT:%u\n", &DO_GA_ERROR_BASED_SELECT); /* default=0; GA selection based on error */

   ReadParameterFromFile(tabFile,(char*)"RIGID:%u\n", &RIGID);
   ReadParameterFromFile(tabFile,(char*)"EPSILON:%lf\n", &EPSILON);
   ReadParameterFromFile(tabFile,(char*)"N_DISTRIBUTION_c:%u\n", &N_DISTRIBUTION_c);
   ReadParameterFromFile(tabFile,(char*)"N_DISTRIBUTION_m:%u\n", &N_DISTRIBUTION_m);
   ReadParameterFromFile(tabFile,(char*)"xbsINFINITY:%lf\n", &xbsINFINITY);
   ReadParameterFromFile(tabFile,(char*)"REAL_MUTATION_SEED:%lf\n", &REAL_MUTATION_SEED);

   fclose(tabFile);
   return;
}

void Configuration::ReadParameterFromFile(FILE *dataFile,char* format,void *variable)
{
	   int nDataRead=0;
	   nDataRead=fscanf(dataFile, format, variable);
	   if (nDataRead==0)
	   {
		   printf("Error Reading Config File of Parameter:%s\n",format);
		   exit(0);
	   }
}
void Configuration::saveConfigurationsToFile(char *configFileName)
{
   /*Open files for statistics*/
   FILE *tabFile;
   tabFile = (FILE*)(fopen(configFileName, "wt"));
   if (tabFile== NULL)
			{
			  fprintf(stderr, "Cannot open config file");
			  //fprintf(stderr,configFileName);
			  return;
			}
   fprintf(tabFile,"IS_MULTI_STEP:%u\n",IS_MULTI_STEP);
   fprintf(tabFile, "TABOUTFILE:%s\n", TABOUTFILE);
   fprintf(tabFile, "POPULATIONOUTFILE:%s\n", POPULATIONOUTFILE);
   fprintf(tabFile, "DATAFILE:%s\n", DATAFILE);
   fprintf(tabFile, "TESTFILE:%s\n", TESTFILE);
   fprintf(tabFile, "NR_EXPS:%u\n", NR_EXPS);
   fprintf(tabFile, "MAX_NR_STEPS:%lu\n", MAX_NR_STEPS);
   fprintf(tabFile, "MAX_POP_SIZE:%lu\n", MAX_POP_SIZE);\
   fprintf(tabFile, "EXPLORE_EXPLOITE_RATE:%lf\n", EXPLORE_EXPLOITE_RATE);
   fprintf(tabFile, "MIN_PERDICTION_FOR_DELETE:%u\n", MIN_PERDICTION_FOR_DELETE);
   fprintf(tabFile, "MIN_EXP_FOR_CHECKINGPREDICTION:%u\n", MIN_EXP_FOR_CHECKINGPREDICTION);

   fprintf(tabFile, "NR_MIXING_DIFFERENT_POPULATION:%u\n", NR_MIXING_DIFFERENT_POPULATION);
   fprintf(tabFile, "NR_DIFFERENT_ISLANDS:%u\n", NR_DIFFERENT_ISLANDS);
   fprintf(tabFile, "NR_PARALLEL_RUNNING_ISLANDS:%u\n", NR_PARALLEL_RUNNING_ISLANDS);
   fprintf(tabFile, "PORTION_OF_MIGRATABLE_INDIVIDUAL:%lf\n", PORTION_OF_MIGRATABLE_INDIVIDUAL);
   fprintf(tabFile, "CONDITION_LENGTH:%u\n", CONDITION_LENGTH);
   fprintf(tabFile, "ACTION_LENGTH:%u\n", ACTION_LENGTH);
   fprintf(tabFile, "NUMBER_OF_ACTIONS:%u\n", NUMBER_OF_ACTIONS);
   fprintf(tabFile, "PAYMENT_RANGE:%d\n", PAYMENT_RANGE);
   fprintf(tabFile, "NPAYMENT_RANGE:%d\n", NPAYMENT_RANGE);
   fprintf(tabFile, "PAYOFF_LANDSCAPE:%d\n", PAYOFF_LANDSCAPE);
   fprintf(tabFile, "NBR_RATING_CATEGORY:%u\n", NBR_RATING_CATEGORY);

   fprintf(tabFile, "BETA:%lf\n", BETA);
   fprintf(tabFile, "GAMMA:%lf\n", GAMMA);
   fprintf(tabFile, "MEAN_LAST_GATIME:%u\n", MEAN_LAST_GATIME);
   fprintf(tabFile, "MINIMUM_ERROR:%lf\n", MINIMUM_ERROR);
   fprintf(tabFile, "FALL_OFF_RATE:%lf\n", FALL_OFF_RATE);
   fprintf(tabFile, "CROSSPX:%lf\n", CROSSPX);
   fprintf(tabFile, "MUTPMUE:%lf\n", MUTPMUE);
   fprintf(tabFile, "RANGE_INI:%lf\n", RANGE_INI);

   fprintf(tabFile, "DELTA:%lf\n", DELTA);
   fprintf(tabFile, "DELETION_EXPERIENCE:%u\n", DELETION_EXPERIENCE);
   fprintf(tabFile, "PREDICTION_THRESHOLD:%lf\n", PREDICTION_THRESHOLD);
   fprintf(tabFile, "DONTCARE_PROBABILITY:%lf\n", DONTCARE_PROBABILITY);
   fprintf(tabFile, "PRE_INI:%lf\n", PRE_INI);
   fprintf(tabFile, "PREER_INI:%lf\n", PREER_INI);
   fprintf(tabFile, "FIT_INI:%lf\n", FIT_INI);
   fprintf(tabFile, "PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS:%lf\n", PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS);
   fprintf(tabFile, "FITNESS_REDUCTION_IN_NEW_CLASSIFIERS:%lf\n", FITNESS_REDUCTION_IN_NEW_CLASSIFIERS);
   fprintf(tabFile, "KAPPA_MULTIPLIER:%lf\n", KAPPA_MULTIPLIER);
   fprintf(tabFile, "REDUCTION_THRESHOLD:%lf\n", REDUCTION_THRESHOLD);
   fprintf(tabFile, "FITNESS_REDUCTION:%lf\n", FITNESS_REDUCTION);
   fprintf(tabFile, "SUBSUME_EXPERIENCE:%u\n", SUBSUME_EXPERIENCE);
   fprintf(tabFile, "INITIALIZE_POP:%u\n", INITIALIZE_POP);

   fprintf(tabFile, "DECREASE_FITNESS_IN_M_NOT_A:%lf\n", DECREASE_FITNESS_IN_M_NOT_A);
   fprintf(tabFile, "FITNESS_REDUCTION_IN_M_NOT_A:%lf\n", FITNESS_REDUCTION_IN_M_NOT_A);
   fprintf(tabFile, "DECREASE_FITNESS_IF_MUTATED:%lf\n", DECREASE_FITNESS_IF_MUTATED);
   fprintf(tabFile, "TELETRANSPORTATION:%lu\n", TELETRANSPORTATION);

   fprintf(tabFile,"TOURNAMENT_SIZE:%lf\n", TOURNAMENT_SIZE); /* default=0.4; percentage of action set that takes part in tournament; if set to zero, proportionate selection is used; if set greater than one, tournament with fixed size is used */
   fprintf(tabFile,"FORCE_DIFFERENT_IN_TOURNAMENT:%lf\n", FORCE_DIFFERENT_IN_TOURNAMENT); /* default =0; the probability that the two participants must be different */
   fprintf(tabFile,"SELECT_TOLERANCE:%lf\n", SELECT_TOLERANCE); /* default=.001; the tolerance with which classifier fitness/error is considered similar */
   fprintf(tabFile,"DO_GA_ERROR_BASED_SELECT:%u\n", DO_GA_ERROR_BASED_SELECT); /* default=0; GA selection based on error */

   fprintf(tabFile, "RIGID:%u\n", RIGID);
   fprintf(tabFile, "EPSILON:%lf\n", EPSILON);
   fprintf(tabFile, "N_DISTRIBUTION_c:%u\n", N_DISTRIBUTION_c);
   fprintf(tabFile, "N_DISTRIBUTION_m:%u\n", N_DISTRIBUTION_m);
   fprintf(tabFile, "xbsINFINITY:%lf\n", xbsINFINITY);
   fprintf(tabFile, "REAL_MUTATION_SEED:%lf\n", REAL_MUTATION_SEED);
   fclose(tabFile);
   return;
}

void Configuration::saveDefaultConfigurationsToFile(char *configFileName)
{
    /*Open files for statistics*/
    FILE *tabFile = (FILE*)(fopen(configFileName, "wt"));
		  if (tabFile== NULL)
			{
			  fprintf(stderr, "Cannot open config file");
			  fprintf(stderr,configFileName);
			  return;
			}

			fprintf(tabFile,"IS_MULTI_STEP:%u\n",0);
			fprintf(tabFile,"TABOUTFILE:%s\n","XCStest.txt" ); /* the output file for the results */
			fprintf(tabFile,"POPULATIONOUTFILE:%s\n","Clssifiers.txt" );
			fprintf(tabFile,"DATAFILE:%s\n","GEN_0BRCA_1SPRO.csv");
			fprintf(tabFile,"TESTFILE:%s\n","GEN_0BRCA_1SPRO.csv");
			fprintf(tabFile,"NR_EXPS:%u\n",10 ); /* the number of experiments to take */
			fprintf(tabFile,"MAX_NR_STEPS:%u\n",200 ); /* the number of problems to solve in one experiment */
			fprintf(tabFile,"MAX_POP_SIZE:%lu\n",2000L );  /* default=2000 (highly problem dependent); the maximum size of the population */
			fprintf(tabFile,"EXPLORE_EXPLOITE_RATE:%lf\n",1.0 );
			fprintf(tabFile,"MIN_PERDICTION_FOR_DELETE:%u\n",500 );
			fprintf(tabFile,"MIN_EXP_FOR_CHECKINGPREDICTION:%u\n",40 );

			fprintf(tabFile,"NR_MIXING_DIFFERENT_POPULATION:%u\n",300);
			fprintf(tabFile,"NR_DIFFERENT_ISLANDS:%u\n",1);
			fprintf(tabFile,"NR_PARALLEL_RUNNING_ISLANDS:%u\n",2);
			fprintf(tabFile,"PORTION_OF_MIGRATABLE_INDIVIDUAL:%lf\n",0.1000);
			fprintf(tabFile,"CONDITION_LENGTH:%u\n",37);
			fprintf(tabFile,"ACTION_LENGTH:%u\n",1);
			fprintf(tabFile,"NUMBER_OF_ACTIONS:%u\n",2);
			fprintf(tabFile,"PAYMENT_RANGE:%d\n",1000   );
			fprintf(tabFile,"NPAYMENT_RANGE:%d\n",0   );
			fprintf(tabFile,"PAYOFF_LANDSCAPE:%u\n",0);
			fprintf(tabFile,"NBR_RATING_CATEGORY:%u\n",3);

			fprintf(tabFile,"BETA:%lf\n",0.2000 ); /* default=0.2; Learning Rate parameter */
			fprintf(tabFile,"GAMMA:%lf\n",0.8000 ); /* default=0.95; Discounting factor that discounts the maximum of the prediction array */
			fprintf(tabFile,"MEAN_LAST_GATIME:%u\n",25);
			fprintf(tabFile,"MINIMUM_ERROR:%lf\n",0.010 );
			fprintf(tabFile,"FALL_OFF_RATE:%lf\n",0.1 );
			fprintf(tabFile,"CROSSPX:%lf\n",1.0  ); /* default=1.0; the probability to do crossover */
			fprintf(tabFile,"MUTPMUE:%lf\n",0.01 ); /* default=0.01 (problem dependent); probability of mutating one bit */
			fprintf(tabFile,"RANGE_INI:%lf\n",0.005);

			fprintf(tabFile,"DELTA:%lf\n",0.1 );
			fprintf(tabFile,"DELETION_EXPERIENCE:%u\n",20 );
			fprintf(tabFile,"PREDICTION_THRESHOLD:%lf\n",0.5 );
			fprintf(tabFile,"DONTCARE_PROBABILITY:%lf\n",0.66 ); /* default=0.66 (problem dependent); Probability of choosing DONT_CARE in a newly generated classifier */
			fprintf(tabFile,"PRE_INI:%lf\n",10.0 );
			fprintf(tabFile,"PREER_INI:%lf\n",0.0 );
			fprintf(tabFile,"FIT_INI:%lf\n",0.5);
			fprintf(tabFile,"PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS:%lf\n",0.25 );
			fprintf(tabFile,"FITNESS_REDUCTION_IN_NEW_CLASSIFIERS:%lf\n",0.1 );
			fprintf(tabFile,"KAPPA_MULTIPLIER:%lf\n",5.0 ); /* default=5; exponent of the power function */
			fprintf(tabFile,"REDUCTION_THRESHOLD:%lf\n",0.0 );
			fprintf(tabFile,"FITNESS_REDUCTION:%lf\n",0.0625 );
			fprintf(tabFile,"SUBSUME_EXPERIENCE:%u\n",20 );
			fprintf(tabFile,"INITIALIZE_POP:%u\n",0 );

			fprintf(tabFile,"DECREASE_FITNESS_IN_M_NOT_A:%lf\n",0.0 );
			fprintf(tabFile,"FITNESS_REDUCTION_IN_M_NOT_A:%lf\n",0.2 );
			fprintf(tabFile,"DECREASE_FITNESS_IF_MUTATED:%lf\n",0.0 );
			fprintf(tabFile,"TELETRANSPORTATION:%lu\n",100000L );

			fprintf(tabFile,"TOURNAMENT_SIZE:%lf\n",0.4 ); /* default=0.4; percentage of action set that takes part in tournament; if set to zero, proportionate selection is used; if set greater than one, tournament with fixed size is used */
			fprintf(tabFile,"FORCE_DIFFERENT_IN_TOURNAMENT:%lf\n",0.0 ); /* default =0; the probability that the two participants must be different */
			fprintf(tabFile,"SELECT_TOLERANCE:%lf\n",0.001 ); /* default=.001; the tolerance with which classifier fitness/error is considered similar */
			fprintf(tabFile,"DO_GA_ERROR_BASED_SELECT:%u\n", 0 ); /* default=0; GA selection based on error */

			fprintf(tabFile,"RIGID:%u\n",1);
			fprintf(tabFile,"EPSILON:%lf\n",1e-6); /* default=0.01; TOLERATED ERROR in prediction error*/
			fprintf(tabFile,"N_DISTRIBUTION_c:%u\n",2);
			fprintf(tabFile,"N_DISTRIBUTION_m:%u\n",100);
			fprintf(tabFile,"xbsINFINITY:%lf\n",1e7);
			fprintf(tabFile,"REAL_MUTATION_SEED:%lf\n",0.1);

			fclose(tabFile);
			return;
	}
