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
                                                                        
// $Revision: 1.1 $
// $Date: 2003-03-04 00:39:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/sensitivity/OpenSeesGradGEvaluator.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef OpenSeesGradGEvaluator_h
#define OpenSeesGradGEvaluator_h

#include <GradGEvaluator.h>
#include <GFunEvaluator.h>
#include <Vector.h>
#include <ReliabilityDomain.h>
#include <tcl.h>

#include <fstream>
using std::ofstream;

class OpenSeesGradGEvaluator : public GradGEvaluator
{

public:
	OpenSeesGradGEvaluator(GFunEvaluator *theGFunEvaluator,
						   Tcl_Interp *passedTclInterp, 
				           ReliabilityDomain *passedReliabilityDomain,
				           bool doGradientCheck);
	~OpenSeesGradGEvaluator();

	int		evaluateGradG(double gFunValue, Vector passed_x);
	Vector	getGradG();
	Matrix  getDgDdispl();

protected:

private:

	Vector *grad_g;
	Matrix *DgDdispl;
	GFunEvaluator *theGFunEvaluator;
	bool doGradientCheck;

};

#endif