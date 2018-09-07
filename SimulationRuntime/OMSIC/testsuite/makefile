#Makefile for OpenModelicaSimulationInterface tests

# varibales for OMSI tests
TEST_SRCS = test_omsu_main.c

TEST_OBJS = $(TEST_SRCS:%.c=%.o)

TEST_LIBS = -L. -lOMSIBase -lOMSU -lOMSISolver

PATH_TO_OMSI = /d/workspace/OpenModelica/OMCompiler/SimulationRuntime/OMSI/
PATH_TO_OMSIC = /d/workspace/OpenModelica/OMCompiler/SimulationRuntime/OMSIC/

TEST_INCS = -I$(PATH_TO_OMSIC)/include/fmi2 -I$(PATH_TO_OMSIC)/include/omsu \
 -I$(PATH_TO_OMSI)/include -I$(PATH_TO_OMSI)/base/include \
 -I$(PATH_TO_OMSIC)/include/solver


test: $(TEST_OBJS)

	cp -u ../../OMSI/Build_dynamic/base/libOMSIBase.dll .
	cp -u ../Build_dynamic/src/omsu/libOMSU.dll .
	cp -u ../Build_dynamic/src/solver/libOMSISolver.dll .
	$(CC) $(TEST_OBJS) -Wl,-rpath=$(RUNTIMEPATH) $(TEST_LIBS) -o test_omsu_main
	./test_omsu_main.exe

$(TEST_OBJS): $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_INCS) -c -o $@ $(@:%.o=%.c)

.PHONY: clean_test_all
clean_test_all: clean_test_o
	rm -rf $(TEST_SRCS:%.c=%.exe) $(TEST_SRCS:%.c=%)
	rm -f libOMSIBase.*

.PHONY: clean_test_o
clean_test_o:
	rm -rf $(TEST_OBJS)
