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
                                                                        
// $Revision: 1.12 $
// $Date: 2001-08-04 01:52:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/TclModelBuilderNDMaterialCommand.cpp,v $
                                                                       
                                                                      
// Description: This file contains the function invoked when the user invokes
// the nDMaterial command in the interpreter.
//
// What: "@(#) TclModelBuilderNDMaterialCommand.C, revA"

#include <TclModelBuilder.h>

#include <ElasticIsotropicMaterial.h>
#include <ElasticIsotropic3D.h>
#include <PressureDependentElastic3D.h>
#include <J2Plasticity.h>
#include <BidirectionalMaterial.h>


#include <PressureIndependMultiYield.h>
#include <PressureDependMultiYield.h>
#include <FluidSolidPorousMaterial.h>

#include <string.h>

#include <Template3Dep.h>

Template3Dep *
TclModelBuilder_addTemplate3Dep(ClientData clientData, Tcl_Interp *interp,  int argc, 
				char **argv, TclModelBuilder *theTclBuilder, int eleArgStart);

static void printCommand(int argc, char **argv)
{
    cerr << "Input command: ";
    for (int i=0; i<argc; i++)
	cerr << argv[i] << " ";
    cerr << endl;
} 

int
TclModelBuilderNDMaterialCommand (ClientData clientData, Tcl_Interp *interp, int argc,
				  char **argv, TclModelBuilder *theTclBuilder)
{
    // Make sure there is a minimum number of arguments
    if (argc < 3) {
	cerr << "WARNING insufficient number of ND material arguments\n";
	cerr << "Want: nDMaterial type? tag? <specific material args>" << endl;
	return TCL_ERROR;
    }

    // Pointer to an ND material that will be added to the model builder
    NDMaterial *theMaterial = 0;

    // Check argv[1] for ND material type

			 //Jul. 07, 2001 Boris Jeremic & ZHaohui Yang jeremic|zhyang@ucdavis.edu
    // Pressure dependent elastic material
    if (strcmp(argv[1],"PressureDependentElastic3D") == 0) {
	if (argc < 6) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial PressureDependentElastic3D tag? E? v? rho?" << endl;
	    return TCL_ERROR;
	}    

	int tag = 0;
	double E = 0.0;
	double v = 0.0;
	double rho = 0.0;
	double expp = 0.0;
	double prp = 0.0;
	double pop = 0.0;
	
	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid PressureDependentElastic3D tag" << endl;
	    return TCL_ERROR;		
	}

	if (Tcl_GetDouble(interp, argv[3], &E) != TCL_OK) {
	    cerr << "WARNING invalid E\n";
	    cerr << "nDMaterial PressureDependentElastic3D: E" << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[4], &v) != TCL_OK) {
	    cerr << "WARNING invalid v\n";
	    cerr << "nDMaterial PressureDependentElastic3D: v" << tag << endl;
	    return TCL_ERROR;	
	}
				 
	if (Tcl_GetDouble(interp, argv[5], &rho) != TCL_OK) {
	    cerr << "WARNING invalid v\n";
	    cerr << "nDMaterial PressureDependentElastic3D: rho" << tag << endl;
	    return TCL_ERROR;	
	}
	
//////////////////////////////////////////////////////////////////////////////////
	if( argc == 6 ) 
	{
     	   theMaterial = new PressureDependentElastic3D (tag, E, v, rho);
	   //cerr << "nDMaterial PressureDependentElastic3D: expp =" << expp << endl;
	}
//////////////////////////////////////////////////////////////////////////////////
	else if( argc == 7 ) 
	{
	   //get the exponent of the pressure sensitive elastic material)
	   if (Tcl_GetDouble(interp, argv[6], &expp) != TCL_OK) {
	       cerr << "WARNING invalid v\n";
	       cerr << "nDMaterial PressureDependentElastic3D: " << tag << endl;
	       return TCL_ERROR;	
	   }
     	   theMaterial = new PressureDependentElastic3D (tag, E, v, rho, expp);
	   //cerr << "nDMaterial PressureDependentElastic3D: expp =" << expp << endl;
	}
//////////////////////////////////////////////////////////////////////////////////
	else if (argc == 8 ) 
	{
	   //get the exponent pressure of the pressure sensitive elastic material)
	   if (Tcl_GetDouble(interp, argv[6], &expp) != TCL_OK) {
	       cerr << "WARNING invalid v\n";
	       cerr << "nDMaterial PressureDependentElastic3D: expp" << tag << endl;
	       return TCL_ERROR;	
	   } 
	   //get the reference pressure of the pressure sensitive elastic material)
	   if (Tcl_GetDouble(interp, argv[7], &prp) != TCL_OK) {
	       cerr << "WARNING invalid v\n";
	       cerr << "nDMaterial PressureDependentElastic3D: prp " << tag << endl;
	       return TCL_ERROR;	
	   }
	   //cerr << "nDMaterial ElasticIsotropic3D: prp =" << prp << endl;
     	   theMaterial = new PressureDependentElastic3D (tag, E, v, rho, expp, prp);
	}
