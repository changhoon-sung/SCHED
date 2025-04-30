# Compiler settings
CC = gcc
CLANG ?= clang
LLC ?= llc
STRIP ?= strip
BPFTOOL ?= bpftool

# Directories
SRCDIR = src
BUILDDIR = build
BPFDIR = $(BUILDDIR)/bpf
USERDIR = $(BUILDDIR)/user
INCLUDEDIR = include
LIBBPF_DIR = $(HOME)/libbpf/src

# Flags
CFLAGS = -g -O2 -Wall
BPFCFLAGS = -g -O2 -target bpf \
            -D__TARGET_ARCH_arm64 \
            -D__BPF_TRACING__ \
            -I$(shell dirname $(shell which clang))/../include/bpf \
            -I$(INCLUDEDIR) \
            -DDEBUG \
            -mcpu=v3 \
            -Wno-compare-distinct-pointer-types \
            -Wno-unknown-attributes
BPFCFLAGS += -Wno-unused-value \
             -Wno-pointer-sign \
             -Wno-compare-distinct-pointer-types \
             -Wno-gnu-variable-sized-type-not-at-end \
             -Wno-address-of-packed-member \
             -Wno-tautological-compare \
             -Wno-unknown-warning-option
BPFCFLAGS += -fno-stack-protector \
             -fno-jump-tables \
             -fno-unwind-tables \
             -fno-asynchronous-unwind-tables

# Include paths
INCLUDES = -I$(INCLUDEDIR)
INCLUDES += -I/usr/include/bpf
INCLUDES += -I/usr/include
INCLUDES += -I$(BUILDDIR)
INCLUDES += -I/usr/local/include
INCLUDES += -I$(LIBBPF_DIR)
SCX_INCLUDES = -I$(INCLUDEDIR)/scx

LIBRARY_PATHS = -L/usr/local/lib64 -L/usr/local/lib 
LIBRARY_PATHS = -L$(LIBBPF_DIR)

# Libraries
LIBS = $(LIBRARY_PATHS) -lbpf -lelf -lz

# Source files
BPF_SRC = $(SRCDIR)/rossched.bpf.c
USER_SRC = $(SRCDIR)/rossched_user.c

# Output files
BPF_LL = $(BPFDIR)/rossched.bpf.ll
BPF_OBJ = $(BPFDIR)/rossched.bpf.o
USER_OBJ = $(USERDIR)/rossched_user.o
TARGET = $(BUILDDIR)/rossched
SKEL_HEAD = $(BUILDDIR)/rossched.skel.h

# Default target
.PHONY: all
all: $(TARGET)

# Create build directories
$(BUILDDIR) $(BPFDIR) $(USERDIR):
	mkdir -p $@

# Generate BPF skeleton
$(SKEL_HEAD): $(BPF_OBJ) | $(BUILDDIR)
	$(BPFTOOL) gen skeleton $< > $@

# Compile BPF program
$(BPF_LL): $(BPF_SRC) | $(BPFDIR)
	$(CLANG) $(BPFCFLAGS) $(INCLUDES) $(SCX_INCLUDES) -c -emit-llvm -o $@ $<

$(BPF_OBJ): $(BPF_LL)
	$(LLC) -march=bpf -filetype=obj -o $@ $<

# Compile user space program
$(USER_OBJ): $(USER_SRC) $(SKEL_HEAD) | $(USERDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(SCX_INCLUDES) -c $< -o $@

# Link final executable
$(TARGET): $(BPF_OBJ) $(USER_OBJ) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(USER_OBJ) $(LIBS) -Wl,-rpath,/usr/local/lib64:/usr/local/lib -o $@

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
	rm -f *.o
	rm -f *.skel.h

# Debug target
.PHONY: debug
debug:
	@echo "BPF source: $(BPF_SRC)"
	@echo "User source: $(USER_SRC)"
	@echo "BPF object: $(BPF_OBJ)"
	@echo "User object: $(USER_OBJ)"
	@echo "Skeleton header: $(SKEL_HEAD)"
	@echo "CLANG version: $(shell $(CLANG) --version)"
	@echo "BPFCFLAGS: $(BPFCFLAGS)"
	@echo "INCLUDES: $(INCLUDES)"
