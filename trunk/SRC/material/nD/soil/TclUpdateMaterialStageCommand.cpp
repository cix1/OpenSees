// $Revision: 1.4 $
// $Date: 2001-08-27 18:11:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/TclUpdateMaterialStageCommand.cpp,v $
                                                                        
// Written: ZHY

#include <TclModelBuilder.h>

#include <PressureIndependMultiYield.h>
#include <PressureDependMultiYield.h>
#include <FluidSolidPorousMaterial.h>
#include <Information.h>

#include <string.h>

// by ZHY
int
TclModelBuilderUpdateMaterialStageCommand(ClientData clientData, 
					  Tcl_Interp *interp, 
					  int argc,
					  char **argv, 
					  TclModelBuilder *theTclBuilder)
{
  if (argc < 5) {
      cerr << "WARNING insufficient number of UpdateMaterialStage arguments\n";
      cerr << "Want: UpdateMaterialStage material matTag? stage value?" << endl;
      return TCL_ERROR;
  }

  if (strcmp(argv[1],"-material") != 0) {
      cerr << "WARNING UpdateMaterialStage: Only accept parameter '-material' for now" << endl;
      return TCL_ERROR;		
  }		

  int tag, value; 

  if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK) {
      cerr << "WARNING MYSstage: invalid material tag" << endl;
      return TCL_ERROR;		
  }

  NDMaterial * a = theTclBuilder->getNDMaterial(tag);
  if (a==0) {
      cerr << "WARNING UpdateMaterialStage: couldn't get NDmaterial tagged: " << tag << endl;
      return TCL_ERROR;		
  }

  if (strcmp(argv[3],"-stage") != 0) {
      cerr << "WARNING UpdateMaterialStage: Only accept parameter '-stage' for now" << endl;
      return TCL_ERROR;		
  }		

  if (Tcl_GetInt(interp, argv[4], &value) != TCL_OK) {
      cerr << "WARNING UpdateMaterialStage: invalid parameter value" << endl;
      return TCL_ERROR;		
  }	

  const char * c = a->getType();
	if (strcmp(c, "PlaneStrain") == 0 || 
      strcmp(c, "ThreeDimensional") == 0 ) {
      Information info;
      a->updateParameter(value,info); 
  }
  else {
      cerr << "WARNING UpdateMaterialStage: The tagged is not a "<<endl;
      cerr << "PressureDependMultiYield/PressureIndependMultiYield/FluidSolidPorous material. " << endl;
      return TCL_ERROR;		
  }

  return TCL_OK;
}
