# Makefile for creating our standalone Cython program
PYTHON := python3
GUROBI_VERSION := gurobi75
CYTHON := cython --force -a

GCCOPT := -shared -pthread -fPIC -fwrapv -O3 -Wall -ffast-math -fno-strict-aliasing 

PYVERSION := $(shell $(PYTHON) -c "import sys; print(sys.version[:3])")

INCDIR := $(shell $(PYTHON) -c "from distutils import sysconfig; print(sysconfig.get_python_inc())")

INCDIR2 := $(shell $(PYTHON) -c "import numpy; print(numpy.get_include())")

PLATINCDIR := $(shell $(PYTHON) -c "from distutils import sysconfig; print(sysconfig.get_python_inc(plat_specific=True))")
LIBDIR1 := $(shell $(PYTHON) -c "from distutils import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")
LIBDIR2 := $(shell $(PYTHON) -c "from distutils import sysconfig; print(sysconfig.get_config_var('LIBPL'))")
PYLIB := $(shell $(PYTHON) -c "from distutils import sysconfig; print(sysconfig.get_config_var('LIBRARY')[3:-2])")

CC := $(shell $(PYTHON) -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_var('CC'))")
LINKCC := $(shell $(PYTHON) -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_var('LINKCC'))")
LINKFORSHARED := $(shell $(PYTHON) -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_var('LINKFORSHARED'))")
LIBS := $(shell $(PYTHON) -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_var('LIBS'))")
SYSLIBS :=  $(shell $(PYTHON) -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_var('SYSLIBS'))")

   
%.so: %.c 
	$(CC) $(GCCOPT) $^ -o $@  -I$(INCDIR)  -I$(INCDIR2) -I${GUROBI_HOME}/include/ -L${GUROBI_HOME}/lib/ -l$(GUROBI_VERSION) -lm
%.c: %.pyx 
	$(CYTHON) $^ -I$(INCDIR)  -I$(INCDIR2) 


all: stdgrb/stdgurobi.so

clean:
	@echo cleaning compilation output
	@rm -f *~ stdgrb/*.o stdgrb/*.so core core.*  stdgrb/stdgurobi.c stdgrb/stdgurobi.html test.output

test: clean all
	LD_LIBRARY_PATH=$(LIBDIR1):$$LD_LIBRARY_PATH ./embedded > test.output
	$(PYTHON) assert_equal.py embedded.output test.output
	


FORCE:
