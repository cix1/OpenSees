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
// $Date: 2000-09-15 08:23:15 $
// $Source: /usr/local/cvs/OpenSees/SRC/G3Globals.h,v $
                                                                        
                                                                        
#ifndef G3Globals_h
#define G3Globals_h

// File: ~/G3Globals.h
//
// Written: fmk 
// Created: 11/99
// Revision: A
//
// Description: This file contains global variables used in G3 files
// if you change a variable, you must recompile the code

#define MAX_FILENAMELENGTH 50
//#define _G3DEBUG 

#include <ErrorHandler.h>
extern ErrorHandler *g3ErrorHandler;

#endif
