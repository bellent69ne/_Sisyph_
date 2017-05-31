.PHONY: all

CXX=clang
NAME=sisyph
BINDIR=bin
BINARY=$(BINDIR)/$(NAME)
DEBUG=-Wall -pedantic
CXXFLAGS=-std=c++14 -I ./include/ -I /usr/include/cryptopp
CLANGLIBS=-lstdc++ -lboost_filesystem -lboost_system -lcryptopp -lpthread
OBJECTS=src/cbctwofish.o src/walker.o src/shredder.o src/main.o 


all: $(OBJECTS) $(NAME)

$(NAME): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(DEBUG) $(CLANGLIBS) $(OBJECTS) -o $(BINARY)

clean:
	rm -rf $(BINDIR)
	rm -f $(shell find . -name *.o)