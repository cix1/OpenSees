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
                                                                        
// $Revision: 1.4 $
// $Date: 2002-06-07 18:01:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/elasticBeamColumn/ElasticBeam3d.h,v $
                                                                        
                                                                        
// File: ~/model/ElasticBeam3d.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for ElasticBeam3d.
// ElasticBeam3d is a plane frame member.

#ifndef ElasticBeam3d_h
#define ElasticBeam3d_h

#include <Element.h>
#include <Node.h>
#include <Matrix.h>
#include <Vector.h>

class Channel;
class Information;
class CrdTransf3d;
class Response;
class Renderer;

class ElasticBeam3d : public Element
{
  public:
    ElasticBeam3d();        
    ElasticBeam3d(int tag, double A, double E, double G, 
		  double Jx, double Iy, double Iz, int Nd1, int Nd2,
		  CrdTransf3d &theTransf, double rho = 0.0);
    ~ElasticBeam3d();

    int getNumExternalNodes(void) const;
    const ID &getExternalNodes(void);
    int getNumDOF(void);
    void setDomain(Domain *theDomain);
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    
    const Matrix &getTangentStiff(void);
    const Matrix &getDamp(void);    
    const Matrix &getMass(void);    

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void);
    const Vector &getResistingForceIncInertia(void);            
    
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    
    void Print(ostream &s, int flag =0);    
    int displaySelf(Renderer &theViewer, int displayMode, float fact);

    Response *setResponse (char **argv, int argc, Information &info);
    int getResponse (int responseID, Information &info);
 
  private:
    double A,E,G,Jx,Iy,Iz;

    double rho;
    
    static Matrix K;
    static Vector P;
    Vector Q;
    
    static Matrix kb;
    Vector q;
    double q0[5];  // Fixed end forces in basic system (no torsion)
    double p0[5];  // Reactions in basic system (no torsion)
 
    Node *node1Ptr, *node2Ptr;
    
    ID  connectedExternalNodes;    

    CrdTransf3d *theCoordTransf;
};

#endif


