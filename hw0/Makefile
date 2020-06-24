# Copyright ©2020 Travis McGaha.  All rights reserved.  Permission is
# hereby granted to students registered for University of Washington
# CSE 333 for use solely during Summer Quarter 2020 for purposes of
# the course.  No other use, copying, distribution, or modification
# is permitted without prior written consent. Copyrights for
# third-party components of this work must be honored.  Instructors
# interested in reusing these course materials should contact the
# author.

CC = gcc
CFLAGS += -g -Wall -O0
OBJS = hello_world.o

# Compile everything; this is the default rule that fires if a user
# just types "make" in the same directory as this Makefile.
all: hello_world

hello_world: $(OBJS)
	$(CC) $(CFLAGS) -o hello_world $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -std=c11 $<

clean:
	/bin/rm -f *.o *~ hello_world
