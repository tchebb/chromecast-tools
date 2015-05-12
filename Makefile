BINARIES = cc-mangle-bootimg
CFLAGS = -Wall -Wextra

all: $(BINARIES)

clean:
	-rm -f $(BINARIES) *.o

.PHONY: clean
