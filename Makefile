#############################################################################
# Makefile for building MSI Keyboard Light Manager (MSIKLM)
#############################################################################

####### Compiler, tools and options

CC            = gcc
FLAGS         = -m64 -pipe -O3 -Wall -W -D_REENTRANT
LINK          = gcc
LFLAGS        = -m64 -Wl,-O3
LIBS          = -lhidapi-libusb
DEL_FILE      = rm -f


####### Files

SRCPATH       = ./src/
HEADERS       = $(SRCPATH)msiklm.h
SOURCES       = $(SRCPATH)main.c $(SRCPATH)msiklm.c
OBJECTS       = main.o msiklm.o
TARGET        = msiklm
INSTALLTARGET = /usr/local/bin/$(TARGET)


####### Build rules

.PHONY: clean delete install

all: Makefile $(TARGET)

$(TARGET): $(HEADERS) $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	-$(DEL_FILE) $(OBJECTS)

delete: clean
	-$(DEL_FILE) $(TARGET)

install:
	@if [ -f $(TARGET) ]; then\
	    sudo mv -v $(TARGET) $(INSTALLTARGET);\
	    sudo chmod 744 $(INSTALLTARGET);\
	fi

####### Compile

main.o: $(SRCPATH)main.c
	$(CC) -c $(FLAGS) -o main.o $(SRCPATH)main.c

msiklm.o: $(SRCPATH)msiklm.h $(SRCPATH)msiklm.c
	$(CC) -c $(FLAGS) -o msiklm.o $(SRCPATH)msiklm.c
