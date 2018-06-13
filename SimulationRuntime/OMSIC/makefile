#Makefile for OpenModelicaSolverInterface shared solver library

CC=gcc
CFLAGS=-Wall -Wextra -O2 -g
LDFLAGS=-shared
TARGET_LIB = so
# =dll or =so for dynamic-link library and shared objects

LIB_DIR = -L/d/workspace/Testlab/libs/
#LIB_DIR = -L/home/wbraun/workspace/OpenModelica/OMCompiler/SimulationRuntime/c/omsi
RUNTIMEPATH = .

PATH_TO_OPENMODELICA = /d/workspace/OpenModelica
#PATH_TO_OPENMODELICA = /home/andreas/workspace/OpenModelica

PATH_TO_OMSIC = $(PATH_TO_OPENMODELICA)/OMCompiler/SimulationRuntime/OMSIC
PATH_TO_OMSI = $(PATH_TO_OPENMODELICA)/OMCompiler/SimulationRuntime/OMSI
C_RUNTIME = $(PATH_TO_OPENMODELICA)/OMCompiler/SimulationRuntime/c
THIRD_PARTY = $(PATH_TO_OPENMODELICA)/OMCompiler/3rdParty

OMSIC_INCLUDE = $(PATH_TO_OMSIC)/include
OMSI_INCLUDE = $(PATH_TO_OMSI)/include

# detect OS
TARGET_TRIPLE := $(subst -, ,$(shell $(CC) -dumpmachine))
TARGET_ARCH   := $(word 1,$(TARGET_TRIPLE))
TARGET_OS     := $(word 3,$(TARGET_TRIPLE))

ifeq      ($(TARGET_OS),mingw32)
else ifeq ($(TARGET_OS),cygwin)
else
CFLAGS += -fPIC
endif


all:
	make -C src/omsu libomsu
#	make -C src/solver libomsisolver
	make -C testsuite test


.PHONY: clean
clean:
	make -C src/omsu clean_omsu_all
#	make -C src/solver clean_solver_all
	make -C testsuite clean_test_all

.PHONY: clean_o_files
clean_o_files:
	make -C src/omsu clean_omsu_o
#	make -C src/solver clean_solver_o
	make -C testsuite clean_test_o
