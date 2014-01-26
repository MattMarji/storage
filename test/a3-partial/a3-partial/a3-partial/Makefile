include ../Makefile.common

# Update compile flags
CFLAGS += -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

# Directory where generated keys are stored in.
KEYSDIR = keys

# Pick a random port between 5000 and 7000
RANDPORT := $(shell /bin/bash -c "expr \( $$RANDOM \% 2000 \) \+ 5000")

# The default target is to build the test.
build: main

# Create the stub query function if there isn't one already.
querystub.c: $(SRCDIR)/$(CLIENTLIB)
	make createquerystub

createquerystub:
ifeq ($(shell nm $(SRCDIR)/$(CLIENTLIB) |grep -w storage_query),)
	echo "int storage_query(const char *a, const char *b, char **c, const int d, void *e) { return -999; }" > querystub.c
else
	echo "" > querystub.c
endif

# Build the test.
main: main.c $(SRCDIR)/$(CLIENTLIB) -lcheck -lcrypt -lcrypto -lglib-2.0 querystub.c -lm
	$(CC) $(CFLAGS) -I $(SRCDIR) $^ -o $@

# Run the test.
run: init storage.h main
	-rm -rf ./mydata
	for conf in `ls *.conf`; do sed -i -e "1,/server_port/s/server_port.*/server_port $(RANDPORT)/" "$$conf"; done
	env CK_VERBOSITY=verbose ./main $(RANDPORT)

# Make storage.h available in the current directory.
storage.h:
	ln -s $(SRCDIR)/storage.h

# Creates a new pair of public/private keys and stores them in keys/
createkeys:
	mkdir -p $(KEYSDIR)
	openssl genrsa -out $(KEYSDIR)/private.pem 1024
	openssl rsa -in $(KEYSDIR)/private.pem \
	-out $(KEYSDIR)/public.pem -outform PEM -pubout

# Clean up
clean:
	-rm -rf $(KEYSDIR) main *.out *.serverout *.log ./storage.h ./$(SERVEREXEC) ./mydata querystub.c

.PHONY: run createquerystub createkeys

