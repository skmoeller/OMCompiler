#Makefile for OpenModelicaSolverInterface shared solver library

CC=gcc
CFLAGS=-fPIC -Wall -Wextra -O2 -g
LDFLAGS=-shared
TARGET_LIB = so
# =dll or =so for dynamic-link library and shared objects

LIB_DIR = -L/d/workspace/Testlab/libs/
#LIB_DIR = -L/home/wbraun/workspace/OpenModelica/OMCompiler/SimulationRuntime/c/omsi
LIBS = $(LAPACKLIB) $(BLASLIB)
LAPACKLIB = -llapack
BLASLIB = -lblas
RUNTIMEPATH = .

PATH_TO_OPENMODELICA = /d/workspace/OpenModelica

PATH_TO_OMSIC = $(PATH_TO_OPENMODELICA)/OMCompiler/SimulationRuntime/OMSIC
PATH_TO_OMSI = $(PATH_TO_OPENMODELICA)/OMCompiler/SimulationRuntime/OMSI
C_RUNTIME = $(PATH_TO_OPENMODELICA)/OMCompiler/SimulationRuntime/c
THIRD_PARTY = $(PATH_TO_OPENMODELICA)/OMCompiler/3rdParty

OMSIC_INCLUDE = $(PATH_TO_OMSIC)/include
OMSI_INCLUDE = $(PATH_TO_OMSI)/include


all:
	make -C src/omsu libomsu


.PHONY: clean
clean:
	make -C src/omsu clean_omsu_all

.PHONY: clean_o_files
clean_o_files:
	make -C src/omsu clean_omsu_o