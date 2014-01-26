
# Build the source, documentation, and tests.
all: src doc test

# Build the source.
src:
	cd src && make clean depend build

# Generate the documentation.
doc:
	cd doc && make clean build

# Build the tests
test:
	cd test && make clean build

# Delete generated files.
clean:
	cd src && make -s clean
	cd doc && make -s clean
	cd test && make -s clean

.PHONY: all clean src doc test
