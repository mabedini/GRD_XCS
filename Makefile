# /******************************************************
#   A collection of XCS based classifiers for big data analysis
#   Specifically designed for high-dimensional classification purpose	
# /******************************************************
#
#		by Mani Abedini
#		
#		The department of computing and Information system
#		Engineering school
#		The University of Melbourne 
#		Australia
#		mabedini@students.unimelb.edu.au
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

#     Makefile

NAME = GRD_XCS
LINKMATH = -Wall -ansi -lm -lpthread -pedantic   

CC = g++
CC_OPTS  = -O3
CommDirPath =  .//CommonSources

$(NAME) :  GRD_XCS.o Classifier.o Utilities.o  Configuration.o XCS.o Original_XCS.o 
		$(CC) $(CC_OPTS) -o $(NAME) $(LINKMATH)  XCS.o Classifier.o Utilities.o GRD_XCS.o Configuration.o Original_XCS.o

GRD_XCS.o:	GRD_XCS.cpp $(CommDirPath)//Classifier.h $(CommDirPath)//Definitions.h $(CommDirPath)//Utilities.h $(CommDirPath)//XCS.h $(CommDirPath)//Configuration.h $(CommDirPath)//Original_XCS.h 
			$(CC) -c $(CC_OPTS) GRD_XCS.cpp -o GRD_XCS.o 

Original_XCS.o:	$(CommDirPath)//Original_XCS.cpp $(CommDirPath)//Classifier.h $(CommDirPath)//Definitions.h $(CommDirPath)//Utilities.h $(CommDirPath)//XCS.h $(CommDirPath)//Configuration.h $(CommDirPath)//Original_XCS.h
			$(CC) -c $(CC_OPTS) $(CommDirPath)//Original_XCS.cpp -o Original_XCS.o 

XCS.o:	$(CommDirPath)//XCS.cpp $(CommDirPath)//XCS.h $(CommDirPath)//Definitions.h $(CommDirPath)//Utilities.h $(CommDirPath)//Classifier.h $(CommDirPath)//Configuration.h
			$(CC) -c $(CC_OPTS) $(CommDirPath)//XCS.cpp -o XCS.o

Utilities.o:	$(CommDirPath)//Utilities.cpp $(CommDirPath)//Utilities.h $(CommDirPath)//Definitions.h $(CommDirPath)//Classifier.h $(CommDirPath)//Configuration.h
			$(CC) -c $(CC_OPTS) $(CommDirPath)//Utilities.cpp -o Utilities.o 

Classifier.o:	$(CommDirPath)//Classifier.cpp $(CommDirPath)//Classifier.h $(CommDirPath)//Definitions.h $(CommDirPath)//Utilities.h $(CommDirPath)//Configuration.h
			$(CC) -c $(CC_OPTS) $(CommDirPath)//Classifier.cpp -o Classifier.o

Configuration.o:	$(CommDirPath)//Configuration.cpp $(CommDirPath)//Configuration.h $(CommDirPath)//Definitions.h  
			$(CC) -c $(CC_OPTS) $(CommDirPath)//Configuration.cpp -o Configuration.o 

clean:
	-rm -f *.o *.out *~
