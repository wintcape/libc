TARGET := c-windows
CC ?= gcc

CFLAGS := -g -O2 -W -Wvarargs -Wall -Werror -Werror=vla -Wno-unused-parameter
INCLUDE := src
DEPENDENCIES := m

TEST := test.exe
TEST_INCLUDE := test\src

POST := build\.post-windows.bat

################################################################################

OBJFILES := math.o test.o clock.o memory.o logger.o string_utils.o string.o string_format.o array_utils.o array.o queue.o hashtable.o freelist.o memory_linear_allocator.o memory_dynamic_allocator.o filesystem.o thread.o mutex.o platform.o
TEST_OBJFILES := test_main.o test_array.o test_queue.o test_hashtable.o test_string.o test_freelist.o test_memory_linear_allocator.o test_memory_dynamic_allocator.o test_filesystem.o

INCFLAGS := $(foreach x,$(INCLUDE), $(addprefix -I,$(x)))
OBJFLAGS := $(CFLAGS) -c
LDFLAGS := $(foreach x,$(DEPENDENCIES), $(addprefix -l,$(x)))
OBJ := $(foreach x,$(OBJFILES), $(addprefix obj\,$(x)))

TEST_INCFLAGS := $(INCFLAGS) $(foreach x,$(TEST_INCLUDE), $(addprefix -I,$(x)))
TEST_OBJFLAGS := $(CFLAGS) -c
TEST_LDFLAGS := -Llib -l$(TARGET) $(LDFLAGS)
TEST_OBJ := $(foreach x,$(TEST_OBJFILES), $(addprefix obj\,$(x)))

CLEAN := lib\lib$(TARGET).a $(OBJ)
TEST_CLEAN := bin\$(TEST) $(TEST_OBJ)

lib\lib$(TARGET).a: $(OBJ)
	ar rcs $@ $^

bin\$(TEST): $(TEST_OBJ)
	$(CC) $(TEST_INCFLAGS) -o $@ $^ $(CFLAGS) $(TEST_LDFLAGS)

$(OBJ):
	$(CC) $(INCFLAGS) $(OBJFLAGS) -o $@ $<
$(TEST_OBJ):
	$(CC) $(TEST_INCFLAGS) $(TEST_OBJFLAGS) -o $@ $<

# Engine objects.
obj\math.o: 							src\math\math.c
obj\test.o:								src\test\test.c
obj\clock.o: 							src\core\clock.c
obj\memory.o: 							src\core\memory.c
obj\logger.o: 							src\core\logger.c
obj\string_utils.o: 					src\core\string.c
obj\string.o: 							src\container\string.c
obj\string_format.o:					src\container\string\format.c
obj\array_utils.o: 						src\core\array.c
obj\array.o: 							src\container\array.c
obj\queue.o:							src\container\queue.c
obj\hashtable.o:						src\container\hashtable.c
obj\freelist.o: 						src\container\freelist.c
obj\memory_linear_allocator.o: 			src\memory\linear_allocator.c
obj\memory_dynamic_allocator.o: 		src\memory\dynamic_allocator.c
obj\filesystem.o:						src\platform\filesystem.c
obj\thread.o: 							src\platform\thread.c
obj\mutex.o: 							src\platform\mutex.c
obj\platform.o: 						src\platform\windows.c

# Test objects.
obj\test_main.o:						test\src\main.c
obj\test_array.o:						test\src\container\test_array.c
obj\test_queue.o:						test\src\container\test_queue.c
obj\test_hashtable.o:					test\src\container\test_hashtable.c
obj\test_string.o:						test\src\container\test_string.c
obj\test_freelist.o:					test\src\container\test_freelist.c
obj\test_memory_linear_allocator.o:		test\src\memory\test_linear_allocator.c
obj\test_memory_dynamic_allocator.o:	test\src\memory\test_dynamic_allocator.c
obj\test_filesystem.o:					test\src\platform\test_filesystem.c

.PHONY: lib
lib: mkdir clean lib\lib$(TARGET).a

.PHONY: test
test: mkdir test-clean bin\$(TEST) post run-test

.PHONY: all
all: lib test

.PHONY: mkdir
mkdir:
	@if not exist bin mkdir bin
	@if not exist lib mkdir lib
	@if not exist obj mkdir obj

.PHONY: clean
clean:
	@echo del $(CLEAN)
	@del $(CLEAN) 2>NUL

.PHONY: test-clean
test-clean:
	@echo del $(TEST_CLEAN)
	@del $(TEST_CLEAN) 2>NUL

.PHONY: run-test
run-test:
	@bin\$(TEST)

.PHONY: post
post:
	@.\$(POST)