//////////////////////////////////////////////////////////////////////////////////
	else if (argc >= 9 ) 
	{
	   //get the exponent of the pressure sensitive elastic material)
	   if (Tcl_GetDouble(interp, argv[6], &expp) != TCL_OK) {
	       cerr << "WARNING invalid v\n";
	       cerr << "nDMaterial PressureDependentElastic3D: expp" << tag << endl;
	       return TCL_ERROR;	
	   }
	   //get the reference pressure of the pressure sensitive elastic material)
	   if (Tcl_GetDouble(interp, argv[7], &prp) != TCL_OK) {
	       cerr << "WARNING invalid v\n";
	       cerr << "nDMaterial PressureDependentElastic3D: prp" << tag << endl;
	       return TCL_ERROR;	
	   }
	   //get the cutoff pressure po of the pressure sensitive elastic material)
	   if (Tcl_GetDouble(interp, argv[8], &pop) != TCL_OK) {
	       cerr << "WARNING invalid v\n";
	       cerr << "nDMaterial PressureDependentElastic3D: pop" << tag << endl;
	       return TCL_ERROR;	
	   }
	   //cerr << "nDMaterial PressureDependentElastic3D: pop =" << pop << endl;
     	   theMaterial = new PressureDependentElastic3D (tag, E, v, rho, expp, prp, pop);
	}

    }
    //Jul. 07, 2001 Boris Jeremic & ZHaohui Yang jeremic|zhyang@ucdavis.edu
    // Linear Elastic Material (non-pressure dependent)
    else if ( strcmp(argv[1],"ElasticIsotropic3D") == 0 )
    {
	if (argc < 6) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial ElasticIsotropic3D tag? E? v? <rho?>" << endl;
	    return TCL_ERROR;
	}    

	int tag = 0;
	double E = 0.0;
	double v = 0.0;
	double rho = 0.0;
	
	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid ElasticIsotropic3D tag" << endl;
	    return TCL_ERROR;		
	}

	if (Tcl_GetDouble(interp, argv[3], &E) != TCL_OK) {
	    cerr << "WARNING invalid E\n";
	    cerr << "nDMaterial ElasticIsotropic3D: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[4], &v) != TCL_OK) {
	    cerr << "WARNING invalid v\n";
	    cerr << "nDMaterial ElasticIsotropic3D: " << tag << endl;
	    return TCL_ERROR;	
	}
	
 
 if (argc > 5 && Tcl_GetDouble(interp, argv[5], &rho) != TCL_OK) 
   {
     cerr << "WARNING invalid rho\n";
     cerr << "nDMaterial ElasticIsotropic: " << tag << endl;
     return TCL_ERROR;   
   }

	theMaterial = new ElasticIsotropic3D (tag, E, v, rho);

    }

    else if (strcmp(argv[1],"ElasticIsotropic") == 0) {
	if (argc < 5) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial ElasticIsotropic tag? E? v? <rho?>" << endl;
	    return TCL_ERROR;
	}    

	int tag;
	double E, v;
	double rho = 0.0;
	
	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid ElasticIsotropic tag" << endl;
	    return TCL_ERROR;		
	}

	if (Tcl_GetDouble(interp, argv[3], &E) != TCL_OK) {
	    cerr << "WARNING invalid E\n";
	    cerr << "nDMaterial ElasticIsotropic: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[4], &v) != TCL_OK) {
	    cerr << "WARNING invalid v\n";
	    cerr << "nDMaterial ElasticIsotropic: " << tag << endl;
	    return TCL_ERROR;	
	}

 if (argc > 5 && Tcl_GetDouble(interp, argv[5], &rho) != TCL_OK) 
   {
     cerr << "WARNING invalid rho\n";
     cerr << "nDMaterial ElasticIsotropic: " << tag << endl;
     return TCL_ERROR;   
   }

	theMaterial = new ElasticIsotropicMaterial (tag, E, v, rho);
	}	
    
    else if (strcmp(argv[1],"Bidirectional") == 0) {
	if (argc < 7) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial Bidirectional tag? E? sigY? Hiso? Hkin?" << endl;
	    return TCL_ERROR;
	}    

	int tag;
	double E, sigY, Hi, Hk;
	
	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid Bidirectional tag" << endl;
	    return TCL_ERROR;		
	}

	if (Tcl_GetDouble(interp, argv[3], &E) != TCL_OK) {
	    cerr << "WARNING invalid E\n";
	    cerr << "nDMaterial Bidirectional: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[4], &sigY) != TCL_OK) {
	    cerr << "WARNING invalid sigY\n";
	    cerr << "nDMaterial Bidirectional: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[5], &Hi) != TCL_OK) {
	    cerr << "WARNING invalid Hiso\n";
	    cerr << "nDMaterial Bidirectional: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[6], &Hk) != TCL_OK) {
	    cerr << "WARNING invalid Hkin\n";
	    cerr << "nDMaterial Bidirectional: " << tag << endl;
	    return TCL_ERROR;	
	}

	theMaterial = new BidirectionalMaterial (tag, E, sigY, Hi, Hk);
	}

    // Check argv[1] for J2PlaneStrain material type
    else if ((strcmp(argv[1],"J2Plasticity") == 0)  ||
	     (strcmp(argv[1],"J2") == 0)) {
	if (argc < 9) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial J2Plasticity tag? K? G? sig0? sigInf? delta? H?" << endl;
	    return TCL_ERROR;
	}    

	int tag;
	double K, G, sig0, sigInf, delta, H;
	
	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid J2Plasticity tag" << endl;
	    return TCL_ERROR;		
	}

	if (Tcl_GetDouble(interp, argv[3], &K) != TCL_OK) {
	    cerr << "WARNING invalid K\n";
	    cerr << "nDMaterial J2Plasticity: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[4], &G) != TCL_OK) {
	    cerr << "WARNING invalid G\n";
	    cerr << "nDMaterial J2Plasticity: " << tag << endl;
	    return TCL_ERROR;	
	}	

	if (Tcl_GetDouble(interp, argv[5], &sig0) != TCL_OK) {
	    cerr << "WARNING invalid sig0\n";
	    cerr << "nDMaterial J2Plasticity: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[6], &sigInf) != TCL_OK) {
	    cerr << "WARNING invalid sigInf\n";
	    cerr << "nDMaterial J2Plasticity: " << tag << endl;
	    return TCL_ERROR;	
	}

	if (Tcl_GetDouble(interp, argv[7], &delta) != TCL_OK) {
	    cerr << "WARNING invalid delta\n";
	    cerr << "nDMaterial J2Plasticity: " << tag << endl;
	    return TCL_ERROR;	
	}	
	if (Tcl_GetDouble(interp, argv[8], &H) != TCL_OK) {
	    cerr << "WARNING invalid H\n";
	    cerr << "nDMaterial J2Plasticity: " << tag << endl;
	    return TCL_ERROR;	
	}			

	theMaterial = new J2Plasticity (tag, 0, K, G, sig0, sigInf, 
					delta, H);
    }	
    
    // Pressure Independend Multi-yield, by ZHY
    else if (strcmp(argv[1],"PressureIndependMultiYield") == 0) {
	const int numParam = 9;
	int tag;  double param[numParam]; 	
	char * arg[] = {"nd", "refShearModul", "refBulkModul", "frictionAng", 
			"peakShearStra", "refPress", "cohesi", 
			"pressDependCoe", "numberOfYieldSurf"};
	if (argc < (3+numParam)) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial PressureIndependMultiYield tag? " << arg[0];
	    cerr << "? "<< "\n";
	    cerr << arg[1] << "? "<< arg[2] << "? "<< arg[3] << "? "<< "\n";
	    cerr << arg[4] << "? "<< arg[5] << "? "<< arg[6] << "? "<< "\n";
	    cerr << arg[7] << "? "<< arg[8] << "? "<<endl;
	    return TCL_ERROR;
	}    
	
	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid PressureIndependMultiYield tag" << endl;
	    return TCL_ERROR;		
	}

	for (int i=3; i<(3+numParam); i++) 
	    if (Tcl_GetDouble(interp, argv[i], &param[i-3]) != TCL_OK) {
		cerr << "WARNING invalid " << arg[i-3] << "\n";
		cerr << "nDMaterial PressureIndependMultiYield: " << tag << endl;
		return TCL_ERROR;	
	    }
	
	PressureIndependMultiYield * temp = 
	    new PressureIndependMultiYield (tag, param[0], param[1], param[2], 
					    param[3], param[4], param[5], param[6], 
					    param[7], param[8]);
	theMaterial = temp;
    }	
    
    
    // Pressure Dependend Multi-yield, by ZHY
    else if (strcmp(argv[1],"PressureDependMultiYield") == 0) {
	const int numParam = 20;
	int tag;  double param[numParam]; 	
	char * arg[] = {"nd", "refShearModul", "refBulkModul", "frictionAng", 
			"peakShearStra", "refPress", "cohesi", "pressDependCoe", 
			"numberOfYieldSurf", "phaseTransformAngle", 
			"contractionParam1", "contractionParam2", 
			"dilationParam1", "dilationParam2", "volLimit",
			"liquefactionParam1", "liquefactionParam2", 
			"liquefactionParam3", "liquefactionParam4", "atm"};
	if (argc < (3+numParam)) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial PressureDependMultiYield tag? "<< arg[0];
	    cerr << "? "<< "\n";
	    cerr << arg[1] << "? "<< arg[2] << "? "<< arg[3] << "? "<< "\n";
	    cerr << arg[4] << "? "<< arg[5] << "? "<< arg[6] << "? "<< "\n";
	    cerr << arg[7] << "? "<< arg[8] << "? "<< arg[9] << "? "<< "\n";
	    cerr << arg[10] << "? "<< arg[11] << "? "<< arg[12] << "? "<< "\n";
	    cerr << arg[13] << "? "<< arg[14] << "? "<< arg[15] << "? "<< "\n"; 
	    cerr << arg[16] << "? "<< arg[17] << "? "<< arg[18] << "? "<< "\n"; 
	    cerr << arg[19] << "? " <<endl;
	    return TCL_ERROR;
	}    

	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid PressureDependMultiYield tag" << endl;
	    return TCL_ERROR;		
	}

	for (int i=3; i<(3+numParam); i++) 
	  if (Tcl_GetDouble(interp, argv[i], &param[i-3] ) != TCL_OK) {
	      cerr << "WARNING invalid " << arg[i-3] << "\n";
	      cerr << "nDMaterial PressureDependMultiYield: " << tag << endl;
	      return TCL_ERROR;	
	  }

	PressureDependMultiYield * temp =
	    new PressureDependMultiYield (tag, param[0], param[1], param[2], 
					  param[3], param[4], param[5], 
					  param[6], param[7], param[8], 
					  param[9], param[10], param[11], 
					  param[12], param[13], param[14], 
					  param[15], param[16], param[17], 
					  param[18], param[19]);
					  
	theMaterial = temp;	
    }	

    // Fluid Solid Porous, by ZHY
    else if (strcmp(argv[1],"FluidSolidPorous") == 0) {

	int tag;  double param[4]; 	
	char * arg[] = {"nd", "soilMatTag", "combinedBulkModul", "atm"};
	if (argc < 7) {
	    cerr << "WARNING insufficient arguments\n";
	    printCommand(argc,argv);
	    cerr << "Want: nDMaterial FluidSolidPorous tag? "<< arg[0];
	    cerr << "? "<< "\n";
	    cerr << arg[1] << "? "<< arg[2] << "? "<< arg[3] << "? "<< endl;
	    return TCL_ERROR;
	}    

	if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	    cerr << "WARNING invalid FluidSolidPorous tag" << endl;
	    return TCL_ERROR;		
	}

	for (int i=3; i<7; i++) 
	  if (Tcl_GetDouble(interp, argv[i], &param[i-3] ) != TCL_OK) {
	      cerr << "WARNING invalid " << arg[i-3] << "\n";
	      cerr << "nDMaterial FluidSolidPorous: " << tag << endl;
	      return TCL_ERROR;	
	  }

	NDMaterial *soil = theTclBuilder->getNDMaterial(param[1]);
	if (soil == 0) {
	      cerr << "WARNING FluidSolidPorous: couldn't get soil material ";
	      cerr << "tagged: " << param[1] << "\n";
	      return TCL_ERROR;	
	}

	soil = soil->getCopy();
	theMaterial = new FluidSolidPorousMaterial (tag, param[0], soil, 
						    param[2], param[3]);
    }	    
    else if (strcmp(argv[1],"Template3Dep") == 0) {
      theMaterial = TclModelBuilder_addTemplate3Dep(clientData, interp, argc, argv, 
						    theTclBuilder, 2);
    }
    
    else {
	cerr << "WARNING unknown type of nDMaterial: " << argv[1];
	cerr << "\nValid types: ElasticIsotropic, J2Plasticity, Bidirectional\n";
	return TCL_ERROR;
    }

    // Ensure we have created the Material, out of memory if got here and no material
    if (theMaterial == 0) {
	cerr << "WARNING ran out of memory creating nDMaterial\n";
	cerr << argv[1] << endl;
	return TCL_ERROR;
    }

    // Now add the material to the modelBuilder
    if (theTclBuilder->addNDMaterial(*theMaterial) < 0) {
	cerr << "WARNING could not add material to the domain\n";
	cerr << *theMaterial << endl;
	delete theMaterial; // invoke the material objects destructor, otherwise mem leak
	return TCL_ERROR;
    }
    
    return TCL_OK;
}

