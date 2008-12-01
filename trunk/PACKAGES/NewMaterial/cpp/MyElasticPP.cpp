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
// $Date: 2008-12-01 23:30:00 $
// $Source: /usr/local/cvs/OpenSees/PACKAGES/NewMaterial/cpp/MyElasticPP.cpp,v $
                                                                        
// Written: fmk 
//
// Description: This file contains the class implementation for 
// ElasticMaterial. 
//
// What: "@(#) MyElasticPP.C, revA"

#include <OPS_ProceduralAPI.h>
#include "MyElasticPP.h"

#include <Vector.h>
#include <Channel.h>
#include <math.h>
#include <float.h>


#ifdef _USRDLL
#define DllExport _declspec(dllexport)
#else
#define DllExport
#endif

extern "C" DllExport void *
OPS_MyElasticPP(int argc, 
		const char **argv)
{
  // Pointer to a uniaxial material that will be returned
  UniaxialMaterial *theMaterial = 0;
  
  if (argc < 5) {
    opserr << "WARNING insufficient arguments\n";
    opserr << "Want: uniaxialMaterial ElasticPP tag? E? epsy?" << endln;
    return 0;
  }
  
  int    iData[1];
  double dData[2];
  int numData;
  
  numData = 1;
  if (OPS_GetIntInput(&numData, iData) != 0) {
    opserr << "WARNING invalid uniaxialMaterial ElasticPP tag" << endln;
    return 0;
  }

  numData = 2;
  if (OPS_GetDoubleInput(&numData, dData) != 0) {
    opserr << "WARNING invalid E & ep\n";
    return 0;	
  }
  
  theMaterial = new MyElasticPP(iData[0], dData[0], dData[1]);       

  if (theMaterial == 0) {
    opserr << "WARNING could not create uniaxialMaterial " << argv[1] << endln;
    return 0;
  }

  return theMaterial;
}




MyElasticPP::MyElasticPP(int tag, double e, double eyp)
:UniaxialMaterial(tag,MAT_TAG_MyElasticPP),
 ezero(0.0), E(e), trialStrain(0.0), ep(0.0),
 trialStress(0.0), trialTangent(E)
{
  fyp = E*eyp;
  fyn = -fyp;
}

MyElasticPP::MyElasticPP()
:UniaxialMaterial(0,MAT_TAG_MyElasticPP),
 fyp(0.0), fyn(0.0), ezero(0.0), E(0.0), trialStrain(0.0), ep(0.0),
 trialStress(0.0), trialTangent(0.0)
{

}

MyElasticPP::~MyElasticPP()
{
  // does nothing
}

int 
MyElasticPP::setTrialStrain(double strain, double strainRate)
{
  /*
    if (fabs(trialStrain - strain) < DBL_EPSILON)
      return 0;
  */
    trialStrain = strain;

    double sigtrial;	// trial stress
    double f;		// yield function

    // compute trial stress
    sigtrial = E * ( trialStrain - ezero - ep );

    //sigtrial  = E * trialStrain;
    //sigtrial -= E * ezero;
    //sigtrial -= E *  ep;

    // evaluate yield function
    if ( sigtrial >= 0.0 )
	f =  sigtrial - fyp;
    else
	f = -sigtrial + fyn;

    double fYieldSurface = - E * DBL_EPSILON;
    if ( f <= fYieldSurface ) {

      // elastic
      trialStress = sigtrial;
      trialTangent = E;

    } else {

      // plastic
      if ( sigtrial > 0.0 ) {
	trialStress = fyp;
      } else {
	trialStress = fyn;
      }

      trialTangent = 0.0;
    }

    return 0;
}

double 
MyElasticPP::getStrain(void)
{
  return trialStrain;
}

double 
MyElasticPP::getStress(void)
{
  return trialStress;
}


double 
MyElasticPP::getTangent(void)
{
  return trialTangent;
}

int 
MyElasticPP::commitState(void)
{
    double sigtrial;	// trial stress
    double f;		// yield function

    // compute trial stress
    sigtrial = E * ( trialStrain - ezero - ep );

    // evaluate yield function
    if ( sigtrial >= 0.0 )
	f =  sigtrial - fyp;
    else
	f = -sigtrial + fyn;

    double fYieldSurface = - E * DBL_EPSILON;
    if ( f > fYieldSurface ) {
      // plastic
      if ( sigtrial > 0.0 ) {
	ep += f / E;
      } else {
	ep -= f / E;
      }
    }

    return 0;
}	


int 
MyElasticPP::revertToLastCommit(void)
{
    return 0;
}


int 
MyElasticPP::revertToStart(void)
{
    ep = 0.0;

    return 0;
}


UniaxialMaterial *
MyElasticPP::getCopy(void)
{
  MyElasticPP *theCopy =
    new MyElasticPP(this->getTag(),E,fyp/E);
  theCopy->ep = this->ep;
  
  return theCopy;
}


int 
MyElasticPP::sendSelf(int cTag, Channel &theChannel)
{
  int res = 0;
  static Vector data(6);
  data(0) = this->getTag();
  data(1) = ep;
  data(2) = E;
  data(3) = ezero;
  data(4) = fyp;
  data(5) = fyn;

  res = theChannel.sendVector(this->getDbTag(), cTag, data);
  if (res < 0) 
    opserr << "MyElasticPP::sendSelf() - failed to send data\n";

  return res;
}

int 
MyElasticPP::recvSelf(int cTag, Channel &theChannel, 
				 FEM_ObjectBroker &theBroker)
{
  int res = 0;
  static Vector data(6);
  res = theChannel.recvVector(this->getDbTag(), cTag, data);
  if (res < 0) 
    opserr << "MyElasticPP::recvSelf() - failed to recv data\n";
  else {
    this->setTag(data(0));
    ep    = data(1);
    E     = data(2);
    ezero = data(3);
    fyp   = data(4);
    fyn   = data(5);  
  }

  return res;
}

void 
MyElasticPP::Print(OPS_Stream &s, int flag)
{
  s << "MyElasticPP tag: " << this->getTag() << endln;
  s << "  E: " << E << endln;
  s << "  ep: " << ep << endln;
  s << "  stress: " << trialStress << " tangent: " << trialTangent << endln;
}


