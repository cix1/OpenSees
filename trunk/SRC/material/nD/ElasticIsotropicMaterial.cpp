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
                                                                        
// $Revision: 1.17 $                                                              
// $Date: 2002-12-05 22:49:09 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicMaterial.cpp,v $                                                                
                                                                        
                                                                        
// File: ~/material/ElasticIsotropicMaterial.C
//
// Written: MHS 
// Created: Feb 2000
// Revision: A
// Boris Jeremic (@ucdavis.edu) 19June2002 added getE, getnu
//
// Description: This file contains the class implementation for ElasticIsotropicMaterial.
//
// What: "@(#) ElasticIsotropicMaterial.C, revA"

#include <string.h>

#include <ElasticIsotropicMaterial.h>
#include <ElasticIsotropicPlaneStress2D.h>
#include <ElasticIsotropicPlaneStrain2D.h>
#include <ElasticIsotropicAxiSymm.h>
#include <ElasticIsotropic3D.h>
#include <PressureDependentElastic3D.h>
#include <ElasticIsotropicPlateFiber.h>
#include <ElasticIsotropicBeamFiber.h>

#include <Tensor.h>
#include <Channel.h>

#include <G3Globals.h>

ElasticIsotropicMaterial::ElasticIsotropicMaterial
(int tag, int classTag, double e, double nu, double r)
  :NDMaterial(tag, classTag), E(e), v(nu), rho(r)
{

}

ElasticIsotropicMaterial::ElasticIsotropicMaterial
(int tag, double e, double nu, double r)
  :NDMaterial(tag, ND_TAG_ElasticIsotropic), E(e), v(nu), rho(r)
{

}

ElasticIsotropicMaterial::~ElasticIsotropicMaterial()
{
	
}

double
ElasticIsotropicMaterial::getRho() 
{ 
  return rho ;
}


// Boris Jeremic (@ucdavis.edu) 19June2002
double ElasticIsotropicMaterial::getE() 
{ 
  return E;
}

// Boris Jeremic (@ucdavis.edu) 19June2002
double ElasticIsotropicMaterial::getnu() 
{ 
  return v;
}



NDMaterial*
ElasticIsotropicMaterial::getCopy (const char *type)
{
    if (strcmp(type,"PlaneStress2D") == 0 || strcmp(type,"PlaneStress") == 0)
    {
	ElasticIsotropicPlaneStress2D *theModel;
	theModel = new ElasticIsotropicPlaneStress2D (this->getTag(), E, v, rho);
		// DOES NOT COPY sigma, D, and epsilon ...
		// This function should only be called during element instantiation, so
		// no state determination is performed on the material model object
		// prior to copying the material model (calling this function)
	return theModel;
    }

    else if (strcmp(type,"PlaneStrain2D") == 0 || strcmp(type,"PlaneStrain") == 0)
    {
	ElasticIsotropicPlaneStrain2D *theModel;
	theModel = new ElasticIsotropicPlaneStrain2D (this->getTag(), E, v, rho);
		// DOES NOT COPY sigma, D, and epsilon ...
		// This function should only be called during element instantiation, so
		// no state determination is performed on the material model object
		// prior to copying the material model (calling this function)
	return theModel;
    }
    else if (strcmp(type,"AxiSymmetric2D") == 0 || strcmp(type,"AxiSymmetric") == 0)
    {
	ElasticIsotropicAxiSymm *theModel;
	theModel = new ElasticIsotropicAxiSymm(this->getTag(), E, v, rho);
		// DOES NOT COPY sigma, D, and epsilon ...
		// This function should only be called during element instantiation, so
		// no state determination is performed on the material model object
		// prior to copying the material model (calling this function)
	return theModel;
    }
///////////////////////////////
    else if (strcmp(type,"ThreeDimensional") == 0 || 
			          strcmp(type,"3D") == 0 || 
												 strcmp(type,"ElasticIsotropic3D") == 0)
    {
	ElasticIsotropic3D *theModel;
	theModel = new ElasticIsotropic3D (this->getTag(), E, v, rho);
		// DOES NOT COPY sigma, D, and epsilon ...
		// This function should only be called during element instantiation, so
		// no state determination is performed on the material model object
		// prior to copying the material model (calling this function)
	return theModel;
    }
///////////////////////////////
    else if (strcmp(type,"PressureDependentElastic3D") == 0 ) 
    {
      PressureDependentElastic3D *theModel;
      theModel = new PressureDependentElastic3D(this->getTag(), E, v, 0.0, 0.5, 100.0, 0.0);
    // DOES NOT COPY sigma, D, and epsilon ...
    // This function should only be called during element instantiation, so
    // no state determination is performed on the material model object
    // prior to copying the material model (calling this function)
    	return theModel;
    }
///////////////////////////////
    else if (strcmp(type,"PlateFiber") == 0)
    {
	ElasticIsotropicPlateFiber *theModel;
	theModel = new ElasticIsotropicPlateFiber(this->getTag(), E, v, rho);
		// DOES NOT COPY sigma, D, and epsilon ...
		// This function should only be called during element instantiation, so
		// no state determination is performed on the material model object
		// prior to copying the material model (calling this function)
	return theModel;
    }
    else if (strcmp(type,"BeamFiber") == 0)
    {
	ElasticIsotropicBeamFiber *theModel;
	theModel = new ElasticIsotropicBeamFiber(this->getTag(), E, v, rho);
		// DOES NOT COPY sigma, D, and epsilon ...
		// This function should only be called during element instantiation, so
		// no state determination is performed on the material model object
		// prior to copying the material model (calling this function)
	return theModel;
    }


    // Handle other cases
    else
    {
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getModel failed to get model %s",
			      type);

	return 0;
    }
}

