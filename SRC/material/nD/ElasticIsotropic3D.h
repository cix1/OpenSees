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
                                                                        
// $Revision: 1.12 $                                                              
// $Date: 2002-06-10 22:24:04 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropic3D.h,v $                                                                
                                                                        
                                                                        
#ifndef ElasticIsotropic3D_h
#define ElasticIsotropic3D_h
	 
#include <ElasticIsotropicMaterial.h>
#include <Matrix.h>
#include <Vector.h>
#include <ID.h>

#include <straint.h>
#include <stresst.h>
#include <Tensor.h>

class ElasticIsotropic3D : public ElasticIsotropicMaterial
{
  public:
    ElasticIsotropic3D (int tag, double E, double nu, double rho);
    ElasticIsotropic3D ();
    ~ElasticIsotropic3D ();

    int setTrialStrain (const Vector &v);
    int setTrialStrain (const Vector &v, const Vector &r);
    int setTrialStrainIncr (const Vector &v);
    int setTrialStrainIncr (const Vector &v, const Vector &r);
    const Matrix &getTangent (void);
    const Vector &getStress (void);
    const Vector &getStrain (void);
    
    int setTrialStrain (const Tensor &v);
    int setTrialStrain (const Tensor &v, const Tensor &r);
    int setTrialStrainIncr (const Tensor &v);
    int setTrialStrainIncr (const Tensor &v, const Tensor &r);
    const Tensor &getTangentTensor (void);
    const stresstensor getStressTensor (void);
    const straintensor getStrainTensor (void);
    const straintensor getPlasticStrainTensor (void);

    int commitState (void);
    int revertToLastCommit (void);
    int revertToStart (void);
    
    NDMaterial *getCopy (void);
    const char *getType (void) const;
    int getOrder (void) const;

    void Print(ostream &s, int flag =0);
    void setInitElasticStiffness(void);

  protected:

  private:
    static Vector sigma;        // Stress vector
    static Matrix D;            // Elastic constantsVector sigma;
    Vector epsilon;		// Strain vector

    stresstensor Stress;	// Stress tensor    
    Tensor Dt;			// Elastic constants tensor
    straintensor Strain;	// Strain tensor    
};


#endif


