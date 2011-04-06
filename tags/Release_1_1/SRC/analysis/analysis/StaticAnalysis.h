/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000-09-15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/StaticAnalysis.h,v $
                                                                        
                                                                        
#ifndef StaticAnalysis_h
#define StaticAnalysis_h

// File: ~/analysis/analysis/StaticAnalysis.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the interface for the StaticAnalysis
// class. StaticAnalysis is a subclass of Analysis, it is used to perform 
// a static analysis on the FE\_Model.
//
// What: "@(#) StaticAnalysis.h, revA"

#include <Analysis.h>

class ConstraintHandler;
class DOF_Numberer;
class AnalysisModel;
class StaticIntegrator;
class LinearSOE;
class EquiSolnAlgo;

class StaticAnalysis: public Analysis
{
  public:
    StaticAnalysis(Domain &theDomain,
		   ConstraintHandler &theHandler,
		   DOF_Numberer &theNumberer,
		   AnalysisModel &theModel,
		   EquiSolnAlgo &theSolnAlgo,		   
		   LinearSOE &theSOE,
		   StaticIntegrator &theIntegrator);

    ~StaticAnalysis();

    void clearAll(void);	    
    
    int  analyze(int numSteps);
    int  domainChanged(void);

    int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    int setIntegrator(StaticIntegrator &theIntegrator);
    int setLinearSOE(LinearSOE &theSOE);
    
  protected: 
    
  private:
    ConstraintHandler 	*theConstraintHandler;    
    DOF_Numberer 	*theDOF_Numberer;
    AnalysisModel 	*theAnalysisModel;
    EquiSolnAlgo 	*theAlgorithm;
    LinearSOE 		*theSOE;
    StaticIntegrator    *theIntegrator;
    int domainStamp;
};

#endif