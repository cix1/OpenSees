/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2001-06-14 08:06:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/UniformRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) during Spring 2000
// Revised: haukaas 06/00 (core code)
//			haukaas 06/01 (made part of official OpenSees)
//

#include <UniformRV.h>
#include <math.h>

UniformRV::UniformRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, passedMean, passedStdv, passedStartValue)
{
	type = new char[100];
	strcpy(type,"UNIFORM");
	tag = passedTag;
	a = passedMean - sqrt(3.0)*passedStdv;
	b = passedMean + sqrt(3.0)*passedStdv;
	startValue = passedStartValue;
}
UniformRV::UniformRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, 
				passedParameter1, 
				passedParameter2, 
				passedParameter3, 
				passedParameter4, 
				passedStartValue)
{
	type = new char[100];
	strcpy(type,"UNIFORM");
	tag = passedTag ;
	a = passedParameter1;
	b = passedParameter2;
	startValue = passedStartValue;
}
UniformRV::UniformRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, passedMean, passedStdv)
{
	type = new char[100];
	strcpy(type,"UNIFORM");
	tag = passedTag;
	a = passedMean - sqrt(3.0)*passedStdv;
	b = passedMean + sqrt(3.0)*passedStdv;
	startValue = getMean();
}
UniformRV::UniformRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, 
				passedParameter1, 
				passedParameter2, 
				passedParameter3, 
				passedParameter4)
{
	type = new char[100];
	strcpy(type,"UNIFORM");
	tag = passedTag ;
	a = passedParameter1;
	b = passedParameter2;
	startValue = getMean();
}


UniformRV::~UniformRV()
{
}


void
UniformRV::Print(ostream &s, int flag)
{
}


double
UniformRV::getPDFvalue(double rvValue)
{
	double result;
	if ( rvValue >= a && rvValue <= b ) {
		result = 1/(b-a);
	}
	else {
		result = 0.0;
	}
	return result;
}


double
UniformRV::getCDFvalue(double rvValue)
{
	double result;
	if ( rvValue >= a && rvValue <= b ) {
		result = (rvValue-a)/(b-a);
	}
	else if (rvValue > b) {
		result = 1.0;
	}
	else {
		result = 0.0;
	}
	return result;
}


double
UniformRV::getInverseCDFvalue(double probValue)
{
	return probValue * b - probValue * a + a;
}


char *
UniformRV::getType()
{
	return type;
}


double 
UniformRV::getMean()
{
	return (a+b)/2.0;
}



double 
UniformRV::getStdv()
{
	return (b-a)/(2.0*sqrt(3.0));
}


double 
UniformRV::getStartValue()
{
	return startValue;
}


double UniformRV::getParameter1()  {return a;}
double UniformRV::getParameter2()  {return b;}
double UniformRV::getParameter3()  {cerr<<"No such parameter in r.v. #"<<tag<<endl; return 0.0;}
double UniformRV::getParameter4()  {cerr<<"No such parameter in r.v. #"<<tag<<endl; return 0.0;}
