CC := g++
RM := rm -f

SRC := util/ntuple-skimmer.cc
TARGET := skimmer

CFLAGS := `root-config --cflags`
LIBS := `root-config --libs`
MISCFLAGS := -std=c++14 -Wall -pedantic-errors -fdiagnostics-color=always

OBJ := $(SRC:.cc=.o)

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(MISCFLAGS) $(LIBS) -o $@ $^

%.o: %.cc
	$(CC) $(MISCFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
