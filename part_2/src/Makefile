######################################################################
OBJS       = main.o trie/trie.o linearhash/linearhash.o parser/parser.o queryresults/queryresults.o bloomfilter/bloomfilter.o hashfunctions/hashfunctions.o ngramcounter/ngramcounter.o
SOURCE     = main.c trie/trie.c linearhash/linearhash.c parser/parser.c queryresults/queryresults.c bloomfilter/bloomfilter.c hashfunctions/hashfunctions.c ngramcounter/ngramcounter.c
TEST_SRC   = trie/trie.c linearhash/linearhash.c parser/parser.c queryresults/queryresults.c bloomfilter/bloomfilter.c hashfunctions/hashfunctions.c ngramcounter/ngramcounter.c
HEADER     = trie/trie.h linearhash/linearhash.h parser/parser.h queryresults/queryresults.h defs.h bloomfilter/bloomfilter.h hashfunctions/hashfunctions.h ngramcounter/ngramcounter.h
OUT        = ngrams
U_TEST_OUT = u_test_trie
CC         = gcc
FLAGS      = -g -O3 -lm
# -g option enables debugging mode
# -O2 option enables optimize mode

all: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $(OUT)

u_test: unit_test.c
	$(CC) unit_test.c $(TEST_SRC) $(FLAGS) -o $(U_TEST_OUT) -pthread -lcheck -lsubunit -lrt -lm

# clean house
clean:
	rm -rf $(OBJS) $(OUT) $(U_TEST_OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################
