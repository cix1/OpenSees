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
// $Date: 2001-08-01 18:06:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/RandomVariable.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) during Spring 2000
// Revised: haukaas 06/00 (core code)
//			haukaas 06/01 (made part of official OpenSees)
//

#ifndef RandomVariable_h
#define RandomVariable_h

#include <ReliabilityDomainComponent.h>
#include <string.h>

class RandomVariable : public ReliabilityDomainComponent
{

public:
	RandomVariable(int tag, 
				double mean,
				double stdv,
				double startValue);
	RandomVariable(int tag, 
				double parameter1,
				double parameter2,
				double parameter3,
				double parameter4,
				double startValue);
	RandomVariable(int tag, 
				double mean,
				double stdv);
	RandomVariable(int tag, 
				double parameter1,
				double parameter2,
				double parameter3,
				double parameter4);
	virtual ~RandomVariable();
	virtual void Print(ostream &s, int flag =0) =0;
	virtual double getPDFvalue(double rvValue) =0;
	virtual double getCDFvalue(double rvValue) =0;
	virtual double getInverseCDFvalue(double rvValue) =0;    
	virtual const char* getType() =0;
	virtual double getMean() =0;
	virtual double getStdv() =0;
	virtual double getParameter1() =0;
	virtual double getParameter2() =0;
	virtual double getParameter3() =0;
	virtual double getParameter4() =0;
	virtual double getStartValue() =0;




protected:
	int tag;
	double startValue;

private:


};

#endif























