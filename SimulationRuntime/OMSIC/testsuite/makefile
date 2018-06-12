#Makefile for OpenModelicaSimulationInterface tests

include ../makefile

# varibales for OMSI tests
TEST_SRCS = test_omsu_main.c

TEST_OBJS = $(TEST_SRCS:%.c=%.o)

TEST_LIBS = -L$(PATH_TO_OMSIC)/lib

TEST_INCS = -I$(OMSIC_INCLUDE)

test_omsu: test_omsu_main
	./$@.exe

test_omsu_main: test_omsu_main.c
	make -C ../src/omsu libomsu
	$(CC) -o test_omsu_main.o $(TEST_INCS) -c ./test_omsu_main.c
	$(CC) test_omsu_main.o -Wl,-rpath=$(RUNTIMEPATH),../lib/libomsu.$(TARGET_LIB) -o $@

$(TEST_OBJS): $(TEST_SRCS) $(FMI2_HDRS)
	$(CC) $(CFLAGS) $(TEST_INCS) -c -o $@ $(@:%.o=%.c)


.PHONY: clean_test
clean_test:
	rm -rf $(TEST_OBJS) $(TEST_SRCS:%.c=%.exe)