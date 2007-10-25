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
                                                                        
// $Revision: 1.4 $
// $Date: 2007-10-25 16:37:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/convergenceCheck/OptimalityConditionReliabilityConvergenceCheck.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <OptimalityConditionReliabilityConvergenceCheck.h>
#include <ReliabilityConvergenceCheck.h>
#include <math.h>
#include <Vector.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;


OptimalityConditionReliabilityConvergenceCheck::OptimalityConditionReliabilityConvergenceCheck(double passedE1, double passedE2, double pscaleValue, int print)
:ReliabilityConvergenceCheck()
{
	e1 = passedE1;
	e2 = passedE2;
	criterium1 = 0.0;
	criterium2 = 0.0;
	scaleValue = pscaleValue;
	printFlag = print;
	logfile.open("ConvergenceCheckLog.txt", ios::out);
}

OptimalityConditionReliabilityConvergenceCheck::~OptimalityConditionReliabilityConvergenceCheck()
{
	logfile.close();
}



int	
OptimalityConditionReliabilityConvergenceCheck::setScaleValue(double passedScaleValue)
{
	if (scaleValue == 0.0) {
		scaleValue = passedScaleValue;
	}

	return 0;
}


int	
OptimalityConditionReliabilityConvergenceCheck::check(const Vector &u, double g, const Vector &gradG)
{
	if (scaleValue == 0.0) {
		opserr << "OptimalityConditionReliabilityConvergenceCheck::check() --" << endln
			<< " scale value has not been set!" << endln;
	}

	// Convergence criteria
	criterium1 = fabs(g / scaleValue);
	criterium2 = 1.0 - fabs(1.0/(gradG.Norm()*u.Norm()) * (gradG^u));


	// Inform user about convergence status 
	char outputString[100];
	sprintf(outputString,"check1=(%11.3e), check2=(%10.3e), dist=%16.14f",criterium1,criterium2,u.Norm());
	if (printFlag != 0) {
		opserr << outputString << endln;
	}
	logfile << outputString << endln;
	logfile.flush();

	// Return '1' if the analysis converged ('-1' otherwise)
	if ( ( criterium1 < e1 ) && ( criterium2 < e2 ) ) {
		return 1;
	}
	else {
		return -1;
	}
}

int 
OptimalityConditionReliabilityConvergenceCheck::getNumberOfCriteria()
{
	return 2;
}

double 
OptimalityConditionReliabilityConvergenceCheck::getCriteriaValue(int whichCriteria)
{
	if (whichCriteria == 1) {
		return criterium1; 
	}
	else if (whichCriteria == 2) {
		return criterium2;
	}
	else {
		opserr << "OptimalityConditionReliabilityConvergenceCheck::getCriteriaValue() -- " << endln
			<< " criterium number " << whichCriteria << " does not exist!" << endln;
		return 0.0;
	}

}
