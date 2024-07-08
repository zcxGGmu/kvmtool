# Define WERROR=0 to disable -Werror

ifeq ($(strip $(V)),)
	ifeq ($(findstring s, $(filter-out --%, $(firstword $(MAKEFLAGS)))),)
		E = @echo
	else
		E = @\#
	endif
	Q = @
else
	E = @\#
	Q =
endif
export E Q

CC := $(CROSS_COMPILE)gcc
CFLAGS :=
LD := $(CROSS_COMPILE)ld
LDFLAGS :=
OBJCOPY := $(CROSS_COMPILE)objcopy

# Program settings
prefix = $(HOME)
bindir_relative = bin
bindir = $(prefix)/$(bindir_relative)

DESTDIR_SQ = $(subst ','\'',$(DESTDIR))
bindir_SQ = $(subst ','\'',$(bindir))

PROGRAM := rvaf
PROGRAM_ALIAS := vm

OBJS += main.o
OBJS += kvm.o
OBJS += kvm-cpu.o
OBJS += mmio.o
OBJS += epoll.o
OBJS += util/read_write.o
OBJS += util/strbuf.o
OBJS += util/threadpool.o
OBJS += util/util.o
OBJS += builtin/rvaf_run.o
OBJS += builtin/rvaf_list.o
OBJS += builtin/rvaf_help.o

# Translate "uname -m" into ARCH string
ARCH ?= $(shell uname -m | sed \
		-e s/riscv64/riscv/ \
		-e s/riscv32/riscv)

# Arch-specific stuff
### RISC-V (RV32 and RV64)
ifeq ($(ARCH),riscv)
	ARCH_INCLUDE 	:= arch/riscv/include
	OBJS	+= hw/serial.o
	OBJS	+= arch/riscv/kvm.o
	OBJS	+= arch/riscv/kvm_cpu.o
	ifeq ($(RISCV_XLEN), 32)
		CFLAGS += -mabi=ilp32d -march=rv32gc
		GUEST_INIT_FLAGS += -mabi=ilp32d -march=rv32gc
	endif
	ifeq ($(RISCV_XLEN), 64)
		CFLAGS += -mabi=lp64d -march=rv64gc
		GUEST_INIT_FLAGS += -mabi=lp64d -march=rv64gc
	endif

	ARCH_WANT_LIBFDT := y
endif
### RISC-V

# Detect optional features.
# On a given system, some libs may link statically, some may not; so, check
# both and only build those that link!

# ifeq (y,$(ARCH_WANT_LIBFDT))

LIBS 	+= -lrt
LIBS	+= -lpthread
LIBS	+= -lutil

comma = ,

# The dependency file for the current target
depfile = $(subst $(comma), _, $(dir $@).$(notdir $@).d)
DEPS	:= $(foreach obj, $(OBJS), 
		$(subst $(comma), _, $(dir $(obj)).$(notdir $(obj)).d))


DEFINES	+= -D_FILE_OFFSET_BITS=64
DEFINES	+= -D_GNU_SOURCE
DEFINES	+= -DBUILD_ARCH='"$(ARCH)"'

KVM_INCLUDE = include
CFLAGS	+= $(CPPFLAGS) $(DEFINES) -I$(KVM_INCLUDE) -I$(ARCH_INCLUDE) \
			-O2 -fno-strict-aliasing -g

WARNINGS += -Wall
WARNINGS += -Wformat=2
WARNINGS += -Winit-self
WARNINGS += -Wmissing-declarations
WARNINGS += -Wmissing-prototypes
WARNINGS += -Wnested-externs
WARNINGS += -Wno-system-headers
WARNINGS += -Wold-style-definition
WARNINGS += -Wredundant-decls
WARNINGS += -Wsign-compare
WARNINGS += -Wstrict-prototypes
WARNINGS += -Wundef
WARNINGS += -Wvolatile-register-var
WARNINGS += -Wwrite-strings
WARNINGS += -Wno-format-nonliteral

CFLAGS += $(WARNINGS)

ifneq ($(WERROR),0)
	CFLAGS += -Werror
endif

all: $(PROGRAM) $(PROGRAM_ALIAS)

#
# When building -static all objects are built with appropriate flags, which
# may differ between static & dynamic .o.  The objects are separated into
# .o and .static.o.  See the %.o: %.c rules below.
#
STATIC_OBJS = $(patsubst %.o, %.static.o, $(OBJS))

STATIC_DEPS := $(foreach obj, $(STATIC_OBJS), \
			$(subst $(comma), _, $(dir $(obj)).$(notdir $(obj)).d))

$(PROGRAM)-static: $(STATIC_OBJS)
	$(E) "	LINK	" $@
	$(Q) $(CC) -static $(CFLAGS) $(STATIC_OBJS) $(LDFLAGS) $(LIBS) -o $@

$(PROGRAM):	$(OBJS)
	$(E) "	LINK	" $@
	$(Q) $(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

$(PROGRAM_ALIAS): $(PROGRAM)
	$(E) "	LN		" $@
	$(Q) ln -f $(PROGRAM) $@

%.s: %.c
	$(Q) $(CC) -o $@ -S $(CFLAGS) -fverbose-asm $<

$(OBJS):
%.static.o: %.c
ifeq ($(C), 1)
	$(E) "	CHECK	" $@
	$(Q) $(CHECK) -static -c $(CFLAGS) $< -o $@
endif
	$(E) "	CC		" $@
	$(Q) $(CC) -static -c $(CFLAGS) $< -o $@

%.o: %.c
ifeq ($(C), 1)
	$(E) "	CHECK	" $@
	$(Q) $(CHECK) -c $(CFLAGS) $< -o $@
endif
	$(E) "	CC		" $@
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

clean:
	$(E) "	CLEAN"
	$(Q) rm -f $(DEPS) $(STATIC_DEPS) $(OBJS) $(STATIC_OBJS) \
				$(PROGRAM) $(PROGRAM_ALIAS) $(PROGRAM)-static
.PHONY: clean
