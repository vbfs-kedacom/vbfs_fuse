CC ?= gcc
FORMAT_SOURCE := vbfs_format.c
DUMPFS_SOURCE := vbfs_dumpfs.c
CFLAGS := -Wall -g -D_LARGEFILE64_SOURCE
LDFLAGS := -luuid
FORMAT_OBJS = $(FORMAT_SOURCE:.c=.o)
DUMPFS_OBJS = $(DUMPFS_SOURCE:.c=.o)

vbfs_format: $(FORMAT_OBJS)
	$(CC) $(CFLAGS) -o $@ $(FORMAT_OBJS) $(LDFLAGS)

vbfs_dump: $(DUMPFS_OBJS)
	$(CC) $(CFLAGS)  -o $@ $(DUMPFS_OBJS)

all: vbfs_format vbfs_dump

clean:
	-rm -f $(FORMAT_OBJS) $(DUMPFS_OBJS)
