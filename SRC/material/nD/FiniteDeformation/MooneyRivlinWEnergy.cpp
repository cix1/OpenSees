//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:             
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              19AUg2003
//# UPDATE HISTORY:   
//#
//#
//===============================================================================


#ifndef MooneyRivlinWEnergy_CPP
#define MooneyRivlinWEnergy_CPP

#include <math.h>
#include <MooneyRivlinWEnergy.h>

//================================================================================
// Normal constructor
//================================================================================
MooneyRivlinWEnergy::MooneyRivlinWEnergy( double c1_in,  double c2_in)
{
        c1 = c1_in;
        c2 = c2_in;
}

//MooneyRivlinWEnergy::MooneyRivlinWEnergy( )
//{
//
//}

//================================================================================
// Normal destructor
//================================================================================
MooneyRivlinWEnergy::~MooneyRivlinWEnergy( )
{

}

//================================================================================
//create a clone of itself
//================================================================================
WEnergy * MooneyRivlinWEnergy::newObj()
  {
    MooneyRivlinWEnergy  *new_WEnergy = new MooneyRivlinWEnergy (c1, c2);
    return new_WEnergy;
  }

//================================================================================
// w
//================================================================================
const double MooneyRivlinWEnergy::wE(const double &J_in, const Vector &lambda_wave_in)
  {
    double temp1 = lambda_wave_in(0) * lambda_wave_in(0)
                           + lambda_wave_in(1) * lambda_wave_in(1)
                           + lambda_wave_in(2) * lambda_wave_in(2) - 3.0;
    double temp2 = 1.0 / lambda_wave_in(0) / lambda_wave_in(0)
                           + 1.0 / lambda_wave_in(1) / lambda_wave_in(1)
                           + 1.0 / lambda_wave_in(2) / lambda_wave_in(2) - 3.0;
    double w_iso = c1 * temp1 + c2 * temp2;
    return w_iso;
  }
  
//================================================================================
// d(iso)w / d(lambda)
//================================================================================
const Vector MooneyRivlinWEnergy::disowOdlambda(const Vector &lambda_wave_in)
  {
        Vector disowOverdlambda(3);
        disowOverdlambda(0) = 2 * c1 *lambda_wave_in(0) - 2.0 * c2 * pow(lambda_wave_in(0), -3);
        disowOverdlambda(1) = 2 * c1 *lambda_wave_in(1) - 2.0 * c2 * pow(lambda_wave_in(1), -3);
        disowOverdlambda(2) = 2 * c1 *lambda_wave_in(2) - 2.0 * c2 * pow(lambda_wave_in(2), -3);
    return disowOverdlambda;
  }

//================================================================================
// d2(iso)w / d(lambda)2
//================================================================================
const Vector MooneyRivlinWEnergy::d2isowOdlambda2(const Vector &lambda_wave_in)
  {
        Vector d2isowOverdlambda2(3);
        d2isowOverdlambda2(0) = 2 * c1  - 6.0 * c2 * pow(lambda_wave_in(0), -4);
        d2isowOverdlambda2(1) = 2 * c1  - 6.0 * c2 * pow(lambda_wave_in(1), -4);
        d2isowOverdlambda2(2) = 2 * c1  - 6.0 * c2 * pow(lambda_wave_in(2), -4);
    return d2isowOverdlambda2;
  }

//================================================================================
// friend ostream functions for output
//================================================================================
////OPS_Stream & operator<< (OPS_Stream & os, const MooneyRivlinWEnergy & W)
////{
////    os << "MooneyRivlin Strain Energy Function: " << endln;
////    return os;
////}

#endif

