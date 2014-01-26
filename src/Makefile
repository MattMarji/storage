# The storage client library.
CLIENTLIB = libstorage.a

# The programs to build.
TARGETS =  cyaccer qyaccer syaccer clexer qlexer slexer $(CLIENTLIB) server client encrypt_passwd 

# The source files.
SRCS = server.c storage.c utils.c client.c encrypt_passwd.c TreeDB.c TreeNode.c TreeEntry.c 

# Compile flags.
CFLAGS = -g -Wall
LDFLAGS = -g -Wall -lcrypt -lpthread

# Dependencies file
DEPEND_FILE = depend.mk

CC=    cc
YYFLAGS= -v  -d
YACC=bison -d
LEX=flex
INCLUDE = -I.

# ---------------------------------------------------------
# Targets to build the storage client and server.
# ---------------------------------------------------------

# Default targets.
build: $(TARGETS)

# Build Config LEX
clexer:	config_parser.l
	${LEX} config_parser.l
	${CC} ${CFLAGS} ${INCLUDE} -c lex.yy.c

# Build Querry LEX
qlexer: query.l
	${LEX} query.l
	${CC} ${CFLAGS} ${INCLUDE} -c lex.qq.c

slexer: set.l
	${LEX} set.l
	${CC} ${CFLAGS} ${INCLUDE} -c lex.ss.c

# Build Config YACC
cyaccer: config_parser.y
	${YACC} config_parser.y
	${CC} ${CFLAGS} ${INCLUDE} -c config_parser.tab.c

qyaccer: query.y
	${YACC} -p qq query.y
	${CC} ${CFLAGS} ${INCLUDE} -c query.tab.c

syaccer: set.y
	${YACC} -p ss set.y
	${CC} ${CFLAGS} ${INCLUDE} -c set.tab.c

# Build the client library.
$(CLIENTLIB): storage.o utils.o
	$(AR) rcs $@ $^

# Build the server.
server: server.o utils.o TreeDB.o TreeNode.o TreeEntry.o lex.yy.o config_parser.tab.o lex.qq.o query.tab.o lex.ss.o set.tab.o
	$(CC) $(LDFLAGS) $^ -o $@

# Build the client.
client: client.o $(CLIENTLIB)
	$(CC) $(LDFLAGS) $^ -o $@

# Build the password encryptor.
encrypt_passwd: encrypt_passwd.o utils.o
	$(CC) $(LDFLAGS) $^ -o $@

# Build the Binary Search Tree Tester


# Compile a .c source file to a .o object file.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Delete generated files.
clean:
	-rm -rf $(TARGETS) *.o tags $(DEPEND_FILE)
	rm -f p config_parser.tab.h config_parser.tab.c \
	   config_parser.tab.o lex.yy.c lex.yy.o *~ \
	rm -f p query.tab.h query.tab.c \
	   query.tab.o lex.qq.c lex.qq.o *~ \
	rm -f p set.tab.h set.tab.c \
	   set.tab.o lex.ss.c lex.ss.o *~ \

# Create dependencies file.
depend:
	$(CC) $(CFLAGS) -M $(SRCS) > $(DEPEND_FILE)

-include $(DEPEND_FILE)


# ---------------------------------------------------------
# Targets to help work with the code.
# ---------------------------------------------------------

# Create tags file to help with code navigation in some editors.
tags: *.c *.h
	ctags -R .

# Create tags file including system header files.
fulltags: *.c *.h
	ctags -R . `$(MAKE) -s print_include_dirs`

# Print the include directories for current system.
print_include_dirs:
	@gcc -x c -Wp,-v -E - < /dev/null 2>&1 |grep '^ '

.PHONY: build depend clean tags fulltags print_include_dirs



