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
                                                                        
// $Revision: 1.33 $
// $Date: 2002-07-01 22:10:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/tcl/commands.cpp,v $
                                                                        
                                                                        
// File: ~/tcl/commands.C
// 
// Written: fmk 
// Created: 04/98
//
// Description: This file contains the functions that will be called by
// the interpreter when the appropriate command name is specified,
// see tkAppInit.C for command names.
//
// What: "@(#) commands.C, revA"

extern "C" {
#include <tcl.h>
#include <tk.h>
}

#include <G3Globals.h>
#include <FEM_ObjectBroker.h>

#include <RigidRod.h>
#include <RigidBeam.h>
#include <RigidDiaphragm.h>

#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Timer.h>

#include <ModelBuilder.h>
#include "commands.h"

// domain
#include <Domain.h>
#include <Element.h>
#include <Node.h>
#include <ElementIter.h>
#include <NodeIter.h>
#include <LoadPattern.h>
#include <LoadPatternIter.h>
#include <ElementalLoad.h>
#include <ElementalLoadIter.h>
#include <SP_Constraint.h> //Joey UC Davis


// analysis model
#include <AnalysisModel.h>

// convergence tests
#include <CTestNormUnbalance.h>
#include <CTestNormDispIncr.h>
#include <CTestEnergyIncr.h>
#include <CTestRelativeNormUnbalance.h>
#include <CTestRelativeNormDispIncr.h>
#include <CTestRelativeEnergyIncr.h>

// soln algorithms
#include <Linear.h>
#include <NewtonRaphson.h>
#include <NewtonLineSearch.h>
#include <ModifiedNewton.h>
#include <FrequencyAlgo.h>
#include <StandardEigenAlgo.h>
#include <Broyden.h>
#include <BFGS.h>
#include <KrylovNewton.h>


// line searches
#include <BisectionLineSearch.h>
#include <InitialInterpolatedLineSearch.h>
#include <RegulaFalsiLineSearch.h>
#include <SecantLineSearch.h>

// constraint handlers
#include <PlainHandler.h>
#include <PenaltyConstraintHandler.h>
#include <LagrangeConstraintHandler.h>
#include <TransformationConstraintHandler.h>

// numberers
#include <PlainNumberer.h>
#include <DOF_Numberer.h>

// integrators
#include <LoadControl.h>
#include <ArcLength.h>
#include <ArcLength1.h>
#include <MinUnbalDispNorm.h>
#include <DisplacementControl.h>
#include <Newmark.h>
#include <WilsonTheta.h>
#include <HHT.h>
#include <HHT1.h>
#include <Newmark1.h> 
#include <EigenIntegrator.h>

// analysis
#include <StaticAnalysis.h>
#include <DirectIntegrationAnalysis.h>
#include <VariableTimeStepDirectIntegrationAnalysis.h>
#include <EigenAnalysis.h>

// system of eqn and solvers
// #include <SlowLinearSOE.h>
// #include <SlowLinearSOESolver.h>
#include <BandSPDLinSOE.h>
#include <BandSPDLinLapackSolver.h>
#include <BandGenLinSOE.h>
#include <BandGenLinLapackSolver.h>


#include <ConjugateGradientSolver.h>

#include <FullGenLinSOE.h>
#include <FullGenLinLapackSolver.h>
#include <ProfileSPDLinSOE.h>
#include <ProfileSPDLinDirectSolver.h>

// #include <ProfileSPDLinDirectBlockSolver.h>
// #include <ProfileSPDLinDirectThreadSolver.h>
// #include <ProfileSPDLinDirectSkypackSolver.h>
// #include <BandSPDLinThreadSolver.h>

#include <SparseGenColLinSOE.h>
#ifdef _THREADS
#include <ThreadedSuperLU.h>
#else
#include <SuperLU.h>
#endif

#include <SymSparseLinSOE.h>
#include <SymSparseLinSolver.h>


#include <UmfpackGenLinSOE.h>
#include <UmfpackGenLinSolver.h>

#include <EigenSOE.h>
#include <EigenSolver.h>
#include <SymArpackSOE.h>
#include <SymArpackSolver.h>
#include <BandArpackSOE.h>
#include <BandArpackSolver.h>
#include <SymBandEigenSOE.h>
#include <SymBandEigenSolver.h>

// graph
#include <RCM.h>

#include <ErrorHandler.h>
#include <ConsoleErrorHandler.h>

#include <TclVideoPlayer.h>
#include <FE_Datastore.h>


#ifdef _RELIABILITY
// AddingSensitivity:BEGIN /////////////////////////////////////////////////
#include <ReliabilityDomain.h>
#include <SensitivityAlgorithm.h>
#include <SensitivityIntegrator.h>
#include <StaticSensitivityIntegrator.h>
#include <RandomVariablePositioner.h>
// AddingSensitivity:END /////////////////////////////////////////////////
#include <TclReliabilityBuilder.h>
static TclReliabilityBuilder *theReliabilityBuilder = 0;
int reliability(ClientData, Tcl_Interp *, int, char **);
#endif

ModelBuilder *theBuilder =0;
Domain theDomain;

static AnalysisModel *theAnalysisModel =0;
static EquiSolnAlgo *theAlgorithm =0;
static ConstraintHandler *theHandler =0;
static DOF_Numberer *theNumberer =0;
static LinearSOE *theSOE =0;
static StaticAnalysis *theStaticAnalysis = 0;
static DirectIntegrationAnalysis *theTransientAnalysis = 0;
static VariableTimeStepDirectIntegrationAnalysis *theVariableTimeStepTransientAnalysis = 0;

// AddingSensitivity:BEGIN /////////////////////////////////////////////
#ifdef _RELIABILITY
static SensitivityAlgorithm *theSensitivityAlgorithm = 0;
static SensitivityIntegrator *theSensitivityIntegrator = 0;
#endif
// AddingSensitivity:END ///////////////////////////////////////////////

static StaticIntegrator *theStaticIntegrator =0;
static TransientIntegrator *theTransientIntegrator =0;
static ConvergenceTest *theTest =0;
static bool builtModel = false;

static EigenAnalysis *theEigenAnalysis = 0;

static char *resDataPtr = 0;
static int resDataSize = 0;

static Timer *theTimer = 0;

FE_Datastore *theDatabase  =0;
FEM_ObjectBroker theBroker;


// init the global variabled defined in G3Globals.h
ErrorHandler *g3ErrorHandler =0;
double        ops_Dt = 1.0;
Domain       *ops_TheActiveDomain = 0;
Element      *ops_TheActiveElement = 0;

TclVideoPlayer *theTclVideoPlayer =0;

// g3AppInit() is the method called by tkAppInit() when the
// interpreter is being set up .. this is where all the
// commands defined in this file are registered with the interpreter.

extern int myCommands(Tcl_Interp *interp);

