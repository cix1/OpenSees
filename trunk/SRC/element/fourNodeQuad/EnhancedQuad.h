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
                                                                        
// $Revision: 1.5 $
// $Date: 2001-11-26 22:53:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/EnhancedQuad.h,v $
                                                                        
#include <iostream.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 

#include <ID.h> 
#include <Vector.h>
#include <Matrix.h>
#include <Element.h>
#include <Node.h>
#include <NDMaterial.h>

class EnhancedQuad : public Element {

  public:

  //full constructor
    EnhancedQuad(int tag, 
		 int nd1, 
		 int nd2, 
		 int nd3, 
		 int nd4,
		 NDMaterial &theMaterial, 
		 const char *type ) ;

    //null constructor
    EnhancedQuad();

    //destructor
    virtual ~EnhancedQuad();



    //set domain
    void setDomain( Domain *theDomain ) ;

    //get the number of external nodes
    int getNumExternalNodes( ) const ;
 
    //return connected external nodes
    const ID &getExternalNodes( ) ;

    //return number of dofs
    int getNumDOF( ) ;

    //commit state
    int commitState( ) ;
    
    //revert to last commit 
    int revertToLastCommit( ) ;
    
    //revert to start 
    int revertToStart( ) ;

    //print out element data
    void Print( ostream &s, int flag ) ;
	
    //return stiffness matrix 
    const Matrix &getTangentStiff( ) ;
    
    //return secant matrix 
    const Matrix &getSecantStiff( ) ;
    
    //return damping matrix
    const Matrix &getDamp( ) ;
    
    //return mass matrix
    const Matrix &getMass( ) ;

    //zero the load -- what load?
    void zeroLoad( ) ;
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual
    const Vector &getResistingForce( ) ;
    
    //get residual with inertia terms
    const Vector &getResistingForceIncInertia( ) ;

    // public methods for element output
    int sendSelf (int commitTag, Channel &theChannel);
    int recvSelf (int commitTag, Channel &theChannel, FEM_ObjectBroker 
		  &theBroker);
      
    //plotting 
    int displaySelf(Renderer &theViewer, int displayMode, float fact);
    
  private:

    //static data
    static Matrix stiff ;
    static Vector resid ;
    static Matrix mass ;
    static Matrix damping ;

    //quadrature data
    static const double root3 ;
    static const double one_over_root3 ;    
    static const double sg[4] ;
    static const double tg[4] ;
    static const double wg[4] ;

    //stress data
    static double stressData[][4] ;

    //tangent data 
    static double tangentData[][3][4] ;

    //node information
    ID connectedExternalNodes ;  //four node numbers
    Node *nodePointers[4] ;      //pointers to four nodes

    //enhanced strain parameters
    Vector alpha ;

    //material information
    NDMaterial *materialPointers[4] ; //pointers to four materials
					  
    //local nodal coordinates, two coordinates for each of four nodes
    //    static double xl[2][4] ; 
    static double xl[][4] ; 

    //save stress and tangent data
    void saveData( int gp, 
		   const Vector &stress,
		   const Matrix &tangent ) ;

    //recover stress and tangent data
    void getData( int gp,
		  Vector &stress,
		  Matrix &tangent ) ;

    //compute enhanced strain B-matrices
    const Matrix& computeBenhanced( int node, 
			     double L1,
			     double L2,
			     double j, 
			     const Matrix &Jinv ) ;

			   
    //compute local coordinates and basis
    void computeBasis( ) ;
        
    //form residual and tangent					  
    void formResidAndTangent( int tang_flag ) ;

    //inertia terms
    void formInertiaTerms( int tangFlag ) ;


    //compute Jacobian matrix and inverse at point {L1,L2}
    void  computeJacobian( double L1, double L2, 
			   const double x[2][4], 
                           Matrix &JJ, 
                           Matrix &JJinv ) ;

    //compute Bbend matrix
    const Matrix& computeB( int node, const double shp[3][4] ) ;

    //Matrix transpose of a 3x2 matrix
    const Matrix& transpose( const Matrix &M ) ;

    //shape function routine for four node quads
    void shape2d( double ss, double tt, 
		  const double x[2][4], 
		  double shp[3][4], 
		  double &xsj ) ;

    Vector *load;
} ; 


