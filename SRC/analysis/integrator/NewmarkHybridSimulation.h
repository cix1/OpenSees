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

// $Revision: 1.1 $
// $Date: 2005-12-19 22:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/NewmarkHybridSimulation.h,v $

#ifndef NewmarkHybridSimulation_h
#define NewmarkHybridSimulation_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/05
// Revision: A
//
// Description: This file contains the class definition for NewmarkHybridSimulation.
// NewmarkHybridSimulation is an algorithmic class for performing a transient analysis
// using the Newmark integration scheme.
//
// What: "@(#) NewmarkHybridSimulation.h, revA"

#include <TransientIntegrator.h>

class DOF_Group;
class FE_Element;
class Vector;
class ConvergenceTest;

class NewmarkHybridSimulation : public TransientIntegrator
{
public:
    // constructors
    NewmarkHybridSimulation();
    NewmarkHybridSimulation(double gamma, double beta, ConvergenceTest &theTest);
    NewmarkHybridSimulation(double gamma, double beta, ConvergenceTest &theTest,
        double alphaM, double betaK, double betaKi, double betaKc); 
    
    // destructor
    ~NewmarkHybridSimulation();
    
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        
    
    int domainChanged(void);
    int newStep(double deltaT);
    int revertToLastStep(void);
    int update(const Vector &deltaU);
    
    virtual int sendSelf(int commitTag, Channel &theChannel);
    virtual int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    
    void Print(OPS_Stream &s, int flag = 0);
    
protected:
    
private:
    double gamma;
    double beta;

    // rayleigh damping factors
    double alphaM;
    double betaK;
    double betaKi;
    double betaKc;

    ConvergenceTest *theTest;       // convergence test
    double rFact;                   // displacement increment reduction factor
    double c1, c2, c3;              // some constants we need to keep
    Vector *Ut, *Utdot, *Utdotdot;  // response quantities at time t
    Vector *U, *Udot, *Udotdot;     // response quantities at time t+deltaT
};

#endif
