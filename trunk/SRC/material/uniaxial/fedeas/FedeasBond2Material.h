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
// $Date: 2001-08-19 21:56:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasBond2Material.h,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasBond2Material. FedeasBond2Material wraps the FEDEAS
// 1d material subroutine Bond_2.

#ifndef FedeasBond2Material_h
#define FedeasBond2Material_h

#include <FedeasMaterial.h>

class FedeasBond2Material : public FedeasMaterial
{
  public:
	FedeasBond2Material(int tag,
		double u1p, double q1p, double u2p, double u3p, double q3p,
		double u1n, double q1n, double u2n, double u3n, double q3n,
		double s0, double bb, double alp, double aln);
	FedeasBond2Material(void);
    virtual ~FedeasBond2Material();

  protected:

  private:

};


#endif

