######################################################################
OBJS 	= main.o shell.o graph.o hashtable.o
SOURCE	= main.c shell.c graph.c hashtable.c
HEADER  = edge.h graph.h hashtable.h node.h pointer_to_nodes.h shell.h
OUT  	= elegxos
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################