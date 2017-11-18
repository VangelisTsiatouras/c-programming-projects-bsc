######################################################################
OBJS       = main.o trie.o parser.o queryresults.o bloomfilter.o hashfunctions.o ngramcounter.o
SOURCE     = main.c trie.c parser.c queryresults.c bloomfilter.c hashfunctions.c ngramcounter.c
TEST_SRC   = trie.c parser.c queryresults.c bloomfilter.c hashfunctions.c ngramcounter.c
HEADER     = trie.h parser.h queryresults.h defs.h bloomfilter.h hashfunctions.h ngramcounter.h
OUT        = ngrams
U_TEST_OUT = u_test_trie
CC         = gcc
FLAGS      = -g -O2 -lm
# -g option enables debugging mode
# -O2 option enables optimize mode

all: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $(OUT)

u_test: unit_test.c
	$(CC) unit_test.c $(TEST_SRC) $(FLAGS) -o $(U_TEST_OUT) -pthread -lcheck -lsubunit -lrt -lm

# clean house
clean:
	rm -f $(OBJS) $(OUT) $(U_TEST_OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################
