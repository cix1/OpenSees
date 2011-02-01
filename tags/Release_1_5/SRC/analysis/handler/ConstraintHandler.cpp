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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/ConstraintHandler.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/handler/ConstraintHandler.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of ConstraintHandler.
//
// What: "@(#) ConstraintHandler.h, revA"

#include <ConstraintHandler.h>
#include <Domain.h>
#include <AnalysisModel.h>
#include <Integrator.h>

ConstraintHandler::ConstraintHandler(int clasTag)
:MovableObject(clasTag),
 theDomainPtr(0),theAnalysisModelPtr(0),theIntegratorPtr(0)
{
}


ConstraintHandler::~ConstraintHandler()
{
    
}

void 
ConstraintHandler::setLinks(Domain &theDomain, 
			    AnalysisModel &theModel,
			    Integrator &theIntegrator)
{
    theDomainPtr = &theDomain;
    theAnalysisModelPtr = &theModel;
    theIntegratorPtr = &theIntegrator;
}
	


Domain *
ConstraintHandler::getDomainPtr(void) const
{
    return theDomainPtr;
}

AnalysisModel *
ConstraintHandler::getAnalysisModelPtr(void) const
{
    return theAnalysisModelPtr;
}

Integrator *
ConstraintHandler::getIntegratorPtr(void) const
{
    return theIntegratorPtr;
}


