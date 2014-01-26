
# Generate doxygen documentation.
build:
	doxygen doxygen.conf
	cd doxygen/latex && make

# Delete generated doxygen files.
clean:
	-rm -rf doxygen

.PHONY: all depend clean build tags fulltags print_include_dirs

