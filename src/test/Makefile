CC=gcc
CFLAGS=-g -Wall -I.. -I. -std=c99 -DTEST_MODE=1 -D_GNU_SOURCE
LDFLAGS=-lcmocka -lm
R2_CFLAGS=$(shell pkg-config --cflags r_core)
R2_LDFLAGS=$(shell pkg-config --libs r_core)

# Source files to test (excluding main plugin entry points)
SRC_DIR=..
SOURCES=$(SRC_DIR)/messages.c $(SRC_DIR)/tools.c $(SRC_DIR)/vdb.c $(SRC_DIR)/markdown.c $(SRC_DIR)/r2ai_http.c

# Test files
TEST_SOURCES=test_messages.c test_tools.c test_vdb.c test_markdown.c test_http.c test_main.c

# Object files
OBJECTS=$(SOURCES:.c=.o)
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)

# Test executable
TEST_BINARY=run_tests

.PHONY: all test clean install-deps

all: $(TEST_BINARY)

$(TEST_BINARY): $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(CFLAGS) $(R2_CFLAGS) -o $@ $^ $(LDFLAGS) $(R2_LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(R2_CFLAGS) -c -o $@ $<

test: $(TEST_BINARY)
	./$(TEST_BINARY)

coverage: CFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: clean $(TEST_BINARY)
	./$(TEST_BINARY)
	gcov $(SOURCES)
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_html

install-deps:
	# Install cmocka testing framework
	sudo apt-get update && sudo apt-get install -y libcmocka-dev lcov

clean:
	rm -f *.o ../*.o $(TEST_BINARY) *.gcda *.gcno *.gcov coverage.info
	rm -rf coverage_html

help:
	@echo "Available targets:"
	@echo "  all        - Build test binary"
	@echo "  test       - Run all tests"
	@echo "  coverage   - Generate coverage report"
	@echo "  clean      - Clean build artifacts"
	@echo "  install-deps - Install testing dependencies"