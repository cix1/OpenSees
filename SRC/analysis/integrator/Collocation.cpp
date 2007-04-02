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

// $Revision: 1.2 $
// $Date: 2007-04-02 23:42:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Collocation.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the implementation of Collocation.
//
// What: "@(#) Collocation.cpp, revA"

#include <Collocation.h>
#include <FE_Element.h>
#include <LinearSOE.h>
#include <AnalysisModel.h>
#include <Vector.h>
#include <DOF_Group.h>
#include <DOF_GrpIter.h>
#include <AnalysisModel.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <math.h>


Collocation::Collocation()
    : TransientIntegrator(INTEGRATOR_TAGS_Collocation),
    theta(1.0), gamma(0.0), beta(0.0), deltaT(0.0),
    alphaM(0.0), betaK(0.0), betaKi(0.0), betaKc(0.0),
    c1(0.0), c2(0.0), c3(0.0), 
    Ut(0), Utdot(0), Utdotdot(0), U(0), Udot(0), Udotdot(0)
{
    
}


Collocation::Collocation(double _theta)
    : TransientIntegrator(INTEGRATOR_TAGS_Collocation),
    theta(_theta), gamma(0.5), beta(0.0), deltaT(0.0),
    alphaM(0.0), betaK(0.0), betaKi(0.0), betaKc(0.0),
    c1(0.0), c2(0.0), c3(0.0), 
    Ut(0), Utdot(0), Utdotdot(0), U(0), Udot(0), Udotdot(0)
{
    beta = -6.018722044382699e+002 * pow(theta,9) +
            6.618777151634235e+003 * pow(theta,8) +
           -3.231561059595987e+004 * pow(theta,7) +
            9.195359004558867e+004 * pow(theta,6) +
           -1.680788908312227e+005 * pow(theta,5) +
            2.047005794710718e+005 * pow(theta,4) +
           -1.661421563528177e+005 * pow(theta,3) +
            8.667950092619179e+004 * pow(theta,2) +
           -2.638652989051994e+004 * theta +
            3.572862280471971e+003;
}


Collocation::Collocation(double _theta,
    double _alphaM, double _betaK, double _betaKi, double _betaKc)
    : TransientIntegrator(INTEGRATOR_TAGS_Collocation),
    theta(_theta), gamma(0.5), beta(0.0), deltaT(0.0),
    alphaM(_alphaM), betaK(_betaK), betaKi(_betaKi), betaKc(_betaKc),
    c1(0.0), c2(0.0), c3(0.0), 
    Ut(0), Utdot(0), Utdotdot(0), U(0), Udot(0), Udotdot(0)
{
    beta = -6.018722044382699e+002 * pow(theta,9) +
            6.618777151634235e+003 * pow(theta,8) +
           -3.231561059595987e+004 * pow(theta,7) +
            9.195359004558867e+004 * pow(theta,6) +
           -1.680788908312227e+005 * pow(theta,5) +
            2.047005794710718e+005 * pow(theta,4) +
           -1.661421563528177e+005 * pow(theta,3) +
            8.667950092619179e+004 * pow(theta,2) +
           -2.638652989051994e+004 * theta +
            3.572862280471971e+003;
}


Collocation::Collocation(double _theta, double _beta, double _gamma)
    : TransientIntegrator(INTEGRATOR_TAGS_Collocation),
    theta(_theta), gamma(_gamma), beta(_beta), deltaT(0.0),
    alphaM(0.0), betaK(0.0), betaKi(0.0), betaKc(0.0),
    c1(0.0), c2(0.0), c3(0.0), 
    Ut(0), Utdot(0), Utdotdot(0), U(0), Udot(0), Udotdot(0)
{
    
}


Collocation::Collocation(double _theta, double _beta, double _gamma,
    double _alphaM, double _betaK, double _betaKi, double _betaKc)
    : TransientIntegrator(INTEGRATOR_TAGS_Collocation),
    theta(_theta), gamma(_gamma), beta(_beta), deltaT(0.0),
    alphaM(_alphaM), betaK(_betaK), betaKi(_betaKi), betaKc(_betaKc),
    c1(0.0), c2(0.0), c3(0.0), 
    Ut(0), Utdot(0), Utdotdot(0), U(0), Udot(0), Udotdot(0)
{
    
}


Collocation::~Collocation()
{
    // clean up the memory created
    if (Ut != 0)
        delete Ut;
    if (Utdot != 0)
        delete Utdot;
    if (Utdotdot != 0)
        delete Utdotdot;
    if (U != 0)
        delete U;
    if (Udot != 0)
        delete Udot;
    if (Udotdot != 0)
        delete Udotdot;
}


