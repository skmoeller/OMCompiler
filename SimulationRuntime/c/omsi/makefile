#Makefile for OpenModelicaSolverInterface shared solver library

CC=gcc
CFLAGS=-fPIC -Wall -Wextra -O2 -g
LDFLAGS=-shared
TARGET_LIB = so
# =dll or =so for dynamic-link library and shared objects

LIB_DIR = -L/d/workspace/Testlab/libs/
LIBS = $(LAPACKLIB) $(BLASLIB)
LAPACKLIB = -llapack
BLASLIB = -lblas
RUNTIMEPATH = .

OMSI_HEADERS_DIR=../../OMSI/include
OMC_HEADERS_DIR=../
OMC_UTIL_DIR=../util
OMDEV_MSYS_LIBS=/c/OMDev/tools/msys/mingw64/lib

#Solver directory
DIR=./Solver
OMSI_MATH_DIR=$(DIR)/omsi_math
MATH_SRCS=$(OMSI_MATH_DIR)/omsi_matrix.c $(OMSI_MATH_DIR)/omsi_vector.c
MATH_HDRS=$(OMSI_MATH_DIR)/omsi_matrix.h $(OMSI_MATH_DIR)/omsi_vector.h $(OMSI_MATH_DIR)/omsi_math.h

SOLVER_INC_DIRS=$(DIR)/Lapack/ -I$(DIR)/TotalPivot/
#SOLVER_SRCS=$(DIR)/Lapack/Lapack.c $(DIR)/TotalPivot/TotalPivot.c $(DIR)/TotalPivot/TotalPivot_Interface.c $(DIR)/Newton/Newton.c $(DIR)/Newton/newtonIteration.c
#SOLVER_HDRS=$(DIR)/Lapack/Lapack.h $(DIR)/TotalPivot/TotalPivot.h $(DIR)/TotalPivot/TotalPivot_Interface.h $(DIR)/Newton/Newton.h $(DIR)/Newton/newtonIteration.h
SOLVER_SRCS=$(DIR)/Lapack/Lapack.c $(DIR)/TotalPivot/TotalPivot.c $(DIR)/TotalPivot/TotalPivot_Interface.c
SOLVER_HDRS=$(DIR)/Lapack/Lapack.h $(DIR)/TotalPivot/TotalPivot.h $(DIR)/TotalPivot/TotalPivot_Interface.h

SRCS=$(MATH_SRCS) $(SOLVER_SRCS)
HDRS=$(MATH_HDRS) $(SOLVER_HDRS)
INCS=-I$(SOLVER_INC_DIRS) -I$(OMSI_MATH_DIR) -I$(OMSI_HEADERS_DIR) -I$(OMC_HEADERS_DIR) -I$(OMC_UTIL_DIR)
OBJS=$(SRCS:%.c=%.o)

# varibales for OMSU library creation
OMSU_SRCS = omsu_ContinuousSimulation.c omsu_EventSimulation.c omsu_GettersAndSetters.c omsu_Initialization.c omsu_me.c omsu_utils.c fmi2/osi_fmi2_wrapper.c
OMSU_HDRS = omsu_ContinuousSimulation.h omsu_EventSimulation.h omsu_GettersAndSetters.h omsu_Initialization.h omsu_me.h omsu_utils.h
OMSU_INCS = -I./ -I./fmi2 -I/d/workspace/OpenModelica/OMCompiler/SimulationRuntime/c/omsi/ -I/d/workspace/OpenModelica/OMCompiler/SimulationRuntime/c/ -I/d/workspace/OpenModelica/OMCompiler/3rdParty/gc/include/ -I/d/workspace/OpenModelica/OMCompiler/SimulationRuntime/OMSI/
OMSU_OBJS = $(OMSU_SRCS:%.c=%.o)

.PHONY: all
all: libomsisolver test libomsu

libomsisolver: $(OBJS)
	$(CC) $(LDFLAGS) -o libomsisolver.$(TARGET_LIB) $^ $(LIB_DIR) $(LIBS)

$(OBJS:%.o): $(SRCS:%.c)
	$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

$(SRCS): $(HDRS)

test: ./Solver/testsuit/main.c $(SRCS) $(HDRS)
	$(CC) $(INCS) -Wl,-rpath=$(RUNTIMEPATH),libomsisolver.$(TARGET_LIB) $(LIB_DIR) $(LIBS) -o test ./Solver/testsuit/main.c

libomsu: $(OMSU_OBJS)
	$(CC) $(LDFLAGS) -o omsu.$(TARGET_LIB) $^

$(OMSU_OBJS): $(OMSU_SRCS)
	$(CC) $(CFLAGS) $(OMSU_INCS) -c -o $@ $<

$(OMSU_SRCS): $(OMSU_HDRS)


.PHONY: clean
clean: clean_solver clean_omsu

.PHONY: clean_solver
clean_solver:
	rm -rf libomsisolver.* $(OBJS) test

.PHONY: clean_omsu
clean_omsu:
	rm -rf omsu.* $(OMSU_OBJS)
