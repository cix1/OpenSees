///////////////////////////////////////////////////////////////////////////////
//
// COPYRIGHT (C):     :-))
// PROJECT:           Object Oriented Finite Element Program
// FILE:              TwentyNodeBrick.cpp
// CLASS:             TwentyNodeBrick
// MEMBER FUNCTIONS:
//
// MEMBER VARIABLES
//
// PURPOSE:           Finite Element Class
// RETURN:
// VERSION:
// LANGUAGE:          C++
// TARGET OS:         DOS || UNIX || . . .
// DESIGNER:          Boris Jeremic, Zhaohui Yang and Xiaoyan Wu
// PROGRAMMER:        Boris Jeremic, Zhaohui Yang  and Xiaoyan Wu
// DATE:              Aug. 2001
// UPDATE HISTORY:
//
//
//
///////////////////////////////////////////////////////////////////////////////
//

#ifndef TWENTYNODEBRICK_CPP
#define TWENTYNODEBRICK_CPP

#include <NDMaterial.h>
#include <Matrix.h>
#include <Vector.h>
#include <ID.h>
#include <Renderer.h>
#include <Domain.h>
#include <string.h>
#include <Information.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <ElementResponse.h>

#include "TwentyNodeBrick.h"
#define FixedOrder 3

//====================================================================
// Constructor
//====================================================================

TwentyNodeBrick::TwentyNodeBrick(int element_number,
                               int node_numb_1,  int node_numb_2,  int node_numb_3,  int node_numb_4,
                               int node_numb_5,  int node_numb_6,  int node_numb_7,  int node_numb_8,
                               int node_numb_9,  int node_numb_10, int node_numb_11, int node_numb_12,
                               int node_numb_13, int node_numb_14, int node_numb_15, int node_numb_16,
                               int node_numb_17, int node_numb_18, int node_numb_19, int node_numb_20,
                               NDMaterial * Globalmmodel, double b1, double b2,double b3,
			       double r, double p)
			       // int dirp, double surflevelp)
			       //, EPState *InitEPS)  const char * type,
                               // Set it to 3 //int r_int_order, //int s_int_order, //int t_int_order,
			       //tensor * IN_tangent_E,  //stresstensor * INstress, //stresstensor * INiterative_stress, //double * IN_q_ast_iterative, //straintensor * INstrain):  __ZHaohui 09-29-2000

  :Element(element_number, ELE_TAG_TwentyNodeBrick ),
  connectedExternalNodes(20), K(60, 60), C(60, 60), M(60, 60), P(60),Q(60), bf(3),
  rho(r), pressure(p)
  {
    //elem_numb = element_number;
    bf(0) = b1;
    bf(1) = b2;
    bf(2) = b3;

    determinant_of_Jacobian = 0.0;

    //r_integration_order = r_int_order;
    //s_integration_order = s_int_order;
    //t_integration_order = t_int_order;
    r_integration_order = FixedOrder; // Gauss-Legendre integration order in r direction
    s_integration_order = FixedOrder; // Gauss-Legendre integration order in s direction
    t_integration_order = FixedOrder; // Gauss-Legendre integration order in t direction

    //Not needed. Right now we have one NDMaterial for each material point
    //mmodel = Globalmmodel->getCopy( type ); // One global mat model

    int total_number_of_Gauss_points = r_integration_order*s_integration_order*t_integration_order;


    if ( total_number_of_Gauss_points != 0 )
      {
       	matpoint  = new MatPoint3D * [total_number_of_Gauss_points];
      }
    else
      {
        matpoint  = 0;
      }
    ////////////////////////////////////////////////////////////////////
    short where = 0;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        double r = get_Gauss_p_c( r_integration_order, GP_c_r );
        double rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            double s = get_Gauss_p_c( s_integration_order, GP_c_s );
            double sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                double t = get_Gauss_p_c( t_integration_order, GP_c_t );
                double tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                //DB::printf("\n\nBefore Initialization **************** where = %d \n",where);
                //DB::printf("GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //DB            GP_c_r,GP_c_s,GP_c_t);
                //DB
                //DBGPstress[where].reportshort("stress within before Initialization");
                //DBGPstrain[where].reportshort("strain within before Initialization");
                //DB
                //DB// I suspect that [] should be overloaded so that compiler knows which
                //DB// material model is returning a pointer and fot the purpose
                //DB//matpoint[where].report("mmodel within before Initialization");
                //DB//matpoint[where].report("mmodel within before Initialization"); // operator[] overloaded
                //DB(matpoint)->operator[](where).report("mmodel within before Initialization"); // operator[] overloaded
                //DB                                                               // for NDMaterial and
                //DB                                                               // derived types!

		              matpoint[where] = new MatPoint3D(GP_c_r,
                                                 GP_c_s,
                                                 GP_c_t,
                                                 r, s, t,
                                                 rw, sw, tw,
                                               //InitEPS,
					                                            Globalmmodel);
					 //NMD);
					 //&( GPstress[where] ), //&( GPiterative_stress[where] ), //IN_q_ast_iterative[where] ,//&( GPstrain[where] ),  //&( GPtangent_E[where] ),
                                         //&( (matpoint)->operator[](where) )
                                         // ugly syntax but it works! Still don't know what's wrong   // with the old style matpoint[where]
              }
          }
      }

      // Set connected external node IDs
      connectedExternalNodes( 0) = node_numb_1;
      connectedExternalNodes( 1) = node_numb_2;
      connectedExternalNodes( 2) = node_numb_3;
      connectedExternalNodes( 3) = node_numb_4;
      connectedExternalNodes( 4) = node_numb_5;
      connectedExternalNodes( 5) = node_numb_6;
      connectedExternalNodes( 6) = node_numb_7;
      connectedExternalNodes( 7) = node_numb_8;
			      
      connectedExternalNodes( 8) = node_numb_9;
      connectedExternalNodes( 9) = node_numb_10;
      connectedExternalNodes(10) = node_numb_11;
      connectedExternalNodes(11) = node_numb_12;

      connectedExternalNodes(12) = node_numb_13;
      connectedExternalNodes(13) = node_numb_14;
      connectedExternalNodes(14) = node_numb_15;
      connectedExternalNodes(15) = node_numb_16;

      connectedExternalNodes(16) = node_numb_17;
      connectedExternalNodes(17) = node_numb_18;
      connectedExternalNodes(18) = node_numb_19;
      connectedExternalNodes(19) = node_numb_20;

      nodes_in_brick = 20;

}

//====================================================================
TwentyNodeBrick::TwentyNodeBrick ():Element(0, ELE_TAG_TwentyNodeBrick ),
connectedExternalNodes(20), K(60, 60), C(60, 60), M(60, 60), P(60),Q(60), bf(3), rho(0.0), pressure(0.0), mmodel(0)
{
     matpoint = 0;
}


//#############################################################################


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TwentyNodeBrick::~TwentyNodeBrick ()
{

    int total_number_of_Gauss_points = r_integration_order*s_integration_order*t_integration_order;

    for (int i = 0; i < total_number_of_Gauss_points; i++)
    {
	// Delete the NDMaterials at each integration point
	if (matpoint[i])
	   delete matpoint[i];
    }

    // Delete the array of pointers to NDMaterial pointer arrays
    if (matpoint)
    	delete [] matpoint;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TwentyNodeBrick::incremental_Update()
  {
    double r  = 0.0;
    // double rw = 0.0;
    double s  = 0.0;
    // double sw = 0.0;
    double t  = 0.0;
    // double tw = 0.0;

    short where = 0;
    //,,,,,    double weight = 0.0;

    //double this_one_PP = (matpoint)->operator[](where).IS_Perfect_Plastic();

    int dh_dim[] = {20,3};
    tensor dh(2, dh_dim, 0.0);


    static int disp_dim[] = {20,3};
    tensor incremental_displacements(2,disp_dim,0.0);

    straintensor incremental_strain;

    tensor Jacobian;
    tensor JacobianINV;
    tensor dhGlobal;

    incremental_displacements = incr_disp();

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        //--        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            //--            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
            {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                //--                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);
                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");
                // Inverse of Jacobian tensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");
                // determinant of Jacobian tensor ( matrix )
                //--                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal = dh("ij") * JacobianINV("jk");
                //....                dhGlobal.print("dh","dhGlobal");
                //weight
                //                weight = rw * sw * tw * det_of_Jacobian;
                //::::::   ::printf("\n\nIN THE STIFFNESS TENSOR INTEGRATOR ----**************** where = %d \n", where);
                //::::::   ::printf(" void TwentyNodeBrick::incremental_Update()\n");
                //::::::   ::printf(" GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d    --->>>  where = %d \n",
                //::::::                      GP_c_r,GP_c_s,GP_c_t,where);
                //::::::   ::printf("WEIGHT = %f", weight);
                //::::::   ::printf("determinant of Jacobian = %f", determinant_of_Jacobian);
                //::::::   matpoint[where].report("Gauss Point\n");
                // incremental straines at this Gauss point
                // now in Update we know the incremental displacements so let's find
                // the incremental strain
                incremental_strain =
                    (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                incremental_strain.null_indices();
                //incremental_strain.reportshort("\n incremental_strain tensor at GAUSS point\n");

                // here comes the final_stress calculation actually on only needs to copy stresses
                // from the iterative data . . .
                //(GPstress+where)->reportshortpqtheta("\n stress START GAUSS \n");

		if ( ! ( (matpoint[where]->matmodel)->setTrialStrainIncr( incremental_strain)) )
               	   g3ErrorHandler->warning("TwentyNodeBrick::incremental_Update (tag: %d), not converged",
		 		 this->getTag());
		//matpoint[where].setEPS( mmodel->getEPS() );
            }
          }
      }
  }


