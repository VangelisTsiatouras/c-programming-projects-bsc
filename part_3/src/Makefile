######################################################################
OBJS       = main.o trie/trie.o linearhash/linearhash.o parser/parser.o queryresults/queryresults.o bloomfilter/bloomfilter.o hashfunctions/hashfunctions.o ngramcounter/ngramcounter.o threadpool/threadpool.o querylist/querylist.o
SOURCE     = main.c trie/trie.c linearhash/linearhash.c parser/parser.c queryresults/queryresults.c bloomfilter/bloomfilter.c hashfunctions/hashfunctions.c ngramcounter/ngramcounter.c threadpool/threadpool.c querylist/querylist.c
TEST_SRC   = trie/trie.c linearhash/linearhash.c parser/parser.c queryresults/queryresults.c bloomfilter/bloomfilter.c hashfunctions/hashfunctions.c ngramcounter/ngramcounter.c threadpool/threadpool.c querylist/querylist.c
HEADER     = trie/trie.h linearhash/linearhash.h parser/parser.h queryresults/queryresults.h definitions.h bloomfilter/bloomfilter.h hashfunctions/hashfunctions.h ngramcounter/ngramcounter.h threadpool/threadpool.h querylist/querylist.h

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