int g3AppInit(Tcl_Interp *interp) {

#ifndef _LINUX  
    cerr.setf(ios::scientific, ios::floatfield);
#endif

    Tcl_CreateCommand(interp, "wipe", &wipeModel,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);    
    Tcl_CreateCommand(interp, "wipeAnalysis", &wipeAnalysis,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);    
    Tcl_CreateCommand(interp, "reset", &resetModel,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);        
    Tcl_CreateCommand(interp, "initialize", &initializeAnalysis,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);        
    Tcl_CreateCommand(interp, "loadConst", &setLoadConst,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);            
    Tcl_CreateCommand(interp, "setTime", &setTime,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);     
    Tcl_CreateCommand(interp, "getTime", &getTime,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);     
    Tcl_CreateCommand(interp, "build", &buildModel,
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "analyze", &analyzeModel, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "print", &printModel, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "analysis", &specifyAnalysis, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "system", &specifySOE, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "numberer", &specifyNumberer, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "constraints", &specifyConstraintHandler, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "algorithm", &specifyAlgorithm, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "test", &specifyCTest, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);    
    Tcl_CreateCommand(interp, "integrator", &specifyIntegrator, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "recorder", &addRecorder, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "algorithmRecorder", &addAlgoRecorder, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "database", &addDatabase, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
    Tcl_CreateCommand(interp, "playback", &playbackRecorders, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);        
    Tcl_CreateCommand(interp, "playbackAlgo", &playbackAlgorithmRecorders, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);        
    Tcl_CreateCommand(interp, "rigidLink", &rigidLink, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);                
    Tcl_CreateCommand(interp, "rigidDiaphragm", &rigidDiaphragm, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);   
    Tcl_CreateCommand(interp, "eigen", &eigenAnalysis, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "video", &videoPlayer, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "remove", &removeObject, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "nodeDisp", &nodeDisp, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "start", &startTimer, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "stop", &stopTimer, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
#ifdef _RELIABILITY
    Tcl_CreateCommand(interp, "reliability", &reliability, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL); 
    theReliabilityBuilder = 0;
// AddingSensitivity:BEGIN //////////////////////////////////
    Tcl_CreateCommand(interp, "computeGradients", &computeGradients, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "sensitivityAlgorithm", &sensitivityAlgorithm, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "sensitivityIntegrator", &sensitivityIntegrator, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       
    Tcl_CreateCommand(interp, "sensNodeDisp", &sensNodeDisp, 
		      (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);       

	theSensitivityAlgorithm =0;
	theSensitivityIntegrator =0;
// AddingSensitivity:END //////////////////////////////////

#endif

    theAlgorithm =0;
    theHandler =0;
    theNumberer =0;
    theAnalysisModel =0;  
    theSOE =0;
    theStaticIntegrator =0;
    theTransientIntegrator =0;
    theStaticAnalysis =0;
    theTransientAnalysis =0;    
    theVariableTimeStepTransientAnalysis =0;    
    theTest = 0;

    // create an error handler
    g3ErrorHandler = new ConsoleErrorHandler();
    theTclVideoPlayer = 0;

    return myCommands(interp);
}


#ifdef _RELIABILITY

int 
reliability(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  if (theReliabilityBuilder == 0) {
    theReliabilityBuilder = new TclReliabilityBuilder(theDomain,interp);
    return TCL_OK;
  }
  else
    return TCL_ERROR;
}
// AddingSensitivity:BEGIN /////////////////////////////////////////////////
int 
sensitivityAlgorithm(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
	if (theReliabilityBuilder == 0) {
		cerr << "The command 'reliability' needs to be issued before " << endl
		<< " the sensitivity algorithm can be created." << endl;
		return TCL_ERROR;
	}
	else if (theSensitivityIntegrator == 0) {
		cerr << "The sensitivity integrator needs to be instantiated before " << endl
		<< " the sensitivity algorithm can be created." << endl;
		return TCL_ERROR;
	}
	else {
		if (strcmp(argv[1],"PathDep") == 0) {
			ReliabilityDomain *theReliabilityDomain;
			theReliabilityDomain = theReliabilityBuilder->getReliabilityDomain();
			theSensitivityAlgorithm = new SensitivityAlgorithm(theReliabilityDomain,
										theAlgorithm,
										theSensitivityIntegrator,
										true);
		return TCL_OK;
		}
		else if (strcmp(argv[1],"PathIndep") == 0) {
			ReliabilityDomain *theReliabilityDomain;
			theReliabilityDomain = theReliabilityBuilder->getReliabilityDomain();
			theSensitivityAlgorithm = new SensitivityAlgorithm(theReliabilityDomain,
										theAlgorithm,
										theSensitivityIntegrator,
										false);
		return TCL_OK;
		}
		else {
			cerr << "WARNING: Invalid type of sensitivity algorithm." << endl;
			return TCL_ERROR;
		}
	}
}
int 
sensitivityIntegrator(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
	if (strcmp(argv[1],"Static") == 0) {

		if (theAnalysisModel == 0) {
			theAnalysisModel = new AnalysisModel();
		}

		theSensitivityIntegrator = new StaticSensitivityIntegrator(theAnalysisModel, theSOE);
		return TCL_OK;
	}
	else {
		cerr << "WARNING: Invalid type of sensitivity integrator." << endl;
		return TCL_ERROR;
	}
}
// AddingSensitivity:END /////////////////////////////////////////////////

#endif

int 
wipeModel(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  // to build the model make sure the ModelBuilder has been constructed
  // and that the model has not already been constructed
  if (theBuilder != 0) {
    delete theBuilder;
    builtModel = false;
    theBuilder = 0;
  }

  if (theStaticAnalysis != 0) {
      theStaticAnalysis->clearAll();
      delete theStaticAnalysis;
  }
  
  if (theTransientAnalysis != 0) {
      theTransientAnalysis->clearAll();
      delete theTransientAnalysis;  
  }

#ifdef _RELIABILITY
  if (theReliabilityBuilder != 0) {
    delete theReliabilityBuilder;
    theReliabilityBuilder = 0;
  }
#endif

  // NOTE : DON'T do the above on theVariableTimeStepAnalysis
  // as it and theTansientAnalysis are one in the same

  /*
  if (theEigenAnalysis != 0) {
    delete theEigenAnalysis;
    theEigenAnalysis = 0;
  }
  */

  theDomain.clearAll();

  if (theTest != 0)
      delete theTest;
  
  if (theTclVideoPlayer != 0) {
	  delete theTclVideoPlayer;
	  theTclVideoPlayer = 0;
  }

  theAlgorithm =0;
  theHandler =0;
  theNumberer =0;
  theAnalysisModel =0;  
  theSOE =0;
  theStaticIntegrator =0;
  theTransientIntegrator =0;
  theStaticAnalysis =0;
  theTransientAnalysis =0;    
  theVariableTimeStepTransientAnalysis =0;    
  theTest = 0;
// AddingSensitivity:BEGIN /////////////////////////////////////////////////
#ifdef _RELIABILITY
  theSensitivityAlgorithm =0;
  theSensitivityIntegrator =0;
#endif
// AddingSensitivity:END /////////////////////////////////////////////////
  // the domain deletes the record objects, 
  // just have to delete the private array
  return TCL_OK;  
}

int 
wipeAnalysis(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{


  if (theStaticAnalysis != 0) {
      theStaticAnalysis->clearAll();
      delete theStaticAnalysis;
  }
  
  if (theTransientAnalysis != 0) {
      theTransientAnalysis->clearAll();
      delete theTransientAnalysis;  
  }

  // NOTE : DON'T do the above on theVariableTimeStepAnalysis
  // as it and theTansientAnalysis are one in the same

  if (theTest != 0)
      delete theTest;
  
  theAlgorithm =0;
  theHandler =0;
  theNumberer =0;
  theAnalysisModel =0;  
  theSOE =0;
  theStaticIntegrator =0;
  theTransientIntegrator =0;
  theStaticAnalysis =0;
  theTransientAnalysis =0;    
  theVariableTimeStepTransientAnalysis =0;    
  theTest = 0;
// AddingSensitivity:BEGIN /////////////////////////////////////////////////
#ifdef _RELIABILITY
  theSensitivityAlgorithm =0;
  theSensitivityIntegrator =0;
#endif
// AddingSensitivity:END /////////////////////////////////////////////////
  // the domain deletes the record objects, 
  // just have to delete the private array
  return TCL_OK;  
}


int 
resetModel(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  theDomain.revertToStart();
  return TCL_OK;
}

int
initializeAnalysis(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  if (theTransientAnalysis != 0)
    theTransientAnalysis->initialize();
  else if (theStaticAnalysis != 0)
    theStaticAnalysis->initialize();
  
  theDomain.initialize();

  return TCL_OK;
}


int 
setLoadConst(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  theDomain.setLoadConstant();
  if (argc == 3) {
      if( strcmp(argv[1],"-time") == 0) {
	  double newTime;
	  if (Tcl_GetDouble(interp, argv[2], &newTime) != TCL_OK) {
	      cerr << "WARNING readingvalue - loadConst -time value ";
	      return TCL_ERROR;
	  } else {
	      theDomain.setCurrentTime(newTime);
	  }
      }    	  
  }
	  
  return TCL_OK;
}


int 
setTime(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  if (argc < 2) {
      cerr << "WARNING illegal command - time pseudoTime? \n";
      return TCL_ERROR;
  }
  double newTime;
  if (Tcl_GetDouble(interp, argv[1], &newTime) != TCL_OK) {
      cerr << "WARNING reading time value - time pseudoTime? ";
      return TCL_ERROR;
  } else
      theDomain.setCurrentTime(newTime);
  return TCL_OK;
}

int 
getTime(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  double time = theDomain.getCurrentTime();
  // now we copy the value to the tcl string that is returned
  sprintf(interp->result,"%f",time);
  return TCL_OK;
}


// command invoked to build the model, i.e. to invoke buildFE_Model() 
// on the ModelBuilder
int 
buildModel(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
    // to build the model make sure the ModelBuilder has been constructed
  // and that the model has not already been constructed
  if (theBuilder != 0 && builtModel == false) {
    builtModel = true;
    return theBuilder->buildFE_Model();
  }  else if (theBuilder != 0 && builtModel == true) {
      interp->result = "WARNING Model has already been built - not built again ";
      return TCL_ERROR;
  }
  else {
      interp->result = "WARNING No ModelBuilder type has been specified ";
      return TCL_ERROR;
  }    
}


//
// command invoked to build the model, i.e. to invoke analyze() 
// on the Analysis object
//
int 
analyzeModel(ClientData clientData, Tcl_Interp *interp, int argc, 
	     char **argv)
{
  int result = 0;


  if (theStaticAnalysis != 0) {
    if (argc < 2) {
      interp->result = "WARNING static analysis: analysis numIncr?";
      return TCL_ERROR;
    }
    int numIncr;

    if (Tcl_GetInt(interp, argv[1], &numIncr) != TCL_OK)	
      return TCL_ERROR;	      
    result = theStaticAnalysis->analyze(numIncr);
  } else if (theTransientAnalysis != 0) {
    if (argc < 3) {
      interp->result = "WARNING transient analysis: analysis numIncr? deltaT?";
      return TCL_ERROR;
    }
    int numIncr;
    if (Tcl_GetInt(interp, argv[1], &numIncr) != TCL_OK)	
      return TCL_ERROR;
    double dT;
    if (Tcl_GetDouble(interp, argv[2], &dT) != TCL_OK)	
      return TCL_ERROR;

    // Set global timestep variable
    ops_Dt = dT;

    if (argc == 6) {
      int Jd;
      double dtMin, dtMax;
      if (Tcl_GetDouble(interp, argv[3], &dtMin) != TCL_OK)	
	return TCL_ERROR;
      if (Tcl_GetDouble(interp, argv[4], &dtMax) != TCL_OK)	
	return TCL_ERROR;
      if (Tcl_GetInt(interp, argv[5], &Jd) != TCL_OK)	
	return TCL_ERROR;

      if (theVariableTimeStepTransientAnalysis != 0)
	result =  theVariableTimeStepTransientAnalysis->analyze(numIncr, dT, dtMin, dtMax, Jd);
      else {
	interp->result = "WARNING analyze - no variable time step transient analysis object constructed";
	return TCL_ERROR;
      }

    } else {
      result = theTransientAnalysis->analyze(numIncr, dT);
    }

  } else {
    interp->result = "WARNING No Analysis type has been specified ";
    return TCL_ERROR;
  }    

  if (result < 0) {
    g3ErrorHandler->warning("OpenSees > analyze failed, returned %d error flag\n",result);
  }

  sprintf(interp->result,"%d",result);    
  return TCL_OK;

}


int 
printElement(ClientData clientData, Tcl_Interp *interp, int argc, 
	  char **argv, int nodeArg, ostream &output);


int 
printNode(ClientData clientData, Tcl_Interp *interp, int argc, 
	  char **argv, int nodeArg, ostream &output);
	  
int 
printIntegrator(ClientData clientData, Tcl_Interp *interp, int argc, 
		char **argv, int nodeArg, ostream &output);	  
		
int 
printAlgorithm(ClientData clientData, Tcl_Interp *interp, int argc, 
		char **argv, int nodeArg, ostream &output);	  		

int 
printModel(ClientData clientData, Tcl_Interp *interp, int argc, 
			 char **argv)
{
  // if just 'print' then print out the entire domain
  if (argc == 1) {
    cerr << theDomain;
    return TCL_OK;
  }    

  // if 'print ele i j k..' print out some elements
  if ((strcmp(argv[1],"-ele") == 0) || (strcmp(argv[1],"ele") == 0))
    return printElement(clientData, interp, argc, argv, 3, cerr);    

  // if 'print node i j k ..' print out some nodes
  else if ((strcmp(argv[1],"-node") == 0) || (strcmp(argv[1],"node") == 0)) 
      return printNode(clientData, interp, argc, argv, 3, cerr);
  
  // if 'print integrator flag' print out the integrator
  else if ((strcmp(argv[1],"integrator") == 0) || 
	   (strcmp(argv[1],"-integrator") == 0)) 
    return printIntegrator(clientData, interp, argc, argv, 3, cerr);  
  
  // if 'print algorithm flag' print out the algorithm
  else if ((strcmp(argv[1],"algorithm") == 0) || 
	   (strcmp(argv[1],"-algorithm") == 0))
    return printAlgorithm(clientData, interp, argc, argv, 3, cerr);    

  else { // it must be a file we are going to print to
    ofstream output(argv[1],ios::app); // open for appending to
    if (!output) {
      cerr << "print <filename> .. - failed to open file: " << argv[1] << endl;
      return TCL_ERROR;
    }
    // if just 'print <filename>' then print out the entire domain to eof
    if (argc == 2) {
      output << theDomain;
      return TCL_OK;
    }    

    int pos = 2;
    if ((strcmp(argv[pos],"string") == 0) || 
	(strcmp(argv[pos],"-string") == 0)) {
	output << argv[3] << endl;
	pos +=2;
    }
    int res = TCL_OK;    

    // if 'print <filename> ele i j k..' print out some elements
    if ((strcmp(argv[pos],"ele") == 0) || 
	(strcmp(argv[pos],"-ele") == 0))
      res = printElement(clientData, interp, argc, argv, pos+2, output);    

    // if 'print <filename> node i j k ..' print out some nodes
    else if ((strcmp(argv[pos],"node") == 0) || (strcmp(argv[pos],"-node") == 0))
      res = printNode(clientData, interp, argc, argv, pos+2, output);
    
    // if 'print integrator flag' print out the integrator
    else if ((strcmp(argv[pos],"integrator") == 0) 
	     || (strcmp(argv[pos],"-integrator") == 0))
	return printIntegrator(clientData, interp, argc, argv, pos+2, cerr);  
  
    // if 'print algorithm flag' print out the algorithm
    else if ((strcmp(argv[pos],"-algorithm") == 0)|| 
	     (strcmp(argv[pos],"algorithm") == 0))
	return printAlgorithm(clientData, interp, argc, argv, pos+2, cerr);    


    // close the output file
    output.close();
    return res;
  }
  
}


// printNode():
// function to print out the nodal information conatined in line
//     print <filename> node <flag int> <int int int>
// input: nodeArg: integer equal to arg count to node plus 1
//        output: output stream to which the results are sent
// 
int 
printNode(ClientData clientData, Tcl_Interp *interp, int argc, 
	  char **argv, int nodeArg, ostream &output)
{
  int flag = 0; // default flag sent to a nodes Print() method

  // if just 'print <filename> node' print all the nodes - no flag
  if (argc < nodeArg) { 
      NodeIter &theNodes = theDomain.getNodes();
      Node *theNode;
      while ((theNode = theNodes()) != 0)
	theNode->Print(output);
      return TCL_OK;
  }    

  // if 'print <filename> node flag int <int int ..>' get the flag
  if ((strcmp(argv[nodeArg-1],"flag") == 0) ||
      (strcmp(argv[nodeArg-1],"-flag") == 0)) { 
      // get the specified flag
    if (argc <= nodeArg) {
      cerr << "WARNING print <filename> node <flag int> no int specified \n";
      return TCL_ERROR;
    }    
    if (Tcl_GetInt(interp, argv[nodeArg], &flag) != TCL_OK) {
      cerr << "WARNING print node failed to get integer flag: ";
      cerr << argv[nodeArg] << endl; 
      return TCL_ERROR;
    }    
    nodeArg += 2;
  }

  // now print the nodes with the specified flag, 0 by default

  // if 'print <filename> node flag' 
  //     print out all the nodes in the domain with flag
  if (argc < nodeArg) { 
    NodeIter &theNodes = theDomain.getNodes();
    Node *theNode;
    while ((theNode = theNodes()) != 0)
      theNode->Print(output, flag);
    return TCL_OK;
  } else { 
    // otherwise print out the specified nodes i j k .. with flag
    for (int i= nodeArg-1; i<argc; i++) {
      int nodeTag;
      if (Tcl_GetInt(interp, argv[i], &nodeTag) != TCL_OK) {
	cerr << "WARNING print node failed to get integer: " << argv[i] << endl;
	return TCL_ERROR;
      }
      Node *theNode = theDomain.getNode(nodeTag);
      if (theNode != 0)
	theNode->Print(output,flag);
    }
    return TCL_OK;
  }
}


int 
printElement(ClientData clientData, Tcl_Interp *interp, int argc, 
	  char **argv, int eleArg, ostream &output)
{
  int flag = 0; // default flag sent to a nodes Print() method

  // if just 'print <filename> node' print all the nodes - no flag
  if (argc < eleArg) { 
      ElementIter &theElements = theDomain.getElements();
      Element *theElement;
      while ((theElement = theElements()) != 0)
	theElement->Print(output);
      return TCL_OK;
  }    

  // if 'print <filename> Element flag int <int int ..>' get the flag
  if ((strcmp(argv[eleArg-1],"flag") == 0) ||
      (strcmp(argv[eleArg-1],"-flag")) == 0) { // get the specified flag
    if (argc <= eleArg) {
      cerr << "WARNING print <filename> ele <flag int> no int specified \n";
      return TCL_ERROR;
    }    
    if (Tcl_GetInt(interp, argv[eleArg], &flag) != TCL_OK) {
      cerr << "WARNING print ele failed to get integer flag: ";
      cerr << argv[eleArg] << endl; 
      return TCL_ERROR;
    }    
    eleArg += 2;
  }

  // now print the Elements with the specified flag, 0 by default

  // if 'print <filename> Element flag' 
  //     print out all the Elements in the domain with flag
  if (argc < eleArg) { 
    ElementIter &theElements = theDomain.getElements();
    Element *theElement;
    while ((theElement = theElements()) != 0)      
      theElement->Print(output, flag);
    return TCL_OK;
  } else { 
    // otherwise print out the specified Elements i j k .. with flag
    for (int i= eleArg-1; i<argc; i++) {
      int ElementTag;
      if (Tcl_GetInt(interp, argv[i], &ElementTag) != TCL_OK) {
	cerr << "WARNING print ele failed to get integer: " << argv[i] << endl;
	return TCL_ERROR;
      }
      Element *theElement = theDomain.getElement(ElementTag);
      if (theElement != 0)
	theElement->Print(output,flag);
    }
    return TCL_OK;
  }
}


int 
printAlgorithm(ClientData clientData, Tcl_Interp *interp, int argc, 
	       char **argv, int eleArg, ostream &output)
{

  if (theAlgorithm == 0)
      return TCL_OK;

  // if just 'print <filename> algorithm'- no flag
  if (argc < eleArg) { 
      theAlgorithm->Print(output);
      return TCL_OK;
  }    

  // if 'print <filename> Algorithm flag' get the flag
  int flag;  
  if (Tcl_GetInt(interp, argv[eleArg-1], &flag) != TCL_OK) {  
      cerr << "WARNING print algorithm failed to get integer flag: ";
      cerr << argv[eleArg] << endl; 
      return TCL_ERROR;
  }    
  theAlgorithm->Print(output,flag);
  return TCL_OK;  
}


int 
printIntegrator(ClientData clientData, Tcl_Interp *interp, int argc, 
	       char **argv, int eleArg, ostream &output)
{

  if (theStaticIntegrator == 0 && theTransientIntegrator == 0)
      return TCL_OK;
  
  IncrementalIntegrator *theIntegrator;
  if (theStaticIntegrator != 0)
      theIntegrator = theStaticIntegrator;
  else
      theIntegrator = theTransientIntegrator;

  // if just 'print <filename> algorithm'- no flag
  if (argc < eleArg) { 
      theIntegrator->Print(output);
      return TCL_OK;
  }    

  // if 'print <filename> Algorithm flag' get the flag
  int flag;  
  if (Tcl_GetInt(interp, argv[eleArg-1], &flag) != TCL_OK) {  
      cerr << "WARNING print algorithm failed to get integer flag: ";
      cerr << argv[eleArg] << endl; 
      return TCL_ERROR;
  }    
  theIntegrator->Print(output,flag);
  return TCL_OK;  
}


//
// command invoked to allow the Analysis object to be built
//
int 
specifyAnalysis(ClientData clientData, Tcl_Interp *interp, int argc, 
		char **argv)
{
    // make sure at least one other argument to contain type of system
    if (argc < 2) {
	interp->result = "WARNING need to specify an analysis type (Static, Transient)";
	return TCL_ERROR;
    }    

    // delete the old analysis
    if (theStaticAnalysis != 0) {
	delete theStaticAnalysis;
	theStaticAnalysis = 0;
    }
    if (theTransientAnalysis != 0) {
	delete theTransientAnalysis;
	theTransientAnalysis = 0;
	theVariableTimeStepTransientAnalysis = 0;
    }
    
    // check argv[1] for type of SOE and create it
    if (strcmp(argv[1],"Static") == 0) {
	// make sure all the components have been built,
	// otherwise print a warning and use some defaults
	if (theAnalysisModel == 0) 
	    theAnalysisModel = new AnalysisModel();
	
	if (theAlgorithm == 0) {
	    cerr << "WARNING analysis Static - no Algorithm yet specified, ";
	    cerr << " NewtonRaphson default will be used\n";	    

	    if (theTest == 0) 
		theTest = new CTestNormUnbalance(1.0e-6,25,0);       
	    theAlgorithm = new NewtonRaphson(*theTest); 
	}
	if (theHandler == 0) {
	    cerr << "WARNING analysis Static - no ConstraintHandler yet specified, ";
	    cerr << " PlainHandler default will be used\n";
	    theHandler = new PlainHandler();       
	}
	if (theNumberer == 0) {
	    cerr << "WARNING analysis Static - no Numberer specified, ";
	    cerr << " RCM default will be used\n";
	    RCM *theRCM = new RCM();	
	    theNumberer = new DOF_Numberer(*theRCM);    	
	}
	if (theStaticIntegrator == 0) {
	    cerr << "WARNING analysis Static - no Integrator specified, ";
	    cerr << " StaticIntegrator default will be used\n";
	    theStaticIntegrator = new LoadControl(1, 1, 1, 1);       
	}
	if (theSOE == 0) {
	    cerr << "WARNING analysis Static - no LinearSOE specified, ";
	    cerr << " ProfileSPDLinSOE default will be used\n";
	    ProfileSPDLinSolver *theSolver;
	    theSolver = new ProfileSPDLinDirectSolver(); 	
	    theSOE = new ProfileSPDLinSOE(*theSolver);      
	}
    
	theStaticAnalysis = new StaticAnalysis(theDomain,
					       *theHandler,
					       *theNumberer,
					       *theAnalysisModel,
					       *theAlgorithm,
					       *theSOE,
					       *theStaticIntegrator);
// AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
	if (theSensitivityAlgorithm != 0 && theSensitivityAlgorithm->isPathDependent()) {
		theStaticAnalysis->setSensitivityAlgorithm(theSensitivityAlgorithm);
	}
#endif
// AddingSensitivity:END /////////////////////////////////

    } else if (strcmp(argv[1],"Transient") == 0) {
	// make sure all the components have been built,
	// otherwise print a warning and use some defaults
	if (theAnalysisModel == 0) 
	    theAnalysisModel = new AnalysisModel();
	
	if (theAlgorithm == 0) {
	    cerr << "WARNING analysis Transient - no Algorithm yet specified, ";
	    cerr << " NewtonRaphson default will be used\n";	    

	    if (theTest == 0) 
		theTest = new CTestNormUnbalance(1.0e-6,25,0);       
	    theAlgorithm = new NewtonRaphson(*theTest); 
	}
	if (theHandler == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no ConstraintHandler";
	    cerr << " yet specified, PlainHandler default will be used\n";
	    theHandler = new PlainHandler();       
	}
	if (theNumberer == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no Numberer specified, ";
	    cerr << " RCM default will be used\n";
	    RCM *theRCM = new RCM();	
	    theNumberer = new DOF_Numberer(*theRCM);    	
	}
	if (theTransientIntegrator == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no Integrator specified, ";
	    cerr << " Newmark(.5,.25) default will be used\n";
	    theTransientIntegrator = new Newmark(0.5,0.25);       
	}
	if (theSOE == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no LinearSOE specified, ";
	    cerr << " ProfileSPDLinSOE default will be used\n";
	    ProfileSPDLinSolver *theSolver;
	    theSolver = new ProfileSPDLinDirectSolver(); 	
	    theSOE = new ProfileSPDLinSOE(*theSolver);      
	}
    
	theTransientAnalysis = new DirectIntegrationAnalysis(theDomain,
							     *theHandler,
							     *theNumberer,
							     *theAnalysisModel,
							     *theAlgorithm,
							     *theSOE,
							     *theTransientIntegrator);

    } else if ((strcmp(argv[1],"VariableTimeStepTransient") == 0) ||
	       (strcmp(argv[1],"TransientWithVariableTimeStep") == 0) ||
	       (strcmp(argv[1],"VariableTransient") == 0)) {
	// make sure all the components have been built,
	// otherwise print a warning and use some defaults
	if (theAnalysisModel == 0) 
	    theAnalysisModel = new AnalysisModel();
	
	if (theAlgorithm == 0) {
	    cerr << "WARNING analysis Transient - no Algorithm yet specified, ";
	    cerr << " NewtonRaphson default will be used\n";	    

	    if (theTest == 0) 
		theTest = new CTestNormUnbalance(1.0e-6,25,0);       
	    theAlgorithm = new NewtonRaphson(*theTest); 
	}
	if (theHandler == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no ConstraintHandler";
	    cerr << " yet specified, PlainHandler default will be used\n";
	    theHandler = new PlainHandler();       
	}
	if (theNumberer == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no Numberer specified, ";
	    cerr << " RCM default will be used\n";
	    RCM *theRCM = new RCM();	
	    theNumberer = new DOF_Numberer(*theRCM);    	
	}
	if (theTransientIntegrator == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no Integrator specified, ";
	    cerr << " Newmark(.5,.25) default will be used\n";
	    theTransientIntegrator = new Newmark(0.5,0.25);       
	}
	if (theSOE == 0) {
	    cerr << "WARNING analysis Transient dt tFinal - no LinearSOE specified, ";
	    cerr << " ProfileSPDLinSOE default will be used\n";
	    ProfileSPDLinSolver *theSolver;
	    theSolver = new ProfileSPDLinDirectSolver(); 	
	    theSOE = new ProfileSPDLinSOE(*theSolver);      
	}
    
	theVariableTimeStepTransientAnalysis = new VariableTimeStepDirectIntegrationAnalysis
	  (theDomain,
	   *theHandler,
	   *theNumberer,
	   *theAnalysisModel,
	   *theAlgorithm,
	   *theSOE,
	   *theTransientIntegrator);

	// set the pointer for variabble time step analysis
	theTransientAnalysis = theVariableTimeStepTransientAnalysis;
	
    } else {
	interp->result = "WARNING No Analysis type exists (Static Transient only) ";
	return TCL_ERROR;
    }
    return TCL_OK;
}


//
// command invoked to allow the SystemOfEqn and Solver objects to be built
//
int 
specifySOE(ClientData clientData, Tcl_Interp *interp, int argc, 
		    char **argv)
{
  // make sure at least one other argument to contain type of system
  if (argc < 2) {
      interp->result = "WARNING need to specify a model type ";
      return TCL_ERROR;
  }    

  // check argv[1] for type of SOE and create it
  // BAND GENERAL SOE & SOLVER
  if (strcmp(argv[1],"BandGeneral") == 0) {
      BandGenLinSolver    *theSolver = new BandGenLinLapackSolver();
      theSOE = new BandGenLinSOE(*theSolver);      
  } 

  // BAND SPD SOE & SOLVER
  else if (strcmp(argv[1],"BandSPD") == 0) {
      BandSPDLinSolver    *theSolver = new BandSPDLinLapackSolver();   
      theSOE = new BandSPDLinSOE(*theSolver);        
  } 

  // PROFILE SPD SOE * SOLVER
  else if (strcmp(argv[1],"ProfileSPD") == 0) {
    // now must determine the type of solver to create from rest of args
    ProfileSPDLinSolver *theSolver = new ProfileSPDLinDirectSolver(); 	

    /* *********** Some misc solvers i play with ******************
    else if (strcmp(argv[2],"Normal") == 0) {
      theSolver = new ProfileSPDLinDirectSolver(); 	
    } 

    else if (strcmp(argv[2],"Block") == 0) {  
      int blockSize = 4;
      if (argc == 4) {
	if (Tcl_GetInt(interp, argv[3], &blockSize) != TCL_OK)
	  return TCL_ERROR;
      }
      theSolver = theSolver = new ProfileSPDLinDirectBlockSolver(1.0e-12,blockSize); 
    }

    
      int blockSize = 4;
      int numThreads = 1;
      if (argc == 5) {
	if (Tcl_GetInt(interp, argv[3], &blockSize) != TCL_OK)
	  return TCL_ERROR;
	if (Tcl_GetInt(interp, argv[4], &numThreads) != TCL_OK)
	  return TCL_ERROR;
      }
      theSolver = new ProfileSPDLinDirectThreadSolver(numThreads,blockSize,1.0e-12); 
      } else if (strcmp(argv[2],"Thread") == 0) {  
      int blockSize = 4;
      int numThreads = 1;
      if (argc == 5) {
	if (Tcl_GetInt(interp, argv[3], &blockSize) != TCL_OK)
	  return TCL_ERROR;
	if (Tcl_GetInt(interp, argv[4], &numThreads) != TCL_OK)
	  return TCL_ERROR;
      }
      theSolver = new ProfileSPDLinDirectThreadSolver(numThreads,blockSize,1.0e-12); 
    } 
    else if (strcmp(argv[2],"Skypack") == 0) {  
      if (argc == 5) {
	int mCols, mRows;
	if (Tcl_GetInt(interp, argv[3], &mCols) != TCL_OK)
	  return TCL_ERROR;
	if (Tcl_GetInt(interp, argv[4], &mRows) != TCL_OK)
	  return TCL_ERROR;
	theSolver = new ProfileSPDLinDirectSkypackSolver(mCols, mRows); 
      } else 
	theSolver = new ProfileSPDLinDirectSkypackSolver(); 	
    }
    else 
      theSolver = new ProfileSPDLinDirectSolver(); 	
    ***************************************************************  */
    
      theSOE = new ProfileSPDLinSOE(*theSolver);      
   }


  // SPARSE GENERAL SOE * SOLVER
  else if ((strcmp(argv[1],"SparseGeneral") == 0) ||
	   (strcmp(argv[1],"SparseGEN") == 0)) {
    
    SparseGenColLinSolver *theSolver;    

#ifdef _THREADS

    int count = 2;
    int np = 2;
    int permSpec = 0;
    int panelSize = 6;
    int relax = 6;
    double thresh = 0.0;


    while (count < argc) {
      if (strcmp(argv[count],"p") == 0 || strcmp(argv[count],"piv") ||
	  strcmp(argv[count],"-piv")) {
	thresh = 1.0;
      }
      if (strcmp(argv[count],"-np") == 0 || strcmp(argv[count],"np")) {
	count++;
	if (count < argc)
	  if (Tcl_GetInt(interp, argv[count], &np) != TCL_OK)
	    return TCL_ERROR;		     
      }
      count++;
    }

    theSolver = new ThreadedSuperLU(np, permSpec, panelSize, relax, thresh); 	

#else

    int count = 2;
    double thresh = 0.0;
    while (count < argc) {
      if (strcmp(argv[count],"p") == 0 || strcmp(argv[count],"piv") ||
	  strcmp(argv[count],"-piv")) {
	thresh = 1.0;
      }
      count++;
    }

    theSolver = new SuperLU(0, thresh); 	

#endif

    theSOE = new SparseGenColLinSOE(*theSolver);      
  }	

  else if (strcmp(argv[1],"SparseSPD") == 0) {
    // now must determine the type of solver to create from rest of args

    // now determine ordering scheme
    //   1 -- MMD
    //   2 -- ND
    //   3 -- RCM
    int lSparse = 1;
    if (argc == 3) {
      if (Tcl_GetInt(interp, argv[3], &lSparse) != TCL_OK)
	return TCL_ERROR;
    }

    SymSparseLinSolver *theSolver = new SymSparseLinSolver();
    theSOE = new SymSparseLinSOE(*theSolver, lSparse);      
  }	
  
  else if (strcmp(argv[1],"UmfPack") == 0) {
    // now must determine the type of solver to create from rest of args
      UmfpackGenLinSolver *theSolver = new UmfpackGenLinSolver();
      theSOE = new UmfpackGenLinSOE(*theSolver);      
  }	  

  else if (strcmp(argv[1],"FullGeneral") == 0) {
    // now must determine the type of solver to create from rest of args
    FullGenLinLapackSolver *theSolver = new FullGenLinLapackSolver();
    theSOE = new FullGenLinSOE(*theSolver);
  }

  else {
    interp->result = "WARNING No SystemOfEqn type exists) ";
    return TCL_ERROR;
  }

  // if the analysis exists - we want to change the SOE
  if (theStaticAnalysis != 0)
    theStaticAnalysis->setLinearSOE(*theSOE);

  return TCL_OK;
}




//
// command invoked to allow the Numberer objects to be built
//
int 
specifyNumberer(ClientData clientData, Tcl_Interp *interp, int argc, 
		      char **argv)
{
  // make sure at least one other argument to contain numberer
  if (argc < 2) {
      interp->result = "WARNING need to specify a Nemberer type ";
      return TCL_ERROR;
  }    

  // check argv[1] for type of Numberer and create the object
  if (strcmp(argv[1],"Plain") == 0) 
    theNumberer = new PlainNumberer();       
  else if (strcmp(argv[1],"RCM") == 0) {
    RCM *theRCM = new RCM();	
    theNumberer = new DOF_Numberer(*theRCM);    	
  }
  else {
    interp->result = "WARNING No Numberer type exists (Plain, RCM only) ";
    return TCL_ERROR;
  }    
  return TCL_OK;
}




//
// command invoked to allow the ConstraintHandler object to be built
//
int 
specifyConstraintHandler(ClientData clientData, Tcl_Interp *interp, int argc, 
		      char **argv)
{
  // make sure at least one other argument to contain numberer
  if (argc < 2) {
      interp->result = "WARNING need to specify a Nemberer type ";
      return TCL_ERROR;
  }    

  // check argv[1] for type of Numberer and create the object
  if (strcmp(argv[1],"Plain") == 0) 
    theHandler = new PlainHandler();       

  else if (strcmp(argv[1],"Penalty") == 0) {
    if (argc < 4) {
      interp->result = "WARNING: need to specify alpha: handler Penalty alpha ";
      return TCL_ERROR;
    }    
    double alpha1, alpha2;
    if (Tcl_GetDouble(interp, argv[2], &alpha1) != TCL_OK)	
      return TCL_ERROR;	
    if (Tcl_GetDouble(interp, argv[3], &alpha2) != TCL_OK)	
      return TCL_ERROR;	
    theHandler = new PenaltyConstraintHandler(alpha1, alpha2);
  }
  
  else if (strcmp(argv[1],"Lagrange") == 0) {
    double alpha1 = 1.0;
    double alpha2 = 1.0;
    if (argc == 4) {
      if (Tcl_GetDouble(interp, argv[2], &alpha1) != TCL_OK)	
	return TCL_ERROR;	
      if (Tcl_GetDouble(interp, argv[3], &alpha2) != TCL_OK)	
	return TCL_ERROR;	
    }
    theHandler = new LagrangeConstraintHandler(alpha1, alpha2);
  }  
  
  else if (strcmp(argv[1],"Transformation") == 0) {
    theHandler = new TransformationConstraintHandler();
  }    

  else {
    cerr << "WARNING No ConstraintHandler type exists (Plain, Penalty,";
    cerr << " Lagrange, Transformation) only\n";
    return TCL_ERROR;
  }    
  return TCL_OK;
}



//
// command invoked to allow the SolnAlgorithm object to be built
//
int
specifyAlgorithm(ClientData clientData, Tcl_Interp *interp, int argc, 
		      char **argv)
{
  // make sure at least one other argument to contain numberer
  if (argc < 2) {
      interp->result = "WARNING need to specify an Algorithm type ";
      return TCL_ERROR;
  }    
  EquiSolnAlgo *theNewAlgo = 0;

  // check argv[1] for type of Algorithm and create the object
  if (strcmp(argv[1],"Linear") == 0) 
    theNewAlgo = new Linear();       

  else if (strcmp(argv[1],"Newton") == 0) {
    int formTangent = CURRENT_TANGENT;
    if (argc > 2) {
      if (strcmp(argv[2],"-secant") == 0) {
	formTangent = CURRENT_SECANT;
      } else if (strcmp(argv[2],"-initial") == 0) {
	formTangent = INITIAL_TANGENT;
      } else if ((strcmp(argv[2],"-initialThenCurrent") == 0) || 
		 (strcmp(argv[2],"-initialCurrent") == 0))  {
	formTangent = INITIAL_THEN_CURRENT_TANGENT;
      }
    }

    if (theTest == 0) {
      interp->result = "ERROR: No ConvergenceTest yet specified\n";
      return TCL_ERROR;	  
    }
    theNewAlgo = new NewtonRaphson(*theTest, formTangent); 
  }

  else if (strcmp(argv[1],"KrylovNewton") == 0) {
    int formTangent = CURRENT_TANGENT;
    int maxDim = -1;
    for (int i = 2; i < argc; i++) {
      if (strcmp(argv[i],"-secant") == 0) {
	formTangent = CURRENT_SECANT;
      } else if (strcmp(argv[i],"-initial") == 0) {
	formTangent = INITIAL_TANGENT;
      } else if (strcmp(argv[i++],"-maxDim") == 0 && i < argc) {
	maxDim = atoi(argv[i]);
      }
    }

    if (theTest == 0) {
      interp->result = "ERROR: No ConvergenceTest yet specified\n";
      return TCL_ERROR;	  
    }
    if (maxDim == -1)
      theNewAlgo = new KrylovNewton(*theTest, formTangent); 
    else
      theNewAlgo = new KrylovNewton(*theTest, formTangent, maxDim); 
  }

  else if (strcmp(argv[1],"Broyden") == 0) {
    int formTangent = CURRENT_TANGENT;
    int count = -1;

    if (theTest == 0) {
      interp->result = "ERROR: No ConvergenceTest yet specified\n";
      return TCL_ERROR;	  
    }
    for (int i = 2; i < argc; i++) {
      if (strcmp(argv[i],"-secant") == 0) {
	formTangent = CURRENT_SECANT;
      } else if (strcmp(argv[i],"-initial") == 0) {
	formTangent = INITIAL_TANGENT;
      } else if (strcmp(argv[i++],"-count") == 0 && i < argc) {
	count = atoi(argv[i]);
      }
    }

    if (count == -1)
      theNewAlgo = new Broyden(*theTest, formTangent); 
    else
      theNewAlgo = new Broyden(*theTest, formTangent, count); 
  }

  else if (strcmp(argv[1],"BFGS") == 0) {
    int formTangent = CURRENT_TANGENT;
    int count = -1;
    for (int i = 2; i < argc; i++) {
      if (strcmp(argv[i],"-secant") == 0) {
	formTangent = CURRENT_SECANT;
      } else if (strcmp(argv[i],"-initial") == 0) {
	formTangent = INITIAL_TANGENT;
      } else if (strcmp(argv[i++],"-count") == 0 && i < argc) {
	count = atoi(argv[i]);
      }
    }

    if (theTest == 0) {
      interp->result = "ERROR: No ConvergenceTest yet specified\n";
      return TCL_ERROR;	  
    }

    if (count == -1)
      theNewAlgo = new BFGS(*theTest, formTangent); 
    else
      theNewAlgo = new BFGS(*theTest, formTangent, count); 
  }
  
  else if (strcmp(argv[1],"ModifiedNewton") == 0) {
    int formTangent = CURRENT_TANGENT;
    if (argc > 2) {
      if (strcmp(argv[2],"-secant") == 0) {
	formTangent = CURRENT_SECANT;
      } else if (strcmp(argv[2],"-initial") == 0) {
	formTangent = INITIAL_TANGENT;
      }
    }
    if (theTest == 0) {
      interp->result = "ERROR: No ConvergenceTest yet specified\n";
      return TCL_ERROR;	  
    }
      
    theNewAlgo = new ModifiedNewton(*theTest, formTangent); 
  }  
  
  else if (strcmp(argv[1],"NewtonLineSearch") == 0) {
      if (theTest == 0) {
	  interp->result = "ERROR: No ConvergenceTest yet specified\n";
	  return TCL_ERROR;	  
      }

      int    count = 2;
      
      // set some default variable
      double tol        = 0.8;
      int    maxIter    = 10;
      double maxEta     = 10.0;
      double minEta     = 0.1;
      int    pFlag      = 1;
      int    typeSearch = 0;
      
      while (count < argc) {
	if (strcmp(argv[count], "-tol") == 0) {
	  count++;
	  if (Tcl_GetDouble(interp, argv[count], &tol) != TCL_OK)	
	    return TCL_ERROR;	      	  
	  count++;
	} else if (strcmp(argv[count], "-maxIter") == 0) {
	  count++;
	  if (Tcl_GetInt(interp, argv[count], &maxIter) != TCL_OK)	
	    return TCL_ERROR;	      	  
	  count++;	  
	} else if (strcmp(argv[count], "-pFlag") == 0) {
	  count++;
	  if (Tcl_GetInt(interp, argv[count], &pFlag) != TCL_OK)	
	    return TCL_ERROR;	      	  
	  count++;
	} else if (strcmp(argv[count], "-minEta") == 0) {
	  count++;
	  if (Tcl_GetDouble(interp, argv[count], &minEta) != TCL_OK)	
	    return TCL_ERROR;	      	  
	  count++;
	} else if (strcmp(argv[count], "-maxEta") == 0) {
	  count++;
	  if (Tcl_GetDouble(interp, argv[count], &maxEta) != TCL_OK)	
	    return TCL_ERROR;	      	  
	  count++;
	} else if (strcmp(argv[count], "-type") == 0) {
	  count++;
	  if (strcmp(argv[count], "Bisection") == 0) 
	    typeSearch = 1;
	  else if (strcmp(argv[count], "Secant") == 0) 
	    typeSearch = 2;
	  else if (strcmp(argv[count], "RegulaFalsi") == 0) 
	    typeSearch = 3;
	  else if (strcmp(argv[count], "LinearInterpolated") == 0) 
	    typeSearch = 3;
	  else if (strcmp(argv[count], "InitialInterpolated") == 0) 
	    typeSearch = 0;
	  count++;
	} else
	  count++;
      }
      
      LineSearch *theLineSearch = 0;      
      if (typeSearch == 0)
	theLineSearch = new InitialInterpolatedLineSearch(tol, maxIter, minEta, maxEta, pFlag);
							  
      else if (typeSearch == 1)
	theLineSearch = new BisectionLineSearch(tol, maxIter, minEta, maxEta, pFlag);
      else if (typeSearch == 2)
	theLineSearch = new SecantLineSearch(tol, maxIter, minEta, maxEta, pFlag);
      else if (typeSearch == 3)
	theLineSearch = new RegulaFalsiLineSearch(tol, maxIter, minEta, maxEta, pFlag);

      theNewAlgo = new NewtonLineSearch(*theTest, theLineSearch); 
  }

  else {
      interp->result = "WARNING No EquiSolnAlgo type exists (Linear, Newton only) ";
      return TCL_ERROR;
  }    


  if (theNewAlgo != 0) {
      theAlgorithm = theNewAlgo;

    // if the analysis exists - we want to change the SOE
    if (theStaticAnalysis != 0)
      theStaticAnalysis->setAlgorithm(*theAlgorithm);
    else if (theTransientAnalysis != 0)
      theTransientAnalysis->setAlgorithm(*theAlgorithm);  
  }

  return TCL_OK;
}


//
// command invoked to allow the SolnAlgorithm object to be built
//
int
specifyCTest(ClientData clientData, Tcl_Interp *interp, int argc, 
		      char **argv)
{
  // make sure at least one other argument to contain numberer
  if (argc < 2) {
      interp->result = "WARNING need to specify a ConvergenceTest Type type ";
      return TCL_ERROR;
  }    

  // get the tolerence first
  double tol;
  int numIter;
  int printIt = 0;
  
  if (argc == 3) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)	
	  return TCL_ERROR;		
      numIter = 25;      
  } else if (argc == 4) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)	
	  return TCL_ERROR;			  
      if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)	
	  return TCL_ERROR;			  
  } else if (argc == 5) {
      if (Tcl_GetDouble(interp, argv[2], &tol) != TCL_OK)	
	  return TCL_ERROR;			  
      if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)	
	  return TCL_ERROR;			  
      if (Tcl_GetInt(interp, argv[4], &printIt) != TCL_OK)	
	  return TCL_ERROR;			  
  }  else {
     tol = 1.0e-6;
     numIter = 25;      
  }

  if (theTest != 0) {
    delete theTest;
    theTest = 0;
  }
      
  if (strcmp(argv[1],"NormUnbalance") == 0) 
    theTest = new CTestNormUnbalance(tol,numIter,printIt);       
  else if (strcmp(argv[1],"NormDispIncr") == 0) 
    theTest = new CTestNormDispIncr(tol,numIter,printIt);             
  else if (strcmp(argv[1],"EnergyIncr") == 0) 
    theTest = new CTestEnergyIncr(tol,numIter,printIt);             
  else if (strcmp(argv[1],"RelativeNormUnbalance") == 0) 
    theTest = new CTestRelativeNormUnbalance(tol,numIter,printIt);       
  else if (strcmp(argv[1],"RelativeNormDispIncr") == 0) 
    theTest = new CTestRelativeNormDispIncr(tol,numIter,printIt);             
  else if (strcmp(argv[1],"RelativeEnergyIncr") == 0) 
    theTest = new CTestRelativeEnergyIncr(tol,numIter,printIt);             
  else {
    cerr << "WARNING No ConvergenceTest type (NormUnbalance, NormDispIncr, EnergyIncr, ";
    cerr << "RelativeNormUnbalance, RelativeNormDispIncr, RelativeEnergyIncr, )"; 
    return TCL_ERROR;
  }    

  // if the algorithm exists - we want to change the test
  if (theAlgorithm != 0)
     theAlgorithm->setTest(*theTest);  

  return TCL_OK;
}





