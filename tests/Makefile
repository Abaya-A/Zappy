TEST_DIR        := tests
TEST_BUILD_DIR  := build/tests

TEST_SRC        := $(wildcard $(TEST_DIR)/gui/test_*.cpp)
TEST_BINS       := $(patsubst $(TEST_DIR)/gui/%.cpp,$(TEST_BUILD_DIR)/%,$(TEST_SRC))

TEST_COMMON_SRC := $(GUI_DIR)/src/ProtocolCommand.cpp \
                   $(GUI_DIR)/src/ProtocolParser.cpp \
                   $(GUI_DIR)/src/Resource.cpp

.PHONY: test tests_run clean_tests fclean_tests

test: $(TEST_BINS)
	@for test_bin in $(TEST_BINS); do \
		echo "Running $$test_bin"; \
		./$$test_bin || exit $$?; \
	done

tests_run: test

$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%: $(TEST_DIR)/gui/%.cpp $(TEST_COMMON_SRC) | $(TEST_BUILD_DIR)
	$(GUI_CXX) $(GUI_CXXFLAGS) -o $@ $< $(TEST_COMMON_SRC)

clean_tests:
	rm -rf $(TEST_BUILD_DIR)

fclean_tests: clean_tests