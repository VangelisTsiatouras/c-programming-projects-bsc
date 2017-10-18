######################################################################
OBJS 	= main.o shell.o skiplist.o hashtable.o
SOURCE	= main.c shell.c skiplist.c hashtable.c
HEADER  = record.h hashtable.h skiplist.c shell.h
OUT  	= stucs
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