int Collocation::newStep(double _deltaT)
{
    deltaT = _deltaT;
    if (theta <= 0.0 )  {
        opserr << "Collocation::newStep() - error in variable\n";
        opserr << "theta: " << theta << " <= 0.0\n";
        return -1;
    }
    
    if (deltaT <= 0.0)  {
        opserr << "Collocation::newStep() - error in variable\n";
        opserr << "dT = " << deltaT << endln;
        return -2;
    }
    
    // get a pointer to the AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModel();

    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*theta*deltaT);
    c3 = 1.0/(beta*theta*theta*deltaT*deltaT);
    
    if (U == 0)  {
        opserr << "Collocation::newStep() - domainChange() failed or hasn't been called\n"; 
        return -3;	
    }
    
    // set response at t to be that at t+deltaT of previous step
    (*Ut) = *U;
    (*Utdot) = *Udot;
    (*Utdotdot) = *Udotdot;

    // increment the time to t+theta*deltaT and apply the load
    double time = theModel->getCurrentDomainTime();
    time += theta*deltaT;
//    theModel->applyLoadDomain(time);
    if (theModel->updateDomain(time, deltaT) < 0)  {
        opserr << "Collocation::newStep() - failed to update the domain\n";
        return -4;
    }
    
    // determine new velocities and accelerations at t+theta*deltaT
    double a1 = (1.0 - gamma/beta); 
    double a2 = theta*deltaT*(1.0 - 0.5*gamma/beta);
    Udot->addVector(a1, *Utdotdot, a2);
      
    double a3 = -1.0/(beta*theta*deltaT);
    double a4 = 1.0 - 0.5/beta;
    Udotdot->addVector(a4, *Utdot, a3);
    
    // set the trial response quantities for the nodes
    theModel->setVel(*Udot);
    theModel->setAccel(*Udotdot);
    
    return 0;
}


int Collocation::revertToLastStep()
{
    // set response at t+deltaT to be that at t .. for next step
    if (U != 0)  {
        (*U) = *Ut;
        (*Udot) = *Utdot;
        (*Udotdot) = *Utdotdot;
    }

    return 0;
}


int Collocation::formEleTangent(FE_Element *theEle)
{
    theEle->zeroTangent();
    if (statusFlag == CURRENT_TANGENT)  {
        theEle->addKtToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
    } else if (statusFlag == INITIAL_TANGENT)  {
        theEle->addKiToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
    }
    
    return 0;
}


int Collocation::formNodTangent(DOF_Group *theDof)
{
    theDof->zeroTangent();

    theDof->addCtoTang(c2);
    theDof->addMtoTang(c3);
    
    return 0;
}    


int Collocation::domainChanged()
{
    AnalysisModel *myModel = this->getAnalysisModel();
    LinearSOE *theLinSOE = this->getLinearSOE();
    const Vector &x = theLinSOE->getX();
    int size = x.Size();
    
    // if damping factors exist set them in the ele & node of the domain
    if (alphaM != 0.0 || betaK != 0.0 || betaKi != 0.0 || betaKc != 0.0)
        myModel->setRayleighDampingFactors(alphaM, betaK, betaKi, betaKc);
    
    // create the new Vector objects
    if (Ut == 0 || Ut->Size() != size)  {
        
        // delete the old
        if (Ut != 0)
            delete Ut;
        if (Utdot != 0)
            delete Utdot;
        if (Utdotdot != 0)
            delete Utdotdot;
        if (U != 0)
            delete U;
        if (Udot != 0)
            delete Udot;
        if (Udotdot != 0)
            delete Udotdot;
        
        // create the new
        Ut = new Vector(size);
        Utdot = new Vector(size);
        Utdotdot = new Vector(size);
        U = new Vector(size);
        Udot = new Vector(size);
        Udotdot = new Vector(size);
        
        // check we obtained the new
        if (Ut == 0 || Ut->Size() != size ||
            Utdot == 0 || Utdot->Size() != size ||
            Utdotdot == 0 || Utdotdot->Size() != size ||
            U == 0 || U->Size() != size ||
            Udot == 0 || Udot->Size() != size ||
            Udotdot == 0 || Udotdot->Size() != size)  {
            
            opserr << "Collocation::domainChanged - ran out of memory\n";
            
            // delete the old
            if (Ut != 0)
                delete Ut;
            if (Utdot != 0)
                delete Utdot;
            if (Utdotdot != 0)
                delete Utdotdot;
            if (U != 0)
                delete U;
            if (Udot != 0)
                delete Udot;
            if (Udotdot != 0)
                delete Udotdot;
            
            Ut = 0; Utdot = 0; Utdotdot = 0;
            U = 0; Udot = 0; Udotdot = 0;

            return -1;
        }
    }        
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFs = myModel->getDOFs();
    DOF_Group *dofPtr;
    
    while ((dofPtr = theDOFs()) != 0)  {
        const ID &id = dofPtr->getID();
        int idSize = id.Size();
        
        int i;
        const Vector &disp = dofPtr->getCommittedDisp();	
        for (i=0; i < idSize; i++)  {
            int loc = id(i);
            if (loc >= 0)  {
                (*U)(loc) = disp(i);		
            }
        }
        
        const Vector &vel = dofPtr->getCommittedVel();
        for (i=0; i < idSize; i++)  {
            int loc = id(i);
            if (loc >= 0)  {
                (*Udot)(loc) = vel(i);
            }
        }
        
        const Vector &accel = dofPtr->getCommittedAccel();	
        for (i=0; i < idSize; i++)  {
            int loc = id(i);
            if (loc >= 0)  {
                (*Udotdot)(loc) = accel(i);
            }
        }        
    }    
    
    return 0;
}


