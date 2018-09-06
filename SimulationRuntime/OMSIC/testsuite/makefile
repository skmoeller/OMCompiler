#Makefile for OpenModelicaSimulationInterface tests

include ../makefile

# varibales for OMSI tests
TEST_SRCS = test_omsu_main.c

TEST_OBJS = $(TEST_SRCS:%.c=%.o)

TEST_LIBS = -L. -lOMSIBase -lomsu

TEST_INCS = -I$(OMSIC_INCLUDE)/fmi2 -I$(OMSIC_INCLUDE)/omsu\
 -I$(OMSI_INCLUDE) -I$(PATH_TO_OMSI)/base/include\
 -I$(OMSIC_INCLUDE)/solver

ifeq ($(TARGET_OS),mingw32)
	MAKE_RULE = -f Makefile.omdev.mingw OMSIBase -j
	TEST_FUNCTION_CALL = ./test_omsu_main.exe
else ifeq ($(TARGET_OS),mingw64)
	MAKE_RULE = -f Makefile.omdev.mingw OMSIBase -j
	TEST_FUNCTION_CALL = ./test_omsu_main.exe
else
	MAKE_RULE = OMSIBase -j
	TEST_FUNCTION_CALL = ./test_omsu_main
endif

test: $(TEST_OBJS)
	make -C ../../OMSI $(MAKE_RULE)
	make -C ../src/omsu/ libomsu
	cp -u ../../OMSI/Build_dynamic/base/libOMSIBase.$(TARGET_LIB) .
	cp -u ../src/libomsu.$(TARGET_LIB) .
	$(CC) $(TEST_OBJS) -Wl,-rpath=$(RUNTIMEPATH) $(TEST_LIBS) -o test_omsu_main
	$(TEST_FUNCTION_CALL)

$(TEST_OBJS): $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_INCS) -c -o $@ $(@:%.o=%.c)

.PHONY: clean_test_all
clean_test_all: clean_test_o
	rm -rf $(TEST_SRCS:%.c=%.exe) $(TEST_SRCS:%.c=%)
	rm -f libOMSIBase.*

.PHONY: clean_test_o
clean_test_o:
	rm -rf $(TEST_OBJS)
