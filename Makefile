#############################################################################
# Makefile for building MSI Keyboard Light Manager (MSIKLM)
#############################################################################

####### Compiler, tools and options
TARGET        = msiklm
CC            = gcc
CFLAGS        = -m64 -pipe -O3 -Wall -W -D_REENTRANT
LFLAGS        = -m64 -Wl,-O3
LIBS          = -lhidapi-libusb
DEL_FILE      = rm -f
INSTALLPREFIX = /usr/local/bin

####### Files
INC_DIR       = src
INC_FILE      = msiklm.h

SRC_DIR       = src
SRC_FILE      = main.c \
                msiklm.c

OBJ_DIR       = .obj
OBJ_FILE      = $(SRC_FILE:.c=.o)

CRT_DIR       = .

SRC           = $(addprefix $(SRC_DIR)/,$(SRC_FILE))
INC           = $(addprefix $(INC_DIR)/,$(INC_FILE))
OBJ           = $(addprefix $(OBJ_DIR)/,$(OBJ_FILE))
CRT           = $(addprefix $(OBJ_DIR)/,$(CRT_DIR))

####### Build rules

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC) Makefile
	@mkdir -p $(CRT) 2> /dev/null || true
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

clean:
	$(DEL_FILE) $(OBJ)
	$(DEL_FILE) -r $(OBJ_DIR)

delete: clean
	$(DEL_FILE) $(TARGET)

install: all
	@cp -v $(TARGET) $(INSTALLPREFIX)/$(TARGET)
	@chmod 755 $(INSTALLPREFIX)/$(TARGET)

re: delete all

.PHONY: all clean delete re
