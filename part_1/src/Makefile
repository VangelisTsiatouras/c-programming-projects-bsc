######################################################################
OBJS 	= main.o trie.o parser.o queryresults.o
SOURCE	= main.o trie.o parser.o queryresults.o
HEADER  = trie.o parser.o queryresults.o
OUT  	= trie
CC	= gcc
FLAGS   = -g -O2
# -g option enables debugging mode 
# -O2 option enables optimize mode

all: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $(OUT)

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################
