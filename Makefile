# Define WERROR=0 to disable -Werror

CC := $(CROSS_COMPILE)gcc
CFLAGS :=
LD := $(CROSS_COMPILE)ld
LDFLAGS :=
OBJCOPY := $(CROSS_COMPILE)objcopy