//
// command invoked to allow the Integrator object to be built
//
int 
specifyIntegrator(ClientData clientData, Tcl_Interp *interp, int argc, 
		      char **argv)
{
  // make sure at least one other argument to contain integrator
  if (argc < 2) {
      interp->result = "WARNING need to specify an Integrator type ";
      return TCL_ERROR;
  }    

  // check argv[1] for type of Numberer and create the object
  if (strcmp(argv[1],"LoadControl") == 0) {
      double dLambda;
      double minIncr, maxIncr;
      int numIter;
      if (argc < 3) {
	interp->result = "WARNING incorrect # args - integrator LoadControl dlam <Jd dlamMin dlamMax>";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &dLambda) != TCL_OK)	
	return TCL_ERROR;	
      if (argc > 5) {
	if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)	
	  return TCL_ERROR;	
	if (Tcl_GetDouble(interp, argv[4], &minIncr) != TCL_OK)	
	  return TCL_ERROR;	
	if (Tcl_GetDouble(interp, argv[5], &maxIncr) != TCL_OK)	
	  return TCL_ERROR;	  
      }
      else {
	minIncr = dLambda;
	maxIncr = dLambda;
	numIter = 1;
      }
      theStaticIntegrator = new LoadControl(dLambda, numIter, minIncr, maxIncr);       
  }

  
  else if (strcmp(argv[1],"ArcLength") == 0) {
      double arcLength;
      double alpha;
      if (argc != 4) {
	interp->result = "WARNING integrator ArcLength arcLength alpha ";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &arcLength) != TCL_OK)	
	return TCL_ERROR;	
      if (Tcl_GetDouble(interp, argv[3], &alpha) != TCL_OK)	
	return TCL_ERROR;	
      theStaticIntegrator = new ArcLength(arcLength,alpha);       
  }

  else if (strcmp(argv[1],"ArcLength1") == 0) {
      double arcLength;
      double alpha;
      if (argc != 4) {
	interp->result = "WARNING integrator ArcLength1 arcLength alpha ";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &arcLength) != TCL_OK)	
	return TCL_ERROR;	
      if (Tcl_GetDouble(interp, argv[3], &alpha) != TCL_OK)	
	return TCL_ERROR;	
      theStaticIntegrator = new ArcLength1(arcLength,alpha);       
  }

  else if (strcmp(argv[1],"MinUnbalDispNorm") == 0) {
      double lambda11, minlambda, maxlambda;
      int numIter;
      if (argc < 3) {
	cerr << "WARNING integrator MinUnbalDispNorm lambda11 <Jd minLambda1j maxLambda1j>\n";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &lambda11) != TCL_OK)	
	return TCL_ERROR;	
      if (argc > 5) {
	if (Tcl_GetInt(interp, argv[3], &numIter) != TCL_OK)	
	  return TCL_ERROR;	
	if (Tcl_GetDouble(interp, argv[4], &minlambda) != TCL_OK)	
	  return TCL_ERROR;	
	if (Tcl_GetDouble(interp, argv[5], &maxlambda) != TCL_OK)	
	  return TCL_ERROR;	
      }
      else {
	minlambda = lambda11;
	maxlambda = lambda11;
	numIter = 1;
	argc += 3;
      }

      int signFirstStepMethod = SIGN_LAST_STEP;
      if (argc == 7)
	if ((strcmp(argv[argc-1],"-determinant") == 0) ||
	    (strcmp(argv[argc-1],"-det") == 0))
	    signFirstStepMethod = CHANGE_DETERMINANT;	    

      theStaticIntegrator = new MinUnbalDispNorm(lambda11,numIter,minlambda,maxlambda,signFirstStepMethod);
  }
  
  else if (strcmp(argv[1],"DisplacementControl") == 0) {
      int node;
      int dof;
      double increment, minIncr, maxIncr;
      int numIter;
      if (argc < 5) {
	cerr << "WARNING integrator DisplacementControl node dof dU ";
	cerr << "<Jd minIncrement maxIncrement>\n";
	return TCL_ERROR;
      }    
      if (Tcl_GetInt(interp, argv[2], &node) != TCL_OK)	
	return TCL_ERROR;	
      if (Tcl_GetInt(interp, argv[3], &dof) != TCL_OK)	
	return TCL_ERROR;	
      if (Tcl_GetDouble(interp, argv[4], &increment) != TCL_OK)	
	return TCL_ERROR;	      
      if (argc > 7) {
	if (Tcl_GetInt(interp, argv[5], &numIter) != TCL_OK)	
	  return TCL_ERROR;	
	if (Tcl_GetDouble(interp, argv[6], &minIncr) != TCL_OK)	
	  return TCL_ERROR;	
	if (Tcl_GetDouble(interp, argv[7], &maxIncr) != TCL_OK)	
	  return TCL_ERROR;	  
      }
      else {
	minIncr = increment;
	maxIncr = increment;
	numIter = 1;
      }


      theStaticIntegrator = new DisplacementControl(node,dof-1,increment, &theDomain,
						    numIter, minIncr, maxIncr);
  }  
  
  else if (strcmp(argv[1],"Newmark") == 0) {
      double gamma;
      double beta;
      double alphaM, betaK, betaKi, betaKc;
      if (argc != 4 && argc != 8) {
	interp->result = "WARNING integrator Newmark gamma beta <alphaM> <betaKcurrent> <betaKi> <betaKlastCommitted>";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &gamma) != TCL_OK) {
	  interp->result = "WARNING integrator Newmark gamma beta - undefined gamma";	  
	  return TCL_ERROR;	
      }
      if (Tcl_GetDouble(interp, argv[3], &beta) != TCL_OK) {
	  interp->result = "WARNING integrator Newmark gamma beta - undefined beta";
	  return TCL_ERROR;	
      }
      if (argc == 8) {
	  if (Tcl_GetDouble(interp, argv[4], &alphaM) != TCL_OK) {
	      cerr << "WARNING integrator Newmark gamma beta alphaM betaK betaKi betaKc - alphaM";	  
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[5], &betaK) != TCL_OK) {
	      cerr << "WARNING integrator Newmark gamma beta alphaM betaK betaKi betaKc - betaK";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[6], &betaKi) != TCL_OK) {
	      cerr << "WARNING integrator Newmark gamma beta alphaM betaK betaKi betaKc - betaKi";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[7], &betaKc) != TCL_OK) {
	      cerr << "WARNING integrator Newmark gamma beta alphaM betaK betaKi betaKc - betaKc";
	      return TCL_ERROR;	
	  }
      }
      if (argc == 4)
	  theTransientIntegrator = new Newmark(gamma,beta);       
      else
	  theTransientIntegrator = new Newmark(gamma,beta,alphaM,betaK,betaKi,betaKc);
  }  
  
  else if (strcmp(argv[1],"HHT") == 0) {
      double alpha;
      double alphaM, betaK, betaKi, betaKc;
      if (argc != 3 && argc != 7) {
	interp->result = "WARNING integrator HHT alpha <alphaM betaKcurrent betaKi betaKlastCommitted>";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &alpha) != TCL_OK) {
	  interp->result = "WARNING integrator HHT alpha - undefined alpha";	  
	  return TCL_ERROR;	
      }
      if (argc == 7) {
	  if (Tcl_GetDouble(interp, argv[3], &alphaM) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha alphaM betaK betaKi betaKc - alphaM";	  
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[4], &betaK) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha alphaM betaK betaKi betaKc - betaK";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[5], &betaKi) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha alphaM betaK betaKi betaKc - betaKi";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[6], &betaKc) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha alphaM betaK betaKi betaKc - betaKc";
	      return TCL_ERROR;	
	  }
      }      
      if (argc == 3)
	  theTransientIntegrator = new HHT(alpha);       
      else
	  theTransientIntegrator = new HHT(alpha,alphaM,betaK, betaKi, betaKc);       
  }    

  else if (strcmp(argv[1],"GeneralizedHHT") == 0) {
      double alpha, beta, gamma;
      double alphaM, betaK, betaKi, betaKc;
      if (argc != 5 && argc != 9) {
	interp->result = "WARNING integrator HHT alpha beta gamma <alphaM betaKcurrent betaKi betaKlastCommitted>";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &alpha) != TCL_OK) {
	  interp->result = "WARNING integrator HHT alpha beta gamma - undefined alpha";	  
	  return TCL_ERROR;	
      }
      if (Tcl_GetDouble(interp, argv[3], &beta) != TCL_OK) {
	  interp->result = "WARNING integrator HHT alpha beta gamma - undefined beta";	  
	  return TCL_ERROR;	
      }
      if (Tcl_GetDouble(interp, argv[4], &gamma) != TCL_OK) {
	  interp->result = "WARNING integrator HHT alpha beta gamma - undefined gamma";	  
	  return TCL_ERROR;	
      }
      if (argc == 9) {
	  if (Tcl_GetDouble(interp, argv[5], &alphaM) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha beta gamma alphaM betaK betaKi betaKc - alphaM";	  
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[6], &betaK) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha beta gamma alphaM betaK betaKi betaKc - betaK";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[7], &betaKi) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha beta gamma alphaM betaK betaKi betaKc - betaKi";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[8], &betaKc) != TCL_OK) {
	      cerr << "WARNING integrator HHT alpha beta gamma alphaM betaK betaKi betaKc - betaKc";
	      return TCL_ERROR;	
	  }
      }      
      if (argc == 5)
	  theTransientIntegrator = new HHT(alpha, beta, gamma);       
      else
	  theTransientIntegrator = new HHT(alpha, beta, gamma, alphaM,betaK, betaKi, betaKc);       
  }    


  else if (strcmp(argv[1],"Newmark1") == 0) {
      double gamma;
      double beta;
      double alphaM, betaK, betaKi, betaKc;
      if (argc != 4 && argc != 8) {
	interp->result = "WARNING integrator Newmark1 gamma beta <alphaM> <betaKcurrent> <betaKi> <betaKlastCommitted>";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &gamma) != TCL_OK) {
	  interp->result = "WARNING integrator Newmark1 gamma beta - undefined gamma";	  
	  return TCL_ERROR;	
      }
      if (Tcl_GetDouble(interp, argv[3], &beta) != TCL_OK) {
	  interp->result = "WARNING integrator Newmark1 gamma beta - undefined beta";
	  return TCL_ERROR;	
      }
      if (argc == 8) {
	  if (Tcl_GetDouble(interp, argv[4], &alphaM) != TCL_OK) {
	      cerr << "WARNING integrator Newmark1 gamma beta alphaM betaK betaKi betaKc - alphaM";	  
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[5], &betaK) != TCL_OK) {
	      cerr << "WARNING integrator Newmark1 gamma beta alphaM betaK betaKi betaKc - betaK";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[6], &betaKi) != TCL_OK) {
	      cerr << "WARNING integrator Newmark1 gamma beta alphaM betaK betaKi betaKc - betaKi";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[7], &betaKc) != TCL_OK) {
	      cerr << "WARNING integrator Newmark1 gamma beta alphaM betaK betaKi betaKc - betaKc";
	      return TCL_ERROR;	
	  }
      }
      if (argc == 4)
	  theTransientIntegrator = new Newmark1(gamma,beta);       
      else
	  theTransientIntegrator = new Newmark1(gamma,beta,alphaM,betaK,betaKi,betaKc);
  }

  else if (strcmp(argv[1],"HHT1") == 0) {
      double alpha;
      double alphaM, betaK, betaKi, betaKc;
      if (argc != 3 && argc != 7) {
	interp->result = "WARNING integrator HHT1 alpha <alphaM> <betaKcurrent> <betaKi> <betaKlastCommitted>";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &alpha) != TCL_OK) {
	  interp->result = "WARNING integrator HHT alpha - undefined alpha";	  
	  return TCL_ERROR;	
      }
      if (argc == 7) {
	  if (Tcl_GetDouble(interp, argv[3], &alphaM) != TCL_OK) {
	      cerr << "WARNING integrator HHT1 gamma beta alphaM betaK betaKi betaKc - alphaM";	  
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[4], &betaK) != TCL_OK) {
	      cerr << "WARNING integrator HHT1 gamma beta alphaM betaK betaKi betaKc - betaK";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[5], &betaKi) != TCL_OK) {
	      cerr << "WARNING integrator HHT1 gamma beta alphaM betaK betaKi betaKc - betaKi";
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[6], &betaKc) != TCL_OK) {
	      cerr << "WARNING integrator HHT1 gamma beta alphaM betaK betaKi betaKc - betaKc";
	      return TCL_ERROR;	
	  }
      }      
      if (argc == 3)
	  theTransientIntegrator = new HHT1(alpha);       
      else
	  theTransientIntegrator = new HHT1(alpha,alphaM,betaK, betaKi, betaKc);       
  }    

  
  else if (strcmp(argv[1],"WilsonTheta") == 0) {
      double theta, alphaM,betaK;
      if (argc != 3 && argc != 5) {
	interp->result = "WARNING integrator WilsonTheta theta <alphaM> <betaK>";
	return TCL_ERROR;
      }    
      if (Tcl_GetDouble(interp, argv[2], &theta) != TCL_OK) {
	  interp->result = "WARNING integrator WilsonTheta theta - undefined theta";	  
	  return TCL_ERROR;	
      }
      if (argc == 5) {
	  if (Tcl_GetDouble(interp, argv[3], &alphaM) != TCL_OK) {
	      cerr << "WARNING integrator WilsonTheta gamma beta alphaM betaK - alphaM";	  
	      return TCL_ERROR;	
	  }
	  if (Tcl_GetDouble(interp, argv[4], &betaK) != TCL_OK) {
	      cerr << "WARNING integrator WilsonTheta gamma beta alphaM betaK  - betaK";
	      return TCL_ERROR;	
	  }
      }            
      if (argc == 3)
	  theTransientIntegrator = new WilsonTheta(theta);       
      else
	  theTransientIntegrator = new WilsonTheta(theta,alphaM,betaK);       	  
  }      

  else {
    interp->result = "WARNING No Integrator type exists ";
    return TCL_ERROR;
  }    

  // if the analysis exists - we want to change the Integrator
  if (theStaticAnalysis != 0)
    theStaticAnalysis->setIntegrator(*theStaticIntegrator);
  else if (theTransientAnalysis != 0)
    theTransientAnalysis->setIntegrator(*theTransientIntegrator);

  return TCL_OK;
}


