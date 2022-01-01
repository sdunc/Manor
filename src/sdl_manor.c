/* 
   Manor
   Stephen Duncanson 
   Fall 2021 -
*/

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Texture* Texture;
static void* Pixels;
static int TextureWidth;

void SDLResizeTexture(SDL_Renderer* Renderer, int Width, int Height) {
     if (Pixels) { free(Pixels); }
     if (Texture) { SDL_DestroyTexture(Texture); }
     
     Texture = SDL_CreateTexture(Renderer, 
				 SDL_PIXELFORMAT_ARGB8888,
				 SDL_TEXTUREACCESS_STREAMING,
				 Width, Height);
     TextureWidth = Width;
     Pixels = malloc(Width * Height * 4);
  
     if ( SDL_UpdateTexture(Texture, NULL, Pixels, TextureWidth * 4) )
     {
	  // do something about error! 
	  printf("Error! SDL_UpdateTexture() failed.\n");
     }
  
     SDL_RenderCopy(Renderer, Texture, 0, 0);
}


void SDLUpdateWindow(SDL_Window* Window, SDL_Renderer* Renderer) {
     SDL_UpdateTexture(Texture, 0, Pixels, TextureWidth * 4);
     SDL_RenderCopy(Renderer, Texture, 0, 0);
     SDL_RenderPresent(Renderer);
}


int HandleEvent(SDL_Event* Event) {
     int ShouldQuit = 0;
  
     switch(Event->type) {
     case SDL_QUIT: {
	  printf("SDL_QUIT\n");
	  ShouldQuit = 1;
     } break;
	  
     case SDL_WINDOWEVENT: {
	  switch(Event->window.event) {
	       
	  case SDL_WINDOWEVENT_SIZE_CHANGED: {
	       SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
	       SDL_Renderer* Renderer = SDL_GetRenderer(Window);
	       printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d,%d)\n",Event->window.data1, Event->window.data2);
	       SDLResizeTexture(Renderer,Event->window.data1, Event->window.data2);
	  } break;
      
	  case SDL_WINDOWEVENT_EXPOSED: {
	       SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
	       SDL_Renderer* Renderer = SDL_GetRenderer(Window);
	       SDLUpdateWindow(Window, Renderer);
	  } break;

	  case SDL_WINDOWEVENT_FOCUS_GAINED: {
	       printf("SDL_WINDOWEVENT_FOCUS_GAINED\n");
	  } break;
	  }
     } break;
     }
     return ShouldQuit;
}

     int main(int argc, char* argv[]) {
	  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
	       // Unrecoverable error, exit here.
	       SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Manor",
					"Error! SDL_Init didn't work!", 0);
	       return 1;
	  }
  
	  SDL_Window* Window;
	  SDL_Renderer* Renderer;
	  SDL_Texture* Texture;

	  Window = SDL_CreateWindow("Manor",
				    SDL_WINDOWPOS_UNDEFINED,
				    SDL_WINDOWPOS_UNDEFINED,
				    640, 480,
				    SDL_WINDOW_RESIZABLE);

	  // TODO(Stephen) check for errors when calling these SDL create
	  // window/rendererfunctions. Also go through with SDL wiki and document
	  Renderer = SDL_CreateRenderer(Window, -1, 0);
    
	  for(;;) {
	       SDL_Event Event;
	       SDL_WaitEvent(&Event);
	       if (HandleEvent(&Event)) {
		    break;
	       }
	  }
  
	  SDL_Quit();
	  return 0;
     }
