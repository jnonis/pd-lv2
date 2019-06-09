# Makefile to build class 'helloworld' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = pd-lv2

common.sources =

# input source file (class name == source file basename)
class.sources = lv2~.c

# all extra files to be included in binary distribution of the library
datafiles = 

#flags = -DTEST -Wno-unused-local-typedefs -I./

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
include Makefile.pdlibbuilder