extern int
TclAddRecorder(ClientData clientData, Tcl_Interp *interp, int argc, 
	       char **argv, Domain &theDomain);

int 
addRecorder(ClientData clientData, Tcl_Interp *interp, int argc, 
	    char **argv)
{
    return TclAddRecorder(clientData, interp, argc, argv, theDomain);
}

extern int
TclAddAlgorithmRecorder(ClientData clientData, Tcl_Interp *interp, int argc, 
			char **argv, Domain &theDomain, EquiSolnAlgo *theAlgorithm);

int 
addAlgoRecorder(ClientData clientData, Tcl_Interp *interp, int argc, 
	    char **argv)
{
	if (theAlgorithm != 0)
		return TclAddAlgorithmRecorder(clientData, interp, argc, argv,
			theDomain, theAlgorithm);

	else
		return 0;
}

extern int
TclAddDatabase(ClientData clientData, Tcl_Interp *interp, int argc, char **argv, 
	       Domain &theDomain, 
	       FEM_ObjectBroker &theBroker);

int 
addDatabase(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  return TclAddDatabase(clientData, interp, argc, argv, theDomain, theBroker);
}


int 
playbackRecorders(ClientData clientData, Tcl_Interp *interp, int argc, 
		  char **argv)
{
  // make sure at least one other argument to contain integrator
  if (argc < 2) {
      interp->result = "WARNING need to specify the commitTag ";
      return TCL_ERROR;
  }    

  // check argv[1] for type of Numberer and create the object
  int cTag;
  if (Tcl_GetInt(interp, argv[1], &cTag) != TCL_OK)	
      return TCL_ERROR;	

  theDomain.playback(cTag);
  return TCL_OK;

}

