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
// $Date: 2000-09-15 08:23:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PathIndependentMaterial.h,v $
                                                      
// Written: MHS
// Created: Aug 2000
//
// Description: This file contains the class definition for 
// PathIndependentMaterial.  PathIndependentMaterial uses a UniaxialMaterial
// object to represent a path-independent uniaxial material.  Since
// it is path-independent, no state information is stored by
// PathIndependentMaterial.

#ifndef PathIndependentMaterial_h
#define PathIndependentMaterial_h

#include <UniaxialMaterial.h>

#define MAT_TAG_PathIndependentMaterial 1340

class PathIndependentMaterial : public UniaxialMaterial
{
  public:
    PathIndependentMaterial(int tag, UniaxialMaterial &material); 
	PathIndependentMaterial();
    ~PathIndependentMaterial();

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void);          
    double getStress(void);
    double getTangent(void);
    double getSecant(void);

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void);
    
    int sendSelf(int commitTag, Channel &theChannel);  
    int recvSelf(int commitTag, Channel &theChannel, 
		 FEM_ObjectBroker &theBroker);    
    
    void Print(ostream &s, int flag =0);
    
  protected:
    
  private:
    double Tstrain;
	UniaxialMaterial *theMaterial;
};


#endif
