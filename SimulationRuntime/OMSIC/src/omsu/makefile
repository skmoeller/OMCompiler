#Makefile for OpenModelicaSimulationUnit (omsu)

include ../../makefile

# varibales for OMSU library creation
OMSU_SRCS = omsu_helper.c

OMSU_OBJS = $(OMSU_SRCS:%.c=%.o)

OMSU_HDRS = $(OMSIC_INCLUDE)/omsu/omsu_helper.h\
 $(OMSIC_INCLUDE)/solver/solver_lapack.h\
 $(OMSI_INCLUDE)/omsi.h

OMSU_LIB_DIR = -L$(PATH_TO_OMSIC)/src
OMSU_LIBS = -l:libomsuSolver.$(TARGET_LIB)

#FMI2_HDRS=

OMSU_INCS = -I$(OMSIC_INCLUDE) -I$(OMSI_INCLUDE)

.PHONY: clean_omsu_all clean_omsu_o

libomsu: $(OMSU_OBJS)
	$(CC) $(LDFLAGS) $(OMSU_INCS) $^ $(OMSU_LIB_DIR) $(SOLVER_LIB_DIR) $(OMSU_LIBS) $(SOLVER_LIBS) -o libomsu.$(TARGET_LIB)

$(OMSU_OBJS): $(OMSU_SRCS) $(OMSU_HDRS)
	$(CC) $(CFLAGS) $(OMSU_INCS) -c -o $@ $(@:%.o=%.c)

clean_omsu_all: clean_omsu_o
	rm -rf ../../lib/libomsu.*

clean_omsu_o:
	rm -rf $(OMSU_OBJS)