int 
playbackAlgorithmRecorders(ClientData clientData, Tcl_Interp *interp, int argc, 
			   char **argv)
{
  // make sure at least one other argument to contain integrator
  if (argc < 2) {
      interp->result = "WARNING need to specify the commitTag ";
      return TCL_ERROR;
  }    

  // check argv[1] for type of Numberer and create the object
  int cTag;
  if (Tcl_GetInt(interp, argv[1], &cTag) != TCL_OK)	
      return TCL_ERROR;	
  if (theAlgorithm != 0)
    theAlgorithm->playback(cTag);

  return TCL_OK;

}


/*
int 
groundExcitation(ClientData clientData, Tcl_Interp *interp, int argc, 
		  char **argv)
{
  // make sure at least one other argument to contain integrator
  if (argc < 2) {
      interp->result = "WARNING need to specify the commitTag ";
      return TCL_ERROR;
  }    

  if (strcmp(argv[1],"Single") == 0) {
      if (argc < 4) {
	interp->result = "WARNING quake single dof motion";
	return TCL_ERROR;
      }    

      int dof;
      if (Tcl_GetInt(interp, argv[2], &dof) != TCL_OK)	
	  return TCL_ERROR;	      
      
      // read in the ground motion
      GroundMotion *theMotion;
      if (strcmp(argv[3],"ElCentro") == 0) {
	  double fact = 1.0;
	  if (argc == 5) {
	      if (Tcl_GetDouble(interp, argv[4], &fact) != TCL_OK)	
		  return TCL_ERROR;	
	  }
	  theMotion = new ElCentroGroundMotion(fact);
      } else {
	  interp->result = "WARNING quake Single motion - no motion type exists ";
	  return TCL_ERROR;      
      }

      Load *theLoad = new SingleExcitation(*theMotion, dof, nextTag++);
      theDomain.addOtherLoad(theLoad);
      return TCL_OK;
  }  
  
  else {
    interp->result = "WARNING No quake type exists ";
    return TCL_ERROR;
  }    
}
*/

