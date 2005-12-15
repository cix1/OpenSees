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
// $Date: 2005-12-15 00:35:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/pattern/TriangleSeries.h,v $
                                                                        
#ifndef TriangleSeries_h
#define TriangleSeries_h

// File: ~/domain/pattern/TriangleSeries.h
//
// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/04
// Revision: A
//
// Purpose: This file contains the class definition for TriangleSeries.
// TriangleSeries is a concrete class. A TriangleSeries object provides
// a triangle time series. The factor is given by the pseudoTime (t),
// triangle period (T) and phase shift (phi),
// and a constant factor provided in the constructor,
// the duration by tStart and tFinal;
//
// What: "@(#) TriangleSeries.h, revA"

#include <TimeSeries.h>

class TriangleSeries : public TimeSeries
{
  public:
    // constructors
    TriangleSeries(double tStart, double tFinish,
        double period, double shift, double cFactor = 1.0);

    TriangleSeries();
    
    // destructor
    ~TriangleSeries();
    
    // method to get load factor
    double getFactor(double pseudoTime);
    double getDuration () {return tFinish-tStart;}
    double getPeakFactor () {return cFactor;}
    double getTimeIncr (double pseudoTime) {return tFinish-tStart;}
    
    // methods for output    
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, 
		 FEM_ObjectBroker &theBroker);

    void Print(OPS_Stream &s, int flag =0);    
    
  protected:
	
  private:
    double tStart;    // start time of time series (sec)
    double tFinish;   // end time of time series (sec)
    double period;    // period of triangle series (sec)
    double shift;     // phase shift of triangle series (sec)
    double cFactor;   // amplitude of triangle series
};

#endif
