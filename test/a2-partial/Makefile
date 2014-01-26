include ../Makefile.common

# Pick a random port between 5000 and 7000
RANDPORT := $(shell /bin/bash -c "expr \( $$RANDOM \% 2000 \) \+ 5000")

# The default target is to build the test.
build: main

# Build the test.
main: main.c $(SRCDIR)/$(CLIENTLIB) -lcheck -lcrypt
	$(CC) $(CFLAGS) -I $(SRCDIR) $^ -o $@ 

# Run the test.
run: init storage.h main
	for conf in `ls *.conf`; do sed -i -e "1,/server_port/s/server_port.*/server_port $(RANDPORT)/" "$$conf"; done
	env CK_VERBOSITY=verbose ./main $(RANDPORT)

# Make storage.h available in the current directory.
storage.h:
	ln -s $(SRCDIR)/storage.h

# Clean up
clean:
	-rm -rf main *.out *.serverout *.log ./storage.h ./$(SERVEREXEC)  

.PHONY: run

