/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.9 $
// $Date: 2010-09-13 21:39:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/gFunction/TclEvaluator.cpp,v $

//
// Written by: 
// Kevin Mackie (kmackie@mail.ucf.edu)
// Michael Scott (mhscott@engr.orst.edu)
//

#include <TclEvaluator.h>
#include <Vector.h>
#include <FunctionEvaluator.h>
#include <ReliabilityDomain.h>
#include <Parameter.h>
#include <RandomVariablePositionerIter.h>

#include <tcl.h>
#include <stdlib.h>
#include <string.h>


TclEvaluator::TclEvaluator(Tcl_Interp *passedTclInterp,
			   ReliabilityDomain *passedReliabilityDomain,
			   Domain *passedOpenSeesDomain,
			   TCL_Char *passed_fileName)
  :FunctionEvaluator(), theTclInterp(passedTclInterp), theReliabilityDomain(passedReliabilityDomain),
   theOpenSeesDomain(passedOpenSeesDomain)
  
{
  int exprLen = strlen(passed_fileName);
  fileName = new char[exprLen+1];
  strcpy(fileName,passed_fileName);
}

TclEvaluator::TclEvaluator(Tcl_Interp *passedTclInterp,
			   ReliabilityDomain *passedReliabilityDomain,
			   Domain *passedOpenSeesDomain)
  :FunctionEvaluator(), theTclInterp(passedTclInterp), theReliabilityDomain(passedReliabilityDomain),
   theOpenSeesDomain(passedOpenSeesDomain)
{
  fileName = 0;
}

TclEvaluator::~TclEvaluator()
{
  if (theExpression != 0)
    delete [] theExpression;
  if (fileName != 0)
    delete [] fileName;
}

int
TclEvaluator::setVariables(const Vector &x)
{
  char theIndex[80];
  double xval;
  Parameter *theParam;
  
  // Set values of random variables in the Tcl intepreter
  int nparam = theOpenSeesDomain->getNumParameters();
  for (int i = 0; i < nparam; i++) {
    theParam = theOpenSeesDomain->getParameterFromIndex(i);
    int paramTag = theParam->getTag();
    
    xval = x(i);
    
    // put in par(1) format
    sprintf(theIndex,"%d",paramTag);
    if (Tcl_SetVar2Ex(theTclInterp,"par",theIndex,Tcl_NewDoubleObj(xval),TCL_LEAVE_ERR_MSG) == NULL) {
      opserr << "ERROR TclEvaluator -- error in setVariables for parameter tag " << paramTag << endln;
      opserr << "of type " << theTclInterp->result << endln;
      return -1;
    }
    
  }
  
  return 0;
}

int 
TclEvaluator::setExpression(const char *passedExpression)
{
  if (theExpression != 0)
    delete [] theExpression;
  
  int exprLen = strlen(passedExpression);
  theExpression = new char[exprLen+1];
  strcpy(theExpression,passedExpression);
  
  return 0;
}

int 
TclEvaluator::addToExpression(const char *in) 
{
  return 0;
}

int
TclEvaluator::evaluateExpression() 
{
  if (Tcl_ExprDouble( theTclInterp, theExpression, &current_val) != TCL_OK) {
    opserr << "TclEvaluator::evaluateExpression -- expression \"" << theExpression;
    opserr << "\" caused error:" << endln << theTclInterp->result << endln;
    return -1;
  }
  
  return 0;
}


double
TclEvaluator::getResult()
{
  return current_val;
}

int
TclEvaluator::runAnalysis(const Vector &x)
{	
  // Let's just make a direct call since we have the pointer to OpenSees domain
  // This replaces above call to Tcl command; however, in the reset command
  // revertToStart() is also called on theTransientIntegrator -- MHS needs to check
  if (theOpenSeesDomain->revertToStart() != 0) {
    opserr << "ERROR TclEvaluator -- error in resetting Domain" << endln;
    return -1;
  }
  
  // Put random variables into the structural domain according to the RandomVariablePositioners
  int rvIndex;
  RandomVariablePositionerIter rvPosIter = theReliabilityDomain->getRandomVariablePositioners();
  RandomVariablePositioner *theRVPos;
  while ((theRVPos = rvPosIter()) != 0) {
    rvIndex = theRVPos->getRvIndex();
    theRVPos->update(x(rvIndex));
  }
  
  // Source the code file that the user has provided
  if (fileName == 0) {
    // no source file provided, this is akin to the basic evaluator of days gone by
    
  } else {
    if (Tcl_Eval(theTclInterp, fileName) == TCL_ERROR) {
      opserr << "ERROR TclEvaluator -- error in Tcl_Eval: " << theTclInterp->result << endln;
      return -1;
    }
  }
  
  return 0;
}
