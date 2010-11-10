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
**                                                                    **
** Additions and changes by:                                          **
**   Boris Jeremic (@ucdavis.edu)                                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.17 $                                                              
// $Date: 2004-07-20 22:39:02 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/NDMaterial.cpp,v $                                                                
                                                                        
// File: ~/material/NDMaterial.C
//
// Written: MHS 
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class implementation for NDMaterial.
//
// What: "@(#) NDMaterial.C, revA"

#include <NDMaterial.h>
#include <Information.h>
#include <OPS_Globals.h>
#include <Matrix.h>
#include <Vector.h>
#include <stresst.h>
#include <straint.h>
#include <MaterialResponse.h>

Matrix NDMaterial::errMatrix(1,1);
Vector NDMaterial::errVector(1);
Tensor NDMaterial::errTensor(2, def_dim_2, 0.0 );
stresstensor NDMaterial::errstresstensor;
straintensor NDMaterial::errstraintensor;

NDMaterial::NDMaterial(int tag, int classTag)
:Material(tag,classTag)
{

}

NDMaterial::NDMaterial()
:Material(0, 0)
{

}

NDMaterial::~NDMaterial()
{

}


double
NDMaterial::getRho(void)
{
  return 0.0;
}

// BJ added 19June2002
double NDMaterial::getE(void) 
  {
    return 0.0;
  }

// BJ added 19June2002
double NDMaterial::getnu(void)
  {
    return 0.0;
  }

// BJ added 19June2002
double NDMaterial::getpsi(void)
  {
    return 0.0;
  }

const Vector &
NDMaterial::getCommittedStress(void) 
{
  return this->getStress();
}

const Vector &
NDMaterial::getCommittedStrain(void) 
{
  return this->getStrain();
}

// methods to set and retrieve state.
int 
NDMaterial::setTrialStrain(const Vector &v)
{
   opserr << "NDMaterial::setTrialStrain -- subclass responsibility\n";
   return -1;    
}

int 
NDMaterial::setTrialStrain(const Vector &v, const Vector &r)
{
   opserr << "NDMaterial::setTrialStrain -- subclass responsibility\n";
   return -1;    
}

int 
NDMaterial::setTrialStrainIncr(const Vector &v)
{
   opserr << "NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
   return -1;    
}

int 
NDMaterial::setTrialStrainIncr(const Vector &v, const Vector &r)
{
   opserr << "NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
   return -1;    
}

const Matrix &
NDMaterial::getTangent(void)
{
   opserr << "NDMaterial::getTangent -- subclass responsibility\n";
   return errMatrix;    
}

const Vector &
NDMaterial::getStress(void)
{
   opserr << "NDMaterial::getStress -- subclass responsibility\n";
   return errVector;    
}

const Vector &
NDMaterial::getStrain(void)
{
   opserr << "NDMaterial::getStrain -- subclass responsibility\n";
   return errVector;    
}

int 
NDMaterial::setTrialStrain(const Tensor &v)
{
   opserr << "NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
   return -1;    
}

int 
NDMaterial::setTrialStrain(const Tensor &v, const Tensor &r)    
{
   opserr << "NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
   return -1;    
}

int 
NDMaterial::setTrialStrainIncr(const Tensor &v)
{
   opserr << "NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
   return -1;    
}

int 
NDMaterial::setTrialStrainIncr(const Tensor &v, const Tensor &r)
{
   opserr << "NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
   return -1;    
}

//Zhao (zcheng@ucdavis.edu) 
// added Sept 22 2003 for Large Deformation, F is the Deformation Grandient
int
NDMaterial::setTrialF(const straintensor &f)
{
   opserr << "NDMaterial::setTrialF -- subclass responsibility\n";
   return -1;
}

int
NDMaterial::setTrialFIncr(const straintensor &df)
{
   opserr << "NDMaterial::setTrialF -- subclass responsibility\n";
   return -1;
}

int
NDMaterial::setTrialC(const straintensor &c)
{
   opserr << "NDMaterial::setTrialC -- subclass responsibility\n";
   return -1;
}

