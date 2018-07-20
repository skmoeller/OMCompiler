#Makefile for OpenModelicaSimulationInterface tests

include ../makefile

# varibales for OMSI tests
TEST_SRCS = test_omsu_main.c

TEST_OBJS = $(TEST_SRCS:%.c=%.o)

TEST_LIBS = -L$(PATH_TO_OMSIC)/lib

TEST_INCS = -I$(OMSIC_INCLUDE)

# set FMI GUID
# if other XML or JSON are tested you have to manualy set the GUID here to
# matching GUID from modelDescription.xml
TEST_GUID = {e9e50f74-bbe4-4c28-8bd2-9894ad8c8c54}

test: test_omsu

test_omsu: test_omsu_main
ifeq ($(TARGET_OS),mingw32)
	./$^.exe SimpleModelLinear_1 $(TEST_GUID)
else ifeq ($(TARGET_OS),mingw64)
	./$^.exe SimpleModelLinear_1 $(TEST_GUID)
else
	./$^ SimpleModelLinear_1 $(TEST_GUID)
endif

test_omsu_main: test_omsu_main.c
	make -C ../src/omsu libomsu
	$(CC) -o test_omsu_main.o $(TEST_INCS) -c ./test_omsu_main.c
	$(CC) test_omsu_main.o -Wl,-rpath=$(RUNTIMEPATH),libomsu.$(TARGET_LIB) -o $@

$(TEST_OBJS): $(TEST_SRCS) $(FMI2_HDRS)
	$(CC) $(CFLAGS) $(TEST_INCS) -c -o $@ $(@:%.o=%.c)


.PHONY: clean_test_all
clean_test_all: clean_test_o
	rm -rf $(TEST_SRCS:%.c=%.exe) $(TEST_SRCS:%.c=%)

.PHONY: clean_test_o
clean_test_o:
	rm -rf $(TEST_OBJS)
