# The tests.
TESTS = a1-partial

# These generated target names prepend "build" to each test.
BUILDTESTS = $(TESTS:%=build%)

# These generated target names prepend "run" to each test.
RUNTESTS = $(TESTS:%=run%)

# These generated target names prepend "clean" to each test.
CLEANTESTS = $(TESTS:%=clean%)



# Build the tests.
build: $(BUILDTESTS)

# Run the tests.
run: $(RUNTESTS)

# Clean the tests.
clean: $(CLEANTESTS)

# Build a single test.
$(BUILDTESTS):
	cd $(@:build%=%) && make -s

# Run a single test.
$(RUNTESTS):
	@cd $(@:run%=%) && make -s run; echo

# Clean a single test.
$(CLEANTESTS):
	cd $(@:clean%=%) && make -s clean

.PHONY: run clean $(TESTS) $(BUILDTESTS) $(RUNTESTS) $(CLEANTESTS)

