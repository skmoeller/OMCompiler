#Makefile for OpenModelicaSimulationUnit (omsu)

include ../../makefile

# varibales for OMSU library creation
OMSU_SRCS = omsu_helper.c

OMSU_OBJS = $(OMSU_SRCS:%.c=%.o)

OMSU_HDRS = $(OMSIC_INCLUDE)/omsu/omsu_helper.h\
 $(OMSI_INCLUDE)/omsi.h


#FMI2_HDRS=

OMSU_INCS = -I$(OMSIC_INCLUDE) -I$(OMSI_INCLUDE)

.PHONY: clean_omsu_all clean_omsu_o

libomsu: $(OMSU_OBJS)
	$(CC) $(LDFLAGS) $(OMSU_INCS) -o libomsu.$(TARGET_LIB) $^

$(OMSU_OBJS): $(OMSU_SRCS) $(OMSU_HDRS)
	$(CC) $(CFLAGS) $(OMSU_INCS) -c -o $@ $(@:%.o=%.c)

clean_omsu_all: clean_omsu_o
	rm -rf ../../lib/libomsu.*

clean_omsu_o:
	rm -rf $(OMSU_OBJS)