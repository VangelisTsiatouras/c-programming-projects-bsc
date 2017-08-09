######################################################################
OBJS 	= main.o hashtable.o master_thread.o threadpool.o worker_thread.o
SOURCE	= main.c hashtable.c master_thread.c threadpool.c worker_thread.c
HEADER  = declare_globals.h hashtable.h master_thread.h threadpool.h worker_thread.h
OUT  	= bankserver
CC	= gcc
FLAGS   = -g -c
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) -lpthread

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################