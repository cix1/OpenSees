
//##################################################################################
//# COPYRIGHT (C):     :-))                                                        #
//# PROJECT:           Object Oriented Finite Element Program                      #
//# PURPOSE:           General platform for elaso-plastic constitutive model       #
//#                    implementation                                              #
//# CLASS:             PotentialSurface(the base class for all potential surfaces) #
//#                                                                                #
//# VERSION:                                                                       #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )    #
//# TARGET OS:         DOS || UNIX || . . .                                        #
//# DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                 #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                 #
//#                                                                                #
//#                                                                                #
//# DATE:              08-03-2000                                                  #
//# UPDATE HISTORY:                                                                #
//#                                                                                #
//#                                                                                #
//#                                                                                #
//#                                                                                #
//# SHORT EXPLANATION: The goal is to create a platform for efficient and easy     #
//#                    implemetation of any elasto-plastic constitutive model!     #
//#                                                                                #
//##################################################################################
//

#ifndef PS_H
#define PS_H

#include <stresst.h>
#include <straint.h>
#include <BJtensor.h>

#include "EPState.h"

class PotentialSurface
{
  public:
    virtual PotentialSurface *newObj()  = 0;  //create a colne of itself
    double  q() const { return 0.0; };
    virtual tensor dQods(const EPState *EPS ) const =  0;  //pure virtual func
    virtual tensor d2Qods2(const EPState *EPS ) const = 0; //pure virtual func   
    virtual void print() = 0; //pure virtual func

    //================================================================================
    // Overloaded Insertion Operator
    // prints an PotentialSurface's contents 
    //================================================================================
    friend ostream& operator<< (ostream& os, const PotentialSurface & PS)
    {
       os << "Potential Surface Parameters: " << endln;
       return os;
    }
};


#endif