int 
rigidLink(ClientData clientData, Tcl_Interp *interp, int argc, 
	  char **argv)
{
  if (argc < 4) {
      interp->result = "WARNING rigidLink linkType? rNode? cNode?\n";
      return TCL_ERROR;
  }    

  int numMPs = theDomain.getNumMPs();
  int rNode, cNode;
  if (Tcl_GetInt(interp, argv[2], &rNode) != TCL_OK) {
      cerr << "WARNING rigidLink linkType? rNode? cNode? - could not read rNode ";
      return TCL_ERROR;	        
  }
  if (Tcl_GetInt(interp, argv[3], &cNode) != TCL_OK) {
      cerr << "WARNING rigidLink linkType? rNode? cNode? - could not read CNode ";
      return TCL_ERROR;	        
  }

  // construct a rigid rod or beam depending on 1st arg
  if ((strcmp(argv[1],"-bar") == 0) || (strcmp(argv[1],"bar") == 0)) {
    RigidRod theLink(theDomain, rNode, cNode, numMPs);
  } else if ((strcmp(argv[1],"-beam") == 0) || (strcmp(argv[1],"beam") == 0)) {
    RigidBeam theLink(theDomain, rNode, cNode, numMPs);
  } else {
      cerr << "WARNING rigidLink linkType? rNode? cNode? - unrecognised link type (-bar, -beam) ";
      return TCL_ERROR;	        
  }

  return TCL_OK;
}



