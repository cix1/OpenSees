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
// $Date: 2001-06-14 08:06:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/misc/MatrixOperations.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) during Spring 2000
// Revised: haukaas 06/00 (core code)
//			haukaas 06/01 (made part of official OpenSees)
//

#ifndef MatrixOperations_h
#define MatrixOperations_h

#include <Vector.h>
#include <Matrix.h>

class MatrixOperations
{

public:
	MatrixOperations(Matrix passedMatrix);
	~MatrixOperations();
	
	int setMatrix(Matrix passedMatrix);

	int computeLowerCholesky();
	int computeInverseLowerCholesky();
	int computeCholeskyAndItsInverse();
	int computeMatrixNorm();
	int computeInverse();
	int computeTranspose();
	int computeSquareRoot();
	int computeTrace();
	

	Matrix getMatrix();
	Matrix getLowerCholesky();
	Matrix getInverseLowerCholesky();
	Matrix getInverse();
	Matrix getTranspose();
	Matrix getSquareRoot();
	double getMatrixNorm();
	double getTrace();

protected:

private:
	Matrix *theMatrix;
	Matrix *theLowerCholesky;
	Matrix *theInverseLowerCholesky;
	Matrix *theInverse;
	Matrix *theTranspose;
	Matrix *theSquareRoot;
	double theMatrixNorm;
	double theTrace; 

};

#endif

