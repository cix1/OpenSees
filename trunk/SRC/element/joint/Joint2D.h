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
// $Date: 2002-12-05 22:20:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/joint/Joint2D.h,v $

// Written: AAA 03/02
// Revised:

// Joint2D.h: interface for the Joint2d class.
//
//////////////////////////////////////////////////////////////////////

#ifndef Joint2D_h
#define Joint2D_h

#include <bool.h>
#include <Node.h>
#include <Matrix.h>
#include <Vector.h>
#include <Element.h>
#include <ID.h>
#include <Domain.h>

class UniaxialMaterial;
class Response;

class Joint2D : public Element  
{
public:
  Joint2D();

  Joint2D(int tag, int nd1, int nd2, int nd3, int nd4, int IntNodeTag,
			     UniaxialMaterial &spring1,
				 UniaxialMaterial &spring2,
			     UniaxialMaterial &spring3,
				 UniaxialMaterial &spring4,
			     UniaxialMaterial &springC,
				 Domain *theDomain,
				 int LrgDisp);
	
  ~Joint2D();

  // methods dealing with domain
  int	getNumExternalNodes(void) const;
  const	ID &getExternalNodes(void);
  Node **getNodePtrs(void);

  int	getNumDOF(void);
  void	setDomain(Domain *theDomain);  
  bool	isSubdomain(void) { return false; } ;
	
  // methods dealing with committed state and update
  int update(void);
  int commitState(void);
  int revertToLastCommit(void);
  int revertToStart(void);

  // methods to return the current linearized stiffness,
  // damping and mass matrices
  const	Matrix &getTangentStiff(void);
  const	Matrix &getInitialStiff(void);
	
  // methods for returning and applying loads
  //virtual Vector &getUVLoadVector(double q1, double q2);
  void	zeroLoad(void); 
  int addLoad(ElementalLoad *theLoad, double loadFactor);
  int addInertiaLoadToUnbalance(const Vector &accel);

  const	Vector &getResistingForce(void);
  const	Vector &getResistingForceIncInertia(void);     

  // method for graphics
  int	displaySelf(Renderer &theViewer, int displayMode, float fact);  
	
  // method for obtaining information specific to an element
  Response* setResponse(char **argv, int argc, Information &eleInformation);
  int getResponse(int responseID, Information &eleInformation);
  int sendSelf(int commitTag, Channel &theChannel) {return -1;}
  int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker) {return -1;}
  void Print(ostream &s, int flag =0);

 protected:
  int 	addMP_Joint(Domain *theDomain, int mpNum, int RnodeID, int CnodeID, int MainDOF, int LrgDispFlag );   

 private:
  UniaxialMaterial *Spring1; 
  UniaxialMaterial *Spring2; 
  UniaxialMaterial *Spring3; 
  UniaxialMaterial *Spring4; 
  UniaxialMaterial *SpringC; 
  ID		ExternalNodes, InternalConstraints;	
  Node	*end1Ptr, *end2Ptr, *end3Ptr, *end4Ptr, *IntNodePtr;
  int		IntNode;
  Domain	*TheDomain;
  int		numDof, nodeRecord, dofRecord;
  static	Matrix K;
  static	Vector V;

  static Node *theNodes[5];
};

#endif