int 
rigidDiaphragm(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  if (argc < 3) {
      interp->result = "WARNING rigidLink perpDirn? rNode? <cNodes?>";
      return TCL_ERROR;
  }    

  int rNode, perpDirn;
  if (Tcl_GetInt(interp, argv[1], &perpDirn) != TCL_OK) {
      cerr << "WARNING rigidLink perpDirn rNode cNodes - could not read perpDirn? ";
      return TCL_ERROR;	        
  }

  if (Tcl_GetInt(interp, argv[2], &rNode) != TCL_OK) {
      cerr << "WARNING rigidLink perpDirn rNode cNodes - could not read rNode ";
      return TCL_ERROR;	        
  }
  
  // read in the constrained Nodes
  int numConstrainedNodes = argc - 3;
  ID constrainedNodes(numConstrainedNodes);
  for (int i=0; i<numConstrainedNodes; i++) {
      int cNode;
      if (Tcl_GetInt(interp, argv[3+i], &cNode) != TCL_OK) {
	  cerr << "WARNING rigidLink perpDirn rNode cNodes - could not read a cNode";
	  return TCL_ERROR;	        
      }
      constrainedNodes(i) = cNode;
  }
  int numMPs = theDomain.getNumMPs();
  RigidDiaphragm theLink(theDomain, rNode, constrainedNodes, 
	perpDirn-1, numMPs);

  return TCL_OK;
}


