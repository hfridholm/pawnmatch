#include "../screen.h"

Mix_Chunk* SOUND_MOVE;
Mix_Chunk* SOUND_CHECK;

Mix_Music* MUSIC_BACKGROUND;


bool chunk_load(Mix_Chunk** chunk, const char filePath[])
{
  if((*chunk = Mix_LoadWAV(filePath)) == NULL)
  {
    fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());

    return false;
  }
  return true;
}

bool music_load(Mix_Music** music, const char filePath[])
{
  if((*music = Mix_LoadMUS(filePath)) == NULL)
  {
    fprintf(stderr, "Mix_LoadMUS: %s\n", Mix_GetError());

    return false;
  }
  return true;
}

bool music_play()
{
// Mix_PlayingMusic()
// Mix_PlayMusic()
  return false;
}

bool music_pause()
{
// Mix_PausedMusic()
// Mix_PauseMusic()
  return false;
}

bool music_resume()
{
// Mix_ResumeMusic()
  return false;
}

bool chunk_play(Mix_Chunk* chunk)
{
  if(Mix_PlayChannel(-1, chunk, 1) == -1)
  {
    fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());

    return false;
  }
  return true;
}

void volume_set(int procent)
{
  float decimal = (procent / 100);
  int volume = (int) ((float) MIX_MAX_VOLUME * decimal);

  Mix_Volume(-1, volume);
}

bool sounds_load()
{
  if(!chunk_load(&SOUND_MOVE, "../source/screen/sounds/sound-move.wav"));

  if(!chunk_load(&SOUND_CHECK, "../source/screen/sounds/sound-check.wav"));

  if(!music_load(&MUSIC_BACKGROUND, "../source/screen/sounds/music-background.mp3"));

  return true;
}

void sounds_free()
{
  Mix_FreeChunk(SOUND_MOVE);

  Mix_FreeChunk(SOUND_CHECK);

  Mix_FreeMusic(MUSIC_BACKGROUND);
}
