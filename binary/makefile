# This is the compiler and the compile flags you want to use 
COMPILER := gcc
COMPILE_FLAGS := -Wall -Werror -g -Og -std=gnu99 -oFast -pthread

SOURCE_FOLDER := ../source
OBJECT_FOLDER := ../object

# These are the source files, object files and linker flags for the logic files
LOGIC_FOLDER := $(SOURCE_FOLDER)/logic

LOGIC_SOURCE_FILES := $(wildcard $(LOGIC_FOLDER)/*.c)
LOGIC_OBJECT_FILES := $(addprefix $(OBJECT_FOLDER)/, $(notdir $(LOGIC_SOURCE_FILES:.c=.o)))

LOGIC_LINKER_FLAGS := -lm

# These are the source files, object files and linker flags for the engine files
ENGINE_FOLDER := $(SOURCE_FOLDER)/engine

ENGINE_SOURCE_FILES := $(wildcard $(ENGINE_FOLDER)/*.c)
ENGINE_OBJECT_FILES := $(addprefix $(OBJECT_FOLDER)/, $(notdir $(ENGINE_SOURCE_FILES:.c=.o)))

ENGINE_LINKER_FLAGS :=

# These are the source files, object files and linker flags for the debug files
DEBUG_FOLDER := $(SOURCE_FOLDER)/debug

DEBUG_SOURCE_FILES := $(wildcard $(DEBUG_FOLDER)/*.c)
DEBUG_OBJECT_FILES := $(addprefix $(OBJECT_FOLDER)/, $(notdir $(DEBUG_SOURCE_FILES:.c=.o)))

DEBUG_LINKER_FLAGS :=

# These are the source files, object files and linker flags for the screen files
SCREEN_FOLDER := $(SOURCE_FOLDER)/screen

SCREEN_SOURCE_FILES := $(wildcard $(SCREEN_FOLDER)/*.c)
SCREEN_OBJECT_FILES := $(addprefix $(OBJECT_FOLDER)/, $(notdir $(SCREEN_SOURCE_FILES:.c=.o)))

SCREEN_LINKER_FLAGS := `sdl2-config --libs --cflags` -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# These are rules for compiling the executable programs
pawnmatch-console: %: $(OBJECT_FOLDER)/%.o $(LOGIC_OBJECT_FILES) $(SOURCE_FOLDER)/%.c $(LOGIC_SOURCE_FILES)
	$(COMPILER) $(OBJECT_FOLDER)/$@.o $(LOGIC_OBJECT_FILES) $(LOGIC_LINKER_FLAGS) -o $@

pawnmatch-screen: %: $(OBJECT_FOLDER)/%.o $(LOGIC_OBJECT_FILES) $(DEBUG_OBJECT_FILES) $(ENGINE_OBJECT_FILES) $(SCREEN_OBJECT_FILES) $(SOURCE_FOLDER)/%.c $(LOGIC_SOURCE_FILES) $(DEBUG_SOURCE_FILES) $(ENGINE_SOURCE_FILES) $(SCREEN_SOURCE_FILES) 
	$(COMPILER) $(OBJECT_FOLDER)/$@.o $(LOGIC_OBJECT_FILES) $(DEBUG_OBJECT_FILES) $(ENGINE_OBJECT_FILES) $(SCREEN_OBJECT_FILES) $(LOGIC_LINKER_FLAGS) $(DEBUG_LINKER_FLAGS) $(ENGINE_LINKER_FLAGS) $(SCREEN_LINKER_FLAGS) -o $@

# These are rules for compiling object files out of source files
$(OBJECT_FOLDER)/%.o: $(SOURCE_FOLDER)/*/%.c 
	$(COMPILER) $< -c $(COMPILE_FLAGS) -o $@

$(OBJECT_FOLDER)/%.o: $(SOURCE_FOLDER)/%.c 
	$(COMPILER) $< -c $(COMPILE_FLAGS) -o $@

# These are the executable programs that you can create
PROGRAMS := $(patsubst %.c, %, $(notdir $(wildcard $(SOURCE_FOLDER)/*.c)))

# These are the files that should be kept (not automatically deleted)
.PRECIOUS: $(OBJECT_FOLDER)/*.o $(PROGRAMS)

# This is the clean target, along with which files to delete and how to delete them
CLEAN_TARGET := clean

DELETE_FLAGS :=
DELETE_COMMAND := rm

DELETE_FILES := $(OBJECT_FOLDER)/*.o $(PROGRAMS)

$(CLEAN_TARGET):
	$(DELETE_COMMAND) $(DELETE_FLAGS) $(DELETE_FILES)
