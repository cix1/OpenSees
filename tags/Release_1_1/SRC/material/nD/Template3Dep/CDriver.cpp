//
//================================================================================
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//#                                                                              #
//# PURPOSE:           Provide a platform for the implementation of elastoplastic#
//#                    Constutive model                                          #
//# CLASS:             CDriver                                                   #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                               #
//#                                                                              #
//#                                                                              #
//# DATE:              August 11 2000                                            #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# Short Explanation: CDriver is used to drive a material point to a new epstate#
//#                    (by  a strain increment currently) using defferent        #
//#                    integratition algorithms(explicit or implicit).     	 #
//#                                                                              #
//================================================================================
//

#ifndef CDRIVER_CPP
#define CDRIVER_CPP

#include "CDriver.h"

#define ITMAX 100

//================================================================================
// Initialize EPstate's hardening vars once material point is formed!
//================================================================================
//void ConstitutiveDriver::Init_Hardening_Vars(Template3Dep & MP ) {
//    MP.EL->Init();
//
//}


//================================================================================
// Predictor EPState by Forward, Backward, MidPoint Methods...
//================================================================================

EPState ConstitutiveDriver::PredictorEPState(straintensor & strain_increment,
                                             Template3Dep & material_point)
{

    EPState Predictor = ForwardEulerEPState( strain_increment, material_point);
    //EPState Predictor = SemiBackwardEulerEPState( strain_increment, material_point);
    return Predictor;
}

