APP = program

SRCS  = src/main.c
SRCS += src/draw_line.c
SRCS += src/draw_line.asm
SRCS += src/frame_counter.c 
SRCS += src/tests.c 

################################################################################
ZCC      ?= zcc
TARGET   ?= +zx
BUILD    := build
BIN      := $(BUILD)/$(APP)
TAP      := $(BUILD)/$(APP).tap
SNA      := $(BUILD)/$(APP).sna

CFLAGS   := -vn -m --list --c-code-in-asm --lstcwd -startup=1 -SO3
LIBFLAGS := -clib=sdcc_iy

ifeq ($(OS),Windows_NT)
    RM = del /F /Q
    RMDIR = rmdir /S /Q
    MKDIR = mkdir
	# Convert forward slashes to backslashes for Windows
    FIXPATH = $(subst /,\,$1)

else
	RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
	FIXPATH = $1
endif

# Generated files to remove on make clean (must do this AFTER adding .png->.asm to SRCS)
GENERATED_FILES = $(SRCS:%=%.lis)


$(SNA): $(SRCS) | $(BUILD)	
	$(ZCC) $(TARGET) $(CFLAGS) $(LIBFLAGS) -o $(BIN) $(SRCS) -create-app -Cz"--sna" 

$(TAP): $(SRCS) | $(BUILD)	
	$(ZCC) $(TARGET) $(CFLAGS) $(LIBFLAGS) -o $(BIN) $(SRCS) -create-app -Cz"--tap" 

$(BUILD):
	@if not exist $(BUILD) $(MKDIR) $(BUILD)

clean:
	@if exist $(BUILD) $(RMDIR) $(BUILD)
	$(RM) $(call FIXPATH,$(GENERATED_FILES))


.PNONY: clean
################################################################################
