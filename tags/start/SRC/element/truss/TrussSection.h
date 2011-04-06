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
// $Date: 2000-09-15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/TrussSection.h,v $
                                                                        
                                                                        
#ifndef TrussSection_h
#define TrussSection_h

// File: ~/element/truss/TrussSection.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for Truss. A Truss 
// object provides the abstraction of the small deformation bar element. 
// Each truss object is assocaited with a section object. This Truss element 
// will work in 1d, 2d or 3d problems.
//
// What: "@(#) Truss.h, revA"

#include <Element.h>
#include <Matrix.h>

class Node;
class Channel;
class SectionForceDeformation;

class TrussSection : public Element
{
  public:
    TrussSection(int tag, 
	  int dimension,
	  int Nd1, int Nd2, 
	  SectionForceDeformation &theSection,
	  double rho=0.0);     
    
    TrussSection();    
    ~TrussSection();

    // public methods to obtain inforrmation about dof & connectivity    
    int getNumExternalNodes(void) const;
    const ID &getExternalNodes(void);
    int getNumDOF(void);	
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void);
    const Matrix &getSecantStiff(void);    
    const Matrix &getDamp(void);    
    const Matrix &getMass(void);    

    void zeroLoad(void);	
    int addLoad(const Vector &addP);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void);
    const Vector &getResistingForceIncInertia(void);            

    // public methods for element output
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    int displaySelf(Renderer &theViewer, int displayMode, float fact);    
    void Print(ostream &s, int flag =0);    
    
    int setResponse(char **argv, int argc, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    
    int setParameter (char **argv, int argc, Information &info);
    int updateParameter (int parameterID, Information &info);

  protected:
    
  private:
    double computeCurrentStrain(void) const;
    
    // private attributes - a copy for each object of the class
    ID  connectedExternalNodes;          // contains the tags of the end nodes
    int dimension;                       // truss in 2 or 3d domain
    int numDOF;	                         // number of dof for truss

    Vector *theLoad;          // pointer to the load vector P    
    Matrix *theMatrix; 	// pointer to objects matrix (one of class Matrices)
    Vector *theVector;      // pointer to objects vector (one of class Vectors)

    Matrix *t;        // hold the transformation matrix, could use a Vector

    double L;		// length of truss based on undeformed configuration
    double M; 		// rho and M*A*L/2 after setDomain()

    Node *end1Ptr;      // pointer to the end1 node object
    Node *end2Ptr;      // pointer to the end1 node object	

    SectionForceDeformation  *theSection;
    
    // static data - single copy for all objects of the class	
    static Matrix trussM2;   // class wide matrix for 2*2
    static Matrix trussM3;   // class wide matrix for 3*3	
    static Matrix trussM4;   // class wide matrix for 4*4
    static Matrix trussM6;   // class wide matrix for 6*6
    static Matrix trussM12;  // class wide matrix for 12*12
    static Vector trussV2;   // class wide Vector for size 2
    static Vector trussV3;   // class wide Vector for size 2	
    static Vector trussV4;   // class wide Vector for size 44
    static Vector trussV6;   // class wide Vector for size 6
    static Vector trussV12;  // class wide Vector for size 12
};

#endif



