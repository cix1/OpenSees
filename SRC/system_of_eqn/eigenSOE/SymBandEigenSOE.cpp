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
// $Date: 2001-11-19 22:44:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/SymBandEigenSOE.cpp,v $

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for
// SymBandEigenSOE, which stores a symmetric banded matrix, A, for
// standard eigenvalue computations.

#include <SymBandEigenSOE.h>
#include <SymBandEigenSolver.h>
#include <Matrix.h>
#include <Graph.h>
#include <Vertex.h>
#include <VertexIter.h>
#include <f2c.h>
#include <math.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <AnalysisModel.h>

SymBandEigenSOE::SymBandEigenSOE(SymBandEigenSolver &theSolvr,  
				 AnalysisModel &aModel)
:EigenSOE(theSolvr, EigenSOE_TAGS_SymBandEigenSOE),
 size(0), numSuperD(0), A(0), 
 Asize(0), factored(false), theModel(&aModel)
{
  theSolvr.setEigenSOE(*this);
}

int
SymBandEigenSOE::getNumEqn(void) const
{
  return size;
}
    
SymBandEigenSOE::~SymBandEigenSOE()
{
  if (A != 0)
    delete [] A;
}

int 
SymBandEigenSOE::setSize(Graph &theGraph)
{
  int result = 0;
  size = theGraph.getNumVertex();
  
  // determine the number of superdiagonals and subdiagonals
  
  numSuperD = 0;
  
  Vertex *vertexPtr;
  VertexIter &theVertices = theGraph.getVertices();
  
  while ((vertexPtr = theVertices()) != 0) {
    int vertexNum = vertexPtr->getTag();
    const ID &theAdjacency = vertexPtr->getAdjacency();
    for (int i=0; i<theAdjacency.Size(); i++) {
      int otherNum = theAdjacency(i);
      int diff = vertexNum - otherNum;
      if (diff > 0) {
	if (diff > numSuperD)
	  numSuperD = diff;
      } else 
	if (diff < -numSuperD)
	  numSuperD = -diff;
    }
  }
  
  int newSize = size*(numSuperD+1);
  if (newSize > Asize) { // we have to get another space for A
    
    if (A != 0) 
      delete [] A;
    
    A = new double[newSize];
    
    if (A == 0) {
      g3ErrorHandler->warning("%s -- ran out of memory for A, size = %d, superD = %d",
			      "SymBandEigenSOE::setSize()", size, numSuperD);
      Asize = 0; size = 0; numSuperD = 0;
      result= -1;
    }
    else  
      Asize = newSize;
  }
  
  // zero the matrix
  for (int i = 0; i < Asize; i++)
    A[i] = 0.0;
  
  factored = false;
  
  // invoke setSize() on the Solver
  EigenSolver *theSolvr = this->getSolver();
  int solverOK = theSolvr->setSize();
  if (solverOK < 0) {
    g3ErrorHandler->warning("%s -- solver failed in setSize()",
			    "SymBandEigenSOE::setSize()");
    return solverOK;
  } 
  
  cerr << "SymBandEigenSOE::setSize() -- size = " << size << ", numSuperD = "
       << numSuperD << endl;

  return result;    
}

int 
SymBandEigenSOE::addA(const Matrix &m, const ID &id, double fact)
{
  // check for a quick return 
  if (fact == 0.0)
    return 0;

  // check that m and id are of similar size
  int idSize = id.Size();    
  if (idSize != m.noRows() && idSize != m.noCols()) {
    g3ErrorHandler->warning("%s -- Matrix and ID not of similar sizes",
			    "SymBandEigenSOE::addA()");
    return -1;
  }

  if (fact == 1.0) { // do not need to multiply 
    for (int i = 0; i < idSize; i++) {
      int col = id(i);
      if (col < size && col >= 0) {
	double *coliiPtr = A +(col+1)*(numSuperD+1) - 1;
	int minColRow = col - (numSuperD+1) + 1;
	for (int j = 0; j < idSize; j++) {
	  int row = id(j);
	  if (row <size && row >= 0 && 
	      row <= col && row >= minColRow) { // only add upper
	    double *APtr = coliiPtr + (row-col);
	    *APtr += m(j,i);
	  }
	}  // for j
      } 
    }  // for i
  } else {
    for (int i = 0; i < idSize; i++) {
      int col = id(i);
      if (col < size && col >= 0) {
	double *coliiPtr = A +(col+1)*(numSuperD+1) - 1;
	int minColRow = col - (numSuperD+1) +1;
	for (int j = 0; j < idSize; j++) {
	  int row = id(j);
	  if (row < size && row >= 0 && 
	      row <= col && row >= minColRow) { // only add upper
	    double *APtr = coliiPtr + (row-col);
	    *APtr += m(j,i)*fact;
	  }
	}  // for j
      } 
    }  // for i
  }

  return 0;
}

void 
SymBandEigenSOE::zeroA(void)
{
  double *Aptr = A;
  
  for (int i = 0; i < Asize; i++)
    *Aptr++ = 0;
  
  factored = false;
}

int 
SymBandEigenSOE::addM(const Matrix &m, const ID &id, double fact)
{
  return 0;
}

void 
SymBandEigenSOE::zeroM(void)
{
  return;
}

int 
SymBandEigenSOE::sendSelf(int commitTag, Channel &theChannel)
{
  return 0;
}
    
int 
SymBandEigenSOE::recvSelf(int commitTag, Channel &theChannel, 
			FEM_ObjectBroker &theBroker)
{
  return 0;
}
