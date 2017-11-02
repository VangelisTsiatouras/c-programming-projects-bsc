######################################################################
OBJS       = main.o trie.o parser.o queryresults.o
SOURCE     = main.c trie.c parser.c queryresults.c
HEADER     = trie.h parser.h queryresults.h defs.h
OUT        = trie
U_TEST_OUT = u_test_trie
CC         = gcc
FLAGS      = -g -O2
# -g option enables debugging mode 
# -O2 option enables optimize mode

all: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $(OUT)

u_test: unit_test.c
	$(CC) unit_test.c trie.c parser.c queryresults.c $(FLAGS) -o $(U_TEST_OUT) -pthread -lcheck -lsubunit -lrt -lm 

# clean house
clean:
	rm -f $(OBJS) $(OUT) $(U_TEST_OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################
