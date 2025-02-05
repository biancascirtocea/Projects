# Scirtocea Bianca 331CB

CXX = g++
CXXFLAGS = -Wall -Wextra -g
LEX = flex
LDFLAGS = -lfl

TARGET = grammar_parser
SRCS = grammar.l

all: $(TARGET)

$(TARGET): lex.yy.c
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

lex.yy.c: $(SRCS)
	$(LEX) $<

clean:
	rm -f $(TARGET) lex.yy.c

.PHONY: all clean