//================================================================================
// New EPState using Forward Euler Algorithm
//================================================================================
EPState ConstitutiveDriver::ForwardEulerEPState( straintensor &strain_increment, 
                                                 Template3Dep &material_point)
{
    EPState forwardEPS( *material_point.getEPS() ); 
    //cout <<"start eps: " <<   forwardEPS;
    //cout << "strain_increment " << strain_increment << endln;
    
    // Building elasticity tensor
    tensor Eep  = material_point.ElasticStiffnessTensor();
    tensor E  = material_point.ElasticStiffnessTensor();
    tensor D  = material_point.ElasticComplianceTensor();
    E.null_indices();
    D.null_indices();
    
    //Checking E and D
    //tensor ed = E("ijpq") * D("pqkl");
    //double edd = ed.trace(); // = 3.
    
    stresstensor stress_increment = E("ijpq") * strain_increment("pq");
    stress_increment.null_indices();
    //cout << " stress_increment: " << stress_increment << endln;
    	 
    EPState startEPS( *material_point.getEPS() );
    stresstensor start_stress = startEPS.getStress();
    start_stress.null_indices();
    //cout << "===== start_EPS =====: " << startEPS;
    
    double f_start = 0.0;
    double f_pred  = 0.0;
    
    EPState IntersectionEPS( startEPS );

    EPState ElasticPredictorEPS( startEPS );
    stresstensor elastic_predictor_stress = start_stress + stress_increment;
    ElasticPredictorEPS.setStress( elastic_predictor_stress );
    //cout << " Elastic_predictor_stress: " << elastic_predictor_stress << endln;
    
    f_start = material_point.getYS()->f( &startEPS );  
    //::printf("\n##############  f_start = %.10e  ",f_start);
    //cout << "\n#######  f_start = " << f_start;
    
    f_pred =  material_point.getYS()->f( &ElasticPredictorEPS );
    //::printf("##############  f_pred = %.10e\n\n",f_pred);
    //cout << "  #######  f_pred = " << f_pred << "\n";
    
    stresstensor intersection_stress = start_stress; // added 20april2000 for forward euler
    stresstensor elpl_start_stress = start_stress;
    stresstensor true_stress_increment = stress_increment;
    
    if ( f_start <= 0 && f_pred <= 0 )
      {
        //Updating elastic strain increment
        straintensor estrain = ElasticPredictorEPS.getElasticStrain();
        straintensor tstrain = ElasticPredictorEPS.getStrain();
        estrain = estrain + strain_increment;
        tstrain = tstrain + strain_increment;
        ElasticPredictorEPS.setElasticStrain( estrain );
        ElasticPredictorEPS.setStrain( tstrain );
        ElasticPredictorEPS.setdElasticStrain( strain_increment );
    
        //Evolve parameters like void ratio (e) according to elastic strain
        //double Delta_lambda = 0.0;
        //material_point.EL->UpdateVar( &ElasticPredictorEPS, 1);
        
        //cout <<" strain_increment.Iinvariant1() " << strain_increment.Iinvariant1() << endln;

        ElasticPredictorEPS.setEep(E);
        return ElasticPredictorEPS;
      }
    
    if ( f_start <= 0 && f_pred > 0 )
      {
        intersection_stress =
           yield_surface_cross( start_stress, elastic_predictor_stress, material_point);
        //cout  << "    start_stress: " <<  start_stress << endln;
        //cout  << "    Intersection_stress: " <<  intersection_stress << endln;
    
        IntersectionEPS.setStress( intersection_stress );
        //intersection_stress.reportshort("Intersection stress \n");
      
        elpl_start_stress = intersection_stress;
        //elpl_start_stress.reportshortpqtheta("elpl start stress AFTER \n");
      
        true_stress_increment = elastic_predictor_stress - elpl_start_stress;
        //true_stress_increment.null_indices();
    
      }
    
    
    forwardEPS.setStress( elpl_start_stress );
    
    //cout <<"elpl start eps: " <<   forwardEPS;
    //double f_cross =  material_point.YS->f( &forwardEPS );
    //cout << " #######  f_cross = " << f_cross << "\n";
    
    //set the initial value of D once the current stress hits the y.s. for Manzari-Dafalias Model
    //if ( f_start <= 0 && f_pred > 0 )
    //    material_point.EL->setInitD(&forwardEPS);
    //cout << " inside ConstitutiveDriver after setInitD " << forwardEPS;
    
    
    //  pulling out some tensor and double definitions
    //tensor dFods( 2, def_dim_2, 0.0);
    //tensor dQods( 2, def_dim_2, 0.0);
    stresstensor dFods;
    stresstensor dQods;
    //  stresstensor s;  // deviator
    tensor H( 2, def_dim_2, 0.0);
    tensor temp1( 2, def_dim_2, 0.0);
    tensor temp2( 2, def_dim_2, 0.0);
    double lower = 0.0;
    tensor temp3( 2, def_dim_2, 0.0);
    
    double Delta_lambda = 0.0;
    double h_s[4]       = {0.0, 0.0, 0.0, 0.0};
    double xi_s[4]      = {0.0, 0.0, 0.0, 0.0};
    stresstensor h_t[4];
    stresstensor xi_t[4];
    double hardMod_     = 0.0;
    
    //double Dq_ast   = 0.0;
    //double q_ast_entry = 0.0;
    //double q_ast = 0.0;
    
    stresstensor plastic_stress;
    straintensor plastic_strain;
    stresstensor elastic_plastic_stress;
    // ::printf("\n������������...... felpred = %lf\n",felpred);
    
    if ( f_pred >= 0 ) {
        

        dFods = material_point.getYS()->dFods( &forwardEPS );
        dQods = material_point.getPS()->dQods( &forwardEPS );

        //cout << "dF/ds" << dFods << endln;
        //cout << "dQ/ds" << dQods << endln;
    
        // Tensor H_kl  ( eq. 5.209 ) W.F. Chen
        H = E("ijkl")*dQods("kl");       //E_ijkl * R_kl
        H.null_indices();
        temp1 = dFods("ij") * E("ijkl"); // L_ij * E_ijkl
        temp1.null_indices();
        temp2 = temp1("ij")*dQods("ij"); // L_ij * E_ijkl * R_kl
        temp2.null_indices();
        lower = temp2.trace();
        
        // Evaluating the hardening modulus: sum of  (df/dq*) * qbar
	
	hardMod_ = 0.0;
	//Of 1st scalar internal vars
	if ( material_point.getELS1() ) {
	   h_s[0]  = material_point.getELS1()->h_s(&forwardEPS, material_point.getPS());
           xi_s[0] = material_point.getYS()->xi_s1( &forwardEPS );	   
   	   hardMod_ = hardMod_ + h_s[0] * xi_s[0];
	}

	//Of 2nd scalar internal vars
	if ( material_point.getELS2() ) {
	   h_s[1]  = material_point.getELS2()->h_s(&forwardEPS, material_point.getPS());
           xi_s[1] = material_point.getYS()->xi_s2( &forwardEPS );	   
   	   hardMod_ = hardMod_ + h_s[1] * xi_s[1];
	}

	//Of 3rd scalar internal vars
	if ( material_point.getELS3() ) {
	   h_s[2]  = material_point.getELS3()->h_s(&forwardEPS, material_point.getPS());
           xi_s[2] = material_point.getYS()->xi_s3( &forwardEPS );	   
   	   hardMod_ = hardMod_ + h_s[2] * xi_s[2];
	}

	//Of 4th scalar internal vars
	if ( material_point.getELS4() ) {
	   h_s[3]  = material_point.getELS4()->h_s(&forwardEPS, material_point.getPS());
           xi_s[3] = material_point.getYS()->xi_s4( &forwardEPS );	   
   	   hardMod_ = hardMod_ + h_s[3] * xi_s[3];
	}
	      
	//Of tensorial internal var
	// 1st tensorial var
	if ( material_point.getELT1() ) {
	   h_t[0]  = material_point.getELT1()->h_t(&forwardEPS, material_point.getPS());
	   xi_t[0] = material_point.getYS()->xi_t1( &forwardEPS );
           tensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 2nd tensorial var
	if ( material_point.getELT2() ) {
	   h_t[1]  = material_point.getELT2()->h_t(&forwardEPS, material_point.getPS());
	   xi_t[1] = material_point.getYS()->xi_t2( &forwardEPS );
           tensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 3rd tensorial var
	if ( material_point.getELT3() ) {
	   h_t[2]  = material_point.getELT3()->h_t(&forwardEPS, material_point.getPS());
	   xi_t[2] = material_point.getYS()->xi_t3( &forwardEPS );
           tensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 4th tensorial var
	if ( material_point.getELT4() ) {
	   h_t[3]  = material_point.getELT4()->h_t(&forwardEPS, material_point.getPS());
	   xi_t[3] = material_point.getYS()->xi_t4( &forwardEPS );
           tensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// Subtract accumulated hardMod_ from lower
        lower = lower - hardMod_;
        
        //Calculating Kp according to Kp = - (df/dq*) * qbar
        //double Kp = material_point.EL->getKp(&forwardEPS, norm_dQods);
        //Kp = 0.0;
        //cout << endln << ">>>>>>>>>   Lower = " << lower << endln;           
        //lower = lower + Kp;
        //cout << endln << ">>>>>>>>>    Kp = " << Kp << endln;           
        
        //cout << " stress_increment "<< stress_increment << endln;
        //cout << " true_stress_increment "<< true_stress_increment << endln;
    
        temp3 = dFods("ij") * true_stress_increment("ij"); // L_ij * E_ijkl * d e_kl (true ep strain increment)
        temp3.null_indices();
        //cout << " temp3.trace() " << temp3.trace() << endln;
        Delta_lambda = (temp3.trace())/lower;
        //cout << "Delta_lambda " <<  Delta_lambda << endln; 
        //if (Delta_lambda<0.0) Delta_lambda=0.0;

        plastic_stress = H("kl") * Delta_lambda;
        plastic_strain = dQods("kl") * Delta_lambda; // plastic strain increment
        plastic_stress.null_indices();
        plastic_strain.null_indices(); 
        //cout << "plastic_stress =   " << plastic_stress << endln;
        //cout << "plastic_strain =   " << plastic_strain << endln;
        //cout << "plastic_strain I1= " << plastic_strain.Iinvariant1() << endln;
        //cout << "plastic_strain vol " << Delta_lambda * ( forwardEPS.getScalarVar( 2 ) )<< endln ; 
        //cout << "  q=" << Delta_lambda * dQods.q_deviatoric()<< endln;
        //plastic_stress.reportshort("plastic stress (with delta_lambda)\n");
        
        elastic_plastic_stress = elastic_predictor_stress - plastic_stress;
        //elastic_plastic_stress.reportshortpqtheta("FE elastic plastic stress \n");
        	     
        //calculating elatic strain increment
        //stresstensor dstress_el = elastic_plastic_stress - start_stress;
        //straintensor elastic_strain = D("ijpq") * dstress_el("pq");
        straintensor elastic_strain = strain_increment - plastic_strain;  // elastic strain increment
        //cout << "elastic_strain I1=" << elastic_strain.Iinvariant1() << endln;
        //cout << "elastic_strain " << elastic_strain << endln;
        //cout << "strain increment I1=" << strain_increment.Iinvariant1() << endln;
        //cout << "strain increment    " << strain_increment << endln;
    
        straintensor estrain = forwardEPS.getElasticStrain(); //get old elastic strain
        straintensor pstrain = forwardEPS.getPlasticStrain(); //get old plastic strain 
    
        straintensor tstrain = forwardEPS.getStrain();        //get old total strain
        pstrain = pstrain + plastic_strain;
        estrain = estrain + elastic_strain;
        tstrain = tstrain + elastic_strain + plastic_strain;
        
        //Setting de_p, de_e, total plastic, elastic strain, and  total strain
        forwardEPS.setdPlasticStrain( plastic_strain );
        forwardEPS.setdElasticStrain( elastic_strain );
        forwardEPS.setPlasticStrain( pstrain );
        forwardEPS.setElasticStrain( estrain );
        forwardEPS.setStrain( tstrain );
        
        //================================================================
     	//Generating Eep using  dQods at the intersection point
        dFods = material_point.getYS()->dFods( &IntersectionEPS );
        dQods = material_point.getPS()->dQods( &IntersectionEPS );

	tensor upperE1 = E("pqkl")*dQods("kl");
        upperE1.null_indices();
	tensor upperE2 = dFods("ij")*E("ijmn");
        upperE2.null_indices();
	tensor upperE = upperE1("pq") * upperE1("mn");
        upperE.null_indices();

        temp2 = upperE2("ij")*dQods("ij"); // L_ij * E_ijkl * R_kl
        temp2.null_indices();
        lower = temp2.trace();
        
        // Evaluating the hardening modulus: sum of  (df/dq*) * qbar
	
	hardMod_ = 0.0;
	//Of 1st scalar internal vars
	if ( material_point.getELS1() ) {
	   h_s[0]  = material_point.getELS1()->h_s(&IntersectionEPS, material_point.getPS());
           xi_s[0] = material_point.getYS()->xi_s1( &IntersectionEPS );	   
   	   hardMod_ = hardMod_ + h_s[0] * xi_s[0];
	}

	//Of 2nd scalar internal vars
	if ( material_point.getELS2() ) {
	   h_s[1]  = material_point.getELS2()->h_s(&IntersectionEPS, material_point.getPS());
           xi_s[1] = material_point.getYS()->xi_s2( &IntersectionEPS );	   
   	   hardMod_ = hardMod_ + h_s[1] * xi_s[1];
	}

	//Of 3rd scalar internal vars
	if ( material_point.getELS3() ) {
	   h_s[2]  = material_point.getELS3()->h_s(&IntersectionEPS, material_point.getPS());
           xi_s[2] = material_point.getYS()->xi_s3( &IntersectionEPS );	   
   	   hardMod_ = hardMod_ + h_s[2] * xi_s[2];
	}

	//Of 4th scalar internal vars
	if ( material_point.getELS4() ) {
	   h_s[3]  = material_point.getELS4()->h_s(&IntersectionEPS, material_point.getPS());
           xi_s[3] = material_point.getYS()->xi_s4( &IntersectionEPS );	   
   	   hardMod_ = hardMod_ + h_s[3] * xi_s[3];
	}
	      
	//Of tensorial internal var
	// 1st tensorial var
	if ( material_point.getELT1() ) {
	   h_t[0]  = material_point.getELT1()->h_t(&IntersectionEPS, material_point.getPS());
	   xi_t[0] = material_point.getYS()->xi_t1( &IntersectionEPS );
           tensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 2nd tensorial var
	if ( material_point.getELT2() ) {
	   h_t[1]  = material_point.getELT2()->h_t(&IntersectionEPS, material_point.getPS());
	   xi_t[1] = material_point.getYS()->xi_t2( &IntersectionEPS );
           tensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 3rd tensorial var
	if ( material_point.getELT3() ) {
	   h_t[2]  = material_point.getELT3()->h_t(&IntersectionEPS, material_point.getPS());
	   xi_t[2] = material_point.getYS()->xi_t3( &IntersectionEPS );
           tensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 4th tensorial var
	if ( material_point.getELT4() ) {
	   h_t[3]  = material_point.getELT4()->h_t(&IntersectionEPS, material_point.getPS());
	   xi_t[3] = material_point.getYS()->xi_t4( &IntersectionEPS );
           tensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// Subtract accumulated hardMod_ from lower
        lower = lower - hardMod_;
	
        tensor Ep = upperE*(1./lower);

	// elastoplastic constitutive tensor
        Eep =  Eep - Ep; 

     	//cout <<" after calculation---Eep.rank()= " << Eep.rank() <<endln;
	//Eep.printshort(" IN ConDriver ");
         
        //--// before the surface is been updated !
        //--//        f_Final = Criterion.f(elastic_plastic_stress);
        //--
        //--        q_ast_entry = Criterion.kappa_get(elastic_plastic_stress);
        //--
        //--//        h_  = h(elastic_plastic_stress);
        //--        Dq_ast = Delta_lambda * h_ * just_this_PP;
        //--//        if (Dq_ast < 0.0 ) Dq_ast = 0.0;
        //--//        Dq_ast = fabs(Delta_lambda * h_ * just_this_PP); // because of softening response...
        //--//::printf(" h_=%.6e  q_ast_entry=%.6e  Dq_ast=%.6e   Delta_lambda=%.6e\n", h_, q_ast_entry, Dq_ast, Delta_lambda);
        //--
        //--        current_lambda_set(Delta_lambda);
        //--
        //--        q_ast = q_ast_entry + Dq_ast;
        //--        Criterion.kappa_set( elastic_plastic_stress, q_ast);
        //--//::fprintf(stdout," Criterion.kappa_get(elastic_plastic_stress)=%.8e\n",Criterion.kappa_get(elastic_plastic_stress));
        //--//::fprintf(stderr," Criterion.kappa_get(elastic_plastic_stress)=%.8e\n",Criterion.kappa_get(elastic_plastic_stress));
        //--
        //--
        //--//::fprintf(stdout," ######## predictor --> q_ast_entry=%.8e Dq_ast=%.8e q_ast=%.8e\n",q_ast_entry, Dq_ast, q_ast);
        //--//::fprintf(stderr," ######## predictor --> q_ast_entry=%.8e Dq_ast=%.8e q_ast=%.8e\n",q_ast_entry, Dq_ast, q_ast);
        //--
        //--//::fprintf(stdout,"ForwardEulerStress IN Criterion.kappa_get(start_stress)=%.8e\n",Criterion.kappa_get(start_stress));
        //--//::fprintf(stderr,"ForwardEulerStress IN Criterion.kappa_get(start_stress)=%.8e\n",Criterion.kappa_get(start_stress));
        //--
        
        //new EPState in terms of stress
        forwardEPS.setStress(elastic_plastic_stress); 
        //cout <<"inside constitutive driver: forwardEPS "<< forwardEPS;

        forwardEPS.setEep(Eep); 
        //forwardEPS.getEep().printshort(" after set"); 
        
        //Before update all the internal vars
        double f_forward =  material_point.getYS()->f( &forwardEPS );
        //::printf("\n************  Before f_forward = %.10e\n",f_forward);
    
        //Evolve the surfaces and hardening vars
	int NS = forwardEPS.getNScalarVar();
	int NT = forwardEPS.getNTensorVar();

	double dS= 0.0;
	double S = 0.0;

	for (int ii = 1; ii <= NS; ii++) {
              dS = Delta_lambda * h_s[ii-1] ;       // Increment to the scalar internal var
              S  = forwardEPS.getScalarVar(ii);     // Get the old value of the scalar internal var
              forwardEPS.setScalarVar(ii, S + dS ); // Update internal scalar var
	}
	
	stresstensor dT;
	stresstensor T;
	stresstensor new_T;

	for (int ii = 1; ii <= NT; ii++) {
	      dT = Delta_lambda * h_t[ii-1] ;       // Increment to the tensor internal var
              T  = forwardEPS.getTensorVar(ii);     // Get the old value of the tensor internal var
              new_T = T + dT;
              forwardEPS.setTensorVar(ii, new_T );
        }
       
        //tensor tempx  = plastic_strain("ij") * plastic_strain("ij");
        //double tempxd = tempx.trace();
        //double e_eq  = pow( 2.0 * tempxd / 3.0, 0.5 );
        ////cout << "e_eq = " << e_eq << endln;
        //
        //double dalfa1 =  e_eq * 10;
        //double alfa1  = forwardEPS.getScalarVar(1);


        
    	//cout << "UpdateAllVars " << forwardEPS<< endln;

        //After update all the internal vars
        f_forward =  material_point.getYS()->f( &forwardEPS );
        //::printf("\n************  After f_forward = %.10e\n\n",f_forward);
    
    
    }
    
    //::fprintf(stderr,"ForwardEulerStress EXIT Criterion.kappa_get(start_stress)=%.8e\n",Criterion.kappa_get(start_stress));
    return forwardEPS;
}

//================================================================================
// Starting EPState using Semi Backward Euler Starting Point
//================================================================================
EPState ConstitutiveDriver::SemiBackwardEulerEPState( straintensor &strain_increment, 
                                                      Template3Dep &material_point)
{
    stresstensor start_stress;
    EPState SemibackwardEPS( *material_point.getEPS() ); 
    start_stress = SemibackwardEPS.getStress();
   
    // building elasticity tensor
    //tensor E = Criterion.StiffnessTensorE(Ey,nu);
    tensor E  = material_point.ElasticStiffnessTensor();
    // building compliance tensor
    //  tensor D = Criterion.ComplianceTensorD(Ey,nu);
    
    //  pulling out some tensor and double definitions
    tensor dFods(2, def_dim_2, 0.0);
    tensor dQods(2, def_dim_2, 0.0);
    tensor temp2(2, def_dim_2, 0.0);
    double lower = 0.0;
    double Delta_lambda = 0.0;
  
    EPState E_Pred_EPS( *material_point.getEPS() );

    stresstensor stress_increment = E("ijkl")*strain_increment("kl");
    stress_increment.null_indices();
    // stress_increment.reportshort("stress Increment\n");

  
    stresstensor plastic_stress;
    stresstensor elastic_predictor_stress;
    stresstensor elastic_plastic_stress;
    //..  double Felplpredictor = 0.0;
  
    double h_s[4]       = {0.0, 0.0, 0.0, 0.0};
    double xi_s[4]      = {0.0, 0.0, 0.0, 0.0};
    stresstensor h_t[4];
    stresstensor xi_t[4];
    double hardMod_ = 0.0;

    double S        = 0.0;
    double dS       = 0.0;
    stresstensor T;
    stresstensor dT;
    //double Dq_ast   = 0.0;
    //double q_ast_entry = 0.0;
    //double q_ast = 0.0;
  
    elastic_predictor_stress = start_stress + stress_increment;
    //..  elastic_predictor_stress.reportshort("ELASTIC PREDICTOR stress\n");
    E_Pred_EPS.setStress( elastic_predictor_stress );
  
    //  double f_start = Criterion.f(start_stress);
    //  ::printf("SEMI BE##############  f_start = %.10e\n",f_start);
    double f_pred =  material_point.getYS()->f( &E_Pred_EPS );
    ::printf("SEMI BE##############  f_pred = %.10e\n",f_pred);
  
    // second of alternative predictors as seen in MAC page 170-171
    if ( f_pred >= 0.0 )
    {
        //el_or_pl_range(1); // set to 1 ( plastic range )
        // PREDICTOR FASE
        //..     ::printf("\n\npredictor part  step_counter = %d\n\n", step_counter);
  
        dFods = material_point.getYS()->dFods( &E_Pred_EPS );
        dQods = material_point.getPS()->dQods( &E_Pred_EPS );
        //.... dFods.print("a","dF/ds  ");
        //.... dQods.print("a","dQ/ds  ");
  
        temp2 = (dFods("ij")*E("ijkl"))*dQods("kl");
        temp2.null_indices();
        lower = temp2.trace();
  
        // Evaluating the hardening modulus: sum of  (df/dq*) * qbar

 	//Of scalar internal var
	hardMod_ = 0.0;

	//Of 1st scalar internal vars
	if ( material_point.getELS1() ) {
	   h_s[0]  = material_point.getELS1()->h_s(&E_Pred_EPS, material_point.getPS());
           xi_s[0] = material_point.getYS()->xi_s1( &E_Pred_EPS );	   
   	   hardMod_ = hardMod_ + h_s[0] * xi_s[0];
	}

	//Of 2nd scalar internal vars
	if ( material_point.getELS2() ) {
	   h_s[1]  = material_point.getELS2()->h_s(&E_Pred_EPS, material_point.getPS());
           xi_s[1] = material_point.getYS()->xi_s2( &E_Pred_EPS );	   
   	   hardMod_ = hardMod_ + h_s[1] * xi_s[1];
	}

	//Of 3rd scalar internal vars
	if ( material_point.getELS3() ) {
	   h_s[2]  = material_point.getELS3()->h_s(&E_Pred_EPS, material_point.getPS());
           xi_s[2] = material_point.getYS()->xi_s3( &E_Pred_EPS );	   
   	   hardMod_ = hardMod_ + h_s[2] * xi_s[2];
	}

	//Of 4th scalar internal vars
	if ( material_point.getELS4() ) {
	   h_s[3]  = material_point.getELS4()->h_s(&E_Pred_EPS, material_point.getPS());
           xi_s[3] = material_point.getYS()->xi_s4( &E_Pred_EPS );	   
   	   hardMod_ = hardMod_ + h_s[3] * xi_s[3];
	}

	//Of tensorial internal var
	// 1st tensorial var
	if ( material_point.getELT1() ) {
	   h_t[0]  = material_point.getELT1()->h_t(&E_Pred_EPS, material_point.getPS());
	   xi_t[0] = material_point.getYS()->xi_t1( &E_Pred_EPS );
           tensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 2nd tensorial var
	if ( material_point.getELT2() ) {
	   h_t[1]  = material_point.getELT2()->h_t(&E_Pred_EPS, material_point.getPS());
	   xi_t[1] = material_point.getYS()->xi_t2( &E_Pred_EPS );
           tensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 3rd tensorial var
	if ( material_point.getELT3() ) {
	   h_t[2]  = material_point.getELT3()->h_t(&E_Pred_EPS, material_point.getPS());
	   xi_t[2] = material_point.getYS()->xi_t3( &E_Pred_EPS );
           tensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// 4th tensorial var
	if ( material_point.getELT4() ) {
	   h_t[3]  = material_point.getELT4()->h_t(&E_Pred_EPS, material_point.getPS());
	   xi_t[3] = material_point.getYS()->xi_t4( &E_Pred_EPS );
           tensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
  	   hardMod_ = hardMod_ + hm.trace();
	}

	// Subtract accumulated hardMod_ from lower
        lower = lower - hardMod_;

        //h_s  = material_point.ELS1->h_s( &E_Pred_EPS, material_point.PS );
        //xi_s = material_point.YS->xi_s1( &E_Pred_EPS );
        //hardMod_ = h_s * xi_s;
        //lower = lower - hardMod_;

 	////Of tensorial internal var
	//h_t  = material_point.ELT1->h_t(&E_Pred_EPS, material_point.PS);
        //xi_t = material_point.YS->xi_t1( &E_Pred_EPS );
        //tensor hm = h_t("ij") * xi_t("ij");
	//hardMod_ = hm.trace();
        //lower = lower - hardMod_;
 

        Delta_lambda = f_pred/lower;
        if ( Delta_lambda < 0.0 )
          {
            ::fprintf(stderr,"\nP!\n");
          }
        plastic_stress = (E("ijkl")*dQods("kl"))*(-Delta_lambda);
        plastic_stress.null_indices();
        //.. plastic_stress.reportshort("plastic stress predictor II\n");
        //.. elastic_predictor_stress.reportshort("elastic predictor stress \n");
        elastic_plastic_stress = elastic_predictor_stress + plastic_stress;
        elastic_plastic_stress.null_indices();
  	
	SemibackwardEPS.setStress( elastic_plastic_stress );

        ////q_ast_entry = Criterion.kappa_get(elastic_plastic_stress);
        //S  = SemibackwardEPS.getScalarVar(1); // Get the old value of the internal var
        //h_s  = material_point.ELS1->h_s( &SemibackwardEPS, material_point.PS );
	//dS = Delta_lambda * h_s ;   // Increment to the internal scalar var
        //h_t  = material_point.ELT1->h_t( &SemibackwardEPS, material_point.PS );
	//dT = Delta_lambda * h_t ;   // Increment to the internal tensorial var

        //Evolve the surfaces and hardening vars
	int NS = SemibackwardEPS.getNScalarVar();
	int NT = SemibackwardEPS.getNTensorVar();

	for (int ii = 1; ii <= NS; ii++) {
              dS = Delta_lambda * h_s[ii-1] ;       // Increment to the scalar internal var
              S  = SemibackwardEPS.getScalarVar(ii);     // Get the old value of the scalar internal var
              SemibackwardEPS.setScalarVar(ii, S + dS ); // Update internal scalar var
	}


        ////current_lambda_set(Delta_lambda);
        ////q_ast = q_ast_entry + Dq_ast;
        ////Criterion.kappa_set( elastic_plastic_stress, q_ast);
        //SemibackwardEPS.setScalarVar(1, S + dS );

	//stresstensor new_T = T + dT;
        //SemibackwardEPS.setTensorVar(1, new_T );

	stresstensor new_T;

	for (int ii = 1; ii <= NT; ii++) {
	      dT = Delta_lambda * h_t[ii-1] ;       // Increment to the tensor internal var
              T  = SemibackwardEPS.getTensorVar(ii);     // Get the old value of the tensor internal var
              new_T = T + dT;
              SemibackwardEPS.setTensorVar(ii, new_T );
        }

  
        //return elastic_plastic_stress;
        return SemibackwardEPS;
    }
    return E_Pred_EPS;
}
  
  
  

//================================================================================
// New EPState using Backward Euler Algorithm
//================================================================================
EPState ConstitutiveDriver::BackwardEulerEPState( straintensor &strain_increment, 
                                                  Template3Dep &material_point)

{
  // Temp matertial point
  Template3Dep MP( material_point );
  
  //EPState to be returned, it can be elastic or elastic-plastic EPState
  EPState backwardEPS( *material_point.getEPS() ); 
  
  EPState startEPS( *material_point.getEPS() );
  stresstensor start_stress = startEPS.getStress();

  //Output for plotting
  cout.precision(5); 
  cout.width(10);
  cout << " " << start_stress.p_hydrostatic() << " ";
  
  cout.precision(5); 
  cout.width(10);
  cout << start_stress.q_deviatoric() << endln;
      
  // Pulling out some tensor and double definitions
  tensor I2("I", 2, def_dim_2);
  tensor I_ijkl("I", 4, def_dim_4);
  I_ijkl = I2("ij")*I2("kl");
  I_ijkl.null_indices();
  tensor I_ikjl("I", 4, def_dim_4);
  I_ikjl = I_ijkl.transpose0110();
  
  //double Ey = Criterion.E();
  //double nu = Criterion.nu();
  //tensor E = StiffnessTensorE(Ey,nu);
  tensor E  = material_point.ElasticStiffnessTensor();
  // tensor D = ComplianceTensorD(Ey,nu);
  // stresstensor REAL_start_stress = start_stress;
  
  tensor dFods(2, def_dim_2, 0.0);
  tensor dQods(2, def_dim_2, 0.0);
  //  tensor dQodsextended(2, def_dim_2, 0.0);
  //  tensor d2Qodqast(2, def_dim_2, 0.0);
  tensor temp2(2, def_dim_2, 0.0);
  double lower = 0.0;  
  double Delta_lambda = 0.0; // Lambda
  double delta_lambda = 0.0; // dLambda

  double Felplpredictor    = 0.0;
  //Kai  double absFelplpredictor = 0.0;
  //  double Ftolerance = pow(d_macheps(),(1.0/2.0))*1000000.00; //FORWARD no iterations
  //double Ftolerance = pow( d_macheps(), 0.5)*100.00;
  double Ftolerance = pow( d_macheps(), 0.5)*10000.00;  //Zhaohui
  //  double Ftolerance = pow(d_macheps(),(1.0/2.0))*1.0;
  //  double entry_kappa_cone = Criterion.kappa_cone_get();
  //  double entry_kappa_cap  = Criterion.kappa_cap_get();

  tensor aC(2, def_dim_2, 0.0);
  stresstensor BEstress;
  stresstensor residual;
  tensor d2Qoverds2( 4, def_dim_4, 0.0);
  tensor T( 4, def_dim_4, 0.0);
  tensor Tinv( 4, def_dim_4, 0.0);

  double Fold = 0.0;
  tensor temp3lower;
  tensor temp5;
  double temp6 = 0.0;
  double upper = 0.0;

  stresstensor dsigma;
  //stresstensor Dsigma;
  stresstensor sigmaBack;
  
  //double dq_ast = 0.0;       // iterative change in internal variable (kappa in this case)
  //double Dq_ast = 0.0;       // incremental change in internal variable (kappa in this case)
  //double q_ast  = 0.0;       // internal variable (kappa in this case)
  //double q_ast_entry  = 0.0; //internal variable from previous increment (kappa in this case)
  
  int step_counter = 0;
  int MAX_STEP_COUNT = 300;
  //  int MAX_STEP_COUNT = 0;
  int flag = 0;

  strain_increment.null_indices();
  stresstensor stress_increment = E("ijkl")*strain_increment("kl");
  stress_increment.null_indices();

  stresstensor Return_stress; //  stress to be returned can be either predictor
                              // or elastic plastic stress.

  EPState ElasticPredictorEPS( startEPS );
  stresstensor elastic_predictor_stress = start_stress + stress_increment;
  ElasticPredictorEPS.setStress( elastic_predictor_stress );
  //  elastic_predictor_stress.reportshortpqtheta("\n . . . .  ELASTIC PREDICTOR stress");

  stresstensor elastic_plastic_predictor_stress;
  EPState EP_PredictorEPS( startEPS );

  //double f_start = material_point.YS->f( &startEPS );
  //cout << " ************** f_start = " << f_start;
  //::fprintf(stdout,"tst##############  f_start = %.10e\n",f_start);
  // f_pred = Criterion.f(elastic_predictor_stress);
  //::fprintf(stdout,"tst##############  f_pred = %.10e\n",f_pred);
  double f_pred =  material_point.getYS()->f( &ElasticPredictorEPS );
  //cout << "*************** f_pred = " << f_pred << endln;
  //int region = 5;

  //double h_s      = 0.0;
  //double xi_s     = 0.0;
  double hardMod_ = 0.0;
  
  double h_s[4]       = {0.0, 0.0, 0.0, 0.0};
  double xi_s[4]      = {0.0, 0.0, 0.0, 0.0};
  stresstensor h_t[4];
  stresstensor xi_t[4];

  //region
  //check for the region than proceede
  //region = Criterion.influence_region(elastic_predictor_stress);
  //if ( region == 1 )  // apex gray region
  //  {
  //    double pc_ = pc();
  //    elastic_plastic_predictor_stress =
  //      elastic_plastic_predictor_stress.pqtheta2stress(pc_, 0.0, 0.0);
  //    return  elastic_plastic_predictor_stress;
  //  }
  
  if ( f_pred <= Ftolerance  )
  {

      //Updating elastic strain increment
      straintensor estrain = ElasticPredictorEPS.getElasticStrain();
      straintensor tstrain = ElasticPredictorEPS.getStrain();
      estrain = estrain + strain_increment;
      tstrain = tstrain + strain_increment;
      ElasticPredictorEPS.setElasticStrain( estrain );
      ElasticPredictorEPS.setStrain( tstrain );
      ElasticPredictorEPS.setdElasticStrain( strain_increment );
      
      backwardEPS = ElasticPredictorEPS;
      //Set Elasto-Plastic stiffness tensor
      ElasticPredictorEPS.setEep(E);

      return backwardEPS;
      //cout <<  "\nbackwardEPS" <<  backwardEPS;
      //cout <<  "\nElasticPredictorEPS " <<  ElasticPredictorEPS;

  }
  if ( f_pred > 0.0 )
  {

      //el_or_pl_range(1); // set to 1 ( plastic range )
      //PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
      //elastic_plastic_predictor_stress = Criterion.PredictorStress( start_stress,
      //                                                              strain_increment,
      //                                                              Criterion);

      //Starting point by applying one Forward Euler step
      EP_PredictorEPS = PredictorEPState( strain_increment, MP);

            
      //cout << " ----------Predictor Stress" << EP_PredictorEPS.getStress();
      //Setting the starting EPState with the starting internal vars in EPState
      MP.setEPS( EP_PredictorEPS );

      //PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
      // IZBACIO ga 27 jan 97 . . .
      //     Delta_lambda = Criterion.current_lambda_get() ;
      //::printf("  Delta_lambda = Criterion.current_lambda_get = %.8e\n", Delta_lambda);
      //PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
      //Felplpredictor = Criterion.f(elastic_plastic_predictor_stress);
      
      Felplpredictor =  MP.getYS()->f(&EP_PredictorEPS);
      //cout <<  " F_elplpredictor " << Felplpredictor << endln;


      //Kai     absFelplpredictor = fabs(Felplpredictor);
      if ( fabs(Felplpredictor) <= Ftolerance || step_counter >= MAX_STEP_COUNT ) // if more iterations than prescribed
      {
         backwardEPS = EP_PredictorEPS;

         //Return_stress = elastic_plastic_predictor_stress;
         flag = 1;
         // this part should have been done up in "Criterion.PredictorStress" function.
         //  Criterion.kappa_set( Return_stress, q_ast) ;
         //  return Return_stress;
      }
      else {
        ////check for the region again and proceede
        //// probably to see if predictor took you to any of the bad regions
        //region = Criterion.influence_region(elastic_plastic_predictor_stress);
        //if ( region == 1 )  // apex gray region
        //{
        //  double pc_ = pc();
        //  elastic_plastic_predictor_stress =
        //  elastic_plastic_predictor_stress.pqtheta2stress(pc_, 0.0, 0.0);
        // return  elastic_plastic_predictor_stress;
        //}

	// NEWTON-RAPHSON RETURN
        // residual stresstensor
        //aC    = Criterion.dQods(elastic_plastic_predictor_stress);
        //dFods = Criterion.dFods(elastic_plastic_predictor_stress);
        //dQods = Criterion.dQods(elastic_plastic_predictor_stress);
        aC    = MP.getPS()->dQods( &EP_PredictorEPS );	 
        dFods = MP.getYS()->dFods( &EP_PredictorEPS );
        dQods = MP.getPS()->dQods( &EP_PredictorEPS );
        
        //   aC    = Criterion.dQods(elastic_predictor_stress);
        //   dFods = Criterion.dFods(elastic_predictor_stress);
        //   dQods = Criterion.dQods(elastic_predictor_stress);
        

        temp2 = (dFods("ij")*E("ijkl"))*dQods("kl");
        temp2.null_indices();
        lower = temp2.trace();
        
        //     Delta_lambda = f_pred/lower; //?????????
        //::printf("  Delta_lambda = f_pred/lower = %.8e\n", Delta_lambda);
        ////     Delta_lambda = Felplpredictor/lower;
        ////::printf("  Delta_lambda = Felplpredictor/lower =%.8e \n", Delta_lambda);

	// Original segment
        elastic_plastic_predictor_stress = elastic_predictor_stress - E("ijkl")*aC("kl")*Delta_lambda;
        EP_PredictorEPS.setStress( elastic_plastic_predictor_stress );

	//Zhaohui modified, sometimes give much better convergence rate
        //elastic_plastic_predictor_stress = EP_PredictorEPS.getStress();
	//cout << "elastic_plastic_predictor_stress" << elastic_plastic_predictor_stress;
	
        cout.precision(5); 
        cout.width(10);
        cout << " " << EP_PredictorEPS.getStress().p_hydrostatic() << " ";
      
        cout.precision(5); 
        cout.width(10);
        cout << EP_PredictorEPS.getStress().q_deviatoric()<< " ";

        cout.precision(5); 
        cout.width(10);
        cout << Delta_lambda << endln;

	//elastic_plastic_predictor_stress.reportshort("......elastic_plastic_predictor_stress");
        //::printf("  F(elastic_plastic_predictor_stress) = %.8e\n", Criterion.f(elastic_plastic_predictor_stress));
        
        //h_s  = MP.ELS1->h_s( &EP_PredictorEPS, MP.PS );
        ////h_  = h(elastic_plastic_predictor_stress);
        ////  Dq_ast = Criterion.kappa_get(elastic_plastic_predictor_stress);
        
	//q_ast_entry = Criterion.kappa_get(elastic_plastic_predictor_stress);
        //Dq_ast = Delta_lambda * h_ * just_this_PP;
        //q_ast = q_ast_entry + Dq_ast;
	
	//Zhaohui comments: internal vars are alreary evolued in ForwardEulerEPS(...), not necessary here!!!
	//..dS = Delta_lambda * h_s ;   // Increment to the internal var
        //..S  = EP_PredictorEPS.getScalarVar(1); // Get the old value of the internal var
	//..new_S = S + dS;
	//..cout << "Internal Var : " << new_S << endln;
	//..EP_PredictorEPS.setScalarVar(1, new_S); // Get the old value of the internal var
      	
	//::fprintf(stdout," ######## predictor --> Dq_ast=%.8e q_ast=%.8e\n", Dq_ast,        q_ast);
        //::fprintf(stderr," ######## predictor --> Dq_ast=%.8e q_ast=%.8e\n", Dq_ast,        q_ast);

        //Criterion.kappa_set( sigmaBack, q_ast);  //????
        //current_lambda_set(Delta_lambda);	   //????

        //::printf("  Delta_lambda = %.8e\n", Delta_lambda);
        //::printf("step = pre iteracija  #############################--   q_ast = %.10e \n", q_ast);
        //::printf("step = pre iteracija  posle predictora  ###########--   Dq_ast = %.10e \n",Dq_ast);
        //**********
        //**********
        //::printf("\nDelta_lambda  before BE = %.10e \n", Delta_lambda );
        }
        
        //========================== main part of iteration =======================
        //      while ( absFelplpredictor > Ftolerance &&

        while ( fabs(Felplpredictor) > Ftolerance && step_counter < MAX_STEP_COUNT ) // if more iterations than prescribed
        //out07may97      do
        {
          BEstress = elastic_predictor_stress - E("ijkl")*aC("kl")*Delta_lambda;
          //BEstress.reportshort("......BEstress ");
          /////          BEstress = elastic_plastic_predictor_stress - E("ijkl")*aC("kl")*Delta_lambda;
          BEstress.null_indices();
          //          Felplpredictor = Criterion.f(BEstress);
          //          ::printf("\nF_backward_Euler BE = %.10e \n", Felplpredictor);
          residual = elastic_plastic_predictor_stress - BEstress;
          //residual.reportshortpqtheta("\n......residual ");
          //          double ComplementaryEnergy = (residual("ij")*D("ijkl")*residual("ij")).trace();
          //::printf("\n Residual ComplementaryEnergy = %.16e\n", ComplementaryEnergy);

          /////          residual = elastic_predictor_stress - BEstress;
          
	  //d2Qoverds2 = Criterion.d2Qods2(elastic_plastic_predictor_stress);
          d2Qoverds2 = MP.getPS()->d2Qods2( &EP_PredictorEPS );
          //d2Qoverds2.print();

	  T = I_ikjl + E("ijkl")*d2Qoverds2("klmn")*Delta_lambda;
          T.null_indices();

          Tinv = T.inverse();
          //dFods = Criterion.dFods(elastic_plastic_predictor_stress);
          //dQods = Criterion.dQods(elastic_plastic_predictor_stress);
          dFods = MP.getYS()->dFods( &EP_PredictorEPS );
          dQods = MP.getPS()->dQods( &EP_PredictorEPS );

          //Fold = Criterion.f(elastic_plastic_predictor_stress);
          Fold = MP.getYS()->f( &EP_PredictorEPS );
          
	  lower = 0.0; // this is old temp variable used here again :-)
          //h_  = h(elastic_plastic_predictor_stress);
          //xi_ = xi(elastic_plastic_predictor_stress);
         
	  //h_s  = MP.ELS1->h_s( &EP_PredictorEPS, MP.PS );
          //xi_s = MP.YS->xi_s1( &EP_PredictorEPS );
          //hardMod_ = h_s * xi_s;
          
          // Evaluating the hardening modulus: sum of  (df/dq*) * qbar
	  
	  hardMod_ = 0.0;
	  //Of 1st scalar internal vars
	  if ( MP.getELS1() ) {
	     h_s[0]  = MP.getELS1()->h_s( &EP_PredictorEPS, MP.getPS());
             xi_s[0] = MP.getYS()->xi_s1( &EP_PredictorEPS );	   
   	     hardMod_ = hardMod_ + h_s[0] * xi_s[0];
	  }
	  
	  //Of 2nd scalar internal vars
	  if ( MP.getELS2() ) {
	     h_s[1]  = MP.getELS2()->h_s( &EP_PredictorEPS, MP.getPS());
             xi_s[1] = MP.getYS()->xi_s2( &EP_PredictorEPS );	   
   	     hardMod_ = hardMod_ + h_s[1] * xi_s[1];
	  }
	  
	  //Of 3rd scalar internal vars
	  if ( MP.getELS3() ) {
	     h_s[2]  = MP.getELS3()->h_s( &EP_PredictorEPS, MP.getPS());
             xi_s[2] = MP.getYS()->xi_s3( &EP_PredictorEPS );	   
   	     hardMod_ = hardMod_ + h_s[2] * xi_s[2];
	  }
	  
	  //Of 4th scalar internal vars
	  if ( MP.getELS4() ) {
	     h_s[3]  = MP.getELS4()->h_s( &EP_PredictorEPS, MP.getPS());
             xi_s[3] = MP.getYS()->xi_s4( &EP_PredictorEPS );	   
   	     hardMod_ = hardMod_ + h_s[3] * xi_s[3];
	  }
	        
	  //Of tensorial internal var
	  // 1st tensorial var
	  if ( MP.getELT1() ) {
	     h_t[0]  = MP.getELT1()->h_t( &EP_PredictorEPS, MP.getPS());
	     xi_t[0] = MP.getYS()->xi_t1( &EP_PredictorEPS );
             tensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
  	     hardMod_ = hardMod_ + hm.trace();
	  }
	  
	  // 2nd tensorial var
	  if ( MP.getELT2() ) {
	     h_t[1]  = MP.getELT2()->h_t( &EP_PredictorEPS, MP.getPS());
	     xi_t[1] = MP.getYS()->xi_t2( &EP_PredictorEPS );
             tensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
  	     hardMod_ = hardMod_ + hm.trace();
	  }
	  
	  // 3rd tensorial var
	  if ( MP.getELT3() ) {
	     h_t[2]  = MP.getELT3()->h_t( &EP_PredictorEPS, MP.getPS());
	     xi_t[2] = MP.getYS()->xi_t3( &EP_PredictorEPS );
             tensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
  	     hardMod_ = hardMod_ + hm.trace();
	  }
	  
	  // 4th tensorial var
	  if ( MP.getELT4() ) {
	     h_t[3]  = MP.getELT4()->h_t( &EP_PredictorEPS, MP.getPS());
	     xi_t[3] = MP.getYS()->xi_t4( &EP_PredictorEPS );
             tensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
  	     hardMod_ = hardMod_ + hm.trace();
	  }
	  
	  // Subtract accumulated hardMod_ from lower
          //lower = lower - hardMod_;
	  
	  //hardMod_ = hardMod_ * just_this_PP;
          //::printf("\n BackwardEulerStress ..  hardMod_ = %.10e \n", hardMod_ );
          //outfornow          d2Qodqast = d2Qoverdqast(elastic_plastic_predictor_stress);
          //outfornow          dQodsextended = dQods + d2Qodqast * Delta_lambda * h_;
          //outfornow          temp3lower = dFods("mn")*Tinv("ijmn")*E("ijkl")*dQodsextended("kl");
          temp3lower = dFods("mn")*Tinv("ijmn")*E("ijkl")*dQods("kl");
          temp3lower.null_indices();
          lower = temp3lower.trace();
          lower = lower - hardMod_;

          temp5 = (residual("ij") * Tinv("ijmn")) * dFods("mn");
          temp6 = temp5.trace();
	  //The same as the above but more computation
          //temp5 = dFods("mn") * residual("ij") * Tinv("ijmn");
          //temp6 = temp5.trace();

          upper = Fold - temp6;

	  //================================================================================
	  //dlambda
          delta_lambda = upper / lower;
          Delta_lambda = Delta_lambda + delta_lambda;

          //::printf(" >> %d  Delta_lambda = %.8e", step_counter, Delta_lambda);
          // stari umesto dQodsextended za stari = dQods
          //outfornow          dsigma =
          //outfornow            ((residual("ij")*Tinv("ijmn"))+
          //outfornow            ((E("ijkl")*dQodsextended("kl"))*Tinv("ijmn")*Delta_lambda) )*-1.0;
          //::printf("    Delta_lambda = %.8e\n", Delta_lambda);
          dsigma = ( (residual("ij")*Tinv("ijmn") )+
                   ( (E("ijkl")*dQods("kl"))*Tinv("ijmn")*delta_lambda) )*(-1.0); //*-1.0???

          dsigma.null_indices();
	  //dsigma.reportshortpqtheta("\n......dsigma ");
          //::printf("  .........   in NR loop   delta_lambda = %.16e\n", delta_lambda);
          //::printf("  .........   in NR loop   Delta_lambda = %.16e\n", Delta_lambda);
          
	  //dq_ast = delta_lambda * h_ * just_this_PP;
          //Dq_ast += dq_ast;
          
	  //dS = delta_lambda * h_s ;   // Increment to the internal var
          //S  = EP_PredictorEPS.getScalarVar(1); // Get the old value of the internal var
	  //new_S = S + dS;
          //EP_PredictorEPS.setScalarVar(1, new_S); 
          
          //Evolve the surfaces and hardening vars
	  int NS = EP_PredictorEPS.getNScalarVar();
	  int NT = EP_PredictorEPS.getNTensorVar();
  	  
	  double dS = 0;
  	  double S  = 0;
  	  //double new_S = 0; 
  	  
  	  stresstensor dT;
  	  stresstensor T;
  	  stresstensor new_T;
  	       
	  for (int ii = 1; ii <= NS; ii++) {
             dS = delta_lambda * h_s[ii-1] ;             // Increment to the scalar internal var
             S  = EP_PredictorEPS.getScalarVar(ii);      // Get the old value of the scalar internal var
             EP_PredictorEPS.setScalarVar(ii, S + dS );  // Update internal scalar var
	  }

	  for (int ii = 1; ii <= NT; ii++) {
	     dT = delta_lambda * h_t[ii-1] ;            // Increment to the tensor internal var
             T  = EP_PredictorEPS.getTensorVar(ii);     // Get the old value of the tensor internal var
             new_T = T + dT;
             EP_PredictorEPS.setTensorVar(ii, new_T );	// Update tensorial scalar var
          }
	      
 	  
	  //=======          Dq_ast = Delta_lambda * h_ * just_this_PP;
          //q_ast = q_ast_entry + Dq_ast;
          
	  //::fprintf(stdout," ######## step = %3d --> Dq_ast=%.8e q_ast=%.8e\n",
          //             step_counter,         Dq_ast,        q_ast);
          //::fprintf(stderr," ######## step = %3d --> Dq_ast=%.8e q_ast=%.8e\n",
          //             step_counter,         Dq_ast,        q_ast);
          
	  //current_lambda_set(Delta_lambda);
          //....          elastic_plastic_predictor_stress.reportshort("elplpredstress");
          //....          dsigma.reportshort("dsigma");
          
          //sigmaBack.reportshortpqtheta("\n before======== SigmaBack");
	  sigmaBack = elastic_plastic_predictor_stress + dsigma;
          //sigmaBack.deviator().reportshort("\n after ======== SigmaBack");
          //sigmaBack.reportshortpqtheta("\n after ======== SigmaBack");
	  

          //======          sigmaBack = elastic_predictor_stress + Dsigma;
          //sigmaBack.reportshortpqtheta("BE................  NR sigmaBack   ");
          //sigmaBack.reportAnim();
          //::fprintf(stdout,"Anim BEpoint0%d   = {Sin[theta]*q, p, Cos[theta]*q} \n",step_counter+1);
          ////::fprintf(stdout,"Anim BEpoint0%dP = Point[BEpoint0%d] \n",step_counter+1,step_counter+1);
          //::fprintf(stdout,"Anim   \n");
          
	  //Criterion.kappa_set( sigmaBack, q_ast) ;
          EP_PredictorEPS.setStress( sigmaBack );
          
	  //Felplpredictor = Criterion.f(sigmaBack);
          //Kai          absFelplpredictor = fabs(Felplpredictor);
          //::printf("  F_bE=%.10e (%.10e)\n", Felplpredictor,Ftolerance);
	  Felplpredictor = MP.getYS()->f( &EP_PredictorEPS );
          //::printf("                    F_bE = %.10e (%.10e)\n", Felplpredictor, Ftolerance);

          //double tempkappa1 = kappa_cone_get();
          //double tempdFodeta = dFoverdeta(sigmaBack);
          //double tempdetaodkappa = detaoverdkappa(tempkappa1);
          //::printf("    h_=%.6e  xi_=%.6e, dFodeta=%.6e, detaodkappa=%.6e, hardMod_=%.6e\n", 
          //     h_, xi_,tempdFodeta, tempdetaodkappa,  hardMod_);
          //::printf("   upper = %.6e    lower = %.6e\n", upper, lower);
          //::printf(" q_ast_entry=%.6e  Dq_ast=%.6e   Delta_lambda=%.6e\n", 
          //      q_ast_entry, Dq_ast, Delta_lambda);

          // now prepare new step
          elastic_plastic_predictor_stress = sigmaBack;
          
	  //Output for plotting
	  cout.precision(5); 
          cout.width(10);
          cout << " " << sigmaBack.p_hydrostatic() << " ";
	  
	  cout.precision(5); 
          cout.width(10);
          cout << sigmaBack.q_deviatoric() << " ";

	  cout.precision(5); 
          cout.width(10);
          cout << Delta_lambda << endln;

	  //::printf("         ...........................  end of step %d\n", step_counter);// getchar();
          step_counter++;
        }
          //out07may97      while ( absFelplpredictor > Ftolerance &&
          //out07may97              step_counter <= MAX_STEP_COUNT  ); // if more iterations than prescribed
     
          //**********
          //**********
          //**********
          //**********
        if ( step_counter > MAX_STEP_COUNT  )
        {
           ::printf("step_counter>MAX_STEP_COUNT iterations in Material_Model::BackwardEulerStress stoped\n");
           //::exit(1);
        }

        // already set everything
        if ( flag !=1   ) {

           //Return_stress = elastic_plastic_predictor_stress;
           //Criterion.kappa_set( Return_stress, q_ast) ;
           
           // Generating Consistent Stiffness Tensor Eep
           tensor I2("I", 2, def_dim_2);
           tensor I_ijkl = I2("ij")*I2("kl");
           I_ijkl.null_indices();
           tensor I_ikjl = I_ijkl.transpose0110();
	   

           dQods = MP.getPS()->dQods( &EP_PredictorEPS ); // this is m_ij
           tensor temp2 = E("ijkl")*dQods("kl");
           temp2.null_indices();
           dFods = MP.getYS()->dFods( &EP_PredictorEPS ); // this is n_ij
           d2Qoverds2 = MP.getPS()->d2Qods2( &EP_PredictorEPS );
           	   
           tensor T = I_ikjl + E("ijkl")*d2Qoverds2("klmn")*Delta_lambda;
  	   //tensor tt = E("ijkl")*d2Qoverds2("klmn")*Delta_lambda;
	   //tt.printshort("temp tt");
	   //T = I_ikjl + tt;
           T.null_indices();
           tensor Tinv = T.inverse();
           
           tensor R = Tinv("ijmn")*E("ijkl");
           R.null_indices();
	   
           // Evaluating the hardening modulus: sum of  (df/dq*) * qbar at the final stress
	   hardMod_ = 0.0;
	   //Of 1st scalar internal vars
	   if ( MP.getELS1() ) {
	      h_s[0]  = MP.getELS1()->h_s( &EP_PredictorEPS, MP.getPS());
              xi_s[0] = MP.getYS()->xi_s1( &EP_PredictorEPS );	   
   	      hardMod_ = hardMod_ + h_s[0] * xi_s[0];
	   }
	   
	   //Of 2nd scalar internal vars
	   if ( MP.getELS2() ) {
	      h_s[1]  = MP.getELS2()->h_s( &EP_PredictorEPS, MP.getPS());
              xi_s[1] = MP.getYS()->xi_s2( &EP_PredictorEPS );	   
   	      hardMod_ = hardMod_ + h_s[1] * xi_s[1];
	   }
	   
	   //Of 3rd scalar internal vars
	   if ( MP.getELS3() ) {
	      h_s[2]  = MP.getELS3()->h_s( &EP_PredictorEPS, MP.getPS());
              xi_s[2] = MP.getYS()->xi_s3( &EP_PredictorEPS );	   
   	      hardMod_ = hardMod_ + h_s[2] * xi_s[2];
	   }
	   
	   //Of 4th scalar internal vars
	   if ( MP.getELS4() ) {
	      h_s[3]  = MP.getELS4()->h_s( &EP_PredictorEPS, MP.getPS());
              xi_s[3] = MP.getYS()->xi_s4( &EP_PredictorEPS );	   
   	      hardMod_ = hardMod_ + h_s[3] * xi_s[3];
	   }
	         
	   //Of tensorial internal var
	   // 1st tensorial var
	   if ( MP.getELT1() ) {
	     h_t[0]  = MP.getELT1()->h_t( &EP_PredictorEPS, MP.getPS());
	     xi_t[0] = MP.getYS()->xi_t1( &EP_PredictorEPS );
             tensor hm = (h_t[0])("ij") * (xi_t[0])("ij");
  	     hardMod_ = hardMod_ + hm.trace();
	   }
	   
	   // 2nd tensorial var
	   if ( MP.getELT2() ) {
	      h_t[1]  = MP.getELT2()->h_t( &EP_PredictorEPS, MP.getPS());
	      xi_t[1] = MP.getYS()->xi_t2( &EP_PredictorEPS );
              tensor hm = (h_t[1])("ij") * (xi_t[1])("ij");
  	      hardMod_ = hardMod_ + hm.trace();
	   }
	   
	   // 3rd tensorial var
	   if ( MP.getELT3() ) {
	      h_t[2]  = MP.getELT3()->h_t( &EP_PredictorEPS, MP.getPS());
	      xi_t[2] = MP.getYS()->xi_t3( &EP_PredictorEPS );
              tensor hm = (h_t[2])("ij") * (xi_t[2])("ij");
  	      hardMod_ = hardMod_ + hm.trace();
	   }
	   
	   // 4th tensorial var
	   if ( MP.getELT4() ) {
	      h_t[3]  = MP.getELT4()->h_t( &EP_PredictorEPS, MP.getPS());
	      xi_t[3] = MP.getYS()->xi_t4( &EP_PredictorEPS );
              tensor hm = (h_t[3])("ij") * (xi_t[3])("ij");
  	      hardMod_ = hardMod_ + hm.trace();
	   }
	    	   
    	   tensor temp3lower = dFods("ot")*R("otpq")*dQods("pq");
    	   temp3lower.null_indices();
    	   
    	   double lower = temp3lower.trace();
    	   lower = lower - hardMod_;  // h
    	      
    	   tensor upper = R("pqkl")*dQods("kl")*dFods("ij")*R("ijmn");
    	   upper.null_indices();
    	   tensor Ep = upper*(1./lower);
    	   tensor Eep =  R - Ep; // elastoplastic constitutive tensor

           //Set Elasto-Plastic stiffness tensor
           ElasticPredictorEPS.setEep(Eep);

	   backwardEPS = EP_PredictorEPS;
        }
  
  }
  //return Return_stress;
  return backwardEPS;
}




//================================================================================
// Elastoplastic constitutive tensors
//================================================================================

//  tensor Material_Model::BackwardEulerCTensor(stresstensor & final_stress,
//                                              stresstensor & start_stress,
//                                              straintensor & strain_increment,
//                                              Material_Model & Criterion,
//                                              double just_this_PP )
//    {
//      double Ey = Criterion.E();
//      double nu = Criterion.nu();
//      tensor Eel = StiffnessTensorE(Ey,nu);
//      stresstensor stress_increment = Eel("ijkl")*strain_increment("kl");
//      stress_increment.null_indices();
//      stresstensor elastic_predictor_stress = start_stress + stress_increment;
//      elastic_predictor_stress.null_indices();
//      //elastic_predictor_stress.reportshort("ELASTIC PREDICTOR stress\n");
//      //    double f_start = Criterion.f(start_stress);
//      //::printf("BackwardEulerCTensor  ##############  f_start = %.10e\n",f_start);
//      //    double f_pred = Criterion.f(elastic_predictor_stress);
//      //::printf("BackwardEulerCTensor  ##############  f_pred = %.10e\n",f_pred);
//      // if el_0r_pl variable ( flag ) is ==0 then it is elastic region
//      // otherwise it is plastic region so the full elastoplastic stiffness
//      // tensor should be calculated!!!!!!!!
//      if ( Criterion.el_or_pl_range() == 0 )
//        {
//          return Eel;
//        }
//      tensor I2("I", 2, def_dim_2);
//      tensor I_ijkl = I2("ij")*I2("kl");
//      I_ijkl.null_indices();
//      tensor I_ikjl = I_ijkl.transpose0110();
//      //region
//      //check for the region na proceede
//      int region = Criterion.influence_region(final_stress);
//  
//      if ( region == 1 )  // apex gray region
//        {
//          ::fprintf(stderr,"BackwardEulerCTensor APEX GRAY REGION CONSTITUTIVE TENSOR (returning Eel*0.01)\n");
//          ::fprintf(stdout,"BackwardEulerCTensor APEX GRAY REGION CONSTITUTIVE TENSOR (returning Eel*0.01)\n");
//          return Eel*0.01;
//          //Kai::exit(1);
//        }
//      // CONSISTENT TENSOR
//      tensor dQods = Criterion.dQods(final_stress);// this is m_kl
//      tensor temp2 = Eel("ijkl")*dQods("kl");
//      temp2.null_indices();
//      //====//-----------------------------------------------------------------------------
//      //====//-----------------------------------------------------------------------------
//      //====// sada dakle krecem da trazim lambda i to tako sto cu
//      //====// da nadjem razliku izmedju predictor napona i krajenjeg napona
//      //====// apo onoj formuli : sigma^n+1 = sigma^predictor - lambda*Eijkl*m_kl(sigma^n+1)
//      //====//-----------------------------------------------------------------------------
//      //====
//      //====    stresstensor stressdifference = elastic_predictor_stress - final_stress;
//      //====
//      //====
//      //====// dobro je izgleda da stvarno postoji veza
//      //====// sigma^n+1 = sigma^predictor - lambda*E_ijkl*m_kl(sigma^n+1)
//      //====// pa cu lambda da odredim iz te jednacine. Testiracu prvi clan ( ako je razlicit
//      //====// od nule ali bilo bi dobro da proverim i ostale!
//      //====    double s11 = stressdifference.cval(1,1);
//      //====//    double s12 = stressdifference.cval(1,2);
//      //====//    double s13 = stressdifference.cval(1,3);
//      //====    double s22 = stressdifference.cval(2,2);
//      //====//    double s23 = stressdifference.cval(2,3);
//      //====    double s33 = stressdifference.cval(3,3);
//      //====
//      //====    double Em11 = temp2.cval(1,1);
//      //====//    double Em12 = temp2.cval(1,2);
//      //====//    double Em13 = temp2.cval(1,3);
//      //====    double Em22 = temp2.cval(2,2);
//      //====//    double Em23 = temp2.cval(2,3);
//      //====    double Em33 = temp2.cval(3,3);
//      //====
//      //====    double lambda11 = 0.0;
//      //====//    double lambda12 = 0.0;
//      //====//    double lambda13 = 0.0;
//      //====    double lambda22 = 0.0;
//      //====//    double lambda23 = 0.0;
//      //====    double lambda33 = 0.0;
//      //====
//      //====    double       machepsilon      = d_macheps();
//      //====//...::printf("\n\n\n machepsilon=%.20e\n\n\n",machepsilon);
//      //====    double numzero = sqrt(machepsilon);
//      //====
//      //====
//      //====    if ( fabs(Em11) >=  numzero ) lambda11 = s11/Em11;
//      //====//    if ( fabs(Em12) >=  numzero ) lambda12 = s12/Em12;
//      //====//    if ( fabs(Em13) >=  numzero ) lambda13 = s13/Em13;
//      //====    if ( fabs(Em22) >=  numzero ) lambda22 = s22/Em22;
//      //====//    if ( fabs(Em23) >=  numzero ) lambda23 = s23/Em23;
//      //====    if ( fabs(Em33) >=  numzero ) lambda33 = s33/Em33;
//      //====::fprintf(stdout,"\n BackwardEulerCTensor  lambda11= %.10e \n",lambda11);
//      //====//::fprintf(stderr,"\n lambda12= %.10e \n",lambda12);
//      //====//::fprintf(stderr,"\n lambda13= %.10e \n",lambda13);
//      //====::fprintf(stdout,"\n BackwardEulerCTensor  lambda22= %.10e \n",lambda22);
//      //====//::fprintf(stderr,"\n lambda23= %.10e \n",lambda23);
//      //====::fprintf(stdout,"\n BackwardEulerCTensor  lambda33= %.10e \n",lambda33);
//      //====
//      //====    double macheps02 = pow(machepsilon,(1./5.));
//      //====
//      //====//    double lambda =
//      //====//      (lambda11+lambda12+lambda13+lambda22+lambda23+lambda33)/6.0;
//      //====//    double lambda =
//      //====//      (lambda11+lambda22+lambda33)/3.0;
//      //====//    double lambda = lambda11;
//      //====    double lambda = lambda33;
//      //====
//      //====    if ( fabs(lambda-lambda11)>macheps02 )
//      //====      {
//      //====        ::fprintf(stderr,"\nfabs(lambda-lambda11)>macheps02 %f\n",fabs(lambda-lambda11));
//      //====      }
//      //====//    if ( fabs(lambda-lambda12)>macheps02 )
//      //====//      {
//      //====//        ::fprintf(stderr,"\nfabs(lambda-lambda12)>macheps02 %f\n",fabs(lambda-lambda12));
//      //====//      }
//      //====//    if ( fabs(lambda-lambda13)>macheps02 )
//      //====//      {
//      //====//        ::fprintf(stderr,"\nfabs(lambda-lambda13)>macheps02 %f\n",fabs(lambda-lambda13));
//      //====//      }
//      //====    if ( fabs(lambda-lambda22)>macheps02 )
//      //====      {
//      //====        ::fprintf(stderr,"\nfabs(lambda-lambda22)>macheps02 %f\n",fabs(lambda-lambda22));
//      //====      }
//      //====//    if ( fabs(lambda-lambda23)>macheps02 )
//      //====//      {
//      //====//        ::fprintf(stderr,"\nfabs(lambda-lambda23)>macheps02 %f\n",fabs(lambda-lambda23));
//      //====//      }
//      //====    if ( fabs(lambda-lambda33)>macheps02 )
//      //====      {
//      //====        ::fprintf(stderr,"\nfabs(lambda-lambda33)>macheps02 %f\n",fabs(lambda-lambda33));
//      //====      }
//      //====
//      //====//====//-----------------------------------------------------------------------------
//      //====//-----------------------------------------------------------------------------
//  
//      double lambda = current_lambda_get();
//      //====::fprintf(stdout,"\n                  BackwardEulerCTensor  lambda= %.10e \n",lambda);
//      tensor dFods = Criterion.dFods(final_stress);
//      //==    // second derivative of potential function Q with
//      //==    // respect to stresses
//      tensor d2Qoverds2 = Criterion.d2Qods2(final_stress);
//      //...d2Qoverds2.print("*","\n\n\n d2Q/ds2  ");
//      //!!!!!!! //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      //!!!!!!!     lambda = 0.0; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      //!!!!!!! //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      tensor T = I_ikjl + Eel("ijkl")*d2Qoverds2("klmn")*lambda;
//      T.null_indices();
//      //...T.print("t","\n\n\n T ");
//      tensor Tinv = T.inverse();
//      //...Tinv.print("ti","\n\n\n Tinv ");
//      tensor R = Tinv("ijmn")*Eel("ijkl");
//      R.null_indices();
//      //...R.print("r","\n\n\n R ");
//  
//      double h_  =  h(final_stress);
//      double xi_ = xi(final_stress);
//      double hardMod_ = h_ * xi_;
//      hardMod_        = hardMod_ * just_this_PP;
//      //::printf("\n BackwardEulerCTensor  ..  hardMod_ = %.10e \n", hardMod_ );
//      //outfornowRunesson    tensor d2Qodqast = d2Qoverdqast(elastic_plastic_predictor_stress);
//      //outfornowRunesson    tensor dQodsextended = dQods + d2Qodqast * lambda * h_;
//      //outfornowRunesson
//      //outfornowRunesson    tensor temp3lower = dFods("mn")*Tinv("ijmn")*Eel("ijkl")*dQodsextended("kl");
//      
//      //tensor temp3lower = dFods("mn")*Tinv("ijmn")*Eel("ijkl")*dQods("kl");
//      tensor temp3lower = dFods("ot")*R("otpq")*dQods("pq");
//      temp3lower.null_indices();
//  
//      double lower = temp3lower.trace();
//      lower = lower - hardMod_;  // h
//      //outfornowRunesson    tensor upper = R("pqkl")*dQodsextended("kl")*dFods("ij")*R("ijmn");
//      tensor upper = R("pqkl")*dQods("kl")*dFods("ij")*R("ijmn");
//      upper.null_indices();
//      tensor Ep = upper*(1./lower);
//      tensor Eep =  R - Ep; // elastoplastic constitutive tensor
//      return Eep;
//    }
//  
//  //================================================================================
//  // LARGE DEFORMATIONS
//  // LARGE DEFORMATIONS
//  //================================================================================
//  tensor Material_Model::BackwardEulerCTensorLD(tensor & Ftotal,
//                                              stresstensor & start_stress,
//                                              tensor & relDefGrad,
//                                              Material_Model & Criterion )
//    {
//          ::fprintf(stderr,"BackwardEulerCTensorLD \n");
//          ::fprintf(stdout,"BackwardEulerCTensorLD \n");
//      //  stresstensor stemp = start_stress; // zbog upozorenja posle cu da izbacim
//      
//      //    tensor temp1 = Ftotal;     // da zeznem gcc
//      //    tensor temp2 = relDefGrad; // da zeznem gcc
//      
//      double Ey = Criterion.E();
//      double nu = Criterion.nu();
//      tensor Eel = StiffnessTensorE(Ey,nu);
//  
//      //--//    stresstensor stress_increment = Eel("ijkl")*strain_increment("kl");
//      //--    stresstensor stress_increment;
//      //--    stress_increment.null_indices();
//      //--    stresstensor elastic_predictor_stress = start_stress + stress_increment;
//      //--    elastic_predictor_stress.null_indices();
//      //--//elastic_predictor_stress.reportshort("ELASTIC PREDICTOR stress\n");
//      //--//    double f_start = Criterion.f(start_stress);
//      //--//::printf("BackwardEulerCTensor  ##############  f_start = %.10e\n",f_start);
//      //--//    double f_pred = Criterion.f(elastic_predictor_stress);
//      //--//::printf("BackwardEulerCTensor  ##############  f_pred = %.10e\n",f_pred);
//      //--// if el_0r_pl variable ( flag ) is ==0 then it is elastic region
//      //--// otherwise it is plastic region so the full elastoplastic stiffness
//      //--// tensor should be calculated!!!!!!!!
//      //--    if ( Criterion.el_or_pl_range() == 0 )
//      //--      {
//      //--        return Eel;
//      //--      }
//      //--    tensor I2("I", 2, def_dim_2);
//      //--    tensor I_ijkl = I2("ij")*I2("kl");
//      //--    I_ijkl.null_indices();
//      //--    tensor I_ikjl = I_ijkl.transpose0110();
//      //--//region
//      //--//check for the region na proceede
//      //--    int region = Criterion.influence_region(final_stress);
//      //--
//      //--    if ( region == 1 )  // apex gray region
//      //--      {
//      //--        ::fprintf(stderr,"BackwardEulerCTensor APEX GRAY REGION CONSTITUTIVE TENSOR (returning Eel*0.01)\n");
//      //--        ::fprintf(stdout,"BackwardEulerCTensor APEX GRAY REGION CONSTITUTIVE TENSOR (returning Eel*0.01)\n");
//      //--        return Eel*0.01;
//      //--        ::exit(1);
//      //--      }
//      //--// CONSISTENT TENSOR
//      //--    tensor dQods = Criterion.dQods(final_stress);// this is m_kl
//      //--    tensor temp2 = Eel("ijkl")*dQods("kl");
//      //--    temp2.null_indices();
//      //--
//      //--    double lambda = current_lambda_get();
//      //--//====::fprintf(stdout,"\n                  BackwardEulerCTensor  lambda= %.10e \n",lambda);
//      //--    tensor dFods = Criterion.dFods(final_stress);
//      //--//==    // second derivative of potential function Q with
//      //--//==    // respect to stresses
//      //--    tensor d2Qoverds2 = Criterion.d2Qods2(final_stress);
//      //--//...d2Qoverds2.print("*","\n\n\n d2Q/ds2  ");
//      //--//!!!!!!! //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      //--//!!!!!!!     lambda = 0.0; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      //--//!!!!!!! //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//      //--    tensor T = I_ikjl + Eel("ijkl")*d2Qoverds2("klmn")*lambda;
//      //--    T.null_indices();
//      //--//...T.print("t","\n\n\n T ");
//      //--    tensor Tinv = T.inverse();
//      //--//...Tinv.print("ti","\n\n\n Tinv ");
//      //--    tensor R = Tinv("ijmn")*Eel("ijkl");
//      //--    R.null_indices();
//      //--//...R.print("r","\n\n\n R ");
//      //--
//      //--    double h_  =  h(final_stress);
//      //--    double xi_ = xi(final_stress);
//      //--    double hardMod_ = h_ * xi_;
//      //--//    hardMod_        = hardMod_ * just_this_PP;
//      //--//::printf("\n BackwardEulerCTensor  ..  hardMod_ = %.10e \n", hardMod_ );
//      //--//outfornowRunesson    tensor d2Qodqast = d2Qoverdqast(elastic_plastic_predictor_stress);
//      //--//outfornowRunesson    tensor dQodsextended = dQods + d2Qodqast * lambda * h_;
//      //--//outfornowRunesson
//      //--//outfornowRunesson    tensor temp3lower = dFods("mn")*Tinv("ijmn")*Eel("ijkl")*dQodsextended("kl");
//      //--
//      //--//    tensor temp3lower = dFods("mn")*Tinv("ijmn")*Eel("ijkl")*dQods("kl");
//      //--    tensor temp3lower = dFods("ot")*R("otpq")*dQods("pq");
//      //--    temp3lower.null_indices();
//      //--
//      //--    double lower = temp3lower.trace();
//      //--    lower = lower - hardMod_;  // h
//      //--//outfornowRunesson    tensor upper = R("pqkl")*dQodsextended("kl")*dFods("ij")*R("ijmn");
//      //--    tensor upper = R("pqkl")*dQods("kl")*dFods("ij")*R("ijmn");
//      //--    upper.null_indices();
//      //--    tensor Ep = upper*(1./lower);
//      //--    tensor Eep =  R - Ep; // elastoplastic constitutive tensor
//      //--    return Eep;
//      return Eel;
//    }
//  
//  //================================================================================
//  //..tensor Material_Model::SemiBackwardEulerCTensor(stresstensor & start_stress,
//  //..                                                straintensor & strain_increment,
//  //..                                                Material_Model & Criterion,
//  //..                                                int number_of_subincrements,
//  //..                                                double just_this_PP )
//  //================================================================================
//  tensor Material_Model::SemiBackwardEulerCTensor(stresstensor & final_stress,
//                                                  stresstensor & ,
//                                                  straintensor & ,
//                                                  Material_Model & ,
//                                                  double  )
//    {
//      ::fprintf(stderr,"NOT YET IMPLEMENTED\n");
//      ::fprintf(stderr,"Material_Model::SemiBackwardEulerCTensor\n");
//  
//      return final_stress;
//    }


////================================================================================
//// New EPState using Forward Euler Algorithm
////================================================================================
//
//tensor ConstitutiveDriver::ForwardEulerETensor( straintensor &strain_increment, 
//                                                 Template3Dep &material_point)
//{
//    EPState forwardEPS( *material_point.EPS ); 
//    strain_increment.null_indices();
//        
//    // Building elasticity tensor
//    tensor Eel  = material_point.ElasticStiffnessTensor();
//
//    stresstensor stress_increment = Eel("ijkl")*strain_increment("kl");
//    stress_increment.null_indices();
//
//    EPState startEPS( *material_point.EPS );
//    stresstensor start_stress = startEPS.getStress();
//    start_stress.null_indices();
//    //cout << "===== start_EPS =====: " << startEPS;
//
//    double f_start = 0.0;
//    double f_pred  = 0.0;
//
//    EPState ElasticPredictorEPS( startEPS );
//    stresstensor elastic_predictor_stress = start_stress + stress_increment;
//    ElasticPredictorEPS.setStress( elastic_predictor_stress );
//    //cout << " Elastic_predictor_stress: " << elastic_predictor_stress << endln;
//
//    f_start = material_point.YS->f( &startEPS );  
//    cout << "\n#######  f_start = " << f_start;
//    
//    f_pred =  material_point.YS->f( &ElasticPredictorEPS );
//    cout << "  #######  f_pred = " << f_pred << "\n";
//
//    stresstensor intersection_stress = start_stress; // added 20apr2000 for forward euler scheme whrn the stress is outside
//    stresstensor elpl_start_stress = start_stress;
//    //stresstensor true_stress_increment = stress_increment;
//    
//    if ( f_start <= 0 && f_pred <= 0 )
//     {
//        //Updating elastic strain increment
//        straintensor estrain = ElasticPredictorEPS.getElasticStrain();
//        straintensor tstrain = ElasticPredictorEPS.getStrain();
//        estrain = estrain + strain_increment;
//        tstrain = tstrain + strain_increment;
//        ElasticPredictorEPS.setElasticStrain( estrain );
//        ElasticPredictorEPS.setStrain( tstrain );
//        ElasticPredictorEPS.setdElasticStrain( strain_increment );
//    
//        //Evolve parameters like void ratio (e) according to elastic strain
//        double Delta_lambda = 0.0;
//        material_point.EL->UpdateAllVars( &ElasticPredictorEPS, Delta_lambda);
//        
//        //cout <<" strain_increment.Iinvariant1() " << strain_increment.Iinvariant1() << endln;
//        
//        return Eel;
//     }
//
//    if ( f_start <= 0 && f_pred > 0 )
//    {
//       intersection_stress = yield_surface_cross( start_stress,
//                                                  elastic_predictor_stress,
//                                                  material_point);
//
//       // intersection_stress.reportshort("Intersection stress \n");
//       elpl_start_stress = intersection_stress;
//       // elpl_start_stress.reportshortpqtheta("elpl start stress AFTER \n");
//    }
//    
//    forwardEPS.setStress( elpl_start_stress );
//
//    double f_cross =  material_point.YS->f( &forwardEPS );
//    cout << " #######  f_cross = " << f_cross << "\n";
//
//    //set the initial value of D once the current stress hits the y.s. for Manzari-Dafalias Model
//    if ( f_start <= 0 && f_pred > 0 )
//        material_point.EL->setInitD(&forwardEPS);
//    //cout << " inside ConstitutiveDriver after setInitD " << forwardEPS;
//
//    tensor I2("I", 2, def_dim_2);
//    tensor I_ijkl = I2("ij")*I2("kl");
//    I_ijkl.null_indices();
//    // tensor I_ikjl = I_ijkl.transpose0110();
//    
//    //int region = Criterion.influence_region(intersection_stress);
//    //if ( region == 1 )
//    //  { ::fprintf(stderr,"ForwardEulerCTensor APEX GRAY REGION CONSTITUTIVE TENSOR (returning Eel*0.01)\n");
//    //    ::fprintf(stdout,"ForwardEulerCTensor APEX GRAY REGION CONSTITUTIVE TENSOR (returning Eel*0.01)\n");
//    //    return Eel*0.01; 
//    ////Kai      ::exit(1);
//    //  }
//    
//    stresstensor dFods;
//    stresstensor dQods;
//    dQods = material_point.PS->dQods( &forwardEPS );
//    dFods = material_point.YS->dFods( &forwardEPS );
//
//    tensor temp2 = Eel("ijkl")*dQods("kl");
//    temp2.null_indices();
//    
//    //double h_  =  h(intersection_stress);
//    //double xi_ = xi(intersection_stress);
//    //double hardMod_ = h_ * xi_;
//    //hardMod_        = hardMod_ * just_this_PP;
//
//    //Calculating Kp according to Kp = - (df/dq*) * qbar
//    double Kp = material_point.EL->getKp(&forwardEPS, norm_dQods);
//
//    tensor temp3lower = dFods("ot")*Eel("otpq")*dQods("pq");
//    temp3lower.null_indices();
//    double lower = temp3lower.trace();
//    //lower = lower - hardMod_;
//    lower = lower + Kp;
//
//    tensor upper = Eel("pqkl")*dQods("kl")*dFods("ij")*Eel("ijmn");
//    upper.null_indices();
//    tensor Ep = upper*(1./lower);
//    tensor Eep =  Eel - Ep; // elastoplastic constitutive tensor
//    return Eep;
//}
//
//
 
//  //#############################################################################
//  //#############################################################################
//  // this one is intended to shell the previous three and to decide
//  // ( according to the data stored in Material_Model object )
//  // which constitutive tensor to return ( forward ( non-constistent
//  // or backward ( consistent ) or . . .
//  // the only redefined ConstitutiveTensor function will be in
//  // Elastic model since it si very different than any other
//  // model ( it is elastic rather than elastoplastic constitutive tensor!!
//  tensor Material_Model::ConstitutiveTensor(stresstensor   & final_stress,
//                                            stresstensor   & start_stress,
//                                            straintensor   & strain_increment,
//                                            Material_Model & Criterion,
//                                            double           just_this_PP )
//    {
//  // ovde pitaj koji je nacin integracije sa 'strcp' pa onda zovi
//  // jednu od ovih
//  //Material_Model::BackwardEulerCTensor
//  //Material_Model::SemiBackwardEulerCTensor
//  //Material_Model::ForwardEulerCTensor
//      short int_type = Criterion.integration_type();
//  
//      tensor E = StiffnessTensorE(Criterion.E(),Criterion.nu());
//  
//      if ( int_type == 1 ) // BackwardEuler
//        {
//          E = BackwardEulerCTensor(final_stress,
//                                   start_stress,
//                                   strain_increment,
//                                   Criterion,
//  //                                 number_of_subincrements,
//                                   just_this_PP );
//        }
//      else if ( int_type == 2 ) // SemiBackwardEuler
//        {
//          E = SemiBackwardEulerCTensor(final_stress,
//                                       start_stress,
//                                       strain_increment,
//                                       Criterion,
//  //                                     number_of_subincrements,
//                                       just_this_PP );
//        }
//      else if ( int_type == 0 ) // ForwardEuler
//        {
//          E = ForwardEulerCTensor(start_stress,
//                                  strain_increment,
//                                  Criterion,
//                                  just_this_PP );
//  
//        }
//  
//      return E;
//    }
//  
//  
//  //-----------------------------------------------------------------------------
//  //-----------------------------------------------------------------------------
//  //-----------------------------------------------------------------------------
//  //-----------------------------------------------------------------------------
//  //#############################################################################
//  // LARGE DEFORMATIONS
//  // LARGE DEFORMATIONS
//  // LARGE DEFORMATIONS
//  // LARGE DEFORMATIONS
//  // LARGE DEFORMATIONS
//  //#############################################################################
//  // this one is intended to shell the previous three and to decide
//  // ( according to the data stored in Material_Model object )
//  // which constitutive tensor to return ( forward ( non-constistent
//  // or backward ( consistent ) or . . .
//  // the only redefined ConstitutiveTensor function will be in
//  // Elastic model since it si very different than any other
//  // model ( it is elastic rather than elastoplastic constitutive tensor!!
//  // LARGE DEFORMATIONS
//  // LARGE DEFORMATIONS
//  // LARGE DEFORMATIONS
//  tensor Material_Model::ConstitutiveTensorLD(stresstensor   & final_stress,
//                                            stresstensor   & start_stress,
//                                            tensor   & refDefGrad,
//                                            Material_Model & Criterion )
//    {
//  //    short int_type = Criterion.integration_type();
//  //
//      tensor E = StiffnessTensorE(Criterion.E(),Criterion.nu());
//  
//  //    if ( int_type == 1 ) // BackwardEuler
//  //      {
//          E = BackwardEulerCTensorLD(final_stress,
//                                   start_stress,
//                                   refDefGrad,
//                                   Criterion);
//  //      }
//  //    else if ( int_type == 2 ) // SemiBackwardEuler
//  //      {
//  //        E = SemiBackwardEulerCTensor(final_stress,
//  //                                     start_stress,
//  //                                     strain_increment,
//  //                                     Criterion,
//  ////                                     number_of_subincrements,
//  //                                     just_this_PP );
//  //      }
//  //    else if ( int_type == 0 ) // ForwardEuler
//  //      {
//  //        E = ForwardEulerCTensor(start_stress,
//  //                                strain_increment,
//  //                                Criterion,
//  //                                just_this_PP );
//  //
//  //      }
//  
//      return E;
//    }
//  
//  //-----------------------------------------------------------------------------
//  //-----------------------------------------------------------------------------
//  //-----------------------------------------------------------------------------
//  //-----------------------------------------------------------------------------
//  
//  //#############################################################################
//  //#############################################################################
//  // this one is intended to shell the previous three and to decide
//  // ( according to the data stored in Material_Model object )
//  // which constitutive tensor to return ( forward ( non-constistent
//  // or backward ( consistent ) or . . .
//  // the only redefined ConstitutiveTensor function will be in
//  // Elastic model since it si very different than any other
//  // model ( it is elastic rather than elastoplastic constitutive tensor!!
//  stresstensor Material_Model::FinalStress(stresstensor & start_stress,
//                                           straintensor & strain_increment,
//                                           Material_Model & Criterion,
//                                           int number_of_subincrements,
//                                           double just_this_PP )
//    {
//      short int_type = Criterion.integration_type();
//      tensor E = StiffnessTensorE(Criterion.E(),Criterion.nu());
//      stresstensor final_stress = start_stress + E("ijkl")*strain_increment("kl");
//  
//      if ( int_type == 1 ) // BackwardEuler
//        {
//          final_stress = BESubIncrementation(start_stress,
//                                             strain_increment,
//                                             Criterion,
//                                             number_of_subincrements,
//                                             just_this_PP);
//        }
//      else if ( int_type == 2 ) // SemiBackwardEuler
//        {
//          final_stress = SemiBESubIncrementation(start_stress,
//                                                 strain_increment,
//                                                 Criterion,
//                                                 number_of_subincrements,
//                                                 just_this_PP );
//        }
//      else if ( int_type == 0 ) // ForwardEuler
//        {
//          final_stress = FESubIncrementation(start_stress,
//                                             strain_increment,
//                                             Criterion,
//                                             number_of_subincrements,
//                                             just_this_PP );
//  
//        }
//      if ( int_type == 4 ) {}// Elastic do nothing
//  
//      return final_stress;
//    }
//  //#############################################################################
//  
//  //#############################################################################
//  stresstensor Material_Model::BESubIncrementation(stresstensor & start_stress,
//                                                   straintensor & strain_increment,
//                                                   Material_Model & Criterion,
//                                                   int number_of_subincrements,
//                                                   double just_this_PP)
//    {
//  //////////////    ::putc("{");
//  //    ::fprintf(stderr,"{");
//  
//  //    double Ey = Criterion.E();
//  //    double nu = Criterion.nu();
//  
//  // building elasticity tensor
//  //    tensor E = StiffnessTensorE(Ey,nu);
//  // building compliance tensor
//  //    tensor D= ComplianceTensorD(Ey,nu);
//  
//  //    stresstensor stress_increment = E("ijkl")*strain_increment("kl");
//  
//      stresstensor back_stress;
//      stresstensor begin_stress = start_stress;
//  //    stresstensor test_back_stress;
//      double sub = 1./( (double) number_of_subincrements );
//  //    stresstensor elastic_subincremental_stress = stress_increment * sub;
//      straintensor subincremental_strain = strain_increment * sub;
//      straintensor total_strain = subincremental_strain;
//  //..    strain_increment.report("strain_increment");
//  //..    subincremental_strain.report("subincremental_strain");
//  
//  //report
//  // only in non subincremental steps
//  
//  //....
//  //if ( number_of_subincrements != 3 )
//  //  {
//  //    static double e_1     = 0.0;
//  //    static double p_hydro = 0.0;
//  //    static double q_dev   = 0.0;
//  //    static double e_dev   = 0.0;
//  //report
//  //  }
//  //....
//  
//  //..    char st[32];
//  //..    int ndig = 10;
//  
//   //..  elastic_subincremental_stress.reportshort("SUB INCREMENT in stresses\n");
//        for( int steps=0 ; steps < number_of_subincrements ; steps++ )
//          {
//  
//  //..          ::gcvt( steps, ndig, st);
//  //..          ::putc(st);      ::putc(" "  );
//  ////////////////          ::putc(".");
//  //report          ::fprintf(stderr,".");
//  //report          ::fprintf(stdout,"\n\n.................................. %d\n\n",steps);
//  
//            back_stress =
//              Criterion.BackwardEulerStress( begin_stress,
//                                             subincremental_strain,
//                                             Criterion,
//                                             just_this_PP);
//  
//  
//            ::fprintf(stdout,"%3d ->  %.10e %.10e   %.6e %.8e  %.4e\n", 
//                steps,
//                back_stress.q_deviatoric(), 
//                total_strain.tau_octahedral(), 
//                kappa_cone_get(), 
//                eta_c(kappa_cone_get()),
//                f(back_stress ));
//  
//  //---          if ( number_of_subincrements != 3 )
//  //---            {
//  //---//report
//  //---// report for plots !!!!!!!!!!!!!!!!!!!!!!!
//  //---//..    stress_principal = back_stress.principal();
//  //---//..    back_strain = D("ijkl")*back_stress("kl");
//  //---//..    test_back_stress  = E("ijkl")*back_strain("kl");
//  //---//..    back_stress.reportshort("back stress\n");
//  //---//..    test_back_stress.reportshort("test back stress\n");
//  //---//..    back_strain.reportshort("back strain\n");
//  //---
//  //---//    e_1     += (subincremental_strain.principal()).val(3,3);
//  //---//    e_1     += subincremental_strain.q_deviatoric();
//  //---//    p_hydro = back_stress.p_hydrostatic();
//  //---//    q_dev   = back_stress.q_deviatoric();
//  //---//    e_dev   += subincremental_strain.q_deviatoric();
//  //---//::printf("#######  ");
//  //---//::printf(" %.4e    %.4e \n",
//  //---//           q_dev,  e_dev);
//  //---//report
//  //---            }
//  //---
//  
//            begin_stress = back_stress;
//            total_strain = total_strain + subincremental_strain;
//          }
//  ///////////////////    ::putc("}");
//  //    ::fprintf(stderr,"}");
//      return back_stress;
//    }
//  
//  
//  //#############################################################################
//  stresstensor Material_Model::SemiBESubIncrementation(stresstensor & start_stress,
//                                                       straintensor & strain_increment,
//                                                       Material_Model & Criterion,
//                                                       int number_of_subincrements,
//                                                       double just_this_PP )
//    {
//      stresstensor back_stress;
//      stresstensor begin_stress = start_stress;
//  //    ::fprintf(stderr,"{");
//  
//  //    double Ey = Criterion.E();
//  //    double nu = Criterion.nu();
//  
//  // building elasticity tensor
//  //    tensor E = Criterion.StiffnessTensorE(Ey,nu);
//  // building compliance tensor
//  //    tensor D = Criterion.ComplianceTensorD(Ey,nu);
//  
//  //    stresstensor stress_increment = E("ijkl")*strain_increment("kl");
//  
//      double sub = 1./( (double) number_of_subincrements );
//  //    stresstensor elastic_subincremental_stress = stress_increment * sub;
//      straintensor elastic_subincremental_strain = strain_increment * sub;
//  
//  //..    char st[32];
//  //..    int ndig = 10;
//  
//   //..  elastic_subincremental_stress.reportshort("SUB INCREMENT in stresses\n");
//        for( int steps=0 ; steps < number_of_subincrements ; steps++ )
//          {
//  
//  //..          ::gcvt( steps, ndig, st);
//  //..          ::putc(st);      ::putc(" "  );
//  //..          ::putc(".");
//  //    ::fprintf(stderr,".");
//  
//            back_stress =
//              Criterion.SemiBackwardEulerStress( begin_stress,
//                                                 elastic_subincremental_strain,
//                                                 Criterion,
//                                                 just_this_PP);
//            begin_stress = back_stress;
//          }
//  //    ::fprintf(stderr,"}");
//      return back_stress;
//    }
  
 

//================================================================================
// New EPState using Forward Euler Subincrement Euler Algorithm
//================================================================================
EPState ConstitutiveDriver::FESubIncrementation( straintensor & strain_increment,
                                                 Template3Dep &material_point,
                                                 int number_of_subincrements)                                                 
{
    EPState FESI_EPS( *material_point.getEPS() ); 
    Template3Dep MP( material_point );
    //cout << "in FESubIncrementation MP " << MP;

    stresstensor back_stress;
    stresstensor begin_stress = material_point.getEPS()->getStress();
    //stresstensor begin_stress = start_stress;
    //::fprintf(stderr,"{");
  
    double sub = 1./( (double) number_of_subincrements );
    //stresstensor elastic_subincremental_stress = stress_increment * sub;

    straintensor elastic_subincremental_strain = strain_increment * sub;
    straintensor total_strain = elastic_subincremental_strain;
    //elastic_subincremental_stress.reportshort("SUB INCREMENT in stresses\n");
   
    for( int steps=0 ; steps < number_of_subincrements ; steps++ ){

        //start_stress.reportshort("START stress\n");
        FESI_EPS = ForwardEulerEPState( elastic_subincremental_strain, MP);
        MP.setEPS( FESI_EPS );
	                      
        back_stress = FESI_EPS.getStress();
	cout.unsetf(ios::showpos);
	cout << setw(4);
        cout << "Step No. " << steps << "  ";

	cout.setf(ios::scientific);
	cout.setf(ios::showpos);
	cout.precision(3);
	cout << setw(7);
	cout << "p " << back_stress.p_hydrostatic() << "  "; 
	cout << setw(7);
	cout << "q " << back_stress.q_deviatoric() << "  "; 
	cout << setw(7);
	cout << "alfa1 " << FESI_EPS.getScalarVar(1) << "  "; 
	cout << setw(7);
	cout << "f = " << MP.getYS()->f( &FESI_EPS ) << "  "<< endln;
  
        //begin_stress = back_stress;  
        //total_strain = total_strain + elastic_subincremental_strain;

     }

     //    ::fprintf(stderr,"}");
     return FESI_EPS;

}
  


//================================================================================
// New EPState using Backward Euler Subincrement Euler Algorithm
//================================================================================
EPState ConstitutiveDriver::BESubIncrementation( straintensor & strain_increment,
                                                 Template3Dep &material_point,
                                                 int number_of_subincrements)                                                 
{
    EPState BESI_EPS( *material_point.getEPS() ); 
    Template3Dep MP( material_point );
    //cout << "in FESubIncrementation MP " << MP;

    stresstensor back_stress;
    stresstensor begin_stress = material_point.getEPS()->getStress();
    //stresstensor begin_stress = start_stress;
    //::fprintf(stderr,"{");
  
    double sub = 1./( (double) number_of_subincrements );
    //stresstensor elastic_subincremental_stress = stress_increment * sub;

    straintensor elastic_subincremental_strain = strain_increment * sub;
    straintensor total_strain = elastic_subincremental_strain;
    //elastic_subincremental_stress.reportshort("SUB INCREMENT in stresses\n");
   
    for( int steps=0 ; steps < number_of_subincrements ; steps++ ){

        //start_stress.reportshort("START stress\n");
        BESI_EPS = BackwardEulerEPState( elastic_subincremental_strain, MP);
        MP.setEPS( BESI_EPS );
	                      
        //back_stress = BESI_EPS.getStress();
	//cout.unsetf(ios::showpos);
	//cout << setw(4);
        //cout << "Step No. " << steps << "  ";
	//
	//cout.setf(ios::scientific);
	//cout.setf(ios::showpos);
	//cout.precision(3);
	//cout << setw(7);
	//cout << "p " << back_stress.p_hydrostatic() << "  "; 
	//cout << setw(7);
	//cout << "q " << back_stress.q_deviatoric() << "  "; 
	//cout << setw(7);
	//cout << "alfa1 " << BESI_EPS.getScalarVar(1) << "  "; 
	//cout << setw(7);
	//cout << "f = " << MP.YS->f( &BESI_EPS ) << "  "<< endln;
  
        ////begin_stress = back_stress;  
        ////total_strain = total_strain + elastic_subincremental_strain;

     }

     //    ::fprintf(stderr,"}");
     return BESI_EPS;

}
  


//================================================================================
// Routine used to generate elastic stiffness tensor E
//================================================================================
tensor ConstitutiveDriver::ElasticStiffnessTensor( double E, double nu) const
  {
    tensor ret( 4, def_dim_4, 0.0 );

    tensor I2("I", 2, def_dim_2);
    tensor I_ijkl = I2("ij")*I2("kl");
    I_ijkl.null_indices();
    tensor I_ikjl = I_ijkl.transpose0110();
    tensor I_iljk = I_ijkl.transpose0111();
    tensor I4s = (I_ikjl+I_iljk)*0.5;

    // Building elasticity tensor
    ret = (I_ijkl*((E*nu*2.0)/(2.0*(1.0+nu)*(1-2.0*nu)))) + (I4s*(E/((1.0+nu))));

    return ret;
  }


//================================================================================
// Routine used to generate elastic compliance tensor D
//================================================================================

tensor ConstitutiveDriver::ElasticComplianceTensor( double E, double nu) const
  {
    if (E == 0) {
      cout << endln << "Ey = 0! Can't give you D!!" << endln;
      exit(1);
    }

    tensor ret( 4, def_dim_4, 0.0 );
    //tensor ret;
    
    tensor I2("I", 2, def_dim_2);
    tensor I_ijkl = I2("ij")*I2("kl");
    I_ijkl.null_indices();
    tensor I_ikjl = I_ijkl.transpose0110();
    tensor I_iljk = I_ijkl.transpose0111();
    tensor I4s = (I_ikjl+I_iljk)*0.5;

    // Building compliance tensor
    ret = (I_ijkl * (-nu/E)) + (I4s * ((1.0+nu)/E));

    return ret;
  }


//================================================================================
// trying to find intersection point				  
// according to M. Crisfield's book				  
// "Non-linear Finite Element Analysis of Solids and Structures " 
// chapter 6.6.1 page 168.                                        
//================================================================================
stresstensor ConstitutiveDriver::yield_surface_cross(const stresstensor & start_stress,
                                                     const stresstensor & end_stress,
                                                     const Template3Dep & material_point)
{
  // Bounds
  double x1 = 0.0;
  double x2 = 1.0;
  
  // accuracy
  double const TOL = 1.0e-9;
  //cout << "start_stress "<< start_stress;
  //cout << "end_stress " << end_stress;
  //end_stress.reportshortpqtheta("end stress");
  
  double a = zbrentstress( start_stress, end_stress, material_point, x1, x2, TOL ); // Defined below 
  // ::printf("\n****\n a = %lf \n****\n",a);
  
  stresstensor delta_stress = end_stress - start_stress;
  stresstensor intersection_stress = start_stress + delta_stress * a;
  //***  intersection_stress.reportshort("FINAL Intersection stress\n");

  return intersection_stress;

}


//================================================================================
// Routine used by yield_surface_cross to 
// find the stresstensor at cross point   
//================================================================================

double ConstitutiveDriver::zbrentstress(const stresstensor & start_stress,
                                        const stresstensor & end_stress,
                                        const Template3Dep & material_point,
                                        double x1, double x2, double tol)
{
  double EPS = d_macheps();

  int iter;
  double a=x1;
  double b=x2;
  double c=0.0;
  double d=0.0;
  double e=0.0;
  double min1=0.0;
  double min2=0.0;
  double fa=func(start_stress, end_stress, material_point, a);
  double fb=func(start_stress, end_stress, material_point, b);
  //cout << "fb = " << fb;

  double fc=0.0;
  double p=0.0;
  double q=0.0;
  double r=0.0;
  double s=0.0;
  double tol1=0.0;
  double xm=0.0;
  //::printf("\n############# zbrentstress iterations --\n");
  if (fb*fa > 0.0)
    {
      ::printf("\a\nRoot must be bracketed in ZBRENTstress\n");
      ::exit(1);
    }
  fc=fb;
  for ( iter=1 ; iter<=ITMAX ; iter++ )
  {
      //::printf("iter No. = %d  ;  b = %16.10lf\n", iter, b);
      if (fb*fc > 0.0)
        {
          c=a;
          fc=fa;
          e=d=b-a;
        }
      if (fabs(fc) < fabs(fb))
        {
          a=b;
          b=c;
          c=a;
          fa=fb;
          fb=fc;
          fc=fa;
        }
      tol1=2.0*EPS*fabs(b)+0.5*tol;
      xm=0.5*(c-b);
      if (fabs(xm) <= tol1 || fb == 0.0) return b;
      if (fabs(e) >= tol1 && fabs(fa) > fabs(fb))
        {
          s=fb/fa;
          if (a == c)
            {
              p=2.0*xm*s;
              q=1.0-s;
            }
          else
            {
              q=fa/fc;
              r=fb/fc;
              p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
              q=(q-1.0)*(r-1.0)*(s-1.0);
            }
          if (p > 0.0)  q = -q;
          p=fabs(p);
          min1=3.0*xm*q-fabs(tol1*q);
          min2=fabs(e*q);
          if (2.0*p < (min1 < min2 ? min1 : min2))
            {
              e=d;
              d=p/q;
            }
          else
            {
              d=xm;
              e=d;
            }
        }
      else
        {
          d=xm;
          e=d;
        }
      a=b;
      fa=fb;
      if (fabs(d) > tol1)
        b += d;
      else
        b += (xm > 0.0 ? fabs(tol1) : -fabs(tol1));
      fb=func(start_stress, end_stress, material_point, b);
  }
  ::printf("\a\nMaximum number of iterations exceeded in zbrentstress\n");
  return 0.0; // this just to full the compiler because of the warnings
}


//================================================================================
// routine used by zbrentstress, takes an alfa and returns the
// yield function value for that alfa
//================================================================================
double ConstitutiveDriver::func(const stresstensor & start_stress,
                                const stresstensor & end_stress,
                                const Template3Dep & material_point,
                                      double alfa )
{
   
   EPState *tempEPS = material_point.getEPS()->newObj();
   stresstensor delta_stress = end_stress - start_stress;
   stresstensor intersection_stress = start_stress + delta_stress*alfa;

   tempEPS->setStress(intersection_stress); 
   
   //cout << "*tempEPS" << *tempEPS;
   
   double f = material_point.getYS()->f( tempEPS );
   return f;
}


#endif
