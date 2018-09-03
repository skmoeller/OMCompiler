#Makefile for OpenModelicaSimulationUnit (omsu)

include ../../makefile

# varibales for OMSU library creation
OMSU_SRCS = omsu_helper.c omsu_utils.c omsu_initialization.c

OMSU_OBJS = $(OMSU_SRCS:%.c=%.o)

OMSU_HDRS = $(OMSIC_INCLUDE)/omsu/omsu_helper.h $(OMSIC_INCLUDE)/omsu/omsu_utils.h\
 $(OMSIC_INCLUDE)/omsu/omsu_initialization.h\
 $(OMSIC_INCLUDE)/solver/solver_lapack.h\
 $(OMSI_INCLUDE)/omsi.h\
 $(OMSI_BASE)/omsu_input_json.h $(OMSI_BASE)/omsu_input_xml.h \
 $(OMSI_BASE)/omsu_mmap.h $(OMSI_BASE)/uthash.h

OMSU_LIB_DIR = -L$(PATH_TO_OMSIC)/src -L$(PATH_TO_OMSI) -L$(PATH_TO_OMSI)/Build_dynamic/base
OMSU_LIBS = -l:libomsuSolver.$(TARGET_LIB) -lOMSIBase

#FMI2_HDRS=

OMSU_INCS = -I$(OMSIC_INCLUDE)/omsu -I$(OMSIC_INCLUDE)/solver \
 -I$(OMSI_INCLUDE) -I$(OMSI_BASE)

.PHONY: clean_omsu_all clean_omsu_o

libomsu: $(OMSU_OBJS)
	$(CC) $(LDFLAGS) $(OMSU_INCS) $^ $(OMSU_LIB_DIR) $(SOLVER_LIB_DIR) $(OMSU_LIBS) $(SOLVER_LIBS) -o ../libomsu.$(TARGET_LIB)

$(OMSU_OBJS): $(OMSU_SRCS) $(OMSU_HDRS)
	$(CC) $(CFLAGS) $(OMSU_INCS) -c -o $@ $(@:%.o=%.c)

clean_omsu_all: clean_omsu_o
	rm -rf ../../lib/libomsu.*

clean_omsu_o:
	rm -rf $(OMSU_OBJS)