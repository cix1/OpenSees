# OpenSees -- Open System for Earthquake Engineering Simulation
# Pacific Earthquake Engineering Research Center
# http://opensees.berkeley.edu/
#
# Portal Frame Example 3.2
# ------------------------
#  Reinforced concrete one-bay, one-story frame
#  Distributed vertical load on girder
#  Lateral Load at top of frame
#  
# 
# Example Objectives
# -----------------
#  Nonlinear pushover analysis using Portal Frame Example 1 as starting point
# 
# Units: kips, in, sec
#
# Written: GLF/MHS/fmk
# Date: January 2001




# ----------------------------------------------------
# Start of Model Generation & Initial Gravity Analysis
# ----------------------------------------------------

# Do operations of Example3.1 by sourcing in the tcl file
source Example3.1.tcl
puts "Gravity load analysis completed";

# Set the gravity loads to be constant & reset the time in the domain
loadConst -time 0.0

# ----------------------------------------------------
# End of Model Generation & Initial Gravity Analysis
# ----------------------------------------------------


# ----------------------------------------------------
# Start of additional modelling for lateral loads
# ----------------------------------------------------

# Define lateral loads
# --------------------

# Set some parameters
set H 10.0;		# Reference lateral load

# Set lateral load pattern with a Linear TimeSeries
pattern Plain 2 "Linear" {

        # Create nodal loads at nodes 3 & 4
	#    nd    FX  FY  MZ 
        load 3 $H 0.0 0.0 
        load 4 $H 0.0 0.0 
}

# ----------------------------------------------------
# End of additional modelling for lateral loads
# ----------------------------------------------------



# ----------------------------------------------------
# Start of modifications to analysis for push over
# ----------------------------------------------------

# Set some parameters
set dU 0.1;	        # Displacement increment

# Change the integration scheme to be displacement control
#                             node dof init Jd min max
integrator DisplacementControl  3   1   $dU  1 $dU $dU

# ----------------------------------------------------
# End of modifications to analysis for push over
# ----------------------------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------

# Stop the old recorders by destroying them
# remove recorders

# Create a recorder to monitor nodal displacements
recorder Node node32.out disp -time -node 3 4 -dof 1 2 3
#recorder plot node32.out hi 10 10 300 300 -columns 2 1

# Create a recorder to monitor element forces in columns
recorder Element 1 2 -time -file ele32.out force

# --------------------------------
# End of recorder generation
# ---------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------

# Set some parameters
set maxU 6.0;	        # Max displacement
set numSteps [expr int($maxU/$dU)]

# Perform the analysis
analyze $numSteps
puts "Pushover analysis completed"

# Print the state at node 3
print node 3





