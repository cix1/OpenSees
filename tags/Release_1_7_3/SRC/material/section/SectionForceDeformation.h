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
                                                                        
// $Revision: 1.10 $
// $Date: 2006-08-03 23:49:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionForceDeformation.h,v $
                                                                        
                                                                        
#ifndef SectionForceDeformation_h
#define SectionForceDeformation_h

// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class definition for SectionForceDeformation.
// SectionForceDeformation is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) SectionForceDeformation.h, revA"

#include <Material.h>
#include <Matrix.h>
#include <Vector.h>
#include <ID.h>

class Information;
class Response;

#define MAX_SECTION_RESPONSE_ID 10000

#define SECTION_RESPONSE_MZ		1
#define SECTION_RESPONSE_P		2
#define SECTION_RESPONSE_VY		3
#define SECTION_RESPONSE_MY		4
#define SECTION_RESPONSE_VZ		5
#define SECTION_RESPONSE_T		6	

class SectionForceDeformation : public Material
{
 public:
  SectionForceDeformation (int tag, int classTag);
  SectionForceDeformation ();
  virtual ~SectionForceDeformation ();
  
  virtual int setTrialSectionDeformation (const Vector&) = 0;
  virtual const Vector &getSectionDeformation (void) = 0;
  
  virtual const Vector &getStressResultant (void) = 0;
  virtual const Matrix &getSectionTangent (void) = 0;
  virtual const Matrix &getInitialTangent (void) = 0;
  virtual const Matrix &getSectionFlexibility (void);
  virtual const Matrix &getInitialFlexibility (void);
  
  virtual double getRho(void);
  
  virtual int commitState (void) = 0;
  virtual int revertToLastCommit (void) = 0;
  virtual int revertToStart (void) = 0;
  
  virtual SectionForceDeformation *getCopy (void) = 0;
  virtual const ID &getType (void) = 0;
  virtual int getOrder (void) const = 0;
  
  virtual Response *setResponse(const char **argv, int argc, Information &info, OPS_Stream &s);
  virtual int getResponse(int responseID, Information &info);
  
  // AddingSensitivity:BEGIN //////////////////////////////////////////
  virtual int            setParameter                     (const char **argv, int argc, Information &info);
  virtual int            updateParameter                  (int parameterID, Information &info);
  virtual int            activateParameter                (int parameterID);
  virtual const Vector & getStressResultantSensitivity    (int gradNumber, bool conditional);
  virtual const Vector & getSectionDeformationSensitivity (int gradNumber);
  virtual const Matrix & getSectionTangentSensitivity     (int gradNumber);
  virtual double         getRhoSensitivity                (int gradNumber);
  virtual int            commitSensitivity                (const Vector& sectionDeformationGradient, int gradNumber, int numGrads);
  // AddingSensitivity:END ///////////////////////////////////////////
  
  
 protected:
  Matrix *fDefault;	// Default flexibility matrix
  
 private:
};


#endif