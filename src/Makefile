#libdragon path setup
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
MKSPRITEPATH= $(ROOTDIR)/bin/mksprite
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header
LINK_FLAGS = -L../libs/libdragon -ldragon -ldragonsys -L$(ROOTDIR)/mips64-elf/lib -lc -lm -Tn64.ld
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I../libs/libdragon/include -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy
ifeq ($(N64_BYTE_SWAP),true)
ROM_EXTENSION = .v64
N64_FLAGS = -b -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
else
ROM_EXTENSION = .z64
N64_FLAGS = -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
endif
DFS_DIR = res

OBJS = $(PROG_NAME).o screens/main_screen.o screens/main_menu_screen.o screens/infinite_menu_screen.o screens/minigame_detail_screen.o \
		screens/minigame_play_screen.o screens/no_save_screen.o screens/menu_screen.o utils/mem_pool.o utils/util_defs.o minigames/minigame_flyingbats.o \
		utils/audio.o

# audio
SOX = sox
SOXFLAGS = -b 16 -e signed-integer -B -r 44100
OGG_DIR = raw/sfx
OGG_FILES := $(wildcard $(OGG_DIR)/*.ogg)
PCM_DIR = raw/sfx
PCM_FILES := $(OGG_FILES:.ogg=.raw)

PROG_NAME = mielke_party
PROG_TAG = "MielkeParty"
ROM_DEPLOY_PATH = "./../"

# audio build
$(PCM_DIR)/%.raw: $(OGG_DIR)/%.ogg
#	mkdir -p $(PCM_DIR)
	@command -v $(SOX) >/dev/null 2>&1 || { \
	    echo >&2 'This Makefile requires the `sox` command.'; \
	    echo >&2 'Get it from http://sox.sourceforge.net/sox.html'; \
	    exit 1; \
	}
	$(SOX) $< $(SOXFLAGS) $@ remix -

$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $(PROG_NAME).elf $^ $(LINK_FLAGS)

$(PROG_NAME)$(ROM_EXTENSION): $(PROG_NAME).elf packed_files.dfs
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME)$(ROM_EXTENSION)
	$(N64TOOL) $(N64_FLAGS) -t $(PROG_TAG) $(PROG_NAME).bin -s 1M packed_files.dfs
	$(CHKSUM64PATH) $(PROG_NAME)$(ROM_EXTENSION)


#$(MKSPRITEPATH) [bits] [columns] [rows] [source.png] [destination.sprite]
packed_files.dfs: $(PCM_FILES)
	mkdir -p res
	mv $(OGG_DIR)/*.raw ./res/
	$(MKSPRITEPATH) 16 7 1 ./raw/flying_bat.png ./res/flying_bat.sprite
	$(MKSPRITEPATH) 16 9 1 ./raw/interface.png ./res/interface.sprite
	$(MKSPRITEPATH) 16 12 1 ./raw/minigame_detail.png ./res/minigame_detail.sprite
	$(MKDFSPATH) $@ ./res/

all: $(PROG_NAME)$(ROM_EXTENSION)
	mv $(PROG_NAME).z64 $(ROM_DEPLOY_PATH)

clean:
	find . -name '*.v64' -delete
	find . -name '*.z64' -delete
	find . -name '*.elf' -delete
	find . -name '*.o' -delete
	find . -name '*.bin' -delete
	find . -name '*.plan_bak*' -delete
	find . -name '*.sprite' -delete
	find . -name '*.dfs' -delete
	find . -name '*.raw' -delete
	rm -f ../*.z64 ../*.n64
