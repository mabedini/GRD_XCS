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


#ifndef Definitions_h
#define Definitions_h
#define SETTXTCOLOR 0x1B
#define RESETTXTCOLOR 0
#define BRIGHT 1
#define BLINK  5
#define RED 31
#define GREEN  32
#define YELLOW 33
#define BLUE   34
#define MAGENTA 35
#define CYAN    36
#define WHITE 37

#define BG_BLACK 40


#include <stdlib.h>

//#define IS_MULTI_STEP false

//#define TABOUTFILE "XCStest.txt" /* the output file for the results */
//#define POPULATIONOUTFILE "Clssifiers.txt" /* the classifiers output file for the results */
//#define DATAFILE "GEN_0BRCA_1SPRO.csv"

//#define NR_EXPS 3 /* the number of experiments to take */
//#define MAX_NR_STEPS 20 /* the number of problems to solve in one experiment */
//#define MAX_POP_SIZE 60000 /* the maximum size of the population */
//#define MIN_POP_SIZE 500 /* the minimum size of the population */

//#define NR_MIXING_DIFFERENT_POPULATION 1
//#define NR_DIFFERENT_ISLANDS 1
//#define PORTION_OF_MIGRATABLE_INDIVIDUAL 0.30

//#define CONDITION_LENGTH 3226
//3226 // 33 //12533 //160
//#define ACTION_LENGTH 1
//#define NUMBER_OF_ACTIONS 2
//#define NRBITS 5   /* Number of position bits */

//#define PAYMENT_RANGE 1000   /* 1000, 1800, 3400, 6600 */
//#define NPAYMENT_RANGE -1000   /* 1000, 1800, 3400, 6600 */
//#define PAYOFF_LANDSCAPE 0



//#define BETA 0.2 /* Learning Rate parameter */
//#define GAMMA 0.71 /* Discounting factor that discounts the maximum of the prediction array */
//#define MEAN_LAST_GATIME 1//25 /* do a GA if the average of when a GA was done last time, is at least 25 steps ago */
//#define MINIMUM_ERROR 0.010 /* =epsilon 0 in STEWARTS "CLASSIFIER FITNESS BASED ON ACCURACY" */
//#define FALL_OFF_RATE 0.1 /* =ALPHA IN STEWARTS "CLASSIFIER FITNESS BASED ON ACCURACY" */

//#define CROSSPX 0.8 /* the probability to do crossover */
//#define MUTPMUE 0.2 /* probability of mutating one bit */

// For Binary Simulated Xover
//#define RIGID 1
//#define EPSILON 1e-6
//#define N_DISTRIBUTION_c 2
//#define N_DISTRIBUTION_m 100
//#define xbsINFINITY 1e7
//#define REAL_MUTATION_SEED 0.1


//#define DELTA 0.1 /* used to enforce the deletion of low fitness classifiers (= second deletion method) once they reached DELETION_EXPERIENCE */
//#define DELETION_EXPERIENCE 20 /*The experience required to use second deletion method */

//#define PREDICTION_THRESHOLD 0.5 /* =O| prediction for a loop in the match set ->covering occurs */

//#define DONTCARE_PROBABILITY 0.33 /* probability of choosing DONT_CARE in a newly generated classifier */

//#define PRE_INI 10.0 /* prediction initialization in new classifiers */
//#define PREER_INI 0.0 /* prediction error initialization in new classifiers */
//#define FIT_INI 10. /* fitness initialization in new classifiers */

//#define PRE_ERROR_REDUCTION_IN_NEW_CLASSIFIERS 0.25 /* prediction error reduction in with the discovery component generated classifiers */
//#define FITNESS_REDUCTION_IN_NEW_CLASSIFIERS 0.1 /* fitness reduction in with the discovery component generated classifiers */
//#define KAPPA_MULTIPLIER 0.1 /* decreases the accuracy value if classifier's prediction error is higher than MINIMUM_ERROR*PAYMENT_RANGE */

//#define REDUCTION_THRESHOLD 0 /* experience threshold to reduce the used fitness of a classifier in the prediction array */
//#define FITNESS_REDUCTION 0.0625 /* amount of reducing the fitness of a classifier in the prediction array */

//#define SUBSUME_EXPERIENCE 20 /* the experience required before can be subsumed */

//#define INITIALIZE_POP 0 /* Boolean to check if population should be initialized */

//#define DECREASE_FITNESS_IN_M_NOT_A 0 /* Boolean to decrease the fitness of classifiers that are not in the match set */
//#define FITNESS_REDUCTION_IN_M_NOT_A 0.2 /* reduction of the fitness if not in the action set but in the matchset */

//#define DECREASE_FITNESS_IF_MUTATED 0 /* Boolean to decrease the fitness if being mutated by FITNESS_REDUCTION_IN_NEW_CLASSIFIERS */

//#define TELETRANSPORTATION 100000 /* possible teletransportation in multi step environments (trial ends if counter exeeds this number) */

#define frand() ((double) rand() / (RAND_MAX+1.0)) /* random for values between 0 and 1 */
#define sfrand() frand() * (frand() > 0.5 ? 1 : -1 ) /* random for values between -1 and 1 */
//#define DONT_CARE '#' /* the don't care sign */

#endif
