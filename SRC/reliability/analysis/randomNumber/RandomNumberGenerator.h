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
                                                                        
// $Revision: 1.4 $
// $Date: 2007-10-26 16:36:06 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/randomNumber/RandomNumberGenerator.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef RandomNumberGenerator_h
#define RandomNumberGenerator_h

#include <Vector.h>

class RandomNumberGenerator
{

public:
	RandomNumberGenerator();
	virtual ~RandomNumberGenerator();

	virtual int		generate_nIndependentStdNormalNumbers(int n, int seed=0) =0;
	virtual int     generate_nIndependentUniformNumbers(int n, double lower, double upper, int seed=0) =0;
	virtual const Vector &getGeneratedNumbers() =0;
	virtual int     getSeed() =0;

protected:

private:

};

#endif
