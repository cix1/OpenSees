include ../../../Makefile.def

OBJS       = Brick.o \
	shp3d.o \
	TclBrickCommand.o \
	BbarBrick.o \
	BbarBrickWithSensitivity.o


#	FLBrick.o  ### ????? missing code?

all:         $(OBJS)

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
