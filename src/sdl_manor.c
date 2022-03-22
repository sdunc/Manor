/* 
   2D Story Game Engine
   Stephen Duncanson
   Fall 2021 -
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {false, true} bool;

typedef struct sdl_offscreen_buffer {
  /* NOTE(stephen): pixels are 32-bits wide: BB GG RR XX */
  SDL_Texture* Texture;
  void* Memory;
  int Width;
  int Height;
  int Pitch;
} sdl_offscreen_buffer;

typedef struct sdl_window_dimension {
  int Width;
  int Height;
} sdl_window_dimension;

static sdl_offscreen_buffer globalBackbuffer;

static int x_move_amount = 1;
static int y_move_amount = 1;

sdl_window_dimension SDLGetWindowDimension(SDL_Window* Window) {
  sdl_window_dimension Result;
  SDL_GetWindowSize(Window, &Result.Width, &Result.Height);
  return Result;
}

static void SDLAudioCallback(void* UserData, uint8_t* AudioData, int Length) {
  /* Clear audio buffer to silence called by SDL when more audio data
     is needed, could be put on a different thread from the main game

  */
  memset(AudioData, 0, Length);
}

static void RenderGradient(sdl_offscreen_buffer Buffer, int BlueOffset, int GreenOffset) {
  uint8_t* Row = (uint8_t* )Buffer.Memory;
  
  for (int y=0; y<Buffer.Height; y++) {
    uint32_t* Pixel = (uint32_t *)Row;

    for (int x=0; x<Buffer.Width; x++) {
      uint8_t Blue = (x + BlueOffset);
      uint8_t Green = (y + GreenOffset);
      *Pixel++ = ((Green << 8) | Blue);
    }
    Row+=Buffer.Pitch;
  }
}

static void SDLResizeTexture(struct sdl_offscreen_buffer* Buffer,
			     SDL_Renderer* Renderer, int Width, int Height) {
  const int BytesPerPixel = 4;
  /* pass sdl_offscreen_buffer as a pointer here since we change it. */
  if (Buffer->Memory) { free(Buffer->Memory); }
  if (Buffer->Texture) { SDL_DestroyTexture(Buffer->Texture); }
     
  Buffer->Texture = SDL_CreateTexture(Renderer, 
			      SDL_PIXELFORMAT_ARGB8888,
			      SDL_TEXTUREACCESS_STREAMING,
			      Width, Height);
  Buffer->Width  = Width;
  Buffer->Height = Height;
  Buffer->Pitch = Width * BytesPerPixel;
  Buffer->Memory = malloc(Width * Height * BytesPerPixel);
}

void SDLUpdateWindow(SDL_Window* Window, SDL_Renderer* Renderer, struct sdl_offscreen_buffer Buffer) {
  SDL_UpdateTexture(Buffer.Texture, 0, Buffer.Memory, Buffer.Pitch);
  SDL_RenderCopy(Renderer, Buffer.Texture, 0, 0);
  SDL_RenderPresent(Renderer);
}

int HandleEvent(SDL_Event* Event) {
  bool ShouldQuit = false;
  
  switch(Event->type) {
    
  case SDL_QUIT: {
    printf("SDL_QUIT\n");
    ShouldQuit = true;
  } break;
    
  case SDL_KEYDOWN:
  case SDL_KEYUP: {
    SDL_Keycode KeyCode = Event->key.keysym.sym;
    if ( KeyCode == SDLK_w ) {
      printf("w pressed\n");
      y_move_amount++;
    }
    else if ( KeyCode == SDLK_a ) {
      printf("a pressed\n");
      x_move_amount--;
    }
    else if ( KeyCode == SDLK_s ) {
      printf("s pressed\n");
      y_move_amount--;
    }
    else if ( KeyCode == SDLK_d ) {
      printf("d pressed\n");
      x_move_amount++;
    }
  } break;
    
  case SDL_WINDOWEVENT: {
    switch(Event->window.event) {
	       
    case SDL_WINDOWEVENT_SIZE_CHANGED: {
      SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
      SDL_Renderer* Renderer = SDL_GetRenderer(Window);
      printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d,%d)\n",Event->window.data1, Event->window.data2);
      /* SDLResizeTexture(&globalBackbuffer, Renderer,Event->window.data1, Event->window.data2); */
    } break;
      
    case SDL_WINDOWEVENT_EXPOSED: {
      SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
      SDL_Renderer* Renderer = SDL_GetRenderer(Window);
      SDLUpdateWindow(Window, Renderer, globalBackbuffer);
    } break;

    case SDL_WINDOWEVENT_FOCUS_GAINED: {
      printf("SDL_WINDOWEVENT_FOCUS_GAINED\n");
    } break;

    default: {
      printf("default: window event not handled\n"); } break;
    }
  } break;
    
  default: {
    printf("default: event not handled\n");
  } break;
  }
  return ShouldQuit;
}

static void SDLInitAudio(unsigned int SamplesPerSecond, int BufferSize) {
  SDL_AudioSpec AudioSettings = {0};
  
  AudioSettings.freq = SamplesPerSecond;
  AudioSettings.format = AUDIO_S16LSB;
  AudioSettings.channels = 2;
  AudioSettings.samples = BufferSize;
  AudioSettings.callback = &SDLAudioCallback;
  
  SDL_OpenAudio(&AudioSettings, 0);
  printf("Initialised an Audio devide at freq=%d Hz, %d Channels\n",
	 AudioSettings.freq, AudioSettings.channels);
  
  if (AudioSettings.format != AUDIO_S16LSB) {
    printf("cannot get a S16LSB buffer!\n");
    SDL_CloseAudio();
  }
  
  /* 0 to unpause, 1 to pause */
  SDL_PauseAudio(0);
  return;
}



int main(int argc, char* argv[]) {
    /* Check if SDL_Init() failed to load subsystems that are ||'d together.
       TODO(Stephen) Are there ways to find out specifically why these calls failed? */
  if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER ) < 0 ) {
    printf("SDL_Init failed: [%s]\n", SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Manor", "Error! SDL_Init() failed, [%s]\n", 0);
    return 1;
  }
  
  /* Start Audio: fill out a SDL_AudioSpec struct to request audio settings */
  SDLInitAudio(44000, 4096);

  SDL_Window* Window = SDL_CreateWindow("Manor",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    640, 480,
			    SDL_WINDOW_RESIZABLE);

  if ( Window ) {
    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, 0);
    if ( Renderer ) {
      bool running = true;
      sdl_window_dimension Dimension = SDLGetWindowDimension(Window);

      SDLResizeTexture(&globalBackbuffer, Renderer, Dimension.Width, Dimension.Height);
      int XOffset = 0;
      int YOffset = 0;
      while ( running ) {
	SDL_Event Event;
	while ( SDL_PollEvent(&Event) ) {
	  if (HandleEvent(&Event)) {
	    running = false;
	  }
	}
	RenderGradient(globalBackbuffer, XOffset, YOffset);
	SDLUpdateWindow(Window, Renderer, globalBackbuffer);

	XOffset -= x_move_amount;
	YOffset += y_move_amount;

      }
    }
    else { 
      /* TODO(Stephen) logging if no renderer */
    }
  }
  else {
    /* TODO(Stephen) logging if no window */
  }

  SDL_CloseAudio(); 		/* Done by SDL_Quit() also. */
  SDL_Quit();
  return 0;
}