//#############################################################################
//#############################################################################
//***************************************************************
tensor TwentyNodeBrick::H_3D(double r1, double r2, double r3)
  {

    int dimension[] = {60,3};

    tensor H(2, dimension, 0.0);

    // influence of the node number 20
        H.val(58,1)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
        H.val(59,2)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
        H.val(60,3)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
    // influence of the node number 19
        H.val(55,1)=(1.0-r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
        H.val(56,2)=(1.0-r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
        H.val(57,3)=(1.0-r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
    // influence of the node number 18
        H.val(52,1)=(1.0-r1)*(1.0+r2)*(1.0-r3*r3)/4.0;
        H.val(53,2)=(1.0-r1)*(1.0+r2)*(1.0-r3*r3)/4.0;
        H.val(54,3)=(1.0-r1)*(1.0+r2)*(1.0-r3*r3)/4.0;
    // influence of the node number 17
        H.val(49,1)=(1.0+r1)*(1.0+r2)*(1.0-r3*r3)/4.0;
        H.val(50,2)=(1.0+r1)*(1.0+r2)*(1.0-r3*r3)/4.0;
        H.val(51,3)=(1.0+r1)*(1.0+r2)*(1.0-r3*r3)/4.0;

    // influence of the node number 16
        H.val(46,1)=(1.0+r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
        H.val(47,2)=(1.0+r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
        H.val(48,3)=(1.0+r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
    // influence of the node number 15
        H.val(43,1)=(1.0-r1*r1)*(1.0-r2)*(1.0-r3)/4.0;
        H.val(44,2)=(1.0-r1*r1)*(1.0-r2)*(1.0-r3)/4.0;
        H.val(45,3)=(1.0-r1*r1)*(1.0-r2)*(1.0-r3)/4.0;
    // influence of the node number 14
        H.val(40,1)=(1.0-r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
        H.val(41,2)=(1.0-r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
        H.val(42,3)=(1.0-r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
    // influence of the node number 13
        H.val(37,1)=(1.0-r1*r1)*(1.0+r2)*(1.0-r3)/4.0;
        H.val(38,2)=(1.0-r1*r1)*(1.0+r2)*(1.0-r3)/4.0;
        H.val(39,3)=(1.0-r1*r1)*(1.0+r2)*(1.0-r3)/4.0;

    // influence of the node number 12
        H.val(34,1)=(1.0+r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
        H.val(35,2)=(1.0+r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
        H.val(36,3)=(1.0+r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
    // influence of the node number 11
        H.val(31,1)=(1.0-r1*r1)*(1.0-r2)*(1.0+r3)/4.0;
        H.val(32,2)=(1.0-r1*r1)*(1.0-r2)*(1.0+r3)/4.0;
        H.val(33,3)=(1.0-r1*r1)*(1.0-r2)*(1.0+r3)/4.0;
    // influence of the node number 10
        H.val(28,1)=(1.0-r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
        H.val(29,2)=(1.0-r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
        H.val(30,3)=(1.0-r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
    // influence of the node number 9
        H.val(25,1)=(1.0-r1*r1)*(1.0+r2)*(1.0+r3)/4.0;
        H.val(26,2)=(1.0-r1*r1)*(1.0+r2)*(1.0+r3)/4.0;
        H.val(27,3)=(1.0-r1*r1)*(1.0+r2)*(1.0+r3)/4.0;


    // 9-20 nodes

    // influence of the node number 8
    H.val(22,1)=(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0 - (H.val(43,1)+H.val(48,3)+H.val(60,3))/2.0;
    H.val(23,2)=(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0 - (H.val(43,1)+H.val(48,3)+H.val(60,3))/2.0;
    H.val(24,3)=(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0 - (H.val(43,1)+H.val(48,3)+H.val(60,3))/2.0;
    // influence of the node number 7
    H.val(19,1)=(1.0-r1)*(1.0-r2)*(1.0-r3)/8.0 - (H.val(42,3)+H.val(43,1)+H.val(57,3))/2.0;
    H.val(20,2)=(1.0-r1)*(1.0-r2)*(1.0-r3)/8.0 - (H.val(42,3)+H.val(43,1)+H.val(57,3))/2.0;
    H.val(21,3)=(1.0-r1)*(1.0-r2)*(1.0-r3)/8.0 - (H.val(42,3)+H.val(43,1)+H.val(57,3))/2.0;
    // influence of the node number 6
    H.val(16,1)=(1.0-r1)*(1.0+r2)*(1.0-r3)/8.0 - (H.val(39,3)+H.val(42,3)+H.val(54,3))/2.0;
    H.val(17,2)=(1.0-r1)*(1.0+r2)*(1.0-r3)/8.0 - (H.val(39,3)+H.val(42,3)+H.val(54,3))/2.0;
    H.val(18,3)=(1.0-r1)*(1.0+r2)*(1.0-r3)/8.0 - (H.val(39,3)+H.val(42,3)+H.val(54,3))/2.0;
    // influence of the node number 5
    H.val(13,1)=(1.0+r1)*(1.0+r2)*(1.0-r3)/8.0 - (H.val(39,3)+H.val(48,3)+H.val(51,3))/2.0;
    H.val(14,2)=(1.0+r1)*(1.0+r2)*(1.0-r3)/8.0 - (H.val(39,3)+H.val(48,3)+H.val(51,3))/2.0;
    H.val(15,3)=(1.0+r1)*(1.0+r2)*(1.0-r3)/8.0 - (H.val(39,3)+H.val(48,3)+H.val(51,3))/2.0;

    // influence of the node number 4
    H.val(10,1)=(1.0+r1)*(1.0-r2)*(1.0+r3)/8.0 - (H.val(33,3)+H.val(36,3)+H.val(60,3))/2.0;
    H.val(11,2)=(1.0+r1)*(1.0-r2)*(1.0+r3)/8.0 - (H.val(33,3)+H.val(36,3)+H.val(60,3))/2.0;
    H.val(12,3)=(1.0+r1)*(1.0-r2)*(1.0+r3)/8.0 - (H.val(33,3)+H.val(36,3)+H.val(60,3))/2.0;
    // influence of the node number 3
    H.val(7,1) =(1.0-r1)*(1.0-r2)*(1.0+r3)/8.0 - (H.val(30,3)+H.val(33,3)+H.val(57,3))/2.0;
    H.val(8,2) =(1.0-r1)*(1.0-r2)*(1.0+r3)/8.0 - (H.val(30,3)+H.val(33,3)+H.val(57,3))/2.0;
    H.val(9,3) =(1.0-r1)*(1.0-r2)*(1.0+r3)/8.0 - (H.val(30,3)+H.val(33,3)+H.val(57,3))/2.0;
    // influence of the node number 2
    H.val(4,1) =(1.0-r1)*(1.0+r2)*(1.0+r3)/8.0 - (H.val(30,3)+H.val(54,3)+H.val(27,3))/2.0;
    H.val(5,2) =(1.0-r1)*(1.0+r2)*(1.0+r3)/8.0 - (H.val(30,3)+H.val(54,3)+H.val(27,3))/2.0;
    H.val(6,3) =(1.0-r1)*(1.0+r2)*(1.0+r3)/8.0 - (H.val(30,3)+H.val(54,3)+H.val(27,3))/2.0;
    // influence of the node number 1
    H.val(1,1) =(1.0+r1)*(1.0+r2)*(1.0+r3)/8.0 - (H.val(36,3)+H.val(51,3)+H.val(27,3))/2.0;
    H.val(2,2) =(1.0+r1)*(1.0+r2)*(1.0+r3)/8.0 - (H.val(36,3)+H.val(51,3)+H.val(27,3))/2.0;
    H.val(3,3) =(1.0+r1)*(1.0+r2)*(1.0+r3)/8.0 - (H.val(36,3)+H.val(51,3)+H.val(27,3))/2.0;

    //         double sum = 0;
    //
    // 	for (int i=1; i<=60 ; i++)
    //           {
    // //  	    sum+=H.cval(i,1);
    // 	    for (int j=1; j<= 1; j++)
    // 	       {
    //        	          sum+=H.cval(i,1);
    // 	          ::printf( "  %+9.2e", H.cval(i,j) );
    // 	        }
    //            // ::printf( "  %d \n", i);
    // 	   }
    // 	    ::printf( " \n sum= %+6.2e\n", sum );


    //    printf("r1 = %lf, r2 = %lf, r3 = %lf\n", r1, r2, r3);
    //    H.print("h");

    return H;
  }

//#############################################################################
//***************************************************************
tensor TwentyNodeBrick::interp_poli_at(double r1, double r2, double r3)
  {

    int dimension[] = {20};  // Xiaoyan changed from {20} to {8} for 8 nodes 07/12
    tensor h(1, dimension, 0.0);


    // influence of the node number 20
        h.val(20)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
    // influence of the node number 19
        h.val(19)=(1.0-r1)*(1.0-r2)*(1.0-r3*r3)/4.0;
    // influence of the node number 18
        h.val(18)=(1.0-r1)*(1.0+r2)*(1.0-r3*r3)/4.0;
    // influence of the node number 17
        h.val(17)=(1.0+r1)*(1.0+r2)*(1.0-r3*r3)/4.0;

    // influence of the node number 16
        h.val(16)=(1.0+r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
    // influence of the node number 15
        h.val(15)=(1.0-r1*r1)*(1.0-r2)*(1.0-r3)/4.0;
    // influence of the node number 14
        h.val(14)=(1.0-r1)*(1.0-r2*r2)*(1.0-r3)/4.0;
    // influence of the node number 13
        h.val(13)=(1.0-r1*r1)*(1.0+r2)*(1.0-r3)/4.0;

    // influence of the node number 12
        h.val(12)=(1.0+r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
    // influence of the node number 11
        h.val(11)=(1.0-r1*r1)*(1.0-r2)*(1.0+r3)/4.0;
    // influence of the node number 10
        h.val(10)=(1.0-r1)*(1.0-r2*r2)*(1.0+r3)/4.0;
    // influence of the node number 9
        h.val( 9)=(1.0-r1*r1)*(1.0+r2)*(1.0+r3)/4.0;

      // influence of the node number 8
    h.val(8)=(1.0+r1)*(1.0-r2)*(1.0-r3)/8.0 - (h.val(15)+h.val(16)+h.val(20))/2.0;
      // influence of the node number 7
    h.val(7)=(1.0-r1)*(1.0-r2)*(1.0-r3)/8.0 - (h.val(14)+h.val(15)+h.val(19))/2.0;
      // influence of the node number 6
    h.val(6)=(1.0-r1)*(1.0+r2)*(1.0-r3)/8.0 - (h.val(13)+h.val(14)+h.val(18))/2.0;
      // influence of the node number 5
    h.val(5)=(1.0+r1)*(1.0+r2)*(1.0-r3)/8.0 - (h.val(13)+h.val(16)+h.val(17))/2.0;

      // influence of the node number 4
    h.val(4)=(1.0+r1)*(1.0-r2)*(1.0+r3)/8.0 - (h.val(11)+h.val(12)+h.val(20))/2.0;
      // influence of the node number 3
    h.val(3)=(1.0-r1)*(1.0-r2)*(1.0+r3)/8.0 - (h.val(10)+h.val(11)+h.val(19))/2.0;
      // influence of the node number 2
    h.val(2)=(1.0-r1)*(1.0+r2)*(1.0+r3)/8.0 - (h.val(10)+h.val(18)+h.val(9))/2.0;
      // influence of the node number 1
    h.val(1)=(1.0+r1)*(1.0+r2)*(1.0+r3)/8.0 - (h.val(12)+h.val(17)+h.val(9))/2.0;
    //    printf("r1 = %lf, r2 = %lf, r3 = %lf\n", r1, r2, r3);
    //    h.print("h");

    return h;
  }



tensor TwentyNodeBrick::dh_drst_at(double r1, double r2, double r3)
  {

    int dimensions[] = {20,3};  // Changed from{20,3} to {8,3} Xiaoyan 07/12
    tensor dh(2, dimensions, 0.0);


    // influence of the node number 20
        dh.val(20,1) =   (1.0-r2)*(1.0-r3*r3)/4.0;
        dh.val(20,2) = - (1.0+r1)*(1.0-r3*r3)/4.0;
        dh.val(20,3) = - (1.0+r1)*(1.0-r2)*r3/2.0;
    // influence of the node number 19
        dh.val(19,1) = - (1.0-r2)*(1.0-r3*r3)/4.0;
        dh.val(19,2) = - (1.0-r1)*(1.0-r3*r3)/4.0;
        dh.val(19,3) = - (1.0-r1)*(1.0-r2)*r3/2.0;
    // influence of the node number 18
        dh.val(18,1) = - (1.0+r2)*(1.0-r3*r3)/4.0;
        dh.val(18,2) =   (1.0-r1)*(1.0-r3*r3)/4.0;
        dh.val(18,3) = - (1.0-r1)*(1.0+r2)*r3/2.0;
    // influence of the node number 17
        dh.val(17,1) =   (1.0+r2)*(1.0-r3*r3)/4.0;
        dh.val(17,2) =   (1.0+r1)*(1.0-r3*r3)/4.0;
        dh.val(17,3) = - (1.0+r1)*(1.0+r2)*r3/2.0;

    // influence of the node number 16
        dh.val(16,1) =   (1.0-r2*r2)*(1.0-r3)/4.0;
        dh.val(16,2) = - (1.0+r1)*r2*(1.0-r3)/2.0;
        dh.val(16,3) = - (1.0+r1)*(1.0-r2*r2)/4.0;
    // influnce of the node number 15
        dh.val(15,1) = - r1*(1.0-r2)*(1.0-r3)/2.0;
        dh.val(15,2) = - (1.0-r1*r1)*(1.0-r3)/4.0;
        dh.val(15,3) = - (1.0-r1*r1)*(1.0-r2)/4.0;
    // influence of the node number 14
        dh.val(14,1) = - (1.0-r2*r2)*(1.0-r3)/4.0;
        dh.val(14,2) = - (1.0-r1)*r2*(1.0-r3)/2.0;
        dh.val(14,3) = - (1.0-r1)*(1.0-r2*r2)/4.0;
    // influence of the node number 13
        dh.val(13,1) = - r1*(1.0+r2)*(1.0-r3)/2.0;
        dh.val(13,2) =   (1.0-r1*r1)*(1.0-r3)/4.0;
        dh.val(13,3) = - (1.0-r1*r1)*(1.0+r2)/4.0;

    // influence of the node number 12
        dh.val(12,1) =   (1.0-r2*r2)*(1.0+r3)/4.0;
        dh.val(12,2) = - (1.0+r1)*r2*(1.0+r3)/2.0;
        dh.val(12,3) =   (1.0+r1)*(1.0-r2*r2)/4.0;
    // influence of the node number 11
        dh.val(11,1) = - r1*(1.0-r2)*(1.0+r3)/2.0;
        dh.val(11,2) = - (1.0-r1*r1)*(1.0+r3)/4.0; // bug discovered 01 aug '95 2.0 -> 4.0
        dh.val(11,3) =   (1.0-r1*r1)*(1.0-r2)/4.0;
    // influence of the node number 10
        dh.val(10,1) = - (1.0-r2*r2)*(1.0+r3)/4.0;
        dh.val(10,2) = - (1.0-r1)*r2*(1.0+r3)/2.0;
        dh.val(10,3) =   (1.0-r1)*(1.0-r2*r2)/4.0;
    // influence of the node number 9
        dh.val(9,1)  = - r1*(1.0+r2)*(1.0+r3)/2.0;
        dh.val(9,2)  =   (1.0-r1*r1)*(1.0+r3)/4.0;
        dh.val(9,3)  =   (1.0-r1*r1)*(1.0+r2)/4.0;

      // influence of the node number 8
    dh.val(8,1)= (1.0-r2)*(1.0-r3)/8.0 - (dh.val(15,1)+dh.val(16,1)+dh.val(20,1))/2.0;
    dh.val(8,2)=-(1.0+r1)*(1.0-r3)/8.0 - (dh.val(15,2)+dh.val(16,2)+dh.val(20,2))/2.0;
    dh.val(8,3)=-(1.0+r1)*(1.0-r2)/8.0 - (dh.val(15,3)+dh.val(16,3)+dh.val(20,3))/2.0;
      // influence of the node number 7
    dh.val(7,1)=-(1.0-r2)*(1.0-r3)/8.0 - (dh.val(14,1)+dh.val(15,1)+dh.val(19,1))/2.0;
    dh.val(7,2)=-(1.0-r1)*(1.0-r3)/8.0 - (dh.val(14,2)+dh.val(15,2)+dh.val(19,2))/2.0;
    dh.val(7,3)=-(1.0-r1)*(1.0-r2)/8.0 - (dh.val(14,3)+dh.val(15,3)+dh.val(19,3))/2.0;
      // influence of the node number 6
    dh.val(6,1)=-(1.0+r2)*(1.0-r3)/8.0 - (dh.val(13,1)+dh.val(14,1)+dh.val(18,1))/2.0;
    dh.val(6,2)= (1.0-r1)*(1.0-r3)/8.0 - (dh.val(13,2)+dh.val(14,2)+dh.val(18,2))/2.0;
    dh.val(6,3)=-(1.0-r1)*(1.0+r2)/8.0 - (dh.val(13,3)+dh.val(14,3)+dh.val(18,3))/2.0;
      // influence of the node number 5
    dh.val(5,1)= (1.0+r2)*(1.0-r3)/8.0 - (dh.val(13,1)+dh.val(16,1)+dh.val(17,1))/2.0;
    dh.val(5,2)= (1.0+r1)*(1.0-r3)/8.0 - (dh.val(13,2)+dh.val(16,2)+dh.val(17,2))/2.0;
    dh.val(5,3)=-(1.0+r1)*(1.0+r2)/8.0 - (dh.val(13,3)+dh.val(16,3)+dh.val(17,3))/2.0;

      // influence of the node number 4
    dh.val(4,1)= (1.0-r2)*(1.0+r3)/8.0 - (dh.val(11,1)+dh.val(12,1)+dh.val(20,1))/2.0;
    dh.val(4,2)=-(1.0+r1)*(1.0+r3)/8.0 - (dh.val(11,2)+dh.val(12,2)+dh.val(20,2))/2.0;
    dh.val(4,3)= (1.0+r1)*(1.0-r2)/8.0 - (dh.val(11,3)+dh.val(12,3)+dh.val(20,3))/2.0;
      // influence of the node number 3
    dh.val(3,1)=-(1.0-r2)*(1.0+r3)/8.0 - (dh.val(10,1)+dh.val(11,1)+dh.val(19,1))/2.0;
    dh.val(3,2)=-(1.0-r1)*(1.0+r3)/8.0 - (dh.val(10,2)+dh.val(11,2)+dh.val(19,2))/2.0;
    dh.val(3,3)= (1.0-r1)*(1.0-r2)/8.0 - (dh.val(10,3)+dh.val(11,3)+dh.val(19,3))/2.0;
      // influence of the node number 2
    dh.val(2,1)=-(1.0+r2)*(1.0+r3)/8.0 - (dh.val(10,1)+dh.val(18,1)+dh.val(9,1))/2.0;
    dh.val(2,2)= (1.0-r1)*(1.0+r3)/8.0 - (dh.val(10,2)+dh.val(18,2)+dh.val(9,2))/2.0;
    dh.val(2,3)= (1.0-r1)*(1.0+r2)/8.0 - (dh.val(10,3)+dh.val(18,3)+dh.val(9,3))/2.0;
      // influence of the node number 1
    dh.val(1,1)= (1.0+r2)*(1.0+r3)/8.0 - (dh.val(12,1)+dh.val(17,1)+dh.val(9,1))/2.0;
    dh.val(1,2)= (1.0+r1)*(1.0+r3)/8.0 - (dh.val(12,2)+dh.val(17,2)+dh.val(9,2))/2.0;
    dh.val(1,3)= (1.0+r1)*(1.0+r2)/8.0 - (dh.val(12,3)+dh.val(17,3)+dh.val(9,3))/2.0;

    return dh;
  }



////#############################################################################
TwentyNodeBrick & TwentyNodeBrick::operator[](int subscript)
  {
    return ( *(this+subscript) );
  }

//Finite_Element & TwentyNodeBrick::operator[](short subscript)
//  {
//    return ( *(this+subscript) );
//  }

//Finite_Element & TwentyNodeBrick::operator[](unsigned subscript)
//  {
//    return ( *(this+subscript) );
//  }


////#############################################################################
////#############################################################################
////#############################################################################
////#############################################################################
tensor TwentyNodeBrick::getStiffnessTensor(void)
  {
    int K_dim[] = {20,3,3,20};
    tensor Kk(4,K_dim,0.0);
    tensor Kkt(4,K_dim,0.0);

//debugging
//    matrix Kmat;

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    int dh_dim[] = {20,3};
    tensor dh(2, dh_dim, 0.0);

    //    tensor Constitutive( 4, def_dim_4, 0.0);
    tensor Constitutive;

    double det_of_Jacobian = 0.0;

    static int disp_dim[] = {20,3};
    tensor incremental_displacements(2,disp_dim,0.0); // \Delta u

    straintensor incremental_strain;
//    straintensor total_strain_at_GP;

    tensor Jacobian;
    tensor JacobianINV;
    tensor JacobianINVtemp;
    tensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                   ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh = dh_drst_at(r,s,t);
		//dh.print("dh");
                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                // Inverse of Jacobian tensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                JacobianINVtemp = Jacobian.inverse();
                // determinant of Jacobian tensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal = dh("ij") * JacobianINV("jk");
                //        ::fprintf(stdout," # %d \n\n\n\n\n\n\n\n", El_count);
	              	//dhGlobal.print("dhGlobal");
                //weight
                weight = rw * sw * tw * det_of_Jacobian;
                //::::::
                //printf("\n\nIN THE STIFFNESS TENSOR INTEGRATOR ----**************** where = %d \n", where);
                //printf("  Stifness_Tensor \n");
                //printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //                            GP_c_r,GP_c_s,GP_c_t);
                //printf("WEIGHT = %f", weight);
                //Jacobian.print("J");
                //JacobianINV.print("JINV");
                //JacobianINVtemp.print("JINVtemp");
                //tensor I = JacobianINV("ij")*Jacobian("jk");
                //I.print("I");

                //printf("determinant of Jacobian = %.6e", det_of_Jacobian);
                //matpoint[where].report("Gauss Point\n");

                // incremental straines at this Gauss point
                //GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor1\n");

		              incremental_strain =
                     (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                //incremental_strain.null_indices();
                //incremental_strain.report("\n incremental_strain tensor at GAUSS point\n");

              		// incremental_strain.reportshort("\n incremental_strain tensor at GAUSS point\n");
                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor2\n");
                // intialize total strain with the strain at this Gauss point before
                // adding this increments strains!
                //                total_strain_at_GP.Initialize(*(GPstrain+where));
                //total_strain_at_GP.reportshort("\n total_strain tensor at GAUSS point BEFORE\n");
                // this is the addition of incremental strains to the previous strain state at
                // this Gauss point
                //                total_strain_at_GP = total_strain_at_GP + incremental_strain;
                //total_strain_at_GP.reportshort("\n total_strain tensor at GAUSS point AFTER\n");
                //..   dakle ovde posalji strain_increment jer se stari stress cuva u okviru svake
                //..   Gauss tacke a samo saljes strain_increment koji ce da se prenese
                //..   u integracionu rutinu pa ce ta da vrati krajnji napon i onda moze da
                //..   se pravi ConstitutiveStiffnessTensor.
                // here comes the final_stress calculation
                // this final stress after integration is used ONLY to obtain Constitutive tensor
                // at this Gauss point.

                //final_stress_after_integration =
                //    (matpoint)->operator[](where).FinalStress(*(GPstress+where),
                //                                 incremental_strain,
                //                                 (matpoint)->operator[](where),
                //                                 number_of_subincrements,
                //                                 this_one_PP);
                //final_stress_after_integration.reportshortpqtheta("\n final_stress_after_integration in stiffness_tensor5\n");

                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor3\n");
                //final_stress_after_integration.reportshortpqtheta("\n final_stress_after_integration GAUSS \n");
                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor4\n");

                // this final stress after integration is used ONLY to obtain Constitutive tensor
                // at this Gauss point AND to set up the iterative_stress that is used in calculting
                // internal forces during iterations!!

                //GPiterative_stress[where].Initialize(final_stress_after_integration);
                //GPiterative_stress[where].reportshortpqtheta("\n iterative_stress at GAUSS point in stiffness_tensor5\n");


                // Stress state at Gauss point will be updated ( in the
                // sense of updating stresses ( integrating them ) ) in function Update (...) ! ! ! ! !
                // calculate the constitutive tensor
                //......         Constitutive =  GPtangent_E[where];

		//Constitutive =  (matpoint)->operator[](where).ConstitutiveTensor(final_stress_after_integration,
                //                                         *(GPstress+where),
                //                                          incremental_strain,
                //                                          (matpoint)->operator[](where),
                //                                          this_one_PP);
                //Constitutive.print("C","\n\n C tensor \n");

	        //EPState *tmp_eps = (matpoint[where]).getEPS();
	        //NDMaterial *tmp_ndm = (matpoint[where]).getNDMat();

              		Constitutive = (matpoint[where]->matmodel)->getTangentTensor();
                //Constitutive.print("C","\n\n C tensor \n");

		//    matpoint[where].setEPS( mmodel->getEPS() );
		//}
		//else if ( tmp_ndm ) { //Elastic case
		//    (matpoint[where].p_matmodel)->setTrialStrainIncr( incremental_strain );
		//    Constitutive = (matpoint[where].p_matmodel)->getTangentTensor();
		//}
		//else {
               	//   g3ErrorHandler->fatal("TwentyNodeBrick::incremental_Update (tag: %d), could not getTangentTensor", this->getTag());
		//   exit(1);
		//}

		//printf("Constitutive.trace = %12.6e\n", Constitutive.trace());
                //Kmat = this->stiffness_matrix(Constitutive);
                //printf("Constitutive tensor max:= %10.3e\n", Kmat.mmax());

                //----   GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor5\n");
                // this is update of constitutive tensor at this Gauss point
                //GPtangent_E[where].Initialize(Constitutive);
                //....GPtangent_E[where].print(" tangent E at GAUSS point");

                //GPstress[where].reportshortpqtheta("\n stress at GAUSS point in stiffness_tensor6\n");

                //K = K + temp2;

              		Kkt = dhGlobal("ib")*Constitutive("abcd");
              		Kk = Kk + Kkt("aicd")*dhGlobal("jd")*weight;

		//Kk = Kk + dhGlobal("ib")*Constitutive("abcd")*dhGlobal("jd")*weight;
                //....K.print("K","\n\n K tensor \n");

		//Kmat = this->stiffness_matrix(Kk);
                //printf("K tensor max= %10.3e\n", Kmat.mmax());

                //convert constitutive and K to matrix and find min and max and print!



              }
          }
      }
    //Kk.print("K","\n\n K tensor \n");
    //K = Kk;
    return Kk;
  }


//#############################################################################

void TwentyNodeBrick::set_strain_stress_tensor(FILE *fp, double * u)
  {
    int dh_dim[] = {20,3};
    tensor dh(2, dh_dim, 0.0);

//    tensor Constitutive( 4, def_dim_4, 0.0);
    tensor Constitutive;
    double r  = 0.0;
    double s  = 0.0;
    double t  = 0.0;
    int where = 0;

    double det_of_Jacobian;

    straintensor strain;
    stresstensor stress;

    tensor Jacobian;
    tensor JacobianINV;
    tensor dhGlobal;


    static int disp_dim[] = {20,3};
    tensor total_displacements(2,disp_dim,0.0); //

    total_displacements = total_disp(fp, u);

    ::printf("\n  displacement(x-y-z) at GAUSS pt %d \n\n", where+1);
    for (int ii=1; ii<=20;ii++)
     {
      ::printf("Global# %d Local#%d  %+0.5e %+0.5e %+0.5e\n",
                     //G_N_numbs[ii-1],
		     connectedExternalNodes(ii-1),
		     ii,total_displacements.val(ii,1),
        	     total_displacements.val(ii,2),
		     total_displacements.val(ii,3));
     }

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh = dh_drst_at(r,s,t);
                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                // Inverse of Jacobian tensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                // determinant of Jacobian tensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal = dh("ij") * JacobianINV("jk");
                //weight
                // straines at this Gauss point from displacement
                strain = (dhGlobal("ib")*total_displacements("ia")).symmetrize11();
                strain.null_indices();
                // here comes the final_stress calculation
                // at this Gauss point.

                //Constitutive =  GPtangent_E[where];
                //Constitutive =  (matpoint->getEPS() )->getEep();
                // if set total displ, then it should be elstic material
              		Constitutive =  ( matpoint[where]->matmodel)->getTangentTensor();

              		stress = Constitutive("ijkl") * strain("kl");  
                stress.null_indices();

                ::printf("\n  strain tensor at GAUSS point %d \n", where+1);
                strain.reportshort("");
                ::printf("\n  stress tensor at GAUSS point %d \n", where+1);
                stress.reportshort("");


              }
          }
      }
  }


////#############################################################################
//  tensor TwentyNodeBrick::mass_tensor(Elastic  mmodel)
tensor TwentyNodeBrick::getMassTensor(void)
  {
    //int M_dim[] = {8,3,3,8};
    int M_dim[] = {60,60};
    tensor Mm(2,M_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    int dh_dim[] = {20,3};

    tensor dh(2, dh_dim, 0.0);

    int h_dim[] = {60,3};	// Xiaoyan changed from {60,3} to {24,3}
    tensor H(2, h_dim, 0.0);

    double det_of_Jacobian = 0.0;

    tensor Jacobian;

    double RHO;
    RHO= rho; 	 //global

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates
                dh = dh_drst_at(r,s,t);
                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                // 		Jacobian.print("J","Jacobian");
                // Inverse of Jacobian tensor ( matrix )
                //                JacobianINV = Jacobian_3Dinv(dh);
                // determinant of Jacobian tensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                // 		printf("det_of_Jacobian = %6.2e \n",det_of_Jacobian);
                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                //                dhGlobal = dh("ij") * JacobianINV("jk");
                // derivatives of local coordinates with respect to local coordinates


                // printf("\n\nIN THE MASS TENSOR INTEGRATOR ----**************** where = %d \n", where);
                // printf("  Mass_Tensor \n");
                // printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //                             GP_c_r,GP_c_s,GP_c_t);
                //
                H = H_3D(r,s,t);

                //	double sum = 0.0;
                //	for (int i=1; i<=60 ; i++)
                //           {
                // //  	    sum+=H.cval(i,1);
                // 	    for (int j=1; j<= 3; j++)
                // 	       {
                //        	          sum+=H.cval(i,j);
                // 	          ::printf( "  %+9.2e", H.cval(i,j) );
                // 	        }
                //             ::printf( "  %d \n", i);
                // 	   }
                // 	    ::printf( " \n sum= %+6.2e\n", sum );




                // matpoint GaPo = MatPoint3D::GP()+where;

                weight = rw * sw * tw * RHO * det_of_Jacobian;

              		Mm = Mm + H("ib")*H("kb")*weight;
	       //	printf("\n +++++++++++++++++++++++++ \n\n");
	      	//Mm.printshort("M");
              }
          }
      }
    //M = Mm;
    //Mm.printshort("M");
    return Mm;
  }


////#############################################################################

tensor TwentyNodeBrick::stiffness_matrix(const tensor & K)
  {
//    int K_dim[] = {20,3,3,20};
//    tensor K(4,K_dim,0.0);
    matrix Kmatrix(60,60,0.0);

    int Ki=0;
    int Kj=0;

    for ( int i=1 ; i<=20 ; i++ )
      {
        for ( int j=1 ; j<=20 ; j++ )
          {
            for ( int k=1 ; k<=3 ; k++ )
              {
                for ( int l=1 ; l<=3 ; l++ )
                  {
                    Ki = k+3*(i-1);
                    Kj = l+3*(j-1);
                    //::printf("i=%d k=%d  Ki=%d       j=%d l=%d  Kj=%d\n",i,k,Ki,j,l,Kj);

                    Kmatrix.val( Ki , Kj ) = K.cval(i,k,l,j);
                  }
              }
          }
      }
    return Kmatrix;
  }

//#############################################################################

////#############################################################################
tensor TwentyNodeBrick::mass_matrix(const tensor & M)
  {
    //    int K_dim[] = {20,3,3,20};
    //    tensor K(4,K_dim,0.0);
    matrix Mmatrix(60,60,0.0);

    for ( int i=1 ; i<=60 ; i++ )
      {
        for ( int j=1 ; j<=60 ; j++ )
          {
             Mmatrix.val( i , j ) = M.cval(i,j);
             //  ::printf("Mi Mj %d %d %+6.2e ",Mi,Mj,Mmatrix.val( Mi , Mj ) );
          }
      }
    return Mmatrix;
  }
////#############################################################################

////#############################################################################
tensor TwentyNodeBrick::Jacobian_3D(tensor dh)
  {
     tensor N_C = Nodal_Coordinates();
     tensor Jacobian_3D = dh("ij") * N_C("ik");
     return Jacobian_3D;
  }

//#############################################################################
tensor TwentyNodeBrick::Jacobian_3Dinv(tensor dh)
  {
     tensor N_C = Nodal_Coordinates();
     tensor Jacobian_3Dinv = (dh("ij") * N_C("ik")).inverse();
     return Jacobian_3Dinv;
  }


////#############################################################################
tensor TwentyNodeBrick::Nodal_Coordinates(void)
  {
    const int dimensions[] = {20,3}; 
    tensor N_coord(2, dimensions, 0.0);

    //Zhaohui using node pointers, which come from the Domain
    const Vector &nd1Crds = nd1Ptr->getCrds();
    const Vector &nd2Crds = nd2Ptr->getCrds();
    const Vector &nd3Crds = nd3Ptr->getCrds();
    const Vector &nd4Crds = nd4Ptr->getCrds();
    const Vector &nd5Crds = nd5Ptr->getCrds();
    const Vector &nd6Crds = nd6Ptr->getCrds();
    const Vector &nd7Crds = nd7Ptr->getCrds();
    const Vector &nd8Crds = nd8Ptr->getCrds();

    const Vector &nd9Crds  =  nd9Ptr->getCrds();
    const Vector &nd10Crds = nd10Ptr->getCrds();
    const Vector &nd11Crds = nd11Ptr->getCrds();
    const Vector &nd12Crds = nd12Ptr->getCrds();

    const Vector &nd13Crds = nd13Ptr->getCrds();
    const Vector &nd14Crds = nd14Ptr->getCrds();
    const Vector &nd15Crds = nd15Ptr->getCrds();
    const Vector &nd16Crds = nd16Ptr->getCrds();


    const Vector &nd17Crds = nd17Ptr->getCrds();
    const Vector &nd18Crds = nd18Ptr->getCrds();
    const Vector &nd19Crds = nd19Ptr->getCrds();
    const Vector &nd20Crds = nd20Ptr->getCrds();
    
    N_coord.val(1,1)=nd1Crds(0); N_coord.val(1,2)=nd1Crds(1); N_coord.val(1,3)=nd1Crds(2);
    N_coord.val(2,1)=nd2Crds(0); N_coord.val(2,2)=nd2Crds(1); N_coord.val(2,3)=nd2Crds(2);
    N_coord.val(3,1)=nd3Crds(0); N_coord.val(3,2)=nd3Crds(1); N_coord.val(3,3)=nd3Crds(2);
    N_coord.val(4,1)=nd4Crds(0); N_coord.val(4,2)=nd4Crds(1); N_coord.val(4,3)=nd4Crds(2);
    N_coord.val(5,1)=nd5Crds(0); N_coord.val(5,2)=nd5Crds(1); N_coord.val(5,3)=nd5Crds(2);
    N_coord.val(6,1)=nd6Crds(0); N_coord.val(6,2)=nd6Crds(1); N_coord.val(6,3)=nd6Crds(2);
    N_coord.val(7,1)=nd7Crds(0); N_coord.val(7,2)=nd7Crds(1); N_coord.val(7,3)=nd7Crds(2);
    N_coord.val(8,1)=nd8Crds(0); N_coord.val(8,2)=nd8Crds(1); N_coord.val(8,3)=nd8Crds(2);

    N_coord.val(9 ,1)=nd9Crds(0);  N_coord.val(9 ,2)=nd9Crds(1);  N_coord.val(9 ,3)=nd9Crds(2);
    N_coord.val(10,1)=nd10Crds(0); N_coord.val(10,2)=nd10Crds(1); N_coord.val(10,3)=nd10Crds(2);
    N_coord.val(11,1)=nd11Crds(0); N_coord.val(11,2)=nd11Crds(1); N_coord.val(11,3)=nd11Crds(2);
    N_coord.val(12,1)=nd12Crds(0); N_coord.val(12,2)=nd12Crds(1); N_coord.val(12,3)=nd12Crds(2);

    N_coord.val(13,1)=nd13Crds(0); N_coord.val(13,2)=nd13Crds(1); N_coord.val(13,3)=nd13Crds(2);
    N_coord.val(14,1)=nd14Crds(0); N_coord.val(14,2)=nd14Crds(1); N_coord.val(14,3)=nd14Crds(2);
    N_coord.val(15,1)=nd15Crds(0); N_coord.val(15,2)=nd15Crds(1); N_coord.val(15,3)=nd15Crds(2);
    N_coord.val(16,1)=nd16Crds(0); N_coord.val(16,2)=nd16Crds(1); N_coord.val(16,3)=nd16Crds(2);
    
    N_coord.val(17,1)=nd17Crds(0); N_coord.val(17,2)=nd17Crds(1); N_coord.val(17,3)=nd17Crds(2);
    N_coord.val(18,1)=nd18Crds(0); N_coord.val(18,2)=nd18Crds(1); N_coord.val(18,3)=nd18Crds(2);
    N_coord.val(19,1)=nd19Crds(0); N_coord.val(19,2)=nd19Crds(1); N_coord.val(19,3)=nd19Crds(2);
    N_coord.val(20,1)=nd20Crds(0); N_coord.val(20,2)=nd20Crds(1); N_coord.val(20,3)=nd20Crds(2);
    
    return N_coord;

  }

////#############################################################################
tensor TwentyNodeBrick::incr_disp(void)
  {
    const int dimensions[] = {20,3};
    tensor increment_disp(2, dimensions, 0.0);

    //for ( int i=0 ; i<20 ; i++ )
    //
    //  {
    //    // increment_disp.val(i+1,1) = nodes[ G_N_numbs[i] ].incremental_translation_x();
    //    // increment_disp.val(i+1,2) = nodes[ G_N_numbs[i] ].incremental_translation_y();
    //    // increment_disp.val(i+1,3) = nodes[ G_N_numbs[i] ].incremental_translation_z();
    //
    //    increment_disp.val(i+1,1) = IncremenDis(0);
    //    increment_disp.val(i+1,2) = IncremenDis(1);
    //    increment_disp.val(i+1,3) = IncremenDis(2);
    //
    //  }

    //Zhaohui using node pointers, which come from the Domain
    //const Vector &TotDis1 = nd1Ptr->getTrialDisp();
    //const Vector &incrdelDis1 = nd1Ptr->getIncrDisp();
    //Have to get IncrDeltaDisp, not IncrDisp for cumulation of incr_disp
    const Vector &IncrDis1 = nd1Ptr->getIncrDeltaDisp();
    const Vector &IncrDis2 = nd2Ptr->getIncrDeltaDisp();
    const Vector &IncrDis3 = nd3Ptr->getIncrDeltaDisp();
    const Vector &IncrDis4 = nd4Ptr->getIncrDeltaDisp();
    const Vector &IncrDis5 = nd5Ptr->getIncrDeltaDisp();
    const Vector &IncrDis6 = nd6Ptr->getIncrDeltaDisp();
    const Vector &IncrDis7 = nd7Ptr->getIncrDeltaDisp();
    const Vector &IncrDis8 = nd8Ptr->getIncrDeltaDisp();

    const Vector &IncrDis9  = nd9Ptr->getIncrDeltaDisp();
    const Vector &IncrDis10 = nd10Ptr->getIncrDeltaDisp();
    const Vector &IncrDis11 = nd11Ptr->getIncrDeltaDisp();
    const Vector &IncrDis12 = nd12Ptr->getIncrDeltaDisp();

    const Vector &IncrDis13 = nd13Ptr->getIncrDeltaDisp();
    const Vector &IncrDis14 = nd14Ptr->getIncrDeltaDisp();
    const Vector &IncrDis15 = nd15Ptr->getIncrDeltaDisp();
    const Vector &IncrDis16 = nd16Ptr->getIncrDeltaDisp();
    
    const Vector &IncrDis17 = nd17Ptr->getIncrDeltaDisp();
    const Vector &IncrDis18 = nd18Ptr->getIncrDeltaDisp();
    const Vector &IncrDis19 = nd19Ptr->getIncrDeltaDisp();
    const Vector &IncrDis20 = nd20Ptr->getIncrDeltaDisp();

    increment_disp.val(1,1)=IncrDis1(0); increment_disp.val(1,2)=IncrDis1(1);increment_disp.val(1,3)=IncrDis1(2);
    increment_disp.val(2,1)=IncrDis2(0); increment_disp.val(2,2)=IncrDis2(1);increment_disp.val(2,3)=IncrDis2(2);
    increment_disp.val(3,1)=IncrDis3(0); increment_disp.val(3,2)=IncrDis3(1);increment_disp.val(3,3)=IncrDis3(2);
    increment_disp.val(4,1)=IncrDis4(0); increment_disp.val(4,2)=IncrDis4(1);increment_disp.val(4,3)=IncrDis4(2);
    increment_disp.val(5,1)=IncrDis5(0); increment_disp.val(5,2)=IncrDis5(1);increment_disp.val(5,3)=IncrDis5(2);
    increment_disp.val(6,1)=IncrDis6(0); increment_disp.val(6,2)=IncrDis6(1);increment_disp.val(6,3)=IncrDis6(2);
    increment_disp.val(7,1)=IncrDis7(0); increment_disp.val(7,2)=IncrDis7(1);increment_disp.val(7,3)=IncrDis7(2);
    increment_disp.val(8,1)=IncrDis8(0); increment_disp.val(8,2)=IncrDis8(1);increment_disp.val(8,3)=IncrDis8(2);

    increment_disp.val(9 ,1)=IncrDis9(0);  increment_disp.val(9 ,2)=IncrDis9(1); increment_disp.val(9 ,3)=IncrDis9(2);
    increment_disp.val(10,1)=IncrDis10(0); increment_disp.val(10,2)=IncrDis10(1);increment_disp.val(10,3)=IncrDis10(2);
    increment_disp.val(11,1)=IncrDis11(0); increment_disp.val(11,2)=IncrDis11(1);increment_disp.val(11,3)=IncrDis11(2);
    increment_disp.val(12,1)=IncrDis12(0); increment_disp.val(12,2)=IncrDis12(1);increment_disp.val(12,3)=IncrDis12(2);
																						   										   																							   										   																						   										   
    increment_disp.val(13,1)=IncrDis13(0); increment_disp.val(13,2)=IncrDis13(1);increment_disp.val(13,3)=IncrDis13(2);
    increment_disp.val(14,1)=IncrDis14(0); increment_disp.val(14,2)=IncrDis14(1);increment_disp.val(14,3)=IncrDis14(2);
    increment_disp.val(15,1)=IncrDis15(0); increment_disp.val(15,2)=IncrDis15(1);increment_disp.val(15,3)=IncrDis15(2);
    increment_disp.val(16,1)=IncrDis16(0); increment_disp.val(16,2)=IncrDis16(1);increment_disp.val(16,3)=IncrDis16(2);
																						   										   																							   										   																						   										   
    increment_disp.val(17,1)=IncrDis17(0); increment_disp.val(17,2)=IncrDis17(1);increment_disp.val(17,3)=IncrDis17(2);
    increment_disp.val(18,1)=IncrDis18(0); increment_disp.val(18,2)=IncrDis18(1);increment_disp.val(18,3)=IncrDis18(2);
    increment_disp.val(19,1)=IncrDis19(0); increment_disp.val(19,2)=IncrDis19(1);increment_disp.val(19,3)=IncrDis19(2);
    increment_disp.val(20,1)=IncrDis20(0); increment_disp.val(20,2)=IncrDis20(1);increment_disp.val(20,3)=IncrDis20(2);


    return increment_disp;
  }

////#############################################################################
tensor TwentyNodeBrick::total_disp(void)
  {
    const int dimensions[] = {20,3};
    tensor total_disp(2, dimensions, 0.0);

    //Zhaohui using node pointers, which come from the Domain
    const Vector &TotDis1 = nd1Ptr->getTrialDisp();
    cout<<"\ntot node " << nd1Ptr->getTag() <<" x "<< TotDis1(0) <<" y "<< TotDis1(1) << " z "<< TotDis1(2) << endln;
    const Vector &TotDis2 = nd2Ptr->getTrialDisp();
    cout << "tot node " << nd2Ptr->getTag() << " x " << TotDis2(0) <<" y "<< TotDis2(1) << " z "<< TotDis2(2) << endln;
    const Vector &TotDis3 = nd3Ptr->getTrialDisp();
    cout << "tot node " << nd3Ptr->getTag() << " x " << TotDis3(0) <<" y "<< TotDis3(1) << " z "<< TotDis3(2) << endln;
    const Vector &TotDis4 = nd4Ptr->getTrialDisp();
    cout << "tot node " << nd4Ptr->getTag() << " x " << TotDis4(0) <<" y "<< TotDis4(1) << " z "<< TotDis4(2) << endln;
    const Vector &TotDis5 = nd5Ptr->getTrialDisp();
    cout << "tot node " << nd5Ptr->getTag() << " x " << TotDis5(0) <<" y "<< TotDis5(1) << " z "<< TotDis5(2) << endln;
    const Vector &TotDis6 = nd6Ptr->getTrialDisp();
    cout << "tot node " << nd6Ptr->getTag() << " x " << TotDis6(0) <<" y "<< TotDis6(1) << " z "<< TotDis6(2) << endln;
    const Vector &TotDis7 = nd7Ptr->getTrialDisp();
    cout << "tot node " << nd7Ptr->getTag() << " x " << TotDis7(0) <<" y "<< TotDis7(1) << " z "<< TotDis7(2) << endln;
    const Vector &TotDis8 = nd8Ptr->getTrialDisp();
    cout << "tot node " << nd8Ptr->getTag() << " x " << TotDis8(0) <<" y "<< TotDis8(1) << " z "<< TotDis8(2) << endln;

    const Vector &TotDis9 = nd9Ptr->getTrialDisp();
    cout << "tot node " << nd9Ptr->getTag() << " x " << TotDis9(0) <<" y "<< TotDis9(1) << " z "<< TotDis9(2) << endln;
    const Vector &TotDis10 = nd10Ptr->getTrialDisp();
    cout << "tot node " << nd10Ptr->getTag() << " x " << TotDis10(0) <<" y "<< TotDis10(1) << " z "<< TotDis10(2) << endln;
    const Vector &TotDis11 = nd11Ptr->getTrialDisp();
    cout << "tot node " << nd11Ptr->getTag() << " x " << TotDis11(0) <<" y "<< TotDis11(1) << " z "<< TotDis11(2) << endln;
    const Vector &TotDis12 = nd12Ptr->getTrialDisp();
    cout << "tot node " << nd12Ptr->getTag() << " x " << TotDis12(0) <<" y "<< TotDis12(1) << " z "<< TotDis12(2) << endln;

    const Vector &TotDis13 = nd13Ptr->getTrialDisp();
    cout << "tot node " << nd13Ptr->getTag() << " x " << TotDis13(0) <<" y "<< TotDis13(1) << " z "<< TotDis13(2) << endln;
    const Vector &TotDis14 = nd14Ptr->getTrialDisp();
    cout << "tot node " << nd14Ptr->getTag() << " x " << TotDis14(0) <<" y "<< TotDis14(1) << " z "<< TotDis14(2) << endln;
    const Vector &TotDis15 = nd15Ptr->getTrialDisp();
    cout << "tot node " << nd15Ptr->getTag() << " x " << TotDis15(0) <<" y "<< TotDis15(1) << " z "<< TotDis15(2) << endln;
    const Vector &TotDis16 = nd16Ptr->getTrialDisp();
    cout << "tot node " << nd16Ptr->getTag() << " x " << TotDis16(0) <<" y "<< TotDis16(1) << " z "<< TotDis16(2) << endln;

    const Vector &TotDis17 = nd17Ptr->getTrialDisp();
    cout << "tot node " << nd17Ptr->getTag() << " x " << TotDis17(0) <<" y "<< TotDis17(1) << " z "<< TotDis17(2) << endln;
    const Vector &TotDis18 = nd18Ptr->getTrialDisp();
    cout << "tot node " << nd18Ptr->getTag() << " x " << TotDis18(0) <<" y "<< TotDis18(1) << " z "<< TotDis18(2) << endln;
    const Vector &TotDis19 = nd19Ptr->getTrialDisp();
    cout << "tot node " << nd19Ptr->getTag() << " x " << TotDis19(0) <<" y "<< TotDis19(1) << " z "<< TotDis19(2) << endln;
    const Vector &TotDis20 = nd20Ptr->getTrialDisp();
    cout << "tot node " << nd20Ptr->getTag() << " x " << TotDis20(0) <<" y "<< TotDis20(1) << " z "<< TotDis20(2) << endln;




    total_disp.val(1,1)=TotDis1(0); total_disp.val(1,2)=TotDis1(1);total_disp.val(1,3)=TotDis1(2);
    total_disp.val(2,1)=TotDis2(0); total_disp.val(2,2)=TotDis2(1);total_disp.val(2,3)=TotDis2(2);
    total_disp.val(3,1)=TotDis3(0); total_disp.val(3,2)=TotDis3(1);total_disp.val(3,3)=TotDis3(2);
    total_disp.val(4,1)=TotDis4(0); total_disp.val(4,2)=TotDis4(1);total_disp.val(4,3)=TotDis4(2);
    total_disp.val(5,1)=TotDis5(0); total_disp.val(5,2)=TotDis5(1);total_disp.val(5,3)=TotDis5(2);
    total_disp.val(6,1)=TotDis6(0); total_disp.val(6,2)=TotDis6(1);total_disp.val(6,3)=TotDis6(2);
    total_disp.val(7,1)=TotDis7(0); total_disp.val(7,2)=TotDis7(1);total_disp.val(7,3)=TotDis7(2);
    total_disp.val(8,1)=TotDis8(0); total_disp.val(8,2)=TotDis8(1);total_disp.val(8,3)=TotDis8(2);

    total_disp.val(9,1)=TotDis9(0); total_disp.val(9,2)=TotDis9(1);total_disp.val(9,3)=TotDis9(2);
    total_disp.val(10,1)=TotDis10(0); total_disp.val(10,2)=TotDis10(1);total_disp.val(10,3)=TotDis10(2);
    total_disp.val(11,1)=TotDis11(0); total_disp.val(11,2)=TotDis11(1);total_disp.val(11,3)=TotDis11(2);
    total_disp.val(12,1)=TotDis12(0); total_disp.val(12,2)=TotDis12(1);total_disp.val(12,3)=TotDis12(2);

    total_disp.val(13,1)=TotDis13(0); total_disp.val(13,2)=TotDis13(1);total_disp.val(13,3)=TotDis13(2);
    total_disp.val(14,1)=TotDis14(0); total_disp.val(14,2)=TotDis14(1);total_disp.val(14,3)=TotDis14(2);
    total_disp.val(15,1)=TotDis15(0); total_disp.val(15,2)=TotDis15(1);total_disp.val(15,3)=TotDis15(2);
    total_disp.val(16,1)=TotDis16(0); total_disp.val(16,2)=TotDis16(1);total_disp.val(16,3)=TotDis16(2);

    total_disp.val(17,1)=TotDis17(0); total_disp.val(17,2)=TotDis17(1);total_disp.val(17,3)=TotDis17(2);
    total_disp.val(18,1)=TotDis18(0); total_disp.val(18,2)=TotDis18(1);total_disp.val(18,3)=TotDis18(2);
    total_disp.val(19,1)=TotDis19(0); total_disp.val(19,2)=TotDis19(1);total_disp.val(19,3)=TotDis19(2);
    total_disp.val(20,1)=TotDis20(0); total_disp.val(20,2)=TotDis20(1);total_disp.val(20,3)=TotDis20(2);

    return total_disp;
  }


////#############################################################################
tensor TwentyNodeBrick::total_disp(FILE *fp, double * u)
  {
    const int dimensions[] = {20,3};
    tensor total_disp(2, dimensions, 0.0);
    //    double totalx, totaly, totalz;
    //    totalx=0;
    //    totaly=0;
    //    totalz=0;

    //for ( int i=0 ; i<20 ; i++ )
    //
    //  {
    //    // total_disp.val(i+1,1) = nodes[ G_N_numbs[i] ].total_translation_x(u);
    //    // total_disp.val(i+1,2) = nodes[ G_N_numbs[i] ].total_translation_y(u);
    //    // total_disp.val(i+1,3) = nodes[ G_N_numbs[i] ].total_translation_z(u);
    //    Vector TotalTranDis = nodes[ G_N_numbs[i] ].getDisp();
    //
    //    total_disp.val(i+1,1) = TotalTranDis(0);
    //	total_disp.val(i+1,2) = TotalTranDis(1);
    //    total_disp.val(i+1,3) = TotalTranDis(2);
    //
    //  }
    
    // Need more work
    
    //Zhaohui using node pointers, which come from the Domain
    const Vector &TotDis1 = nd1Ptr->getTrialDisp();
    const Vector &TotDis2 = nd2Ptr->getTrialDisp();
    const Vector &TotDis3 = nd3Ptr->getTrialDisp();
    const Vector &TotDis4 = nd4Ptr->getTrialDisp();
    const Vector &TotDis5 = nd5Ptr->getTrialDisp();
    const Vector &TotDis6 = nd6Ptr->getTrialDisp();
    const Vector &TotDis7 = nd7Ptr->getTrialDisp();
    const Vector &TotDis8 = nd8Ptr->getTrialDisp();

    total_disp.val(1,1)=TotDis1(0); total_disp.val(1,2)=TotDis1(1);total_disp.val(1,3)=TotDis1(2);
    total_disp.val(2,1)=TotDis2(0); total_disp.val(2,2)=TotDis2(1);total_disp.val(2,3)=TotDis2(2);
    total_disp.val(3,1)=TotDis3(0); total_disp.val(3,2)=TotDis3(1);total_disp.val(3,3)=TotDis3(2);
    total_disp.val(4,1)=TotDis4(0); total_disp.val(4,2)=TotDis4(1);total_disp.val(4,3)=TotDis4(2);
    total_disp.val(5,1)=TotDis5(0); total_disp.val(5,2)=TotDis5(1);total_disp.val(5,3)=TotDis5(2);
    total_disp.val(6,1)=TotDis6(0); total_disp.val(6,2)=TotDis6(1);total_disp.val(6,3)=TotDis6(2);
    total_disp.val(7,1)=TotDis7(0); total_disp.val(7,2)=TotDis7(1);total_disp.val(7,3)=TotDis7(2);
    total_disp.val(8,1)=TotDis8(0); total_disp.val(8,2)=TotDis8(1);total_disp.val(8,3)=TotDis8(2);

    return total_disp;
  }


////#############################################################################
int TwentyNodeBrick::get_global_number_of_node(int local_node_number)
{
  //return G_N_numbs[local_node_number];
  return connectedExternalNodes(local_node_number);
}

////#############################################################################
int  TwentyNodeBrick::get_Brick_Number(void)
{
  //return elem_numb;
  return this->getTag();
}

////#############################################################################
int * TwentyNodeBrick::get_LM(void)
  {
    return LM;
  }

//Commented out Zhaohui 09-27-2000

//////#############################################################################
//void TwentyNodeBrick::set_LM(Node * node)
//  {
////    unsigned int BrickNumber = this->get_Brick_Number();
////    this->reportshort("");
//// for element numbered BrickNumber create LM array (see Bathe pp984
////    for (int LocalNodeNumber = 1 ; LocalNodeNumber<=20 ; LocalNodeNumber++ )
//    for (int LocalNodeNumber = 1 ; LocalNodeNumber<=8 ; LocalNodeNumber++ )// for 8noded brick
//      {
////        unsigned int global_node_number = b3d[BrickNumber-1].get_global_number_of_node(LocalNodeNumber-1);
//        unsigned int global_node_number = this->get_global_number_of_node(LocalNodeNumber-1);
//        LM[3*LocalNodeNumber-3] = node[global_node_number].eqn_tx();
//        LM[3*LocalNodeNumber-2] = node[global_node_number].eqn_ty();
//        LM[3*LocalNodeNumber-1] = node[global_node_number].eqn_tz();
//      }
//
//      // ::printf("\n\n");
//
////===   this->reportLM("LM");
////   for (int count01=1;count01<=8;count01++)
////     {
////       ::printf("element %4d localNode %4d Globalnode %4d  LM   %4d   %4d   %4d\n", BrickNumber, count01,this->get_global_number_of_node(count01-1),  LM[count01*3-3], LM[count01*3-2], LM[count01*3-1] );
////     }
//
//  }


////#############################################################################
// returns nodal forces for given stress field in an element
tensor TwentyNodeBrick::nodal_forces(void)
  {
    int force_dim[] = {20,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor nodal_forces(2,force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    int dh_dim[] = {20,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor dh(2, dh_dim, 0.0);

    stresstensor stress_at_GP(0.0);

    double det_of_Jacobian = 0.0;

    straintensor incremental_strain;

    static int disp_dim[] = {20,3};   // Xiaoyan changed from {20,3} to {8,3}
    tensor incremental_displacements(2,disp_dim,0.0); // \Delta u

    incremental_displacements = incr_disp();

    tensor Jacobian;
    tensor JacobianINV;
    tensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian tensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian tensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal = dh("ij") * JacobianINV("jk");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;
                //..::printf("\n\nIN THE nodal forces ----**************** where = %d \n", where);
                //..::printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //..                           GP_c_r,GP_c_s,GP_c_t);
                //..::printf("WEIGHT = %f", weight);
                //..::printf("determinant of Jacobian = %f", det_of_Jacobian);
                //..matpoint[where].report("Gauss Point\n");

                //..   // samo jos odredi ovaj tensor E i to za svaku gauss tacku drugaciji !!!!!!!!!!!!
                //..   ovde negde bi trebalo da se na osnovu inkrementalnih pomeranja
                //..   nadje inkrementalna deformacija ( strain_increment ) pa sa njom dalje:
                //..
                //// tensor of incremental displacements taken from node objects
                //                incremental_displacements = incr_disp();
                //
                //// incremental straines at this Gauss point
                //                incremental_strain =
                //                  (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                //
                //                incremental_strain.null_indices();
                ////incremental_strain.reportshort("\n incremental_strain tensor at GAUSS point\n");
                //
                ////                integr_type = (matpoint)->operator[](where).integration_type();
                ////                if ( !strcmp(integr_type,"BakcwardEuler")

                //..   dakle ovde posalji strain_increment jer se stari stress cuva u okviru svake
                //..   Gauss tacke a samo saljes strain_increment koji ce da se prenese
                //..   u integracionu rutinu pa ce ta da vrati krajnji napon i onda moze da
                //..   se pravi ConstitutiveStiffnessTensor.
                //.. Ustvari posalji sve sto imas ( incremental_strain, start_stress,
                //.. number_of_subincrements . . . u ovu Constitutive_tensor funkciju
                //.. pa ona nek ide, u zavisnosti od modela koji se koristi i neka
                //.. onda tamo u svakoj posebnoj modelskoj funkciji vrati sta treba
                //.. ( recimo Elastic odmah vraca Eelastic a recimo MRS_Lade prvo
                //.. pita koji nacin integracije da koristi pa onda u zvisnosti od toga
                //.. zove funkcuju koja integrali za taj algoritam ( ForwardEuler, BakcwardEuler,
                //.. SemiBackwardEuler, . . . ) i onda kada funkcija vrati napon onda
                //.. se opet pita koji je tip integracije bio u pitanju pa pravi odgovarajuci
                //.. ConstitutiveTensor i vraca ga nazad!

                //                   stress_at_GP = (GPstress)->operator[](where);
                //stress_at_GP = GPstress[where];

	        //EPState *tmp_eps = (matpoint[where]->matmodel)->getEPS();
		//stress_at_GP = tmp_eps->getStress();
		//cout << "tmp_eps" << (*tmp_eps);

	        //NDMaterial *tmp_ndm = (matpoint[where]).getNDMat();

		//if ( tmp_eps ) {     //Elasto-plastic case

		//stress_at_GP = (matpoint[where].matmodel->getEPS())->getStress();

		//   EPState *tmp_eps = (matpoint[where]->matmodel)->getEPS();
		//   stress_at_GP = tmp_eps->getStress();



		incremental_strain =
                     (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
//		if (where == 0)
//   		//cout << " In nodal_force delta_incremental_strain tag "<< getTag() <<"  " <<incremental_strain << endln;
////		cout << " el tag = "<< getTag();
//
		int err = ( matpoint[where]->matmodel )->setTrialStrainIncr( incremental_strain);
		if ( err)
               	   g3ErrorHandler->warning("TwentyNodeBrick::getStiffnessTensor (tag: %d), not converged",
		    		 this->getTag());



		//char *test = matpoint[where]->matmodel->getType();
		// fmk - changing if so if into else block must be Template3Dep
//		if (strcmp(matpoint[where]->matmodel->getType(),"Template3Dep") != 0)
		   stress_at_GP = matpoint[where]->getStressTensor();

//				 stress_at_GP.report("PROBLEM");
//				 getchar();

//		else
//		{
//	           //Some thing funny happened when getting stress directly from matpoint[where], i have to do it this way!
//		   EPState *tmp_eps = ((Template3Dep *)(matpoint[where]->matmodel))->getEPS();
//		   stress_at_GP = tmp_eps->getStress();
//		   //delete tmp_eps;
//	       	}

           	//double  p = stress_at_GP.p_hydrostatic();
                //if ( p < 0.0 )
	        //{
	        //  cerr << getTag();
	        //  cerr << " ***p  =    " << p << endln;
	        //}

		//cerr << " nodal_force ::: stress_at_GP " << stress_at_GP << endln;

		//}
		//else if ( tmp_ndm ) { //Elastic case
             	//    stress_at_GP = (matpoint[where].getNDMat())->getStressTensor();
		//}
		//else {
               	//   g3ErrorHandler->fatal("TwentyNodeBrick::nodal_forces (tag: %d), could not getStress", this->getTag());
		//   exit(1);
		//}

                //stress_at_GP.report("\n stress_at_GPtensor at GAUSS point for nodal forces \n");

                // nodal forces See Zienkievicz part 1 pp 108
                nodal_forces =
                     nodal_forces + dhGlobal("ib")*stress_at_GP("ab")*weight;
                //nodal_forces.print("nf","\n\n Nodal Forces \n");

              }
          }
      }

    //cout << "\n element no. " << getTag() << endln;
    //nodal_forces.print("nf","\n Nodal Forces \n");
    return nodal_forces;

  }

////#############################################################################
// returns nodal forces for given ITERATIVE stress field in an element
tensor TwentyNodeBrick::iterative_nodal_forces(void)
  {
    int force_dim[] = {20,3}; // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor nodal_forces(2,force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    int dh_dim[] = {20,3};   // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor dh(2, dh_dim, 0.0);

    stresstensor stress_at_GP(0.0);

    double det_of_Jacobian = 0.0;

    tensor Jacobian;
    tensor JacobianINV;
    tensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                //.....
                //.....::printf("TwentyNodeBrick::iterative_nodal_forces(void)  ----**************** where = %d \n", where);
                //.....::printf("UPDATE ");
                //.....::printf("   GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //.....                           GP_c_r,GP_c_s,GP_c_t);
                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian tensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian tensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal = dh("ij") * JacobianINV("jk");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;

                //                   stress_at_GP = (GPstress)->operator[](where);
                //stress_at_GP = GPiterative_stress[where];

		//stress_at_GP = ( matpoint[where].getTrialEPS() )->getStress();
                stress_at_GP = matpoint[where]->getStressTensor();
                stress_at_GP.reportshortpqtheta("\n iterative_stress at GAUSS point in iterative_nodal_force\n");

                // nodal forces See Zienkievicz part 1 pp 108
                nodal_forces =
                  nodal_forces + dhGlobal("ib")*stress_at_GP("ab")*weight;
                //nodal_forces.print("nf","\n TwentyNodeBrick::iterative_nodal_forces Nodal Forces ~~~~\n");

              }
          }
      }


    return nodal_forces;

  }

////#############################################################################
// returns nodal forces for given constant stress field in the element
tensor TwentyNodeBrick::nodal_forces_from_stress(stresstensor & stress)
  {
    int force_dim[] = {20,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor nodal_forces(2,force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    double weight = 0.0;

    int dh_dim[] = {20,3}; // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor dh(2, dh_dim, 0.0);

    double det_of_Jacobian = 0.0;

    tensor Jacobian;
    tensor JacobianINV;
    tensor dhGlobal;

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                //--                where =
                //--                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                //.....
                //.....::printf("TwentyNodeBrick::iterative_nodal_forces(void)  ----**************** where = %d \n", where);
                //.....::printf("UPDATE ");
                //.....::printf("   GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //.....                           GP_c_r,GP_c_s,GP_c_t);
                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian tensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian tensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal = dh("ij") * JacobianINV("jk");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;

                //                   stress_at_GP = (GPstress)->operator[](where);
                //                stress_at_GP = GPiterative_stress[where];
                //GPiterative_stress[where].reportshortpqtheta("\n iterative_stress at GAUSS point in iterative_nodal_force\n");

                // nodal forces See Zienkievicz part 1 pp 108
                nodal_forces =
                  nodal_forces + dhGlobal("ib")*stress("ab")*weight;
                //nodal_forces.print("nf","\n TwentyNodeBrick::iterative_nodal_forces Nodal Forces ~~~~\n");

              }
          }
      }

    return nodal_forces;

  }


////#############################################################################
// returns nodal forces for given incremental strain field in an element
// by using the linearized constitutive tensor from the begining of the step !
tensor TwentyNodeBrick::linearized_nodal_forces(void)
  {
    int force_dim[] = {20,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor linearized_nodal_forces(2,force_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    short where = 0;
    double weight = 0.0;

    int dh_dim[] = {20,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor dh(2, dh_dim, 0.0);

    tensor Constitutive( 4, def_dim_4, 0.0);

    double det_of_Jacobian = 0.0;

    static int disp_dim[] = {20,3};  // Xiaoyan changed from {20,3 to {8,3} for 8 nodes

    tensor incremental_displacements(2,disp_dim,0.0);

    straintensor incremental_strain;

    tensor Jacobian;
    tensor JacobianINV;
    tensor dhGlobal;

    stresstensor final_linearized_stress;
    //    stresstensor incremental_stress;
    // tensor of incremental displacements taken from node objects for this element !
    incremental_displacements = incr_disp();
    //incremental_displacements.print("disp","\n incremental_displacements tensor at GAUSS point in iterative_Update\n");

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        rw = get_Gauss_p_w( r_integration_order, GP_c_r );

        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            sw = get_Gauss_p_w( s_integration_order, GP_c_s );

            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                tw = get_Gauss_p_w( t_integration_order, GP_c_t );

                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                // derivatives of local coordiantes with respect to local coordiantes
                dh = dh_drst_at(r,s,t);

                // Jacobian tensor ( matrix )
                Jacobian = Jacobian_3D(dh);
                //....                Jacobian.print("J");

                // Inverse of Jacobian tensor ( matrix )
                JacobianINV = Jacobian_3Dinv(dh);
                //....                JacobianINV.print("JINV");

                // determinant of Jacobian tensor ( matrix )
                det_of_Jacobian  = Jacobian.determinant();
                //....  ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );

                // Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
                dhGlobal = dh("ij") * JacobianINV("jk");

                //weight
                weight = rw * sw * tw * det_of_Jacobian;
                //..::printf("\n\nIN THE nodal forces ----**************** where = %d \n", where);
                //..::printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                //..                           GP_c_r,GP_c_s,GP_c_t);
                //..::printf("WEIGHT = %f", weight);
                //..::printf("determinant of Jacobian = %f", det_of_Jacobian);
                // incremental straines at this Gauss point
                // now in Update we know the incremental displacements so let's find
                // the incremental strain
                incremental_strain =
                 (dhGlobal("ib")*incremental_displacements("ia")).symmetrize11();
                incremental_strain.null_indices();
                //incremental_strain.reportshort("\n incremental_strain tensor at GAUSS point in iterative_Update\n");

                //Constitutive = GPtangent_E[where];

	        //EPState *tmp_eps = (matpoint[where]).getEPS();
	        //NDMaterial *tmp_ndm = (matpoint[where]).getNDMat();

		//if ( tmp_eps ) {     //Elasto-plastic case
		//    mmodel->setEPS( *tmp_eps );
		if ( ! (matpoint[where]->matmodel)->setTrialStrainIncr( incremental_strain)  )
               	   g3ErrorHandler->warning("TwentyNodeBrick::linearized_nodal_forces (tag: %d), not converged",
		    		 this->getTag());
		Constitutive = (matpoint[where]->matmodel)->getTangentTensor();
      		//    matpoint[where].setEPS( mmodel->getEPS() ); //Set the new EPState back
		//}
		//else if ( tmp_ndm ) { //Elastic case
		//    (matpoint[where].p_matmodel)->setTrialStrainIncr( incremental_strain );
		//    Constitutive = (matpoint[where].p_matmodel)->getTangentTensor();
		//}
		//else {
               	//   g3ErrorHandler->fatal("TwentyNodeBrick::incremental_Update (tag: %d), could not getTangentTensor", this->getTag());
		//   exit(1);
		//}

		//Constitutive = ( matpoint[where].getEPS() )->getEep();
                //..//GPtangent_E[where].print("\n tangent E at GAUSS point \n");

                final_linearized_stress =
                  Constitutive("ijkl") * incremental_strain("kl");

                // nodal forces See Zienkievicz part 1 pp 108
                linearized_nodal_forces = linearized_nodal_forces +
                          dhGlobal("ib")*final_linearized_stress("ab")*weight;
                //::::::                   nodal_forces.print("nf","\n\n Nodal Forces \n");

              }
          }
      }


    return linearized_nodal_forces;

  }

//....////#############################################################################
//....// updates Gauss point stresses and strains from given displacements
//....void TwentyNodeBrick::update_stress_strain(tensor & displacementsT)
//....  {
//....//    int force_dim[] = {20,3};
//....//    tensor nodal_forces(2,force_dim,0.0);
//....
//....    double r  = 0.0;
//....    double rw = 0.0;
//....    double s  = 0.0;
//....    double sw = 0.0;
//....    double t  = 0.0;
//....    double tw = 0.0;
//....
//....    short where = 0;
//....    double weight = 0.0;
//....
//....    int dh_dim[] = {20,3};
//....    tensor dh(2, dh_dim, 0.0);
//....
//....    stresstensor stress_at_GP(0.0);
//....    straintensor strain_at_GP(0.0);
//....
//....    double det_of_Jacobian = 0.0;
//....
//....    tensor Jacobian;
//....    tensor JacobianINV;
//....    tensor dhGlobal;
//....
//....    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
//....      {
//....        r = get_Gauss_p_c( r_integration_order, GP_c_r );
//....        rw = get_Gauss_p_w( r_integration_order, GP_c_r );
//....
//....        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
//....          {
//....            s = get_Gauss_p_c( s_integration_order, GP_c_s );
//....            sw = get_Gauss_p_w( s_integration_order, GP_c_s );
//....
//....            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
//....              {
//....                t = get_Gauss_p_c( t_integration_order, GP_c_t );
//....                tw = get_Gauss_p_w( t_integration_order, GP_c_t );
//....
//....// this short routine is supposed to calculate position of
//....// Gauss point from 3D array of short's
//....                where =
//....                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
//....
//....//........................................................
//....//........................................................
//....// interpolation functions
//....                tensor h = b3darray[0].interp_poli_at(r,s,t);
//....                ::printf("\n\n r = %f, s = %f, t = %f\n", r, s, t);
//....//  h.print("h");
//....
//....// displacements
//....//....   tensor disp_at_rst = h("i")*displacementsT("ia");
//....//....   disp_at_rst.print("disp");
//....
//....// derivatives of interpolation functions
//....                dh = dh_drst_at(r,s,t);
//....//                ::printf("\n\n r = %f, s = %f, t = %f\n", r, s, t);
//....//  dh.print("dh");
//....
//....                Jacobian = b3darray[0].Jacobian_3D(dh);
//....//                Jacobian.print("J");
//....
//....                JacobianINV = b3darray[0].Jacobian_3Dinv(dh);
//....//                JacobianINV.print("JINV");
//....
//....//                det_of_Jacobian = Jacobian.determinant();
//....//                ::printf("determinant of Jacobian is %f\n",Jacobian_determinant );
//....
//....// Derivatives of local coordinates multiplied with inverse of Jacobian (see Bathe p-202)
//....                dhGlobal = dh("ij") * JacobianINV("jk");
//....// straines
//....//  strain = (dh("ib")*displacements("ia")).symmetrize11();
//....                strain = (dhGlobal("ib")*displacementsT("ia")).symmetrize11();
//....//  straintensor strain = dh("ib")*displacements("ia");
//....                strain.reportshort("\n strain tensor\n");
//....                strain.null_indices();
//....
//....//                tensor E = mmElastic.ElasticStiffness();
//....
//....//stresses
//....                stress = E("ijkl") * strain("kl");
//....                stress.reportshort("\n\n stress tensor \n");
//....//...
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....//........................................................
//....
//....
//....              }
//....          }
//....      }
//....
//....  }

////#############################################################################
////#############################################################################
//double TwentyNodeBrick::get_first_q_ast(void)
//  {
//    double ret = matpoint[0].kappa_cone_get();
//
//    return ret;
//
//  }
////#############################################################################
//double TwentyNodeBrick::get_first_etacone(void)
//  {
//    double ret = matpoint[0].etacone();
//
//    return ret;
//
//  }
//

//#############################################################################
void TwentyNodeBrick::report(char * msg)
  {
    if ( msg ) ::printf("** %s",msg);
    ::printf("\n Element Number = %d\n", this->getTag() );
    ::printf("\n Number of nodes in a TwentyNodebrick = %d\n",
                                              nodes_in_brick);
    ::printf("\n Determinant of Jacobian (! ==0 before comp.) = %f\n",
                                                  determinant_of_Jacobian);

    ::printf("Node numbers \n");
    ::printf(".....1.....2.....3.....4.....5.....6.....7.....8.....9.....0.....1.....2\n");
           for ( int i=0 ; i<20 ; i++ )
	    //::printf("%6d",G_N_numbs[i]);
	    ::printf("%6d",connectedExternalNodes(i));
    ::printf("\n");
    //           for ( int j=8 ; j<20 ; j++ )
    //             ::printf("%6d",G_N_numbs[j]);	   // Commented by Xiaoyan
    ::printf("\n\n");

    //    ::printf("Node existance array \n");
    //           for ( int k=0 ; k<15 ; k++ )
    //             ::printf("%6d",node_existance[k]);
    //           ::printf("\n\n");			    // Commented by Xiaoyan


    int total_number_of_Gauss_points = r_integration_order*
                                       s_integration_order*
                                       t_integration_order;
    if ( total_number_of_Gauss_points != 0 )
      {
           // report from Node class
           //for ( int in=0 ; in<8 ; in++ )
           //             (nodes[G_N_numbs[in]]).report("nodes from within element (first 8)\n");
           //Xiaoyan changed .report to . Print in above line 09/27/00
	   //  (nodes[G_N_numbs[in]]).Print(cout);

	   nd1Ptr->Print(cout);
	   nd2Ptr->Print(cout);
	   nd3Ptr->Print(cout);
	   nd4Ptr->Print(cout);
	   nd5Ptr->Print(cout);
	   nd6Ptr->Print(cout);
           nd7Ptr->Print(cout);
	   nd8Ptr->Print(cout);
	   nd9Ptr->Print(cout);
	   nd10Ptr->Print(cout);
	   nd11Ptr->Print(cout);
 	   nd12Ptr->Print(cout);
 	   nd13Ptr->Print(cout);
	   nd14Ptr->Print(cout);
	   nd15Ptr->Print(cout);
	   nd16Ptr->Print(cout);
	   nd17Ptr->Print(cout);
	   nd18Ptr->Print(cout);
	   nd19Ptr->Print(cout);
	   nd20Ptr->Print(cout);

	   //           for ( int jn=8 ; jn<20 ; jn++ )
           //             (nodes[G_N_numbs[jn]]).report("nodes from within element (last 15)\n");
           // Commented by Xiaoyan
      }

    ::printf("\n\nGauss-Legendre integration order\n");
    ::printf("Integration order in r direction = %d\n",r_integration_order);
    ::printf("Integration order in s direction = %d\n",s_integration_order);
    ::printf("Integration order in t direction = %d\n\n",t_integration_order);



    for( int GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        for( int GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            for( int GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                short where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

                ::printf("\n\n----------------**************** where = %d \n", where);
                ::printf("                    GP_c_r = %d,  GP_c_s = %d,  GP_c_t = %d\n",
                            GP_c_r,GP_c_s,GP_c_t);
                matpoint[where]->report("Material Point\n");
                //GPstress[where].reportshort("stress at Gauss Point");
                //GPstrain[where].reportshort("strain at Gauss Point");
                //matpoint[where].report("Material model  at Gauss Point");
              }
          }
      }

  }


//#############################################################################
void TwentyNodeBrick::reportshort(char * msg)
  {
    if ( msg ) ::printf("** %s",msg);
    ::printf("\n Element Number = %d\n", this->getTag() );
    ::printf("\n Number of nodes in a TwentyNodeBrick = %d\n",
                                              nodes_in_brick);
    ::printf("\n Determinant of Jacobian (! ==0 before comp.) = %f\n",
                                                  determinant_of_Jacobian);

    ::printf("Node numbers \n");
    ::printf(".....1.....2.....3.....4.....5.....6.....7.....8.....9.....0.....1.....2\n");
           for ( int i=0 ; i<nodes_in_brick ; i++ )
             //::printf("%6d",G_N_numbs[i]);
             ::printf( "%6d",connectedExternalNodes(i) );

	   ::printf("\n");
           //           for ( int j=8 ; j<20 ; j++ )
           //             ::printf("%6d",G_N_numbs[j]);   //// Commented by Xiaoyan
           ::printf("\n\n");

           //    ::printf("Node existance array \n");
           //           for ( int k=0 ; k<15 ; k++ )
           //             ::printf("%6d",node_existance[k]);	   // Commented by Xiaoyan
           ::printf("\n\n");

  }




//#############################################################################
void TwentyNodeBrick::reportPAK(char * msg)
  {
    if ( msg ) ::printf("%s",msg);
    ::printf("%10d   ",  this->getTag());
    for ( int i=0 ; i<nodes_in_brick ; i++ )
       ::printf( "%6d",connectedExternalNodes(i) );
       //::printf("%6d",G_N_numbs[i]);

    printf("\n");
  }


//#############################################################################
void TwentyNodeBrick::reportpqtheta(int GP_numb)
  {
    short where = GP_numb-1;
    matpoint[where]->reportpqtheta("");
  }

//#############################################################################
void TwentyNodeBrick::reportLM(char * msg)
  {
    if ( msg ) ::printf("%s",msg);
    ::printf("Element # %d, LM->", this->get_Brick_Number());
    for (int count = 0 ; count < 24 ; count++)
      {
        ::printf(" %d", LM[count]);
      }
    ::printf("\n");

  }

//#############################################################################
void TwentyNodeBrick::reportTensor(char * msg)
  {
    //    if ( msg ) ::printf("** %s\n",msg);

    // special case for 8 nodes only
    // special case for 8 nodes only
    double r  = 0.0;
    double s  = 0.0;
    double t  = 0.0;

    short where = 0;

    // special case for 8 nodes only
    static const int dim[] = {3, 20}; // static-> see ARM pp289-290
    tensor NodalCoord(2, dim, 0.0);
    tensor matpointCoord(2, dim, 0.0);
    int h_dim[] = {60,3};   // Xiaoyan changed from {60,3} to {24,3} for 8 nodes
    tensor H(2, h_dim, 0.0);

    //for (int ncount = 1 ; ncount <= 8 ; ncount++ )
    ////  for (int ncount = 0 ; ncount <= 7 ; ncount++ )
    //  {
    //	//int global_node_number = get_global_number_of_node(ncount-1);
    //	// printf("global node num %d",global_node_number);
    //
    //    //   NodalCoord.val(1,ncount) = nodes[global_node_number].x_coordinate();
    //    //   NodalCoord.val(2,ncount) = nodes[global_node_number].y_coordinate();
    //    //   NodalCoord.val(3,ncount) = nodes[global_node_number].z_coordinate();
    //    // Xiaoyan changed to the following:  09/27/00
    //	Vector Coordinates = nodes[global_node_number].getCrds();
    //
    //    NodalCoord.val(1,ncount) = Coordinates(0);
    //    NodalCoord.val(2,ncount) = Coordinates(1);
    //    NodalCoord.val(3,ncount) = Coordinates(2);
    //printf("global point %d     x=%+.6e   y=%+.6e   z=%+.6e \n ", global_node_number,
    //                                                      NodalCoord.val(1,ncount),
    //						      NodalCoord.val(2,ncount),
    //						      NodalCoord.val(3,ncount));
    //}

    //Zhaohui using node pointers, which come from the Domain
    const Vector &nd1Crds = nd1Ptr->getCrds();
    const Vector &nd2Crds = nd2Ptr->getCrds();
    const Vector &nd3Crds = nd3Ptr->getCrds();
    const Vector &nd4Crds = nd4Ptr->getCrds();
    const Vector &nd5Crds = nd5Ptr->getCrds();
    const Vector &nd6Crds = nd6Ptr->getCrds();
    const Vector &nd7Crds = nd7Ptr->getCrds();
    const Vector &nd8Crds = nd8Ptr->getCrds();
    const Vector &nd9Crds = nd9Ptr->getCrds();
    const Vector &nd10Crds = nd10Ptr->getCrds();
    const Vector &nd11Crds = nd11Ptr->getCrds();
    const Vector &nd12Crds = nd12Ptr->getCrds();
    const Vector &nd13Crds = nd13Ptr->getCrds();
    const Vector &nd14Crds = nd14Ptr->getCrds();
    const Vector &nd15Crds = nd15Ptr->getCrds();
    const Vector &nd16Crds = nd16Ptr->getCrds();
    const Vector &nd17Crds = nd17Ptr->getCrds();
    const Vector &nd18Crds = nd18Ptr->getCrds();
    const Vector &nd19Crds = nd19Ptr->getCrds();
    const Vector &nd20Crds = nd20Ptr->getCrds();

    NodalCoord.val(1,1)=nd1Crds(0); NodalCoord.val(2,1)=nd1Crds(1); NodalCoord.val(3,1)=nd1Crds(2);
    NodalCoord.val(1,2)=nd2Crds(0); NodalCoord.val(2,2)=nd2Crds(1); NodalCoord.val(3,2)=nd2Crds(2);
    NodalCoord.val(1,3)=nd3Crds(0); NodalCoord.val(2,3)=nd3Crds(1); NodalCoord.val(3,3)=nd3Crds(2);
    NodalCoord.val(1,4)=nd4Crds(0); NodalCoord.val(2,4)=nd4Crds(1); NodalCoord.val(3,4)=nd4Crds(2);
    NodalCoord.val(1,5)=nd5Crds(0); NodalCoord.val(2,5)=nd5Crds(1); NodalCoord.val(3,5)=nd5Crds(2);
    NodalCoord.val(1,6)=nd6Crds(0); NodalCoord.val(2,6)=nd6Crds(1); NodalCoord.val(3,6)=nd6Crds(2);
    NodalCoord.val(1,7)=nd7Crds(0); NodalCoord.val(2,7)=nd7Crds(1); NodalCoord.val(3,7)=nd7Crds(2);
    NodalCoord.val(1,8)=nd8Crds(0); NodalCoord.val(2,8)=nd8Crds(1); NodalCoord.val(3,8)=nd8Crds(2);
    NodalCoord.val(1,9)=nd9Crds(0); NodalCoord.val(2,9)=nd8Crds(1); NodalCoord.val(3,9)=nd9Crds(2);
    NodalCoord.val(1,10)=nd10Crds(0); NodalCoord.val(2,10)=nd10Crds(1); NodalCoord.val(3,10)=nd10Crds(2);
    NodalCoord.val(1,11)=nd11Crds(0); NodalCoord.val(2,11)=nd11Crds(1); NodalCoord.val(3,11)=nd11Crds(2);
    NodalCoord.val(1,12)=nd12Crds(0); NodalCoord.val(2,12)=nd12Crds(1); NodalCoord.val(3,12)=nd12Crds(2);
    NodalCoord.val(1,13)=nd13Crds(0); NodalCoord.val(2,13)=nd13Crds(1); NodalCoord.val(3,13)=nd13Crds(2);
    NodalCoord.val(1,14)=nd14Crds(0); NodalCoord.val(2,14)=nd14Crds(1); NodalCoord.val(3,14)=nd14Crds(2);
    NodalCoord.val(1,15)=nd15Crds(0); NodalCoord.val(2,15)=nd15Crds(1); NodalCoord.val(3,15)=nd15Crds(2);
    NodalCoord.val(1,16)=nd16Crds(0); NodalCoord.val(2,16)=nd16Crds(1); NodalCoord.val(3,16)=nd16Crds(2);
    NodalCoord.val(1,17)=nd17Crds(0); NodalCoord.val(2,17)=nd17Crds(1); NodalCoord.val(3,17)=nd17Crds(2);
    NodalCoord.val(1,18)=nd18Crds(0); NodalCoord.val(2,18)=nd18Crds(1); NodalCoord.val(3,18)=nd18Crds(2);
    NodalCoord.val(1,19)=nd19Crds(0); NodalCoord.val(2,19)=nd19Crds(1); NodalCoord.val(3,19)=nd19Crds(2);
    NodalCoord.val(1,20)=nd20Crds(0); NodalCoord.val(2,20)=nd20Crds(1); NodalCoord.val(3,20)=nd20Crds(2);
		      

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates

               H = H_3D(r,s,t);

 	       for (int encount=1 ; encount <= nodes_in_brick; encount++ )
                //	       for (int encount=0 ; encount <= 7 ; encount++ )
	         {
                  //  matpointCoord.val(1,where+1) =+NodalCoord.val(1,where+1) * H.val(encount*3-2,1);
                  //  matpointCoord.val(2,where+1) =+NodalCoord.val(2,where+1) * H.val(encount*3-1,2);
                  //  matpointCoord.val(3,where+1) =+NodalCoord.val(3,where+1) * H.val(encount*3-0,3);
                  matpointCoord.val(1,where+1) +=NodalCoord.val(1,encount) * H.val(encount*3-2,1);
                  //::printf("-- NO nodal, H_val :%d %+.2e %+.2e %+.5e\n", encount,NodalCoord.val(1,encount),H.val(encount*3-2,1),matpointCoord.val(1,where+1) );
                  matpointCoord.val(2,where+1) +=NodalCoord.val(2,encount) * H.val(encount*3-1,2);
                  matpointCoord.val(3,where+1) +=NodalCoord.val(3,encount) * H.val(encount*3-0,3);

		  }

    ::printf("gauss point# %d   %+.6e %+.6e %+.6e \n", where+1,
                                                       matpointCoord.val(1,where+1),
                                                       matpointCoord.val(2,where+1),
                                                       matpointCoord.val(3,where+1));

    //matpoint[where].reportTensor("");


              }
          }
      }

 }


////#############################################################################

//#############################################################################
//void TwentyNodeBrick::reportTensor(char * msg)
// ZHaohui added to print gauss point coord. to file fp

void TwentyNodeBrick::reportTensorF(FILE * fp)
  {
    //if ( msg ) ::printf("** %s\n",msg);

    // special case for 8 nodes only
    // special case for 8 nodes only
    double r  = 0.0;
    double s  = 0.0;
    double t  = 0.0;

    short where = 0;

    // special case for 8 nodes only
    static const int dim[] = {3, 20}; // static-> see ARM pp289-290
    tensor NodalCoord(2, dim, 0.0);
    tensor matpointCoord(2, dim, 0.0);
    int h_dim[] = {60,3};  // Xiaoyan changed from {60,3} to {24,3} for 8 nodes

    tensor H(2, h_dim, 0.0);

    //for (int ncount = 1 ; ncount <= 8 ; ncount++ )
    //  // for (int ncount = 0 ; ncount <= 7 ; ncount++ )
    //  {
    //	int global_node_number = get_global_number_of_node(ncount-1);
    //	// printf("global node num %d",global_node_number);
    //
    //    //        NodalCoord.val(1,ncount) = nodes[global_node_number].x_coordinate();
    //    //        NodalCoord.val(2,ncount) = nodes[global_node_number].y_coordinate();
    //    //        NodalCoord.val(3,ncount) = nodes[global_node_number].z_coordinate();
    //    // Xiaoyan changed to the following:  09/27/00
    //	Vector Coordinates = nodes[global_node_number].getCrds();
    //    NodalCoord.val(1,ncount) = Coordinates(0);
    //    NodalCoord.val(2,ncount) = Coordinates(1);
    //    NodalCoord.val(3,ncount) = Coordinates(2);
    //printf("global point %d     x=%+.6e   y=%+.6e   z=%+.6e \n ", global_node_number,
    //                                                      NodalCoord.val(1,ncount),
    //						      NodalCoord.val(2,ncount),
    //						      NodalCoord.val(3,ncount));
    //  }

    //Zhaohui using node pointers, which come from the Domain
    const Vector &nd1Crds = nd1Ptr->getCrds();
    const Vector &nd2Crds = nd2Ptr->getCrds();
    const Vector &nd3Crds = nd3Ptr->getCrds();
    const Vector &nd4Crds = nd4Ptr->getCrds();
    const Vector &nd5Crds = nd5Ptr->getCrds();
    const Vector &nd6Crds = nd6Ptr->getCrds();
    const Vector &nd7Crds = nd7Ptr->getCrds();
    const Vector &nd8Crds = nd8Ptr->getCrds();
    const Vector &nd9Crds  =  nd9Ptr->getCrds();
    const Vector &nd10Crds = nd10Ptr->getCrds();
    const Vector &nd11Crds = nd11Ptr->getCrds();
    const Vector &nd12Crds = nd12Ptr->getCrds();
    const Vector &nd13Crds = nd13Ptr->getCrds();
    const Vector &nd14Crds = nd14Ptr->getCrds();
    const Vector &nd15Crds = nd15Ptr->getCrds();
    const Vector &nd16Crds = nd16Ptr->getCrds();
    const Vector &nd17Crds = nd17Ptr->getCrds();
    const Vector &nd18Crds = nd18Ptr->getCrds();
    const Vector &nd19Crds = nd19Ptr->getCrds();
    const Vector &nd20Crds = nd20Ptr->getCrds();

    NodalCoord.val(1,1)=nd1Crds(0); NodalCoord.val(2,1)=nd1Crds(1); NodalCoord.val(3,1)=nd1Crds(2);
    NodalCoord.val(1,2)=nd2Crds(0); NodalCoord.val(2,2)=nd2Crds(1); NodalCoord.val(3,2)=nd2Crds(2);
    NodalCoord.val(1,3)=nd3Crds(0); NodalCoord.val(2,3)=nd3Crds(1); NodalCoord.val(3,3)=nd3Crds(2);
    NodalCoord.val(1,4)=nd4Crds(0); NodalCoord.val(2,4)=nd4Crds(1); NodalCoord.val(3,4)=nd4Crds(2);
    NodalCoord.val(1,5)=nd5Crds(0); NodalCoord.val(2,5)=nd5Crds(1); NodalCoord.val(3,5)=nd5Crds(2);
    NodalCoord.val(1,6)=nd6Crds(0); NodalCoord.val(2,6)=nd6Crds(1); NodalCoord.val(3,6)=nd6Crds(2);
    NodalCoord.val(1,7)=nd7Crds(0); NodalCoord.val(2,7)=nd7Crds(1); NodalCoord.val(3,7)=nd7Crds(2);
    NodalCoord.val(1,8)=nd8Crds(0); NodalCoord.val(2,8)=nd8Crds(1); NodalCoord.val(3,8)=nd8Crds(2);
    NodalCoord.val(1,9)=nd9Crds(0); NodalCoord.val(2,9)=nd9Crds(1); NodalCoord.val(3,9)=nd9Crds(2);
    NodalCoord.val(1,10)=nd10Crds(0); NodalCoord.val(2,10)=nd10Crds(1); NodalCoord.val(3,10)=nd10Crds(2);
    NodalCoord.val(1,11)=nd11Crds(0); NodalCoord.val(2,11)=nd11Crds(1); NodalCoord.val(3,11)=nd11Crds(2);
    NodalCoord.val(1,12)=nd12Crds(0); NodalCoord.val(2,12)=nd12Crds(1); NodalCoord.val(3,12)=nd12Crds(2);
    NodalCoord.val(1,13)=nd13Crds(0); NodalCoord.val(2,13)=nd13Crds(1); NodalCoord.val(3,13)=nd13Crds(2);
    NodalCoord.val(1,14)=nd14Crds(0); NodalCoord.val(2,14)=nd14Crds(1); NodalCoord.val(3,14)=nd14Crds(2);
    NodalCoord.val(1,15)=nd15Crds(0); NodalCoord.val(2,15)=nd15Crds(1); NodalCoord.val(3,15)=nd15Crds(2);
    NodalCoord.val(1,16)=nd16Crds(0); NodalCoord.val(2,16)=nd16Crds(1); NodalCoord.val(3,16)=nd16Crds(2);
    NodalCoord.val(1,17)=nd17Crds(0); NodalCoord.val(2,17)=nd17Crds(1); NodalCoord.val(3,17)=nd17Crds(2);
    NodalCoord.val(1,18)=nd18Crds(0); NodalCoord.val(2,18)=nd18Crds(1); NodalCoord.val(3,18)=nd18Crds(2);
    NodalCoord.val(1,19)=nd19Crds(0); NodalCoord.val(2,19)=nd19Crds(1); NodalCoord.val(3,19)=nd19Crds(2);
    NodalCoord.val(1,20)=nd20Crds(0); NodalCoord.val(2,20)=nd20Crds(1); NodalCoord.val(3,20)=nd20Crds(2);

    for( short GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
      {
        r = get_Gauss_p_c( r_integration_order, GP_c_r );
        for( short GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
          {
            s = get_Gauss_p_c( s_integration_order, GP_c_s );
            for( short GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
              {
                t = get_Gauss_p_c( t_integration_order, GP_c_t );
                // this short routine is supposed to calculate position of
                // Gauss point from 3D array of short's
                where =
                ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;
                // derivatives of local coordinates with respect to local coordinates

               H = H_3D(r,s,t);

 	       for (int encount=1 ; encount <= nodes_in_brick ; encount++ )
                //	       for (int encount=0 ; encount <= 7 ; encount++ )
	       {
                  //  matpointCoord.val(1,where+1) =+NodalCoord.val(1,where+1) * H.val(encount*3-2,1);
                  //  matpointCoord.val(2,where+1) =+NodalCoord.val(2,where+1) * H.val(encount*3-1,2);
                  //  matpointCoord.val(3,where+1) =+NodalCoord.val(3,where+1) * H.val(encount*3-0,3);
                  matpointCoord.val(1,where+1) +=NodalCoord.val(1,encount) * H.val(encount*3-2,1);
                  //::printf("-- NO nodal, H_val :%d %+.2e %+.2e %+.5e\n", encount,NodalCoord.val(1,encount),H.val(encount*3-2,1),matpointCoord.val(1,where+1) );
                  matpointCoord.val(2,where+1) +=NodalCoord.val(2,encount) * H.val(encount*3-1,2);
                  matpointCoord.val(3,where+1) +=NodalCoord.val(3,encount) * H.val(encount*3-0,3);

	       }

    fprintf(fp, "gauss point# %d   %+.6e %+.6e %+.6e \n", where+1,
                                                          matpointCoord.val(1,where+1),
                                                          matpointCoord.val(2,where+1),
                                                          matpointCoord.val(3,where+1));

    //matpoint[where].reportTensor("");


              }
          }
      }

 }

//=============================================================================
//  The following are come from FourNodeQuad.cc	 Xiaoyan 07/06/00
//  The following are come from FourNodeQuad.cc	 Xiaoyan 07/06/00
//  The following are come from FourNodeQuad.cc	 Xiaoyan 07/06/00
//=============================================================================


//=============================================================================
int TwentyNodeBrick::getNumExternalNodes () const
{
    return nodes_in_brick;  //changed from 4 to 8 Xiaoyan 07/06/00
}


//=============================================================================
const ID& TwentyNodeBrick::getExternalNodes ()
{
    return connectedExternalNodes;
}

//=============================================================================
int TwentyNodeBrick::getNumDOF ()
{
    return 3*nodes_in_brick;  //Changed from 2*4=8 to 3*8=24 Xiaoyan 07/06/00
}

//=============================================================================
void TwentyNodeBrick::setDomain (Domain *theDomain)
{
    // Check Domain is not null - invoked when object removed from a domain
    if (theDomain == 0) {
	nd1Ptr = 0;
	nd2Ptr = 0;
	nd3Ptr = 0;
	nd4Ptr = 0;
	nd5Ptr = 0;
	nd6Ptr = 0;
	nd7Ptr = 0;
	nd8Ptr = 0;
	nd9Ptr	= 0;
        nd10Ptr = 0;
        nd11Ptr = 0;
        nd12Ptr = 0;
        nd13Ptr = 0;
        nd14Ptr = 0;
        nd15Ptr = 0;
        nd16Ptr = 0;
        nd17Ptr = 0;
        nd18Ptr = 0;
        nd19Ptr = 0;
        nd20Ptr = 0;
    }

    //Added if-else for found a bug when trying removeElement from theDomain  07-19-2001 Zhaohui
    else {
      int Nd1 = connectedExternalNodes(0);
      int Nd2 = connectedExternalNodes(1);
      int Nd3 = connectedExternalNodes(2);
      int Nd4 = connectedExternalNodes(3);
      //Xiaoyan added 5-8  07/06/00

      int Nd5 = connectedExternalNodes(4);
      int Nd6 = connectedExternalNodes(5);
      int Nd7 = connectedExternalNodes(6);
      int Nd8 = connectedExternalNodes(7);
      int Nd9  = connectedExternalNodes( 8);
      int Nd10 = connectedExternalNodes( 9);
      int Nd11 = connectedExternalNodes(10);
      int Nd12 = connectedExternalNodes(11);
      int Nd13 = connectedExternalNodes(12);
      int Nd14 = connectedExternalNodes(13);
      int Nd15 = connectedExternalNodes(14);
      int Nd16 = connectedExternalNodes(15);
      int Nd17 = connectedExternalNodes(16);
      int Nd18 = connectedExternalNodes(17);
      int Nd19 = connectedExternalNodes(18);
      int Nd20 = connectedExternalNodes(19);					 


      nd1Ptr = theDomain->getNode(Nd1);
      nd2Ptr = theDomain->getNode(Nd2);
      nd3Ptr = theDomain->getNode(Nd3);
      nd4Ptr = theDomain->getNode(Nd4);
      nd5Ptr = theDomain->getNode(Nd5);
      nd6Ptr = theDomain->getNode(Nd6);
      nd7Ptr = theDomain->getNode(Nd7);
      nd8Ptr = theDomain->getNode(Nd8);
      nd9Ptr = theDomain->getNode(Nd9);
      nd10Ptr = theDomain->getNode(Nd10);
      nd11Ptr = theDomain->getNode(Nd11);
      nd12Ptr = theDomain->getNode(Nd12);
      nd13Ptr = theDomain->getNode(Nd13);
      nd14Ptr = theDomain->getNode(Nd14);
      nd15Ptr = theDomain->getNode(Nd15);
      nd16Ptr = theDomain->getNode(Nd16);
      nd17Ptr = theDomain->getNode(Nd17);
      nd18Ptr = theDomain->getNode(Nd18);
      nd19Ptr = theDomain->getNode(Nd19);
      nd20Ptr = theDomain->getNode(Nd20);

      if (nd1Ptr  == 0 || nd2Ptr  == 0 || nd3Ptr  == 0 || nd4Ptr  == 0 ||
          nd5Ptr  == 0 || nd6Ptr  == 0 || nd7Ptr  == 0 || nd8Ptr  == 0 ||
	  nd9Ptr  == 0 || nd10Ptr == 0 || nd11Ptr == 0 || nd12Ptr == 0 ||
          nd13Ptr == 0 || nd14Ptr == 0 || nd15Ptr == 0 || nd16Ptr == 0 ||
          nd17Ptr == 0 || nd18Ptr == 0 || nd19Ptr == 0 || nd20Ptr == 0 ) {

      	g3ErrorHandler->fatal("FATAL ERROR TwentyNodeBrick (tag: %d), node not found in domain",
      		this->getTag());

      	return;
      }

      int dofNd1 = nd1Ptr->getNumberDOF();
      int dofNd2 = nd2Ptr->getNumberDOF();
      int dofNd3 = nd3Ptr->getNumberDOF();
      int dofNd4 = nd4Ptr->getNumberDOF();
      int dofNd5 = nd5Ptr->getNumberDOF();
      int dofNd6 = nd6Ptr->getNumberDOF();
      int dofNd7 = nd7Ptr->getNumberDOF();
      int dofNd8 = nd8Ptr->getNumberDOF();
      int dofNd9 = nd9Ptr->getNumberDOF();
      int dofNd10 = nd10Ptr->getNumberDOF();
      int dofNd11 = nd11Ptr->getNumberDOF();
      int dofNd12 = nd12Ptr->getNumberDOF();
      int dofNd13 = nd13Ptr->getNumberDOF();
      int dofNd14 = nd14Ptr->getNumberDOF();
      int dofNd15 = nd15Ptr->getNumberDOF();
      int dofNd16 = nd16Ptr->getNumberDOF();
      int dofNd17 = nd17Ptr->getNumberDOF();
      int dofNd18 = nd18Ptr->getNumberDOF();
      int dofNd19 = nd19Ptr->getNumberDOF();
      int dofNd20 = nd20Ptr->getNumberDOF();

      if (dofNd1  != 3 || dofNd2  != 3 || dofNd3  != 3 || dofNd4  != 3 ||  
          dofNd5  != 3 || dofNd6  != 3 || dofNd7  != 3 || dofNd8  != 3 ||
          dofNd9  != 3 || dofNd10 != 3 || dofNd11 != 3 || dofNd12 != 3 ||
          dofNd13 != 3 || dofNd14 != 3 || dofNd15 != 3 || dofNd16 != 3 ||
          dofNd17 != 3 || dofNd18 != 3 || dofNd19 != 3 || dofNd20 != 3 ) {
      	g3ErrorHandler->fatal("FATAL ERROR TwentyNodeBrick (tag: %d), has differing number of DOFs at its nodes",
      		this->getTag());

      	return;
      }
      this->DomainComponent::setDomain(theDomain);
    }
}

//=============================================================================
int TwentyNodeBrick::commitState ()
{
    // int order = theQuadRule->getOrder();     // Commented by Xiaoyan

    int i;
    //int j, k;      // Xiaoyan added k for three dimension
    int retVal = 0;

    // Loop over the integration points and commit the material states
    int count  = r_integration_order* s_integration_order * t_integration_order;
    //for (i = 0; i < r_integration_order; i++)		    // Xiaoyan chaneged order to
    //  for (j = 0; j < s_integration_order; j++)	    // r_integration_order,
    //							    // s_integration_order, and
    //	    for (k = 0; k < t_integration_order; k++)	    // added t_integration_order,
    //         retVal += (GaussPtheMaterial[i][j][k]).commitState();

    Vector pp = getResistingForce();

    //if ( this->getTag() == 1 || this->getTag() == 700)
    //{
      for (i = 0; i < count; i++)
      //for (i = 0; i < 27; i++)
      {
         retVal += matpoint[i]->commitState();
         //if (i == 4 && strcmp(matpoint[i]->matmodel->getType(),"Template3Dep") == 0)
         stresstensor st;
	 stresstensor prin;
         straintensor stn;
         straintensor stnprin;

         st = matpoint[i]->getStressTensor();
       	 prin = st.principal();
         stn = matpoint[i]->getStrainTensor();
       	 stnprin = stn.principal();
         /*
	 cerr << "\nGauss Point: " << i << endln;
	 cerr << "sigma11: "<< st.cval(1, 1) << " "<< st.cval(1, 2) << " " << st.cval(1, 3) << endln;
	 cerr << "sigma21: "<< st.cval(2, 1) << " "<< st.cval(2, 2) << " " << st.cval(2, 3) << endln;
 	 cerr << "sigma31: "<< st.cval(3, 1) << " "<< st.cval(3, 2) << " " << st.cval(3, 3) << endln << endln;
	 */
	 //cerr << "strain11: "<< stn.cval(1, 1) << " "<< stn.cval(1, 2) << " " << stn.cval(1, 3) << endln;
	 //cerr << "strain21: "<< stn.cval(2, 1) << " "<< stn.cval(2, 2) << " " << stn.cval(2, 3) << endln;
 	 //cerr << "strain31: "<< stn.cval(3, 1) << " "<< stn.cval(3, 2) << " " << stn.cval(3, 3) << endln;

	 double  p = -1*( prin.cval(1, 1)+ prin.cval(2, 2) +prin.cval(3, 3) )/3.0;
	 double  ev = -1*( stnprin.cval(1, 1)+ stnprin.cval(2, 2) + stnprin.cval(3, 3) )/3.0;
	 //cerr << "   " << p;

	 //if (p < 0)
	 //  cout  << "gs pnt:" << i << "  p="<< p;


	 double q;
	 //if ( fabs(prin.cval(1, 1) - prin.cval(2, 2) ) <=  0.0001 )
      	 if ( fabs(prin.cval(1, 1) - prin.cval(2, 2) ) <=  0.001 )
      	 {
      	     q = prin.cval(1, 1) - prin.cval(3, 3);
      	     //cerr << "1 = 2";
      	 }
      	 else
      	     q = prin.cval(3, 3) - prin.cval(1, 1);

	 //Triaxial compr.  fabs
      	 //cerr << "     " << st.cval(2, 3); //tau_yz
	 //cerr << "     " << q;
	 ////----cerr << "     " << fabs(q);

      	 //cerr << "     " << ev << endln;

//out22Jan2001	 if (strcmp(matpoint[i]->matmodel->getType(),"Template3Dep") == 0)
//out22Jan2001          {
//out22Jan2001       	  st = ( ((Template3Dep *)(matpoint[i]->matmodel))->getEPS())->getStress();
//out22Jan2001       	  prin = st.principal();
//out22Jan2001 	 }
//out22Jan2001 	 else
//out22Jan2001 	 {
//out22Jan2001        	  st = matpoint[i]->getStressTensor();
//out22Jan2001       	  prin = st.principal();
//out22Jan2001
//out22Jan2001 	 }

	  //double  p = st.p_hydrostatic();
	  //double  p = -1*( prin.cval(1, 1)+ prin.cval(2, 2) +prin.cval(3, 3) )/3.0;
      	  //cerr << "\n " << prin.cval(1, 1) << "   " << prin.cval(2, 2) << "  " <<  prin.cval(3, 3) << endln;
          //if ( getTag() == 960)
          //cerr << " El= " << getTag() << " , p    " << p << endln;

	  //printf(stderr, " Gauss Point i = %d ", (i+1));
	  //printf(stderr, " Gauss Point i = %d ", (i+1));


          //if ( p < 0 )
	  //{
	  //  cerr << getTag();
	  //  cerr << " ***p  =    " << p << endln;
	  //}
      	  //J2D
      	  //cerr << "        " << st.q_deviatoric();

      	  //double q;
      	  //if ( fabs(prin.cval(1, 1) - prin.cval(2, 2) ) <=  0.0001 )
      	  //{
      	  //    q = prin.cval(1, 1) - prin.cval(3, 3);
      	  //    //cerr << "1 = 2";
      	  //}
      	  //else
      	  //    q = prin.cval(3, 3) - prin.cval(1, 1);

      	  //Triaxial compr.
      	  //cerr << "        " << q;
         //}
      }

      //cout << " at elements " << this->getTag() << endln;


      //output nodal force
      //cerr << "    " << pp(2) << endln;
    //}
    return retVal;
}

//=============================================================================
int TwentyNodeBrick::revertToLastCommit ()
{
  //  int order = theQuadRule->getOrder();	// Commented by Xiaoyan
    int i;
    //int j, k;     // Xiaoyan added k for three dimension
    int retVal = 0;

    // Loop over the integration points and revert to last committed material states
    int count  = r_integration_order* s_integration_order * t_integration_order;
    //for (i = 0; i < r_integration_order; i++)		   // Xiaoyan chaneged order to
    //	for (j = 0; j < s_integration_order; j++)	   // r_integration_order,
    //	    for (k = 0; k < t_integration_order; k++)	   // s_integration_order, and
		      					   // added t_integration_order,
	    //retVal += (theMaterial[i][j][k]).revertToLastCommit();

    for (i = 0; i < count; i++)
       retVal += matpoint[i]->revertToLastCommit();


    return retVal;
}

//=============================================================================
int TwentyNodeBrick::revertToStart ()
{
    int i;     // Xiaoyan added k for three dimension
    int retVal = 0;

    // Loop over the integration points and revert to last committed material states
    //for (i = 0; i < r_integration_order; i++)		   // Xiaoyan chaneged order to
    //	for (j = 0; j < s_integration_order; j++)	   // r_integration_order,
    //	    for (k = 0; k < t_integration_order; k++)	   // s_integration_order, and
							   // added t_integration_order,
    //	    retVal += (theMaterial[i][j][k]).revertToLastCommit();

    int count  = r_integration_order* s_integration_order * t_integration_order;

    for (i = 0; i < count; i++)
       retVal += matpoint[i]->revertToStart();


    return retVal;

    // Loop over the integration points and revert to initial material states
   }


//=============================================================================
const Matrix &TwentyNodeBrick::getTangentStiff ()
{
     tensor stifftensor = getStiffnessTensor();
     int Ki=0;
     int Kj=0;

     for ( int i=1 ; i<=nodes_in_brick ; i++ )
     {
        for ( int j=1 ; j<=nodes_in_brick ; j++ )
        {
           for ( int k=1 ; k<=3 ; k++ )
           {
              for ( int l=1 ; l<=3 ; l++ )
              {
                 Ki = k+3*(i-1);
                 Kj = l+3*(j-1);
                 K( Ki-1 , Kj-1 ) = stifftensor.cval(i,k,l,j);
              }
           }
        }
     }

     //cout << " K " << K << endln;
     //K.Output(cout);
     return K;
}

//=============================================================================
const Matrix &TwentyNodeBrick::getSecantStiff ()
{
     return K;
}

//=============================================================================
const Matrix &TwentyNodeBrick::getDamp ()
{
     return C;
}

//=============================================================================
//Get lumped mass
const Matrix &TwentyNodeBrick::getMass ()
{
     tensor masstensor = getMassTensor();
     //int Ki=0;
     //int Kj=0;

     //double tot_mass = 0.0;
     //double diag_mass = 0.0;
     double column_mass;

     for ( int i=1 ; i<=nodes_in_brick*3 ; i++ )
     {
        column_mass = 0.0;
	for ( int j=1 ; j<=nodes_in_brick*3 ; j++ )
        {

	   //M( i-1 , j-1 ) = masstensor.cval(i,j);

	   column_mass += masstensor.cval(i,j);
	   M( i-1 , j-1 ) = 0;
	   //tot_mass += M( i-1 , j-1 );
	   //if (i == j)
	   //   diag_mass += M( i-1 , j-1 );
        }
	M( i-1 , i-1 ) = column_mass;

     }

     //cerr << " tot_mass= "<< tot_mass << " column_mass =" << column_mass << " diag_mass= " <<  diag_mass << endln;
     //cerr << "" << M.Output(cout);
     //cerr << " M " << M;

     return M;
}

//=============================================================================
//Get consistent mass
const Matrix &TwentyNodeBrick::getConsMass ()
{
     tensor masstensor = getMassTensor();
     //int Ki=0;
     //int Kj=0;

     //double tot_mass = 0.0;
     //double diag_mass = 0.0;
     //double column_mass;

     for ( int i=1 ; i<=nodes_in_brick*3 ; i++ )
     {
        //column_mass = 0.0;
	for ( int j=1 ; j<=nodes_in_brick*3 ; j++ )
        {
	   M( i-1 , j-1 ) = masstensor.cval(i,j);

	   //column_mass += masstensor.cval(i,j);
	   //M( i-1 , j-1 ) = 0;
	   //tot_mass += M( i-1 , j-1 );
	   //if (i == j)
	   //   diag_mass += M( i-1 , j-1 );
        }
	//M( i-1 , i-1 ) = column_mass;

     }

     //cerr << " tot_mass= "<< tot_mass << " column_mass =" << column_mass << " diag_mass= " <<  diag_mass << endln;
     //cerr << "" << M.Output(cout);
     //cerr << " M " << M;

     return M;
}

//=============================================================================
void TwentyNodeBrick::zeroLoad(void)
{
     Q.Zero();
}


//=============================================================================
int  TwentyNodeBrick::addLoad(const Vector &addLoad)
{
     if (addLoad.Size() != 60) {
     	g3ErrorHandler->warning("TwentyNodeBrick::addLoad %s\n",
     			"Vector not of correct size");
     	return -1;
     }

     // Add to the external nodal loads
     Q += addLoad;

     return 0;
}

//=============================================================================
int TwentyNodeBrick::addInertiaLoadToUnbalance(const Vector &accel)
{
	// Check for a quick return
	if (rho == 0.0)
		return 0;

	// Get R * accel from the nodes
	const Vector &Raccel1 = nd1Ptr->getRV(accel);
	const Vector &Raccel2 = nd2Ptr->getRV(accel);
	const Vector &Raccel3 = nd3Ptr->getRV(accel);
	const Vector &Raccel4 = nd4Ptr->getRV(accel);
	const Vector &Raccel5 = nd5Ptr->getRV(accel);
	const Vector &Raccel6 = nd6Ptr->getRV(accel);
	const Vector &Raccel7 = nd7Ptr->getRV(accel);
	const Vector &Raccel8 = nd8Ptr->getRV(accel);
	const Vector &Raccel9 = nd9Ptr->getRV(accel);
	const Vector &Raccel10 = nd10Ptr->getRV(accel);
	const Vector &Raccel11 = nd11Ptr->getRV(accel);
	const Vector &Raccel12 = nd12Ptr->getRV(accel);
	const Vector &Raccel13 = nd13Ptr->getRV(accel);
	const Vector &Raccel14 = nd14Ptr->getRV(accel);
	const Vector &Raccel15 = nd15Ptr->getRV(accel);
	const Vector &Raccel16 = nd16Ptr->getRV(accel);
	const Vector &Raccel17 = nd17Ptr->getRV(accel);
	const Vector &Raccel18 = nd18Ptr->getRV(accel);
	const Vector &Raccel19 = nd19Ptr->getRV(accel);
	const Vector &Raccel20 = nd20Ptr->getRV(accel);

    if (3 != Raccel1.Size()  || 3 != Raccel2.Size()  || 3 != Raccel3.Size()  || 3 != Raccel4.Size() ||
        3 != Raccel5.Size()  || 3 != Raccel6.Size()  || 3 != Raccel7.Size()  || 3 != Raccel8.Size() ||
        3 != Raccel9.Size()  || 3 != Raccel10.Size() || 3 != Raccel11.Size() || 3 != Raccel12.Size()||
        3 != Raccel13.Size() || 3 != Raccel14.Size() || 3 != Raccel15.Size() || 3 != Raccel16.Size()||
        3 != Raccel17.Size() || 3 != Raccel18.Size() || 3 != Raccel19.Size() || 3 != Raccel20.Size()   ){
	// Xiaoyan changed 2 to 3 and added Racce15-18  09/27/00
		g3ErrorHandler->warning("TwentyNodeBrick::addInertiaLoadToUnbalance %s\n",
				"matrix and vector sizes are incompatable");
		return -1;
    }

	static Vector ra(60);  // Changed form 8 to 24(3*8)  Xiaoyan 09/27/00

	ra( 0) = Raccel1(0);
	ra( 1) = Raccel1(1);
	ra( 2) = Raccel1(2);
	ra( 3) = Raccel2(0);
	ra( 4) = Raccel2(1);
	ra( 5) = Raccel2(2);
	ra( 6) = Raccel3(0);
	ra( 7) = Raccel3(1);
	ra( 8) = Raccel3(2);
	ra( 9) = Raccel4(0);
	ra(10) = Raccel4(1);
	ra(11) = Raccel4(2);
    	ra(12) = Raccel5(0);
	ra(13) = Raccel5(1);
	ra(14) = Raccel5(2);
	ra(15) = Raccel6(0);
	ra(16) = Raccel6(1);
	ra(17) = Raccel6(2);
	ra(18) = Raccel7(0);
	ra(19) = Raccel7(1);
	ra(20) = Raccel7(2);
	ra(21) = Raccel8(0);
	ra(22) = Raccel8(1);
	ra(23) = Raccel8(2);
	ra(24) = Raccel9(0);
 	ra(25) = Raccel9(1);
	ra(26) = Raccel9(2);
	ra(27) = Raccel10(0);
	ra(28) = Raccel10(1);
	ra(29) = Raccel10(2);
	ra(30) = Raccel11(0);
	ra(31) = Raccel11(1);
	ra(32) = Raccel11(2);
	ra(33) = Raccel12(0);
	ra(34) = Raccel12(1);
	ra(35) = Raccel12(2);
	ra(36) = Raccel13(0);
	ra(37) = Raccel13(1);
	ra(38) = Raccel13(2);
	ra(39) = Raccel14(0);
	ra(40) = Raccel14(1);
	ra(41) = Raccel14(2);
	ra(42) = Raccel15(0);
	ra(43) = Raccel15(1);
	ra(44) = Raccel15(2);
	ra(45) = Raccel16(0);
	ra(46) = Raccel16(1);
	ra(47) = Raccel16(2);
	ra(48) = Raccel17(0);
	ra(49) = Raccel17(1);
	ra(50) = Raccel17(2);
	ra(51) = Raccel18(0);
	ra(52) = Raccel18(1);
	ra(53) = Raccel18(2);
	ra(54) = Raccel19(0);
	ra(55) = Raccel19(1);
	ra(56) = Raccel19(2);
	ra(57) = Raccel20(0);
	ra(58) = Raccel20(1);
	ra(59) = Raccel20(2);


    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    // Mass matrix is computed in setDomain()

    //double column_mass = 0;
    //for (int i = 0; i < 24; i++)
    //   column_mass += M(1,i);
    //column_mass = column_mass/3.0;

    //cerr << " addInerti... column_mass " << column_mass << endln;

    for (int i = 0; i < nodes_in_brick*3; i++)
		Q(i) += -M(i,i)*ra(i);

    return 0;
}

//=============================================================================
const Vector TwentyNodeBrick::FormEquiBodyForce(void)
{
    Vector bforce(60);

    // Check for a quick return
    //cerr << "rho " << rho << endln;
    if (rho == 0.0)
    	return bforce;

    Vector ba(60);

    ba( 0) = bf(0);
    ba( 1) = bf(1);
    ba( 2) = bf(2);
    ba( 3) = bf(0);
    ba( 4) = bf(1);
    ba( 5) = bf(2);
    ba( 6) = bf(0);
    ba( 7) = bf(1);
    ba( 8) = bf(2);
    ba( 9) = bf(0);
    ba(10) = bf(1);
    ba(11) = bf(2);
    ba(15) = bf(0);
    ba(13) = bf(1);
    ba(14) = bf(2);
    ba(15) = bf(0);
    ba(16) = bf(1);
    ba(17) = bf(2);
    ba(18) = bf(0);
    ba(19) = bf(1);
    ba(20) = bf(2);
    ba(21) = bf(0);
    ba(22) = bf(1);
    ba(23) = bf(2);
    ba(24) = bf(0);
    ba(25) = bf(1);
    ba(26) = bf(2);
    ba(27) = bf(0);
    ba(28) = bf(1);
    ba(29) = bf(2);
    ba(30) = bf(0);
    ba(31) = bf(1);
    ba(32) = bf(2);
    ba(33) = bf(0);
    ba(34) = bf(1);
    ba(35) = bf(2);
    ba(36) = bf(0);
    ba(37) = bf(1);
    ba(38) = bf(2);
    ba(39) = bf(0);
    ba(40) = bf(1);
    ba(41) = bf(2);
    ba(42) = bf(0);
    ba(43) = bf(1);
    ba(44) = bf(2);
    ba(45) = bf(0);
    ba(46) = bf(1);
    ba(47) = bf(2);
    ba(48) = bf(0);
    ba(49) = bf(1);
    ba(50) = bf(2);
    ba(51) = bf(0);
    ba(52) = bf(1);
    ba(53) = bf(2);
    ba(54) = bf(0);
    ba(55) = bf(1);
    ba(56) = bf(2);
    ba(57) = bf(0);
    ba(58) = bf(1);
    ba(59) = bf(2);

    //Form equivalent body force
    bforce.addMatrixVector(0.0, M, ba, 1.0);
    //cerr << " ba " << ba;
    //cerr << " M " << M;
    //if (getTag() == 886)
    //cerr << " @@@@@ FormEquiBodyForce  " << bforce;

    return bforce;
}

//=============================================================================
// Setting initial E according to the initial pressure p
//void TwentyNodeBrick::setInitE(void)
//{
//    //Get the coors of each node
//
//    const Vector &nd1Crds = nd1Ptr->getCrds();
//    const Vector &nd2Crds = nd2Ptr->getCrds();
//    const Vector &nd3Crds = nd3Ptr->getCrds();
//    const Vector &nd4Crds = nd4Ptr->getCrds();
//    const Vector &nd5Crds = nd5Ptr->getCrds();
//    const Vector &nd6Crds = nd6Ptr->getCrds();
//    const Vector &nd7Crds = nd7Ptr->getCrds();
//    const Vector &nd8Crds = nd8Ptr->getCrds();
//
//    //dir is the ID for vertial direction, e.g. 1 means x-dir is vertical...
//    double Zavg = nd1Crds( dir-1)+
//    		   nd2Crds( dir-1)+
//    		   nd3Crds( dir-1)+
//    		   nd4Crds( dir-1)+
//    		   nd5Crds( dir-1)+
//    		   nd6Crds( dir-1)+
//    		   nd7Crds( dir-1)+
//    		   nd8Crds( dir-1);
//    Zavg = Zavg / 8;
//
//    //Estimate the pressure at that depth
//    double sigma_v = (Zavg - surflevel) * rho * 9.81; //units in SI system
//    double ko = 0.5;
//    double p_est = sigma_v*( 2.0*ko+1.0)/3.0;
//    //cerr << " Initial P " << p_est << endln;
//
//    int i;
//
//    // Loop over the integration points and set the initial material state
//    int count  = r_integration_order* s_integration_order * t_integration_order;
//
//    //For elastic-isotropic material
//    if (strcmp(matpoint[i]->matmodel->getType(),"ElasticIsotropic3D") == 0)
//    {
//       for (i = 0; i < count; i++)
//           (matpoint[i]->matmodel)->setElasticStiffness( p_est );
//    }
//
//    //return ;
//}


//=============================================================================
const Vector &TwentyNodeBrick::getResistingForce ()
{
    int force_dim[] = {20,3};
    tensor nodalforces(2,force_dim,0.0);

    nodalforces = nodal_forces();

    //converting nodalforce tensor to vector
    for (int i = 0; i< nodes_in_brick; i++)
      for (int j = 0; j < 3; j++)
	P(i *3 + j) = nodalforces.cval(i+1, j+1);

    //cerr << "P" << P;
    //cerr << "Q" << Q;

    P = P - Q;

    //cerr << "P-Q" << P;
    return P;
}

//=============================================================================
const Vector &TwentyNodeBrick::getResistingForceIncInertia ()
{
	// Check for a quick return
	if (rho == 0.0)
		return this->getResistingForce();

	//cerr << "Node555 trialDisp " << nd1Ptr->getTrialDisp();

	const Vector &accel1 = nd1Ptr->getTrialAccel();
        //cout << "\nnode accel " << nd1Ptr->getTag() << " x " << accel1(0) <<" y "<< accel1(1) << " z "<< accel1(2) << endln;

	const Vector &accel2 = nd2Ptr->getTrialAccel();
        //cout << "node accel " << nd2Ptr->getTag() << " x " << accel2(0) <<" y "<< accel2(1) << " z "<< accel2(2) << endln;

	const Vector &accel3 = nd3Ptr->getTrialAccel();
        //cout << "node accel " << nd3Ptr->getTag() << " x " << accel3(0) <<" y "<< accel3(1) << " z "<< accel3(2) << endln;

	const Vector &accel4 = nd4Ptr->getTrialAccel();
        //cout << "node accel " << nd4Ptr->getTag() << " x " << accel4(0) <<" y "<< accel4(1) << " z "<< accel4(2) << endln;

        // Xiaoyan added the following four 09/27/00
	const Vector &accel5 = nd5Ptr->getTrialAccel();
        //cout << "node accel " << nd5Ptr->getTag() << " x " << accel5(0) <<" y "<< accel5(1) << " z "<< accel5(2) << endln;

	const Vector &accel6 = nd6Ptr->getTrialAccel();
        //cout << "node accel " << nd6Ptr->getTag() << " x " << accel6(0) <<" y "<< accel6(1) << " z "<< accel6(2) << endln;

	const Vector &accel7 = nd7Ptr->getTrialAccel();
        //cout << "node accel " << nd7Ptr->getTag() << " x " << accel7(0) <<" y "<< accel7(1) << " z "<< accel7(2) << endln;

	const Vector &accel8 = nd8Ptr->getTrialAccel();
        //cout << "node accel " << nd8Ptr->getTag() << " x " << accel8(0) <<" y "<< accel8(1) << " z "<< accel8(2) << endln;

	const Vector &accel9 = nd9Ptr->getTrialAccel();
        //cout << "node accel " << nd9Ptr->getTag() << " x " << accel9(0) <<" y "<< accel9(1) << " z "<< accel9(2) << endln;

	const Vector &accel10 = nd10Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel11 = nd11Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel12 = nd12Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel13 = nd13Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel14 = nd14Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel15 = nd15Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel16 = nd16Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel17 = nd17Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel18 = nd18Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel19 = nd19Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;

	const Vector &accel20 = nd20Ptr->getTrialAccel();
        //cout << "node accel " << nd10Ptr->getTag() << " x " << accel10(0) <<" y "<< accel10(1) << " z "<< accel10(2) << endln;


	static Vector a(60);  // originally 8

	a( 0) = accel1(0);
	a( 1) = accel1(1);
	a( 2) = accel1(2);
	a( 3) = accel2(0);
	a( 4) = accel2(1);
	a( 5) = accel2(2);
	a( 6) = accel3(0);
	a( 7) = accel3(1);
	a( 8) = accel3(2);
	a( 9) = accel4(0);
	a(10) = accel4(1);
	a(11) = accel4(2);
    	a(15) = accel5(0);
	a(13) = accel5(1);
	a(14) = accel5(2);
	a(15) = accel6(0);
	a(16) = accel6(1);
	a(17) = accel6(2);
	a(18) = accel7(0);
	a(19) = accel7(1);
	a(20) = accel7(2);
	a(21) = accel8(0);
	a(22) = accel8(1);
	a(23) = accel8(2);
	a(24) = accel9(0);
 	a(25) = accel9(1);
	a(26) = accel9(2);
	a(27) = accel10(0);
	a(28) = accel10(1);
	a(29) = accel10(2);
	a(30) = accel11(0);
	a(31) = accel11(1);
	a(32) = accel11(2);
	a(33) = accel12(0);
	a(34) = accel12(1);
	a(35) = accel12(2);
	a(36) = accel13(0);
	a(37) = accel13(1);
	a(38) = accel13(2);
	a(39) = accel14(0);
	a(40) = accel14(1);
	a(41) = accel14(2);
	a(42) = accel15(0);
	a(43) = accel15(1);
	a(44) = accel15(2);
	a(45) = accel16(0);
	a(46) = accel16(1);
	a(47) = accel16(2);
	a(48) = accel17(0);
	a(49) = accel17(1);
	a(50) = accel17(2);
	a(51) = accel18(0);
	a(52) = accel18(1);
	a(53) = accel18(2);
	a(54) = accel19(0);
	a(55) = accel19(1);
	a(56) = accel19(2);
	a(57) = accel20(0);
	a(58) = accel20(1);
	a(59) = accel20(2);

	// Compute the current resisting force
	this->getResistingForce();

	// Take advantage of lumped mass matrix
	// Mass matrix is computed in setDomain()
	//cout << " M_ii \n";

        //double column_mass = 0;
        //for (int i = 0; i < 24; i++)
        //   column_mass += M(1,i);
        //column_mass = column_mass/3.0;

	for (int i = 0; i < 60; i++)
	{
	   P(i) += M(i,i)*a(i);
	   //cout << " " << M(i, i);
	}
	//cout << endln;
	//cerr << "P+=Ma" << P<< endl;
	return P;
}

//=============================================================================
int TwentyNodeBrick::sendSelf (int commitTag, Channel &theChannel)
{
     // Not implemtented yet
     return 0;
}

//=============================================================================
int TwentyNodeBrick::recvSelf (int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
{
     // Not implemtented yet
     return 0;
}


//=============================================================================
int TwentyNodeBrick::displaySelf (Renderer &theViewer, int displayMode, float fact)
{
    //needs more work...Zhaohui 08-19-2001

    // first determine the end points of the quad based on
    // the display factor (a measure of the distorted image)
    // store this information in 4 3d vectors v1 through v4
    const Vector &end1Crd = nd1Ptr->getCrds();
    const Vector &end2Crd = nd2Ptr->getCrds();
    const Vector &end3Crd = nd3Ptr->getCrds();
    const Vector &end4Crd = nd4Ptr->getCrds();
    const Vector &end5Crd = nd5Ptr->getCrds();
    const Vector &end6Crd = nd6Ptr->getCrds();
    const Vector &end7Crd = nd7Ptr->getCrds();
    const Vector &end8Crd = nd8Ptr->getCrds();

    const Vector &end1Disp = nd1Ptr->getDisp();
    const Vector &end2Disp = nd2Ptr->getDisp();
    const Vector &end3Disp = nd3Ptr->getDisp();
    const Vector &end4Disp = nd4Ptr->getDisp();
    const Vector &end5Disp = nd5Ptr->getDisp();
    const Vector &end6Disp = nd6Ptr->getDisp();
    const Vector &end7Disp = nd7Ptr->getDisp();
    const Vector &end8Disp = nd8Ptr->getDisp();

    static Vector v1(3);
    static Vector v2(3);
    static Vector v3(3);
    static Vector v4(3);
    static Vector v5(3);
    static Vector v6(3);
    static Vector v7(3);
    static Vector v8(3);

    for (int i = 0; i < 2; i++)
    {
    	v1(i) = end1Crd(i) + end1Disp(i)*fact;
    	v2(i) = end2Crd(i) + end2Disp(i)*fact;
    	v3(i) = end3Crd(i) + end3Disp(i)*fact;
    	v4(i) = end4Crd(i) + end4Disp(i)*fact;
    	v5(i) = end5Crd(i) + end5Disp(i)*fact;
    	v6(i) = end6Crd(i) + end6Disp(i)*fact;
    	v7(i) = end7Crd(i) + end7Disp(i)*fact;
    	v8(i) = end8Crd(i) + end8Disp(i)*fact;
    }

    int error = 0;

    error += theViewer.drawLine (v1, v2, 1.0, 1.0);
    error += theViewer.drawLine (v2, v3, 1.0, 1.0);
    error += theViewer.drawLine (v3, v4, 1.0, 1.0);
    error += theViewer.drawLine (v4, v1, 1.0, 1.0);

    error += theViewer.drawLine (v5, v6, 1.0, 1.0);
    error += theViewer.drawLine (v6, v7, 1.0, 1.0);
    error += theViewer.drawLine (v7, v8, 1.0, 1.0);
    error += theViewer.drawLine (v8, v5, 1.0, 1.0);

    error += theViewer.drawLine (v1, v5, 1.0, 1.0);
    error += theViewer.drawLine (v2, v6, 1.0, 1.0);
    error += theViewer.drawLine (v3, v7, 1.0, 1.0);
    error += theViewer.drawLine (v4, v8, 1.0, 1.0);

    return error;

}

//=============================================================================
void TwentyNodeBrick::Print(ostream &s, int flag)
{
    //report(" TwentyNodeBrick ");
    s << "TwentyNodeBrick, element id:  " << this->getTag() << endl;
    s << "Connected external nodes:  " << connectedExternalNodes;

    int total_number_of_Gauss_points = r_integration_order*
                                       s_integration_order*
                                       t_integration_order;
    if ( total_number_of_Gauss_points != 0 )
      {
	   nd1Ptr->Print(cout);
	   nd2Ptr->Print(cout);
	   nd3Ptr->Print(cout);
	   nd4Ptr->Print(cout);
	   nd5Ptr->Print(cout);
	   nd6Ptr->Print(cout);
           nd7Ptr->Print(cout);
	   nd8Ptr->Print(cout);
	   nd9Ptr->Print(cout);
	   nd10Ptr->Print(cout);
	   nd11Ptr->Print(cout);
	   nd12Ptr->Print(cout);
	   nd13Ptr->Print(cout);
	   nd14Ptr->Print(cout);
	   nd15Ptr->Print(cout);
	   nd16Ptr->Print(cout);
	   nd17Ptr->Print(cout);
	   nd18Ptr->Print(cout);
	   nd19Ptr->Print(cout);
	   nd20Ptr->Print(cout);

    }
    s << "Element mass density:  " << rho << endl << endl;
    s << "Material model:  " << endl;

    for( int GP_c_r = 1 ; GP_c_r <= r_integration_order ; GP_c_r++ )
    {
      for( int GP_c_s = 1 ; GP_c_s <= s_integration_order ; GP_c_s++ )
      {
        for( int GP_c_t = 1 ; GP_c_t <= t_integration_order ; GP_c_t++ )
        {
           // this short routine is supposed to calculate position of
           // Gauss point from 3D array of short's
           short where =
           ((GP_c_r-1)*s_integration_order+GP_c_s-1)*t_integration_order+GP_c_t-1;

           s << "\n where = " << where << endln;
           s << " GP_c_r= " << GP_c_r << "GP_c_s = " << GP_c_s << " GP_c_t = " << GP_c_t << endln;
           matpoint[where]->report("Material Point\n");
           //GPstress[where].reportshort("stress at Gauss Point");
           //GPstrain[where].reportshort("strain at Gauss Point");
           //matpoint[where].report("Material model  at Gauss Point");
        }
      }
    }

}

//=============================================================================
Response * TwentyNodeBrick::setResponse (char **argv, int argc, Information &eleInformation)
{
    if (strcmp(argv[0],"force") == 0 || strcmp(argv[0],"forces") == 0)
		return new ElementResponse(this, 1, P);

    //else if (strcmp(argv[0],"stiff") == 0 || strcmp(argv[0],"stiffness") == 0)
    //		return new ElementResponse(this, 2, K);

    //========================================================
    else if (strcmp(argv[0],"plastify") == 0 || strcmp(argv[0],"plastified") == 0)
    {
       //checking if element plastified
       int count  = r_integration_order* s_integration_order * t_integration_order;
       straintensor pl_stn;
       int plastify = 0;
       
       for (int i = 0; i < count; i++) {
         pl_stn = matpoint[i]->getPlasticStrainTensor();
	 double  p_plastc = pl_stn.p_hydrostatic();
	 
	 if (  fabs(p_plastc) > 0 ) { 
	    plastify = 1;
	    break;
	 }
       }
  
       return new ElementResponse(this, 2, plastify);    
    } 
	/*else if (strcmp(argv[0],"material") == 0 || strcmp(argv[0],"integrPoint") == 0) {
		int pointNum = atoi(argv[1]);
		if (pointNum > 0 && pointNum <= 4)
			return theMaterial[pointNum-1]->setResponse(&argv[2], argc-2, eleInfo);
	    else
			return 0;
	}*/

    // otherwise response quantity is unknown for the quad class
    else
 	return 0;
}
//=============================================================================

int TwentyNodeBrick::getResponse (int responseID, Information &eleInfo)
{
       switch (responseID) {

	   case 1:
	   	return eleInfo.setVector(this->getResistingForce());

	   case 2:
       	      {
		//checking if element plastified
       	        int count  = r_integration_order* s_integration_order * t_integration_order;
       	        straintensor pl_stn;
       	        int plastify = 0;
       	        
       	        for (int i = 0; i < count; i++) {
       	          pl_stn = matpoint[i]->getPlasticStrainTensor();
       	        	 double  p_plastc = pl_stn.p_hydrostatic();
       	        	 
       	        	 if (  fabs(p_plastc) > 0 ) { 
       	        	    plastify = 1;
       	        	    break;
       	        	 }
       	        }
	   	eleInfo.setInt( plastify );
		return plastify;
	   
	      }
	   /*case 2:
	   	return eleInfo.setMatrix(this->getTangentStiff());
	    */
	   default:
	   	return -1;
	}
     //return 0;
}




//=============================================================================

//const Matrix&
//TwentyNodeBrick::getTangentStiff ()
//{
//	int order = theQuadRule->getOrder();
//	const Vector &intPt = theQuadRule->getIntegrPointCoords();
//	const Vector &intWt = theQuadRule->getIntegrPointWeights();
//
//	const Vector &disp1 = nd1Ptr->getTrialDisp();
//        const Vector &disp2 = nd2Ptr->getTrialDisp();
//	const Vector &disp3 = nd3Ptr->getTrialDisp();
//        const Vector &disp4 = nd4Ptr->getTrialDisp();
//       // Xiaoyan added 5-8 07/06/00
//        const Vector &disp5 = nd5Ptr->getTrialDisp();
//        const Vector &disp6 = nd6Ptr->getTrialDisp();
//	const Vector &disp7 = nd7Ptr->getTrialDisp();
//        const Vector &disp8 = nd8Ptr->getTrialDisp();
//
//	static Vector u(24);	    //Changed from u(8) to u(24) Xiaoyn 07/06/00
//
//	u(0) = disp1(0);
//	u(1) = disp1(1);
//        u(2) = disp1(2);
//	u(3) = disp2(0);
//	u(4) = disp2(1);
//	u(5) = disp2(2);
//        u(6) = disp3(0);
//	u(7) = disp3(1);
//	u(8) = disp3(2);
//	u(9) = disp4(0);
//	u(10) = disp4(1);
//	u(11) = disp4(2);
//	u(15) = disp5(0);
//	u(13) = disp5(1);
//	u(14) = disp5(2);
//	u(15) = disp6(0);
//	u(16) = disp6(1);
//	u(17) = disp6(2);
//	u(18) = disp7(0);
//	u(19) = disp7(1);
//	u(20) = disp7(2);
//	u(21) = disp8(0);
//	u(22) = disp8(1);
//	u(23) = disp8(2);


//	static Vector eps (6);		  // Changed eps(3) to eps(6) Xiaoyan 07/06/00

//	K.Zero();

//	// Loop over the integration points
//	for (int i = 0; i < order; i++)
//	{
//		for (int j = 0; j < order; j++)
//		{
//
//			// Determine Jacobian for this integration point
//			this->setJacobian (intPt(i), intPt(j));
//
//			// Interpolate strains
//			this->formBMatrix (intPt(i), intPt(j));
//			eps = B*u;
//
//			// Set the material strain
//			(theMaterial[i][j])->setTrialStrain (eps);
//
//			// Get the material tangent
//			const Matrix &D = (theMaterial[i][j])->getTangent();
//
//			// Form the Jacobian of the coordinate transformation
//			double detJ = this->formDetJ (intPt(i), intPt(j));
//
//			// Perform numerical integration
//			K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
//		}
//	}
//
//	K = K * thickness;
//
//	return K;
//}

//const Matrix&
//TwentyNodeBrick::getSecantStiff ()
//{
//	return K;
//}

//Commented by Xiaoyan     Use the form like Brick3d
//const Matrix & TwentyNodeBrick::getDamp ()
//{
//	return C;
//}
// Commented by Xiaoyan 08/04/00

//const Matrix&
//TwentyNodeBrick::getMass ()
//{
//	int order = theQuadRule->getOrder();
//	const Vector &intPt = theQuadRule->getIntegrPointCoords();
//	const Vector &intWt = theQuadRule->getIntegrPointWeights();
//
//	M.Zero();
//
//	int i, j;
//
//	// Loop over the integration points
//	for (i = 0; i < order; i++)
//	{
//		for (j = 0; j < order; j++)
//		{
//			// Determine Jacobian for this integration point
//			this->setJacobian (intPt(i), intPt(j));
//
//			// Interpolate strains
//			this->formNMatrix (intPt(i), intPt(j));
//
//			// Form the Jacobian of the coordinate transformation
//			double detJ = this->formDetJ (intPt(i), intPt(j));
//
//			// Perform numerical integration
//			M = M + (N^ N) * intWt(i)*intWt(j) * detJ;
//		}
//	}
//
//	M = M * thickness * rho;
//
//	// Lumped mass ... can be optional
//	for (i = 0; i < 24; i++)	     // Changed 8 to 24  Xiaoyan 07/06/00
//	{
//		double sum = 0.0;
//		for (j = 0; j < 24; j++)    // Changed 8 to 24  Xiaoyan 07/06/00
//		{
//			sum += M(i,j);
//			M(i,j) = 0.0;
//		}
//		M(i,i) = sum;
//	}
//
//	return M;
//}
//
//const Vector&
//TwentyNodeBrick::getResistingForce ()
//{
//	int order = theQuadRule->getOrder();
//	const Vector &intPt = theQuadRule->getIntegrPointCoords();
//	const Vector &intWt = theQuadRule->getIntegrPointWeights();
//
//	const Vector &disp1 = nd1Ptr->getTrialDisp();
//        const Vector &disp2 = nd2Ptr->getTrialDisp();
//	const Vector &disp3 = nd3Ptr->getTrialDisp();
//        const Vector &disp4 = nd4Ptr->getTrialDisp();
//	//6-8 added by Xiaoyan 07/06/00
//	const Vector &disp5 = nd5Ptr->getTrialDisp();
//        const Vector &disp6 = nd6Ptr->getTrialDisp();
//	const Vector &disp7 = nd7Ptr->getTrialDisp();
//        const Vector &disp8 = nd8Ptr->getTrialDisp();
//
//
//	static Vector u(24);	    //Changed from u(8) to u(24) Xiaoyn 07/06/00
//
//	u(0) = disp1(0);
//	u(1) = disp1(1);
//        u(2) = disp1(2);
//	u(3) = disp2(0);
//	u(4) = disp2(1);
//	u(5) = disp2(2);
//        u(6) = disp3(0);
//	u(7) = disp3(1);
//	u(8) = disp3(2);
//	u(9) = disp4(0);
//	u(10) = disp4(1);
//	u(11) = disp4(2);
//	u(15) = disp5(0);
//	u(13) = disp5(1);
//	u(14) = disp5(2);
//	u(15) = disp6(0);
//	u(16) = disp6(1);
//	u(17) = disp6(2);
//	u(18) = disp7(0);
//	u(19) = disp7(1);
//	u(20) = disp7(2);
//	u(21) = disp8(0);
//	u(22) = disp8(1);
//	u(23) = disp8(2);
//
//	eps (6);      //Changed eps(3) to eps(6) Xiaoyan 07/06/00
//
//	P.Zero();
//
//	// Loop over the integration points
//	for (int i = 0; i < order; i++)
//	{
//		for (int j = 0; j < order; j++)
//		{
//			// Determine Jacobian for this integration point
//			this->setJacobian (intPt(i), intPt(j));
//
//			// Interpolate strains
//			this->formBMatrix (intPt(i), intPt(j));
//			eps = B*u;
//
//			// Set the material strain
//			(theMaterial[i][j])->setTrialStrain (eps);
//
//			// Get material stress response
//			const Vector &sigma = (theMaterial[i][j])->getStress();
//
//			// Form the Jacobian of the coordinate transformation
//			double detJ = this->formDetJ (intPt(i), intPt(j));
//
//			// Perform numerical integration
//			P = P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
//		}
//	}
//
//	P = P * thickness * -1;
//
//	return P;
//}
//
//const Vector&
//TwentyNodeBrick::getResistingForceIncInertia ()
//{
//	// Yet to implement
//	return P;
//}
//
//
//
//void
//TwentyNodeBrick::Print (ostream &s, int flag)
//{
//	s << "TwentyNodeBrick, element id:  " << this->getTag() << endl;
//	s << "Connected external nodes:  " << connectedExternalNodes;
//	s << "Material model:  " << theMaterial[0][0]->getType() << endl;
//	s << "Element thickness:  " << thickness << endl;
//	s << "Element mass density:  " << rho << endl << endl;
//}
//
//
//int
//TwentyNodeBrick::displaySelf (Renderer &theViewer, int displayMode, float fact)
//{
    // first determine the end points of the quad based on
    // the display factor (a measure of the distorted image)
    // store this information in 2 3d vectors v1 and v2
//        const Vector &end1Crd = nd1Ptr->getCrds();
//        const Vector &end2Crd = nd2Ptr->getCrds();
//	const Vector &end3Crd = nd3Ptr->getCrds();
//	const Vector &end4Crd = nd4Ptr->getCrds();
//	// 5-8 were added by Xiaoyan
//        const Vector &end5Crd = nd5Ptr->getCrds();
//        const Vector &end6Crd = nd6Ptr->getCrds();
//	const Vector &end7Crd = nd7Ptr->getCrds();
//	const Vector &end8Crd = nd8Ptr->getCrds();
////---------------------------------------------------------------
//    	const Vector &end1Disp = nd1Ptr->getDisp();
//	const Vector &end2Disp = nd2Ptr->getDisp();
//	const Vector &end3Disp = nd3Ptr->getDisp();
//	const Vector &end4Disp = nd4Ptr->getDisp();
//
	// 5-8 were added by Xiaoyan
//        const Vector &end5Disp = nd5Ptr->getDisp();
//	const Vector &end6Disp = nd6Ptr->getDisp();
//	const Vector &end7Disp = nd7Ptr->getDisp();
//	const Vector &end8Disp = nd8Ptr->getDisp();
//
//	Vector v1(3);
//	Vector v2(3);
//	Vector v3(3);
//	Vector v4(3);
//	//5-8 added by Xiaoyan 07/06/00
//	Vector v5(3);
//	Vector v6(3);
//	Vector v7(3);
//	Vector v8(3);
//
//	for (int i = 0; i < 3; i++)	    //Changed from i<2 to i<3, Xiaonyan 07/06/00
//	{
//		v1(i) = end1Crd(i) + end1Disp(i)*fact;
//		v2(i) = end2Crd(i) + end2Disp(i)*fact;
//		v3(i) = end3Crd(i) + end3Disp(i)*fact;
//		v4(i) = end4Crd(i) + end4Disp(i)*fact;
//
//		//5-8 added by Xiaoyan 07/06/00
//   		v5(i) = end5Crd(i) + end1Disp(i)*fact;
//		v6(i) = end6Crd(i) + end2Disp(i)*fact;
//		v7(i) = end7Crd(i) + end3Disp(i)*fact;
//		v8(i) = end8Crd(i) + end4Disp(i)*fact;
//	}
//	int error = 0;
//
//	error += theViewer.drawLine (v1, v2, 1.0, 1.0);
//	error += theViewer.drawLine (v2, v3, 1.0, 1.0);
//	error += theViewer.drawLine (v3, v4, 1.0, 1.0);
//	error += theViewer.drawLine (v4, v5, 1.0, 1.0);   // 5-8 added by Xiaoyan 07/06/00
//	error += theViewer.drawLine (v5, v6, 1.0, 1.0);
//	error += theViewer.drawLine (v6, v7, 1.0, 1.0);
//	error += theViewer.drawLine (v7, v8, 1.0, 1.0);
//	error += theViewer.drawLine (v8, v1, 1.0, 1.0);
//
//	return error;
//}
// The following are all commented by  Xiaoyan. We use the Brick3D to form these

//
//void
//TwentyNodeBrick::formNMatrix (double r, double s,double t)
////Changed xi, eta to r,s and added t Xiaoyan  07/06/00
//{
//	N.Zero();
//
////	N(0,0) = N(1,1) = 0.25*(1.0-xi)*(1.0-eta);		// N_1
//// 	N(0,2) = N(1,3) = 0.25*(1.0+xi)*(1.0-eta);		// N_2
////	N(0,4) = N(1,5) = 0.25*(1.0+xi)*(1.0+eta);		// N_3
////	N(0,6) = N(1,7) = 0.25*(1.0-xi)*(1.0+eta);		// N_4
//
////	Changed by Xiaoyan 07/06/00
// The shape functions have been changed from N(2,8) to N(3,24)
// I take the node order according to Bathe's book p344-345. Xiaoyan
//        N(0,0)=N(1,1)=N(2,2)=1/8.*(1.0+r)*(1.0+s)*(1.0+t);
//	N(0,3)=N(1,4)=N(2,5)=1/8.*(1.0-r)*(1.0+s)*(1.0+t);
//	N(0,6)=N(1,7)=N(2,8)=1/8.*(1.0-r)*(1.0-s)*(1.0+t);
//	N(0,9)=N(1,10)=N(2,11)=1/8.*(1.0+r)*(1.0-s)*(1.0+t);
//	N(0,15)=N(1,13)=N(2,14)=1/8.*(1.0+r)*(1.0+s)*(1.0-t);
//	N(0,15)=N(1,16)=N(2,17)=1/8.*(1.0-r)*(1.0+s)*(1.0-t);
//	N(0,18)=N(1,19)=N(2,20)=1/8.*(1.0-r)*(1.0-s)*(1.0-t);
//	N(0,21)=N(1,22)=N(2,23)=1/8.*(1.0+r)*(1.0-s)*(1.0-t);
// }
//void
//TwentyNodeBrick::setJacobian (double r, double s, double t)
////Changed xi, eta to r,s and added t Xiaoyan 07/06/00
//{
//	const Vector &nd1Crds = nd1Ptr->getCrds();
//	const Vector &nd2Crds = nd2Ptr->getCrds();
//	const Vector &nd3Crds = nd3Ptr->getCrds();
//	const Vector &nd4Crds = nd4Ptr->getCrds();
//	// Xiaoyan added 5-8 07/06/00
//	const Vector &nd5Crds = nd5Ptr->getCrds();
//	const Vector &nd6Crds = nd6Ptr->getCrds();
//	const Vector &nd7Crds = nd7Ptr->getCrds();
//	const Vector &nd8Crds = nd8Ptr->getCrds();
//
////	J(0,0) = -nd1Crds(0)*(1.0-eta) + nd2Crds(0)*(1.0-eta) +
////				nd3Crds(0)*(1.0+eta) - nd4Crds(0)*(1.0+eta);
////
//	J(0,1) = -nd1Crds(0)*(1.0-xi) - nd2Crds(0)*(1.0+xi) +
//				nd3Crds(0)*(1.0+xi) + nd4Crds(0)*(1.0-xi);
//
//	J(1,0) = -nd1Crds(1)*(1.0-eta) + nd2Crds(1)*(1.0-eta) +
//				nd3Crds(1)*(1.0+eta) - nd4Crds(1)*(1.0+eta);
//
//	J(1,1) = -nd1Crds(1)*(1.0-xi) - nd2Crds(1)*(1.0+xi) +
//				nd3Crds(1)*(1.0+xi) + nd4Crds(1)*(1.0-xi);
//    	J = J * 0.25;
//
//	// For 3D problem Jacobi Matrix changed from J(2,2) to J(3,3)
//	// Xiaoyan  changed 07/06/00
//
//
//	J(0,0) = nd1Crds(0)*(1.0+s)*(1.0+t) - nd2Crds(0)*(1.0+s)*(1.0+t) -
//		 nd3Crds(0)*(1.0-s)*(1.0+t) + nd4Crds(0)*(1.0-s)*(1.0+t) +
//		 nd5Crds(0)*(1.0+s)*(1.0-t) - nd6Crds(0)*(1.0+s)*(1.0-t) -
//		 nd7Crds(0)*(1.0-s)*(1.0-t) + nd8Crds(0)*(1.0-s)*(1.0-t);
//
//	J(0,1) = nd1Crds(1)*(1.0+s)*(1.0+t) - nd2Crds(1)*(1.0+s)*(1.0+t) -
//		 nd3Crds(1)*(1.0-s)*(1.0+t) + nd4Crds(1)*(1.0-s)*(1.0+t) +
//		 nd5Crds(1)*(1.0+s)*(1.0-t) - nd6Crds(1)*(1.0+s)*(1.0-t) -
//		 nd7Crds(1)*(1.0-s)*(1.0-t) + nd8Crds(1)*(1.0-s)*(1.0-t);
//
//	J(0,2) = nd1Crds(2)*(1.0+s)*(1.0+t) - nd2Crds(2)*(1.0+s)*(1.0+t) -
//		 nd3Crds(2)*(1.0-s)*(1.0+t) + nd4Crds(2)*(1.0-s)*(1.0+t) +
//		 nd5Crds(2)*(1.0+s)*(1.0-t) - nd6Crds(2)*(1.0+s)*(1.0-t) -
//		 nd7Crds(2)*(1.0-s)*(1.0-t) + nd8Crds(2)*(1.0-s)*(1.0-t);
//
//	J(1,0) = nd1Crds(0)*(1.0+r)*(1.0+t) + nd2Crds(0)*(1.0-r)*(1.0+t) -
//		 nd3Crds(0)*(1.0-r)*(1.0+t) - nd4Crds(0)*(1.0+r)*(1.0+t) +
//		 nd5Crds(0)*(1.0+r)*(1.0-t) + nd6Crds(0)*(1.0-r)*(1.0-t) -
//		 nd7Crds(0)*(1.0-r)*(1.0-t) - nd8Crds(0)*(1.0+r)*(1.0-t);
//
//	J(1,1) = nd1Crds(1)*(1.0+r)*(1.0+t) + nd2Crds(1)*(1.0-r)*(1.0+t) -
//		 nd3Crds(1)*(1.0-r)*(1.0+t) - nd4Crds(1)*(1.0+r)*(1.0+t) +
//		 nd5Crds(1)*(1.0+r)*(1.0-t) + nd6Crds(1)*(1.0-r)*(1.0-t) -
//		 nd7Crds(1)*(1.0-r)*(1.0-t) - nd8Crds(1)*(1.0+r)*(1.0-t);
//
//        J(1,2) = nd1Crds(2)*(1.0+r)*(1.0+t) + nd2Crds(2)*(1.0-r)*(1.0+t) -
//		 nd3Crds(2)*(1.0-r)*(1.0+t) - nd4Crds(2)*(1.0+r)*(1.0+t) +
//		 nd5Crds(2)*(1.0+r)*(1.0-t) + nd6Crds(2)*(1.0-r)*(1.0-t) -
//		 nd7Crds(2)*(1.0-r)*(1.0-t) - nd8Crds(2)*(1.0+r)*(1.0-t);
//
//	J(2,0) = nd1Crds(0)*(1.0+r)*(1.0+s) + nd2Crds(0)*(1.0-r)*(1.0+s) +
//		 nd3Crds(0)*(1.0-r)*(1.0-s) + nd4Crds(0)*(1.0+r)*(1.0-s) -
//		 nd5Crds(0)*(1.0+r)*(1.0+s) - nd6Crds(0)*(1.0-r)*(1.0+s) -
//		 nd7Crds(0)*(1.0-r)*(1.0-s) - nd8Crds(0)*(1.0+r)*(1.0-s);
//
//	J(2,1) = nd1Crds(1)*(1.0+r)*(1.0+s) + nd2Crds(1)*(1.0-r)*(1.0+s) +
//		 nd3Crds(1)*(1.0-r)*(1.0-s) + nd4Crds(1)*(1.0+r)*(1.0-s) -
//		 nd5Crds(1)*(1.0+r)*(1.0+s) - nd6Crds(1)*(1.0-r)*(1.0+s) -
//		 nd7Crds(1)*(1.0-r)*(1.0-s) - nd8Crds(1)*(1.0+r)*(1.0-s);
//
//	J(2,2) = nd1Crds(2)*(1.0+r)*(1.0+s) + nd2Crds(2)*(1.0-r)*(1.0+s) +
//		 nd3Crds(2)*(1.0-r)*(1.0-s) + nd4Crds(2)*(1.0+r)*(1.0-s) -
//		 nd5Crds(2)*(1.0+r)*(1.0+s) - nd6Crds(2)*(1.0-r)*(1.0+s) -
//		 nd7Crds(2)*(1.0-r)*(1.0-s) - nd8Crds(2)*(1.0+r)*(1.0-s);
//
//	 J=J*0.155
//
//		// L = inv(J)  Changed from L(2,2) to L(3,3)  07/07/00
//
//	L(0,0)=-J(1,2)*J(2,1) + J(1,1)*J(2,2);
//	L(0.1)= J(0,2)*J(2,1) - J(0,1)*J(2,2);
//	L(0,3)=-J(0,2)*J(1,1) + J(0,1)*J(1,2);
//	L(1,0)= J(1,2)*J(2,0) - J(1,0)*J(2,2);
//	L(1,1)=-J(0,2)*J(2,0) + J(0,0)*J(2.2);
//	L(1,2)= J(0,2)*J(1,0) - J(0,0)*J(1,2);
//	L(2,0)=-J(1,1)*J(2,0) + J(1,0)*J(2,1);
//	L(2,1)= J(0,1)*J(2,0) - J(0,0)*J(2,1);
//	L(2,2)=-J(0,1)*J(1,0) + J(0,0)*J(1,1);
//	L=L/formDetJ(r,s,t)
//
//	L(0,0) = J(1,1);
//	L(1,0) = -J(0,1);
//	L(0,1) = -J(1,0);
//	L(1,1) = J(0,0);

//	L = L / formDetJ (xi, eta);
//}
//
//void
//TwentyNodeBrick::formBMatrix (double r, double s, double t)
////Changed xi, eta to r,s and added t Xiaoyan  07/06/00
//{
//    B.Zero();
//
//    //Changed by Xiaoyan 07/06/00
//    double L00 = L(0,0);
//    double L01 = L(0,1);
//    double L02 = L(0,1);
//    double L10 = L(1,0);
//    double L11 = L(1,1);
//    double L15 = L(1,2);
//    double L20 = L(2,0);
//    double L21 = L(2,1);
//    double L22 = L(2,2);
//
//    // See Cook, Malkus, Plesha p. 169 for the derivation of these terms
//    B(0,0) = L00*-0.25*(1.0-eta) + L01*-0.25*(1.0-xi);		// N_1,1
//    B(0,2) = L00*0.25*(1.0-eta) + L01*-0.25*(1.0+xi);		// N_2,1
//    B(0,4) = L00*0.25*(1.0+eta) + L01*0.25*(1.0+xi);		// N_3,1
//    B(0,6) = L00*-0.25*(1.0+eta) + L01*0.25*(1.0-xi);		// N_4,1
//
//    B(1,1) = L10*-0.25*(1.0-eta) + L11*-0.25*(1.0-xi);  	// N_1,2
//    B(1,3) = L10*0.25*(1.0-eta) + L11*-0.25*(1.0+xi);		// N_2,2
//    B(1,5) = L10*0.25*(1.0+eta) + L11*0.25*(1.0+xi);		// N_3,2
//    B(1,7) = L10*-0.25*(1.0+eta) + L11*0.25*(1.0-xi);		// N_4,2
//
//    B(2,0) = B(1,1);
//    B(2,1) = B(0,0);
//    B(2,2) = B(1,3);
//    B(2,3) = B(0,2);
//    B(2,4) = B(1,5);
//    B(2,5) = B(0,4);
//    B(2,6) = B(1,7);
//    B(2,7) = B(0,6);
//}
//
//
//
////The derivative  of shape function to r,s,t respectly.
//// For example dh1dr means dh1/dr etc. Xiaoyan  07/07/00
//double  dh1dr=0.155*(1+s)*(1+t);
//double  dh1ds=0.155*(1+r)*(1+t);
//double  dh1dt=0.155*(1+r)*(1+s);
//
//double  dh2dr=-0.155*(1+s)*(1+t);
//double  dh2ds=0.155*(1-r)*(1+t);
//double  dh2dt=0.155*(1-r)*(1+s);
//
//double  dh3dr=-0.155*(1-s)*(1+t);
//double  dh3ds=-0.155*(1-r)*(1+t);
//double  dh3dt=0.155*(1-r)*(1-s);
//
//double  dh4dr=0.155*(1-s)*(1+t);
//double  dh4ds=-0.155*(1+r)*(1+t);
//double  dh4dt=0.155*(1+r)*(1-s);
//
//double  dh5dr=0.155*(1+s)*(1-t);
//double  dh5ds=0.155*(1+r)*(1-t);
//double  dh5dt=-0.155*(1+r)*(1+s);
//
//double  dh6dr=-0.155*(1+s)*(1-t);
//double  dh6ds=0.155*(1-r)*(1-t);
//double  dh6dt=-0.155*(1-r)*(1+s);
//
//double  dh7dr=-0.155*(1-s)*(1-t);
//double  dh7ds=-0.155*(1-r)*(1-t);
//double  dh7dt=-0.155*(1-r)*(1-s);
//
//double  dh8dr=0.155*(1-s)*(1-t);
//double  dh8ds=-0.155*(1+r)*(1-t);
//double  dh8dt=-0.155*(1+r)*(1-s);
//
//// B Matrix B(6,24)
//B(0,0)=L00*dh1dr+L01*dh1ds+L02*dh1dt;
//B(0,3)=L00*dh2dr+L01*dh2ds+L02*dh2dt;
//B(0,6)=L00*dh3dr+L01*dh3ds+L02*dh3dt;
//B(0,9)=L00*dh4dr+L01*dh4ds+L02*dh4dt;
//B(0,15)=L00*dh5dr+L01*dh5ds+L02*dh5dt;
//B(0,15)=L00*dh6dr+L01*dh6ds+L02*dh6dt;
//B(0,18)=L00*dh7dr+L01*dh7ds+L02*dh7dt;
//B(0,21)=L00*dh8dr+L01*dh8ds+L02*dh8dt;
//
//B(1,1)=L10*dh1dr+L11*dh1ds+L15*dh1dt;
//B(1,4)=L10*dh2dr+L11*dh2ds+L15*dh2dt;
//B(1,7)=L10*dh3dr+L11*dh3ds+L15*dh3dt;
//B(1,10)=L10*dh4dr+L11*dh4ds+L15*dh4dt;
//B(1,13)=L10*dh5dr+L11*dh5ds+L15*dh5dt;
//B(1,16)=L10*dh6dr+L11*dh6ds+L15*dh6dt;
//B(1,19)=L10*dh7dr+L11*dh7ds+L15*dh7dt;
//B(1,22)=L10*dh8dr+L11*dh8ds+L15*dh8dt;
//
//B(2,2)=L20d*h1dr+L21*dh1ds+L22*dh1dt;
//B(2,5)=L20d*h2dr+L21*dh2ds+L22*dh2dt;
//B(2,8)=L20d*h3dr+L21*dh3ds+L22*dh3dt;
//B(2,11)=L20*dh4dr+L21*dh4ds+L22*dh4dt;
//B(2,14)=L20*dh5dr+L21*dh5ds+L22*dh5dt;
//B(2,17)=L20*dh6dr+L21*dh6ds+L22*dh6dt;
//B(2,20)=L20*dh7dr+L21*dh7ds+L22*dh7dt;
//B(2,23)=L20*dh8dr+L21*dh8ds+L22*dh8dt;
//
//B(3,0)=B(1,1);
//B(3,1)=B(0,0);
//B(3,3)=B(1,4);
//B(3,4)=B(0,3);
//B(3,6)=B(1,7);
//B(3,7)=B(0,6);
//B(3,9)=B(1,10);
//B(3,10)=B(0,9);
//B(3,15)=B(1,13);
//B(3,13)=B(0,15);
//B(3,15)=B(1,16);
//B(3,16)=B(0,15);
//B(3,18)=B(1,19);
//B(3,19)=B(0,18);
//B(3,21)=B(1,22);
//B(3,22)=B(0,21);
//
//B(4,1)=B(2,2);
//B(4,2)=B(1,1);
//B(4,4)=B(2,5);
//B(4,5)=B(1,4);
//B(4,7)=B(2,8);
//B(4,8)=B(1,7);
//B(4,10)=B(2,11);
//B(4,11)=B(1,10);
//B(4,13)=B(2,14);
//B(4,14)=B(1,13);
//B(4,16)=B(2,17);
//B(4,17)=B(1,16);
//B(4,19)=B(2,20);
//B(4,20)=B(1,19);
//B(4,21)=B(2,23);
//B(4,23)=B(1,22);
//
//B(5,0)=B(2,2);
//B(5,2)=B(0,0);
//B(5,3)=B(2,5);
//B(5,5)=B(0,3);
//B(5,6)=B(2,8);
//B(5,8)=B(0,6);
//B(5,9)=B(2,11);
//B(5,11)=B(0,9);
//B(5,15)=B(2,14);
//B(5,14)=B(0,15);
//B(5,15)=B(2,17);
//B(5,17)=B(0,15);
//B(5,18)=B(2,20);
//B(5,20)=B(2,18);
//B(5,21)=B(0,23);
//B(5,23)=B(0,21);
//
//B(3,3)= L00*dh2dr+L01*dh2ds+L02*dh2dt;
//B(3,6)= L00*dh3dr+L01*dh3ds+L02*dh3dt;
//B(3,9)= L00*dh4dr+L01*dh4ds+L02*dh4dt;
//B(3,15)=L00*dh5dr+L01*dh5ds+L02*dh5dt;
//B(3,15)=L00*dh6dr+L01*dh6ds+L02*dh6dt;
//B(3,18)=L00*dh7dr+L01*dh7ds+L02*dh7dt;
//B(3,21)=L00*dh8dr+L01*dh8ds+L02*dh8dt;
//double
//TwentyNodeBrick::formDetJ (double r, double s, double t)
//{
//    return J(0,0)*J(1,1)*J(2,2)+J(1,0)*J(2,1)*J(0,2)+J(2,0)*J(0,1)*J(1,2)
//         - J(2,0)*J(1,1)*J(0,2)-J(0,0)*J(2,1)*J(1,2)-J(0,1)*J(1,0)*J(2,2);
//}


double TwentyNodeBrick::get_Gauss_p_c(short order, short point_numb)
  {
//  Abscissae coefficient of the Gaussian quadrature formula
// starting from 1 not from 0
    static double Gauss_coordinates[7][7];

    Gauss_coordinates[1][1] = 0.0 ;
    Gauss_coordinates[2][1] = -0.577350269189626;
    Gauss_coordinates[2][2] = -Gauss_coordinates[2][1];
    Gauss_coordinates[3][1] = -0.774596669241483;
    Gauss_coordinates[3][2] = 0.0;
    Gauss_coordinates[3][3] = -Gauss_coordinates[3][1];
    Gauss_coordinates[4][1] = -0.861136311594053;
    Gauss_coordinates[4][2] = -0.339981043584856;
    Gauss_coordinates[4][3] = -Gauss_coordinates[4][2];
    Gauss_coordinates[4][4] = -Gauss_coordinates[4][1];
    Gauss_coordinates[5][1] = -0.906179845938664;
    Gauss_coordinates[5][2] = -0.538469310105683;
    Gauss_coordinates[5][3] = 0.0;
    Gauss_coordinates[5][4] = -Gauss_coordinates[5][2];
    Gauss_coordinates[5][5] = -Gauss_coordinates[5][1];
    Gauss_coordinates[6][1] = -0.932469514203152;
    Gauss_coordinates[6][2] = -0.661509386466265;
    Gauss_coordinates[6][3] = -0.238619186083197;
    Gauss_coordinates[6][4] = -Gauss_coordinates[6][3];
    Gauss_coordinates[6][5] = -Gauss_coordinates[6][2];
    Gauss_coordinates[6][6] = -Gauss_coordinates[6][1];

    return Gauss_coordinates[order][point_numb];
 }

double TwentyNodeBrick::get_Gauss_p_w(short order, short point_numb)
  {
//  Weight coefficient of the Gaussian quadrature formula
// starting from 1 not from 0
    static double Gauss_weights[7][7]; // static data ??

    Gauss_weights[1][1] = 2.0;
    Gauss_weights[2][1] = 1.0;
    Gauss_weights[2][2] = 1.0;
    Gauss_weights[3][1] = 0.555555555555556;
    Gauss_weights[3][2] = 0.888888888888889;
    Gauss_weights[3][3] = Gauss_weights[3][1];
    Gauss_weights[4][1] = 0.347854845137454;
    Gauss_weights[4][2] = 0.652145154862546;
    Gauss_weights[4][3] = Gauss_weights[4][2];
    Gauss_weights[4][4] = Gauss_weights[4][1];
    Gauss_weights[5][1] = 0.236926885056189;
    Gauss_weights[5][2] = 0.478628670499366;
    Gauss_weights[5][3] = 0.568888888888889;
    Gauss_weights[5][4] = Gauss_weights[5][2];
    Gauss_weights[5][5] = Gauss_weights[5][1];
    Gauss_weights[6][1] = 0.171324492379170;
    Gauss_weights[6][2] = 0.360761573048139;
    Gauss_weights[6][3] = 0.467913934572691;
    Gauss_weights[6][4] = Gauss_weights[6][3];
    Gauss_weights[6][5] = Gauss_weights[6][2];
    Gauss_weights[6][6] = Gauss_weights[6][1];

    return Gauss_weights[order][point_numb];
  }


#endif
