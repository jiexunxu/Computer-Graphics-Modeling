############################################################################
#
#  Program:         SuperLU
#
#  Module:          make.inc
#
#  Purpose:         Top-level Definitions
#
#  Creation date:   October 2, 1995
#
#  Modified:	    February 4, 1997  Version 1.0
#		    November 15, 1997 Version 1.1
#		    September 1, 1999 Version 2.0
#
############################################################################
#
#  The machine (platform) identifier to append to the library names
#
PLAT = _jacquard

#
#  The name of the libraries to be created/linked to
#
SuperLUroot	= $(HOME)/SuperLU_4.0
SUPERLULIB   	= $(SuperLUroot)/lib/libsuperlu_4.0.a
TMGLIB       	= libtmglib.a
BLASDEF	     	= -DUSE_VENDOR_BLAS
BLASLIB      	= -L/usr/common/usg/acml/4.0/pathscale64/lib -lacml -lacml_mv
# BLASLIB      	=  $(ACML)
LIBS		= $(SUPERLULIB) $(BLASLIB)

#
#  The archiver and the flag(s) to use when building archive (library)
#  If your system has no ranlib, set RANLIB = echo.
#
ARCH         = ar
ARCHFLAGS    = cr
RANLIB       = ranlib

CC           = pathcc
CFLAGS       = -O3 -OPT:IEEE_arithmetic=1 -OPT:IEEE_NaN_inf=ON -m64 -fPIC
FORTRAN	     = pathf90
FFLAGS       = -O3 -m64
LOADER       = pathf90
LOADOPTS     = -m64

#
#  C preprocessor defs for compilation (-DNoChange, -DAdd_, or -DUpCase)
#
CDEFS        = -DAdd_
#
# The directory in which Matlab is installed
#
MATLAB       = /usr/common/usg/matlab/R2006a
# MATLAB	     = /usr/sww/pkg/matlab
