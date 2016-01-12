SRC_DIR = src
BUILD_DIR = build
TARGET_DIR = bin
SRC_EXT = cpp
TARGET = $(TARGET_DIR)/zippie
IGNORED = zippiemodule.cpp

CC = gcc
CFLAGS += -g -Wall -std=c++11
RM = rm -rf

INC = -I $(SRC_DIR)
LIB = -lstdc++
SOURCES = $(shell find $(SRC_DIR) -type f \( -name *.$(SRC_EXT) ! -name "$(IGNORED)" \))
OBJS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SRC_EXT)=.o))

$(TARGET): $(OBJS)
	@echo " Linking..."
	@mkdir -p $(TARGET_DIR)
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(@D)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) $(BUILD_DIR) $(TARGET_DIR)"; $(RM) $(BUILD_DIR) $(TARGET_DIR)
