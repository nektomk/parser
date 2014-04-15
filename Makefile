TARGETS=parser
#CFLAGS=-Wall -Wextra

all: $(TARGETS)

parser: main.o tag.o str.o type.o eval.o

.PHONY: clean

clean:
		$(RM) *.o
		$(RM) $TARGETS
		$(RM) a.out
