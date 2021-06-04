#libdragon path setup
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header
LINK_FLAGS = -L$(ROOTDIR)/mips64-elf/lib -ldragon -lc -lm -ldragonsys -Tn64.ld
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

OBJS = $(PROG_NAME).o screens/main_screen.o screens/main_menu_screen.o screens/infinite_menu_screen.o screens/minigame_detail_screen.o \
		screens/minigame_play_screen.o utils/mem_pool.o minigames/minigame_flyingbats.o

ifeq ($(N64_BYTE_SWAP),true)
ROM_EXTENSION = .v64
N64_FLAGS = -b -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
else
ROM_EXTENSION = .z64
N64_FLAGS = -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
endif

#your program name
PROG_NAME = platformer
PROG_TAG = "Platformer"
ROM_DEPLOY_PATH = "./../"

$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $(PROG_NAME).elf $^ $(LINK_FLAGS)

$(PROG_NAME)$(ROM_EXTENSION): $(PROG_NAME).elf
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME)$(ROM_EXTENSION)
	$(N64TOOL) $(N64_FLAGS) -t $(PROG_TAG) $(PROG_NAME).bin
	$(CHKSUM64PATH) $(PROG_NAME)$(ROM_EXTENSION)

#pack our icons.sprite into dfs (Dragon File System)
#$(MKSPRITEPATH), 16 bits 1 column, 17 rows of sprites (17 sprites total) used
#to convert spritesheet.png into compat. Dragon format
# packed_files.dfs:
# 	$(MKSPRITEPATH) 16 1 31 ./res/spritesheet.png ./res/spritesheet.sprite 
# 	$(MKDFSPATH) packed_files.dfs ./res/

all: $(PROG_NAME)$(ROM_EXTENSION)
	@echo end
	mv $(PROG_NAME).z64 $(ROM_DEPLOY_PATH)

clean:
	find . -name '*.v64' -delete
	find . -name '*.z64' -delete
	find . -name '*.elf' -delete
	find . -name '*.o' -delete
	find . -name '*.bin' -delete
	find . -name '*.plan_bak*' -delete
	rm -f ../*.z64 ../*.n64