int Collocation::update(const Vector &deltaU)
{
    AnalysisModel *theModel = this->getAnalysisModel();
    if (theModel == 0)  {
        opserr << "WARNING Collocation::update() - no AnalysisModel set\n";
        return -1;
    }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if (Ut == 0)  {
        opserr << "WARNING Collocation::update() - domainChange() failed or not called\n";
        return -2;
    }	
    
    // check deltaU is of correct size
    if (deltaU.Size() != U->Size())  {
        opserr << "WARNING Collocation::update() - Vectors of incompatible size ";
        opserr << " expecting " << U->Size() << " obtained " << deltaU.Size() << endln;
        return -3;
    }
    
    // determine the response at t+theta*deltaT
    (*U) += deltaU;

    Udot->addVector(1.0, deltaU, c2);
    
    Udotdot->addVector(1.0, deltaU, c3);
    
    // update the response at the DOFs
    theModel->setResponse(*U,*Udot,*Udotdot);        
    if (theModel->updateDomain() < 0)  {
        opserr << "Collocation::update() - failed to update the domain\n";
        return -4;
    }
    
    return 0;
}    


int Collocation::commit(void)
{
    
    AnalysisModel *theModel = this->getAnalysisModel();
    if (theModel == 0)  {
        opserr << "WARNING Collocation::commit() - no AnalysisModel set\n";
        return -1;
    }	  
        
    // determine response quantities at t+deltaT
    Udotdot->addVector(1.0/theta, *Utdotdot, (theta-1.0)/theta);
    
    (*Udot) = *Utdot;
    double a1 = deltaT*(1.0 - gamma);
    double a2 = deltaT*gamma;
    Udot->addVector(1.0, *Utdotdot, a1);
    Udot->addVector(1.0, *Udotdot, a2);
    
    (*U) = *Ut;
    U->addVector(1.0, *Utdot, deltaT);
    double a3 = deltaT*deltaT*(0.5 - beta);
    double a4 = deltaT*deltaT*beta;
    U->addVector(1.0, *Utdotdot, a3);
    U->addVector(1.0, *Udotdot, a4);

    // update the response at the DOFs
    theModel->setResponse(*U,*Udot,*Udotdot);        
//    if (theModel->updateDomain() < 0)  {
//        opserr << "Collocation::commit() - failed to update the domain\n";
//        return -4;
//    }
    
    // set the time to be t+delta t
    double time = theModel->getCurrentDomainTime();
    time += (1.0-theta)*deltaT;
    theModel->setCurrentDomainTime(time);
    
    return theModel->commitDomain();
}


int Collocation::sendSelf(int cTag, Channel &theChannel)
{
    static Vector data(5);
    data(0) = theta;
    data(1) = alphaM;
    data(2) = betaK;
    data(3) = betaKi;
    data(4) = betaKc;
    
    if (theChannel.sendVector(this->getDbTag(), cTag, data) < 0)  {
        opserr << "WARNING Collocation::sendSelf() - failed to send the data\n";
        return -1;
    }

    return 0;
}


int Collocation::recvSelf(int cTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
{
    Vector data(5);
    if (theChannel.recvVector(this->getDbTag(), cTag, data) < 0)  {
        opserr << "WARNING Collocation::recvSelf() - could not receive data\n";
        return -1;
    }
    
    theta  = data(0);
    alphaM = data(1);
    betaK  = data(2);
    betaKi = data(3);
    betaKc = data(4);
    
    return 0;
}


void Collocation::Print(OPS_Stream &s, int flag)
{
    AnalysisModel *theModel = this->getAnalysisModel();
    if (theModel != 0)  {
        double currentTime = theModel->getCurrentDomainTime();
        s << "\t Collocation - currentTime: " << currentTime << endln;
        s << "  theta: " << theta << endln;
        s << "  Rayleigh Damping - alphaM: " << alphaM;
        s << "  betaK: " << betaK << "  betaKi: " << betaKi << endln;	    
    } else 
        s << "\t Collocation - no associated AnalysisModel\n";
}
