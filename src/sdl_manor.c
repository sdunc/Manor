// Entry point for Manor 
// C w/ SDL library
#include <SDL.h>

void SDLResizeTexture(SDL_Renderer* Renderer, int Width, int Height) {
  int TextureWidth = 1;

  SDL_Texture* Texture = SDL_CreateTexture(Renderer,
					   SDL_PIXELFORMAT_ARGB8888,
					   SDL_TEXTUREACCESS_STREAMING,
					   Width, Height);
  void* Pixels = malloc(Width*Height*4);

  // if we already have a Texture we can destroy it
  if (Texture)
    SDL_DestroyTexture(Texture);

  if ( SDL_UpdateTexture(Texture, NULL, Pixels, TextureWidth * 4) )
    {
      // do something about error! 
      printf("Error! SDL_UpdateTexture() failed.\n");
    }
  
  SDL_RenderCopy(Renderer, Texture, 0, 0);
}


int HandleEvent(SDL_Event* Event) {
  int ShouldQuit = 0;

  switch(Event->type) {
  case SDL_QUIT: 
    printf("SDL_QUIT\n");
    ShouldQuit = 1;
    break;
  case SDL_WINDOWEVENT:
    {
      switch(Event->window.event) {
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	  {
	    SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
	    SDL_Renderer* Renderer = SDL_GetRenderer(Window);
 
	    int Width, Height;
	    SDL_GetWindowSize(Window, &Width, &Height);
	    SDLResizeTexture(Renderer, Width, Height);
	  }
	case SDL_WINDOWEVENT_EXPOSED:
	  { 
	    SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
	    SDL_Renderer* Renderer = SDL_GetRenderer(Window);
	    static int is_white = 1;
	    if (is_white) {
	      SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
	      is_white = 0;
	    }
	    else {
	      SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	      is_white = 1;
	    }
	    SDL_RenderClear(Renderer);
	    SDL_RenderPresent(Renderer);
	  } break;
	}
    }
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
