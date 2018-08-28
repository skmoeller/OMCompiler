#Makefile for OpenModelicaSimulationUnit (omsu) Solvers

include ../../makefile

SOLVER_SRCS = lapack/solver_lapack.c solver_helper.c

SOLVER_OBJS = $(SOLVER_SRCS:%.c=%.o)

SOLVER_HDRS = $(OMSIC_INCLUDE)/solver/solver_lapack.h $(OMSIC_INCLUDE)/solver/solver_helper.h\
 $(OMSI_INCLUDE)/omsi.h

SOLVER_INCS = -I$(OMSIC_INCLUDE) -I$(OMSI_INCLUDE)


.PHONY: clean_solver_all clean_solver_o $(SOLVER_OBJS)


libomsisolver: $(SOLVER_OBJS)
	$(CC) $(LDFLAGS) $(SOLVER_INCS) -o ../libomsuSolver.$(TARGET_LIB) $^ $(SOLVER_LIB_DIR) $(SOLVER_LIBS)

$(SOLVER_OBJS): $(SOLVER_SRCS) $(SOLVER_HDRS)
	$(CC) $(CFLAGS) $(SOLVER_INCS) -c -o $@ $(@:%.o=%.c)

clean_solver_all: clean_solver_o
	rm -rf ../../lib/libomsuSolver.*

clean_solver_o:
	rm -rf $(SOLVER_OBJS)
