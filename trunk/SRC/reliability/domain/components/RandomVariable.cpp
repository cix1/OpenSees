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
                                                                        
// $Revision: 1.2 $
// $Date: 2001-06-14 08:06:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/RandomVariable.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) during Spring 2000
// Revised: haukaas 06/00 (core code)
//			haukaas 06/01 (made part of official OpenSees)
//

#include <RandomVariable.h>

RandomVariable::RandomVariable(int passedTag, 
							double passedMean,
							double passedStdv,
							double passedStartValue)
:ReliabilityDomainComponent(passedTag, 14725)
{
}
RandomVariable::RandomVariable(int passedTag, 
							double passedParameter1,
							double passedParameter2,
							double passedParameter3,
							double passedParameter4,
							double passedStartValue)
:ReliabilityDomainComponent(passedTag, 14725)
{
}
RandomVariable::RandomVariable(int passedTag, 
							double passedMean,
							double passedStdv)
:ReliabilityDomainComponent(passedTag, 14725)
{
}
RandomVariable::RandomVariable(int passedTag, 
							double passedParameter1,
							double passedParameter2,
							double passedParameter3,
							double passedParameter4)
:ReliabilityDomainComponent(passedTag, 14725)
{
}


RandomVariable::~RandomVariable()
{
}
