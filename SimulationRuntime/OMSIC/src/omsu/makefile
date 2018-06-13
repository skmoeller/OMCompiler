#Makefile for OpenModelicaSimulationUnit (omsu)

include ../../makefile

# varibales for OMSU library creation
OMSU_SRCS = omsu_ContinuousSimulation.c omsu_EventSimulation.c omsu_GettersAndSetters.c\
 omsu_Initialization.c omsu_me.c omsu_utils.c omsu_common.c omsu_helper.c omsu_input_xml.c\
  ../fmi2/osi_fmi2_wrapper.c

OMSU_OBJS = $(OMSU_SRCS:%.c=%.o)

OMSU_HDRS = $(OMSIC_INCLUDE)/omsu/omsu_ContinuousSimulation.h $(OMSIC_INCLUDE)/omsu/omsu_EventSimulation.h\
 $(OMSIC_INCLUDE)/omsu/omsu_GettersAndSetters.h $(OMSIC_INCLUDE)/omsu/omsu_Initialization.h\
 $(OMSIC_INCLUDE)/omsu/omsu_me.h $(OMSIC_INCLUDE)/omsu/omsu_utils.h\
 $(OMSIC_INCLUDE)/omsu/omsu_common.h $(OMSIC_INCLUDE)/omsu/omsu_helper.h\
 $(OMSIC_INCLUDE)/omsu/omsu_input_xml.h\
 $(OMSIC_INCLUDE)/fmi2/osi_fmi2_wrapper.h

FMI2_HDRS=$(OMSIC_INCLUDE)/fmi2/osi_fmi2_wrapper.h $(OMSIC_INCLUDE)/fmi2/fmi2Functions.h\
 $(OMSIC_INCLUDE)/fmi2/fmi2FunctionTypes.h  $(OMSIC_INCLUDE)/fmi2/fmi2TypesPlatform.h

OMSU_INCS = -I$(OMSIC_INCLUDE) -I$(OMSI_INCLUDE) -I$(C_RUNTIME) -I$(THIRD_PARTY)/gc/include

libomsu: $(OMSU_OBJS)
	$(CC) $(LDFLAGS) $(OMSU_INCS) -o ../../testsuite/libomsu.$(TARGET_LIB) $^ -lexpat
#$(CC) $(LDFLAGS) $(OMSU_INCS) -o libomsu.$(TARGET_LIB) $^

$(OMSU_OBJS): $(OMSU_SRCS) $(OMSU_HDRS) $(FMI2_HDRS)
	$(CC) $(CFLAGS) $(OMSU_INCS) -c -o $@ $(@:%.o=%.c)


.PHONY: clean_omsu_all
clean_omsu_all: clean_omsu_o
	rm -rf ../../lib/libomsu.$(TARGET_LIB)

.PHONY: clean_omsu_o
clean_omsu_o:
	rm -rf $(OMSU_OBJS)