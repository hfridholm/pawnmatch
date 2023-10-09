CLEAN_TARGET := clean

SDL_FLAGS := # `sdl2-config --libs --cflags` -lSDL2main -lSDL2 -lSDL2_image
SDL_FLAGS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

COMPILE_FLAGS := -Wall -Werror -g -Og -std=gnu99 -oFast $(SDL_FLAGS)
COMPILE_PRGRM := gcc
COMPILE_LINKS := -lm

DELETE_FLAGS :=
DELETE_COMND := rm

SOURCE_FOLDER := ../source
OBJECT_FOLDER := ../object

SOURCE_FILES := $(wildcard $(SOURCE_FOLDER)/*/*.c)
OBJECT_FILES := $(addprefix $(OBJECT_FOLDER)/, $(notdir $(SOURCE_FILES:.c=.o)))

RUNBLE_FILES := $(patsubst %.c, %, $(notdir $(wildcard $(SOURCE_FOLDER)/*.c)))

$(RUNBLE_FILES): %: $(OBJECT_FOLDER)/%.o $(OBJECT_FILES) $(SOURCE_FOLDER)/%.c $(SOURCE_FILES) $(HEADER_FILES)
	$(COMPILE_PRGRM) $(OBJECT_FOLDER)/$@.o $(OBJECT_FILES) $(COMPILE_FLAGS) $(COMPILE_LINKS) -o $@

$(OBJECT_FOLDER)/%.o: $(SOURCE_FOLDER)/*/%.c 
	$(COMPILE_PRGRM) $< -c $(COMPILE_FLAGS) -o $@

$(OBJECT_FOLDER)/%.o: $(SOURCE_FOLDER)/%.c 
	$(COMPILE_PRGRM) $< -c $(COMPILE_FLAGS) -o $@

.PRECIOUS: $(OBJECT_FILES) $(RUNBLE_FILES)

DELETE_FILES := $(OBJECT_FOLDER)/*.o $(RUNBLE_FILES)

$(CLEAN_TARGET):
	$(DELETE_COMND) $(DELETE_FLAGS) $(DELETE_FILES)
