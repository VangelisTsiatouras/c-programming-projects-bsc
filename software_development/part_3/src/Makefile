######################################################################
OBJS       = main.o trie/trie.o linearhash/linearhash.o parser/parser.o queryresults/queryresults.o oldqueryresults/oldqueryresults.o bloomfilter/bloomfilter.o hashfunctions/hashfunctions.o ngramcounter/ngramcounter.o threadpool/threadpool.o querylist/querylist.o bloomfilterstorage/bfstorage.o
SOURCE     = main.c trie/trie.c linearhash/linearhash.c parser/parser.c queryresults/queryresults.c oldqueryresults/oldqueryresults.c bloomfilter/bloomfilter.c hashfunctions/hashfunctions.c ngramcounter/ngramcounter.c threadpool/threadpool.c querylist/querylist.c bloomfilterstorage/bfstorage.c
TEST_SRC   = trie/trie.c linearhash/linearhash.c parser/parser.c queryresults/queryresults.c oldqueryresults/oldqueryresults.c bloomfilter/bloomfilter.c hashfunctions/hashfunctions.c ngramcounter/ngramcounter.c threadpool/threadpool.c querylist/querylist.c bloomfilterstorage/bfstorage.c
HEADER     = trie/trie.h linearhash/linearhash.h parser/parser.h queryresults/queryresults.h oldqueryresults/oldqueryresults.h definitions.h bloomfilter/bloomfilter.h hashfunctions/hashfunctions.h ngramcounter/ngramcounter.h threadpool/threadpool.h querylist/querylist.h bloomfilterstorage/bfstorage.h

OUT        = ngrams
U_TEST_OUT = u_test_ngrams

CC         = gcc
FLAGS      = -pthread -lm -Wall

all: FLAGS += -O2
all: executable

debug: FLAGS += -g -O0
debug: executable

executable: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $(OUT)

%.o: %.c
	$(CC) -c $(FLAGS) -o $@ $<

u_test: FLAGS += -lcheck -lsubunit -lrt
u_test: unit_test.c
	$(CC) unit_test.c $(TEST_SRC) $(FLAGS) -o $(U_TEST_OUT)

clean:
	rm -rf $(OBJS) $(OUT) $(U_TEST_OUT)

count:
	wc $(SOURCE) $(HEADER)
#########################################################################
