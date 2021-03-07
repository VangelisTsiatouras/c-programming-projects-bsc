######################################################################
OBJS 	= main.o handler.o master.o worker.o
SOURCE	= main.c handler.c master.c worker.c
HEADER  = handler.h master.h point.c worker.h
OUT  	= shapes
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	cd ../Circle && make
	cd ../Ellipse && make
	cd ../Ring && make
	cd ../Semicircle && make
	cd ../Square && make
	$(CC) -g $(OBJS) -o $(OUT)

# clean house
clean:
	cd ../Circle && make clean
	cd ../Ellipse && make clean
	cd ../Ring && make clean
	cd ../Semicircle && make clean
	cd ../Square && make clean
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	cd ../Circle && make count
	cd ../Ellipse && make count
	cd ../Ring && make count
	cd ../Semicircle && make count
	cd ../Square && make count
	wc $(SOURCE) $(HEADER)
#########################################################################