int
NDMaterial::setTrialCIncr(const straintensor &c)
{
   opserr << "NDMaterial::setTrialC -- subclass responsibility\n";
   return -1;
}

const stresstensor NDMaterial::getPK1StressTensor(void)
{
   opserr << "NDMaterial::getPK1StressTensor -- subclass responsibility\n";
   return errstresstensor;    
}

const stresstensor NDMaterial::getCauchyStressTensor(void)
{
   opserr << "NDMaterial::getCauchyStressTensor -- subclass responsibility\n";
   return errstresstensor;    
}

const straintensor NDMaterial::getF(void)
{
   opserr << "NDMaterial::getF -- subclass responsibility\n";
   return errstraintensor;    
}

const straintensor NDMaterial::getC(void)
{
   opserr << "NDMaterial::getF -- subclass responsibility\n";
   return errstraintensor;    
}

const straintensor NDMaterial::getFp(void)
{
   opserr << "NDMaterial::getFp -- subclass responsibility\n";
   return errstraintensor;    
}
// Only For Large Deformation, END////////////////////////////

const Tensor &
NDMaterial::getTangentTensor(void)
{
   opserr << "NDMaterial::getTangentTensor -- subclass responsibility\n";
   return errTensor;    
}

const stresstensor NDMaterial::getStressTensor(void)
{
   opserr << "NDMaterial::getStressTensor -- subclass responsibility\n";
   return errstresstensor;    
}

const straintensor NDMaterial::getStrainTensor(void)
{
   opserr << "NDMaterial::getStrainTensor -- subclass responsibility\n";
   return errstraintensor;    
}

const straintensor NDMaterial::getPlasticStrainTensor(void)
{
   opserr << "NDMaterial::getPlasticStrainTensor -- subclass responsibility\n";
   return errstraintensor;    
}


//const Tensor &
//NDMaterial::getStrainTensor(void)
//{
//   opserr << "NDMaterial::getStrainTensor -- subclass responsibility\n";
//   return errTensor;    
//}

Response*
NDMaterial::setResponse (const char **argv, int argc, Information &matInfo)
{
    if (strcmp(argv[0],"stress") == 0 || strcmp(argv[0],"stresses") == 0)
		return new MaterialResponse(this, 1, this->getStress());

    else if (strcmp(argv[0],"strain") == 0 || strcmp(argv[0],"strains") == 0)
		return new MaterialResponse(this, 2, this->getStrain());
    
    else if (strcmp(argv[0],"tangent") == 0)
      return new MaterialResponse(this, 3, this->getTangent());
    
    else
      return 0;
}

int 
NDMaterial::getResponse (int responseID, Information &matInfo)
{
	switch (responseID) {
		case 1:
			return matInfo.setVector(this->getStress());

		case 2:
			return matInfo.setVector(this->getStrain());

		case 3:
			return matInfo.setMatrix(this->getTangent());
			
		default:
			return -1;
	}
}



// AddingSensitivity:BEGIN ////////////////////////////////////////
int
NDMaterial::setParameter(const char **argv, int argc, Information &info)
{
    return -1;
}

int
NDMaterial::updateParameter(int parameterID, Information &info)
{
    return -1;
}

int
NDMaterial::activateParameter(int parameterID)
{
    return -1;
}

const Vector &
NDMaterial::getStressSensitivity(int gradNumber, bool conditional)
{
	static Vector dummy(1);
	return dummy;
}

const Vector &
NDMaterial::getStrainSensitivity(int gradNumber)
{
	static Vector dummy(1);
	return dummy;
}

double
NDMaterial::getRhoSensitivity(int gradNumber)
{
	return 0.0;
}

const Matrix &
NDMaterial::getDampTangentSensitivity(int gradNumber)
{
	static Matrix dummy(1,1);
	return dummy;
}
const Matrix &
NDMaterial::getTangentSensitivity(int gradNumber)
{
	static Matrix dummy(1,1);
	return dummy;
}
int
NDMaterial::commitSensitivity(Vector & strainSensitivity, int gradNumber, int numGrads)
{
	return 0;
}
// AddingSensitivity:END //////////////////////////////////////////

