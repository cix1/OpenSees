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
// $Date: 2003-02-14 23:01:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/GSA_Recorder.h,v $
                                                                        
#ifndef GSA_Recorder_h
#define GSA_Recorder_h

// Written: fmk 
// Created: 02/03
//
// Description: This file contains the class definition for 
// GSA_Recorder. A GSA_Recorder is used to create an o/p file
// which can be read by the Ove Arup's GSA program for
// postprocessing.
//
// What: "@(#) GSA_Recorder.h, revA"


#include <Recorder.h>
#include <ID.h>
#include <Vector.h>
#include <FileStream.h>

class Domain;
class FE_Datastore;

class GSA_Recorder: public Recorder
{
  public:
    GSA_Recorder(Domain &theDomain, char *fileName, double deltaT = 0.0); 

    ~GSA_Recorder();
    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    void restart(void);    
    
  protected:
    
  private:	
    Domain *theDomain;
    int ndm, ndf;
    int counter;
    FileStream theFile;
    double deltaT;
    double nextTimeStampToRecord;
};

#endif