int 
eigenAnalysis(ClientData clientData, Tcl_Interp *interp, int argc, 
		char **argv)
{
     // make sure at least one other argument to contain type of system
    if (argc < 2) {
	interp->result = "WARNING want - eigen <type> numModes?";
	return TCL_ERROR;
    }    

    int typeAlgo = 0; // 0 - frequency (default), 1 - standard, 2 - buckling
    int typeSolver = 0; // 0 - SymmBandArpack (default), 1 - SymmSparseArpack, 2 - GenBandArpack
    int loc = 1;

    // Check type of eigenvalue analysis
    while (loc < (argc-1)) {
      
      if ((strcmp(argv[loc],"frequency") == 0) || 
	  (strcmp(argv[loc],"-frequency") == 0))
	typeAlgo = 0;

      else if ((strcmp(argv[loc],"standard") == 0) || 
	  (strcmp(argv[loc],"-standard") == 0))
	typeAlgo = 1;

      else if ((strcmp(argv[loc],"symmBandArpack") == 0) || 
	  (strcmp(argv[loc],"-symmBandArpack") == 0))
	typeSolver = 0;

      else if ((strcmp(argv[loc],"symmSparseArpack") == 0) || 
	  (strcmp(argv[loc],"-symmSparseArpack") == 0))
	typeSolver = 1;

      else if ((strcmp(argv[loc],"genBandArpack") == 0) || 
	  (strcmp(argv[loc],"-genBandArpack") == 0))
	typeSolver = 2;

      else {
	cerr << "eigen - unknown option specified " << argv[loc] << endl;
	return TCL_ERROR;
      }

      loc++;
    }


    // check argv[loc] for number of modes
    int numEigen;
    if ((Tcl_GetInt(interp, argv[loc], &numEigen) != TCL_OK) || numEigen < 0) {
      interp->result = "WARNING eigen numModes?  - illegal numModes";    
      return TCL_ERROR;	
    }

    EigenAlgorithm *theEigenAlgo = 0;
    EigenSOE       *theEigenSOE = 0;
    AnalysisModel *theEigenModel = new AnalysisModel();

    // create the algorithm
    if (typeAlgo == 0) 
	theEigenAlgo = new FrequencyAlgo();
    else if (typeAlgo == 1)
	theEigenAlgo = new StandardEigenAlgo();

    // create the eigen system and solver
    if (typeSolver == 0) {
      SymBandEigenSolver *theEigenSolver = new SymBandEigenSolver(); 
      theEigenSOE = new SymBandEigenSOE(*theEigenSolver, *theEigenModel);    
    } else if (typeSolver == 1) {
      SymArpackSolver *theEigenSolver = new SymArpackSolver(numEigen); 
      theEigenSOE = new SymArpackSOE(*theEigenSolver, *theEigenModel);    
    } else if (typeSolver == 3) {  
      BandArpackSolver *theEigenSolver = new BandArpackSolver(numEigen); 
      theEigenSOE = new BandArpackSOE(*theEigenSolver, *theEigenModel);    
    }      

    // create the rest of components of an eigen analysis
    EigenIntegrator  *theEigenIntegrator = new EigenIntegrator();    
    RCM *theRCM = new RCM();	
    DOF_Numberer *theEigenNumberer = new DOF_Numberer(*theRCM);    	
    ConstraintHandler *theEigenHandler = new TransformationConstraintHandler();

    // create the eigen analysis
    theEigenAnalysis = new EigenAnalysis(theDomain,
					 *theEigenHandler,
					 *theEigenNumberer,
					 *theEigenModel,
					 *theEigenAlgo,
					 *theEigenSOE,
					 *theEigenIntegrator);

    int requiredDataSize = 20*numEigen;
    if (requiredDataSize > resDataSize) {
      if (resDataPtr != 0)
	delete [] resDataPtr;
      
      resDataPtr = new char[requiredDataSize];
    }
    for (int i=0; i<requiredDataSize; i++)
      resDataPtr[i] = '\n';

    // perfrom the eigen analysis & store the results with the interpreter
    if (theEigenAnalysis->analyze(numEigen) == 0) {
      //      char *eigenvalueS = new char[15 * numEigen];    
      const Vector &eigenvalues = theDomain.getEigenvalues();
      int cnt = 0;

      for (int i=0; i<numEigen; i++) {
	cnt += sprintf(&resDataPtr[cnt], "%.6e  ", eigenvalues[i]);
      }
      
      Tcl_SetResult(interp, resDataPtr, TCL_STATIC);
    }
      
    // finally invoke the destructor on the eigen analysis
    delete theEigenAnalysis;
    theEigenAnalysis = 0;
    
    return TCL_OK;
}



int 
videoPlayer(ClientData clientData, Tcl_Interp *interp, int argc, 
	    char **argv)
{
    // make sure at least one other argument to contain type of system
    if (argc < 5) {
	interp->result = "WARNING want - video -window windowTitle? -file fileName?\n";
	return TCL_ERROR;
    }    

    char *wTitle =0;
    char *fName = 0;
    char *imageName = 0;
    char *offsetName = 0;

    int endMarker = 1;
    while (endMarker < (argc-1)) {
      if (strcmp(argv[endMarker],"-window") == 0) {
	wTitle = argv[endMarker+1];
	endMarker+=2;
      } else if (strcmp(argv[endMarker],"-file") == 0) {
	fName = argv[endMarker+1];
	endMarker+=2;
      } else if (strcmp(argv[endMarker],"-image") == 0) {
	imageName = argv[endMarker+1];
	endMarker += 2;
      } else if (strcmp(argv[endMarker],"-offset") == 0) {
	offsetName = argv[endMarker+1];
	endMarker += 2;
      }
      else {
	g3ErrorHandler->warning("WARNING unknown %s want - video -window windowTitle? -file fileName?\n", 
				argv[endMarker]);
	return TCL_ERROR;
      }
    }
    
    if (wTitle != 0 && fName != 0) {
      // delete the old video player if one exists
      if (theTclVideoPlayer != 0)
	delete theTclVideoPlayer;

      // create a new player
      theTclVideoPlayer = new TclVideoPlayer(wTitle, fName, imageName, interp, offsetName);
    }
    else
      return TCL_ERROR;

    return TCL_OK;
}



int 
removeObject(ClientData clientData, Tcl_Interp *interp, int argc, 
	     char **argv)
{

    // make sure at least one other argument to contain type of system
      if (argc < 2) {
	interp->result = "WARNING want - remove objectType?\n";
	return TCL_ERROR;
      }    

    int tag;
    if (strcmp(argv[1],"element") == 0) {
      if (argc < 3) {
	interp->result = "WARNING want - remove element eleTag?\n";
	return TCL_ERROR;
      }    

      if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	cerr << "WARNING remove element tag? failed to read tag: " << argv[2] << endl;
	return TCL_ERROR;
      }      
      Element *theEle = theDomain.removeElement(tag);
      if (theEle != 0) {
	// we also have to remove any elemental loads from the domain
	LoadPatternIter &theLoadPatterns = theDomain.getLoadPatterns();
	LoadPattern *thePattern;
	
	// go through all load patterns
	while ((thePattern = theLoadPatterns()) != 0) {
	  ElementalLoadIter theEleLoads = thePattern->getElementalLoads();
	  ElementalLoad *theLoad;

	  // go through all elemental loads in the pattern
	  while ((theLoad = theEleLoads()) != 0) {

	    // remove & destroy elemental from elemental load if there
	    // note - if last element in load, remove the load and delete it
	    
	    /* *****************
	    int numLoadsLeft = theLoad->removeElement(tag);
	    if (numLoadsLeft == 0) {
	      thePattern->removeElementalLoad(theLoad->getTag());
	      delete theLoad;
	    }
	    *********************/
	  }
	}

	// finally invoke the destructor on the element
	delete theEle;
      }
    }


    else if (strcmp(argv[1],"loadPattern") == 0) {
      if (argc < 3) {
	interp->result = "WARNING want - remove loadPattern patternTag?\n";
	return TCL_ERROR;
      }    
      if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
	cerr << "WARNING remove loadPattern tag? failed to read tag: " << argv[2] << endl;
	return TCL_ERROR;
      }      
      LoadPattern *thePattern = theDomain.removeLoadPattern(tag);
      if (thePattern != 0) {
	delete thePattern;
      }
    }


    else if (strcmp(argv[1],"recorders") == 0) {
      theDomain.removeRecorders();
    }

    //Boris Jeremic and Joey Yang -- UC Davis
    else if (strcmp(argv[1],"SPconstraint") == 0) {
      if (argc < 3) {
      	interp->result = "WARNING want - remove loadPattern patternTag?\n";
      	return TCL_ERROR;
      }    
      if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
      	cerr << "WARNING remove loadPattern tag? failed to read tag: " << argv[2] << endl;
      	return TCL_ERROR;
      }      
      
      SP_Constraint *theSPconstraint = theDomain.removeSP_Constraint(tag);
      if (theSPconstraint != 0) {
	delete theSPconstraint;
      }
    }
    
#ifdef _RELIABILITY
// AddingSensitivity:BEGIN ///////////////////////////////////////
    else if (strcmp(argv[1],"randomVariablePositioner") == 0) {
		int rvPosTag;
		if (Tcl_GetInt(interp, argv[2], &rvPosTag) != TCL_OK) {
			cerr << "WARNING invalid input: rvPositionerTag \n";
			return TCL_ERROR;
		}
		ReliabilityDomain *theReliabilityDomain = theReliabilityBuilder->getReliabilityDomain();
		theReliabilityDomain->removeRandomVariablePositioner(rvPosTag);
	}
// AddingSensitivity:END ///////////////////////////////////////
#endif

    else
      cerr << "WARNING remove element, loadPattern - only commands  available at the moment: " << endl;

    return TCL_OK;
}


int 
nodeDisp(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    
    // make sure at least one other argument to contain type of system
    if (argc < 3) {
	interp->result = "WARNING want - nodeDisp nodeTag? dof?\n";
	return TCL_ERROR;
   }    

    int tag, dof;

    if (Tcl_GetInt(interp, argv[1], &tag) != TCL_OK) {
	cerr << "WARNING nodeDisp nodeTag? dof? - could not read nodeTag? ";
	return TCL_ERROR;	        
    }    
    if (Tcl_GetInt(interp, argv[2], &dof) != TCL_OK) {
	cerr << "WARNING nodeDisp nodeTag? dof? - could not read dof? ";
	return TCL_ERROR;	        
    }        
    
    Node *theNode = theDomain.getNode(tag);
    double value = 0.0;
    if (theNode != 0) {
	const Vector &disp = theNode->getTrialDisp();
	if (disp.Size() >= dof && dof > 0) {
	    value = disp(dof-1); // -1 for OpenSees vs C indexing
	}
    }
    
    // now we copy the value to the tcl string that is returned
    sprintf(interp->result,"%35.20f",value);
    
	
    return TCL_OK;
}


// AddingSensitivity:BEGIN ////////////////////////////////////
int 
sensNodeDisp(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int tag, dof, gradNum;

    if (Tcl_GetInt(interp, argv[1], &tag) != TCL_OK) {
	cerr << "WARNING nodeDisp nodeTag? dof? gradNum?- could not read nodeTag? ";
	return TCL_ERROR;	        
    }    
    if (Tcl_GetInt(interp, argv[2], &dof) != TCL_OK) {
	cerr << "WARNING nodeDisp nodeTag? dof? gradNum?- could not read dof? ";
	return TCL_ERROR;	        
    }        
    if (Tcl_GetInt(interp, argv[3], &gradNum) != TCL_OK) {
	cerr << "WARNING nodeDisp nodeTag? dof? gradNum?- could not read dof? ";
	return TCL_ERROR;	        
    }        
    
    Node *theNode = theDomain.getNode(tag);
	double value = theNode->getGradient(dof,gradNum);
    
    // copy the value to the tcl string that is returned
    sprintf(interp->result,"%35.20f",value);
	
    return TCL_OK;
}



int 
computeGradients(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
	// Comment to the developer:
	// This Tcl command is meant to be called only for
	// path-independent problems.  In such cases the 
	// gradients can be computed AFTER the complete
	// structural analysis is completed.
	// No error messages is returned if the user tries to invoke this command
	// during a path-dependent analysis.  The reason is that the reliability
	// analysis will call this method BOTH for path dependent and independent problems. 

#ifdef _RELIABILITY
	if (theSensitivityAlgorithm->isPathDependent()) {
	}
	else {

		theSensitivityAlgorithm->computeGradients();
	}
#endif
    return TCL_OK;
}
// AddingSensitivity:END //////////////////////////////////////


int 
startTimer(ClientData clientData, Tcl_Interp *interp, int argc, 
	   char **argv)
{
  if (theTimer == 0)
    theTimer = new Timer();
  
  theTimer->start();
  return TCL_OK;
}

int 
stopTimer(ClientData clientData, Tcl_Interp *interp, int argc, 
	  char **argv)
{
  if (theTimer == 0)
    return TCL_OK;
  
  theTimer->pause();
  cerr << *theTimer;
  return TCL_OK;
}