int
ElasticIsotropicMaterial::setTrialStrain (const Vector &v)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrain -- subclass responsibility");

    return 0;
}

int
ElasticIsotropicMaterial::setTrialStrain (const Vector &v, const Vector &rate)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrain -- subclass responsibility");

    return 0;
}

int
ElasticIsotropicMaterial::setTrialStrainIncr (const Vector &v)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility");

    return 0;
}

int
ElasticIsotropicMaterial::setTrialStrainIncr (const Vector &v, const Vector &rate)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility");

    return 0;
}

const Matrix&
ElasticIsotropicMaterial::getTangent (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getTangent -- subclass responsibility");

	// Just to make it compile
	Matrix *ret = new Matrix();
	return *ret;
}

const Matrix&
ElasticIsotropicMaterial::getInitialTangent (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getInitialTangent -- subclass responsibility");

	// Just to make it compile
	Matrix *ret = new Matrix();
	return *ret;
}

const Vector&
ElasticIsotropicMaterial::getStress (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getStress -- subclass responsibility");

	// Just to make it compile
	Vector *ret = new Vector();
	return *ret;
}

const Vector&
ElasticIsotropicMaterial::getStrain (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getStrain -- subclass responsibility");

	// Just to make it compile
	Vector *ret = new Vector();
	return *ret;
}

int
ElasticIsotropicMaterial::setTrialStrain (const Tensor &v)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrain -- subclass responsibility");

    return 0;
}

int
ElasticIsotropicMaterial::setTrialStrain (const Tensor &v, const Tensor &r)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrain -- subclass responsibility");

    return 0;
}

int
ElasticIsotropicMaterial::setTrialStrainIncr (const Tensor &v)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility");

    return 0;
}

int
ElasticIsotropicMaterial::setTrialStrainIncr (const Tensor &v, const Tensor &r)
{
    g3ErrorHandler->fatal("ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility");

    return 0;
}

const Tensor&
ElasticIsotropicMaterial::getTangentTensor (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getTangentTensor -- subclass responsibility");

	// Just to make it compile
	Tensor *t = new Tensor;
	return *t;
}

const stresstensor ElasticIsotropicMaterial::getStressTensor (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getStressTensor -- subclass responsibility");

	// Just to make it compile
        stresstensor t;
	return t;
}

const straintensor ElasticIsotropicMaterial::getStrainTensor (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getStrainTensor -- subclass responsibility");
	// Just to make it compile
        straintensor t;
        return t;
}

const straintensor ElasticIsotropicMaterial::getPlasticStrainTensor (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getPlasticStrainTensor -- subclass responsibility");
	// Just to make it compile
	straintensor t;
        return t;
}

int
ElasticIsotropicMaterial::commitState (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::commitState -- subclass responsibility");

	return 0;
}

int
ElasticIsotropicMaterial::revertToLastCommit (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::revertToLastCommit -- subclass responsibility");

	return 0;
}

int
ElasticIsotropicMaterial::revertToStart (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::revertToStart -- subclass responsibility");

	return 0;
}

NDMaterial*
ElasticIsotropicMaterial::getCopy (void)
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getCopy -- subclass responsibility");

	return 0;
}

const char*
ElasticIsotropicMaterial::getType (void) const
{
	g3ErrorHandler->fatal("ElasticIsotropicMaterial::getType -- subclass responsibility");
	
	return 0;
}

int
ElasticIsotropicMaterial::getOrder (void) const
{
	 g3ErrorHandler->fatal("ElasticIsotropicMaterial::getOrder -- subclass responsibility");

	return 0;
}

int
ElasticIsotropicMaterial::sendSelf (int commitTag, Channel &theChannel)
{
  int res = 0;

  static Vector data(4);
  
  data(0) = this->getTag();
  data(1) = E;
  data(2) = v;
  data(3) = rho;
  
 res += theChannel.sendVector(this->getDbTag(), commitTag, data);
 if (res < 0) {
   g3ErrorHandler->warning("%s -- could not send Vector",
			   "ElasticIsotropicMaterial::sendSelf");
   return res;
 }

 return res;
}

int
ElasticIsotropicMaterial::recvSelf (int commitTag, Channel &theChannel, 
		 FEM_ObjectBroker &theBroker)
{
  int res = 0;
  
  static Vector data(4);
  
  res += theChannel.recvVector(this->getDbTag(), commitTag, data);
  if (res < 0) {
    g3ErrorHandler->warning("%s -- could not receive Vector",
			    "ElasticIsotropicMaterial::recvSelf");
    return res;
  }
    
  this->setTag((int)data(0));
  E = data(1);
  v = data(2);
  rho = data(3);
  
  return res;
}

void
ElasticIsotropicMaterial::Print (ostream &s, int flag)
{
	s << "Elastic Isotropic Material Model" << endl;
	s << "\tE:  " << E << endl;
	s << "\tv:  " << v << endl;
	s << "\trho:  " << rho << endl;

	return;
}

int 
ElasticIsotropicMaterial::setParameter(char **argv, int argc, Information &info)
{
  return -1;
}

int 
ElasticIsotropicMaterial::updateParameter(int parameterID, Information &info)
{ 
  return -1;
}
