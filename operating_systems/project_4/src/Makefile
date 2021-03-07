######################################################################
OBJS 	= main.o mydiz_routines.o
SOURCE	= main.c mydiz_routines.c
HEADER  = handler.h remydiz_routines.h definitions.h di_header.h dinodes_structure.h
OUT  	= mydiz
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