CFLAGS := -ggdb3 -O2 -Wall -Wextra -std=c11
CFLAGS += -Wmissing-prototypes -Wvla
CPPFLAGS := -D_DEFAULT_SOURCE

PROGS := pingpong primes find xargs

all: $(PROGS)

cp: cp.o
find: find.o
ls: ls.o
xargs: xargs.o
primes: primes.o
pingpong: pingpong.o
ps: ps.o

format: .clang-files .clang-format
	xargs -r clang-format -i <$<

clean:
	rm -f $(PROGS) *.o core vgcore.*

.PHONY: all clean format
