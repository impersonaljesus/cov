PROJECT_NAME = Otterly-CCD-CLI

CC = gcc
PKGCONFIG = $(shell which pkg-config)



PROJECT_SRC = src
PROJECT_OBJ = obj
PROJECT_BIN = bin
vpath %.c $(PROJECT_SRC)

#gcc -Wall -g -o mainc main_connect.c -export-dynamic `pkg-config --cflags --libs glib-2.0 gtk+-3.0`

SRCS = main.c
SRCS += uart.c
SRCS += gnuplot_i.c


CFLAGS=-g -Wall -o $(PROJECT_NAME)
WFLAGS += -Wall -Wextra -Warray-bounds -Wno-unused-parameter
GTKFLAGS=-export-dynamic `pkg-config --cflags --libs glib-2.0`
LIBS = $(shell $(PKGCONFIG))

#GTKFLAGS=-export-dynamic `pkg-config --cflags --libs glib-2.0 gtk+-3.0`
#LIBS = $(shell $(PKGCONFIG) --libs gtk+-3.0)


# top-level rule to create the program.
.PHONY: all
all: $(PROJECT_NAME)

$(PROJECT_NAME): $(SRCS)
	$(CC) $(CFLAGS) $(WFLAGS) $^ -o $@ $(GTKFLAGS)

clean:
	rm -f *.o $(PROJECT_NAME)
