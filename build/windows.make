TARGET := hello.exe
CC ?= gcc

POST := build\.post-windows.bat
TEST := test.exe

CFLAGS := -g -O2 -W -Wvarargs -Wall -Werror -Werror=vla -Wno-unused-parameter
DEPS := m user32
INCLUDE := src test\src

SHARED_OBJFILES := memory.o logger.o clock.o array.o queue.o hashtable.o thread.o mutex.o string_utils.o string.o string_format.o math.o test.o memory_linear_allocator.o memory_dynamic_allocator.o freelist.o platform.o filesystem.o
TARGET_OBJFILES := main.o
TEST_OBJFILES := test_main.o test_array.o test_queue.o test_hashtable.o test_string.o test_freelist.o test_memory_linear_allocator.o test_memory_dynamic_allocator.o test_filesystem.o

################################################################################

INCFLAGS := $(foreach x,$(INCLUDE), $(addprefix -I,$(x)))
OBJFLAGS := $(CFLAGS) -c
LDFLAGS := $(foreach x,$(DEPS), $(addprefix -l,$(x)))

SHARED_OBJ := $(foreach x,$(SHARED_OBJFILES), $(addprefix obj\,$(x)))
TARGET_UNIQUE_OBJ := $(foreach x,$(TARGET_OBJFILES), $(addprefix obj\,$(x)))
TARGET_OBJ :=  $(TARGET_UNIQUE_OBJ) $(SHARED_OBJ)
TEST_UNIQUE_OBJ := $(foreach x,$(TEST_OBJFILES), $(addprefix obj\,$(x)))
TEST_OBJ :=  $(TEST_UNIQUE_OBJ) $(SHARED_OBJ)

CLEAN := bin\$(TARGET) bin\$(TEST) $(SHARED_OBJ) $(TARGET_UNIQUE_OBJ) $(TEST_UNIQUE_OBJ)

bin\$(TARGET): $(TARGET_OBJ)
	$(CC) $(INCFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)

bin\$(TEST): $(TEST_OBJ)
	$(CC) $(INCFLAGS) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	
$(TARGET_OBJ):
	$(CC) $(INCFLAGS) $(OBJFLAGS) -o $@ $<
$(TEST_OBJ):
	$(CC) $(INCFLAGS) $(OBJFLAGS) -o $@ $<

# Target objects.
obj\main.o: 							src\main.c

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

# Engine objects.
obj\memory.o: 							src\core\memory.c
obj\logger.o: 							src\core\logger.c
obj\clock.o: 							src\core\clock.c
obj\array.o: 							src\container\array.c
obj\queue.o:							src\container\queue.c
obj\hashtable.o:						src\container\hashtable.c
obj\thread.o: 							src\core\thread.c
obj\mutex.o: 							src\core\mutex.c
obj\string_utils.o: 					src\core\string.c
obj\string.o: 							src\container\string.c
obj\string_format.o:					src\container\string\format.c
obj\freelist.o: 						src\container\freelist.c
obj\math.o: 							src\math\math.c
obj\test.o:								src\test\test.c
obj\memory_linear_allocator.o: 			src\memory\linear_allocator.c
obj\memory_dynamic_allocator.o: 		src\memory\dynamic_allocator.c
obj\platform.o: 						src\platform\windows.c
obj\filesystem.o:						src\platform\filesystem.c

.PHONY: app
app: mkdir clean bin\$(TARGET) post

.PHONY: test
test: mkdir clean bin\$(TEST) bin\$(TARGET) post run

.PHONY: mkdir
mkdir:
	@if not exist bin mkdir bin
	@if not exist obj mkdir obj

.PHONY: clean
clean:
	@echo del $(CLEAN)
	@del $(CLEAN) 2>NUL

.PHONY: run-test
run-test:
	@bin\$(TEST)

.PHONY: run-app
run-app:
	@bin\$(TARGET)

.PHONY: run
run: run-test run-app

.PHONY: post
post:
	@.\$(POST)

.PHONY: all
all: app run-app
