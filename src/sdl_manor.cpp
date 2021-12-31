// in this file we are going to insert the entry point. 
#include <SDL2/SDL.h>

// accepts a pointer to an event. an instance of SDL_event is 56 bytes
// while the pointer is 4 or 8 bytes. "This sort of efficiency is
// usually not important, but since it doesn't make the code any more
// complicated, it's not too bad of an idea."
bool HandleEvent(SDL_Event *Event)
{
  bool ShouldQuit = false;
  switch(Event->type)
    {

    case SDL_QUIT:
      {
	printf("SDL_QUIT\n");
	ShouldQuit = true;
      } break;

    case SDL_WINDOWEVENT:
      // there is a struct in SDL_Event called SDL_WINDOWEVENT
      // with more events. we need to nest switch statements to
      // access these events and handle them.
      {
	switch(Event->window.event)
	  {

	  case SDL_WINDOWEVENT_EXPOSED:
	    // when the application needs to redraw itself
	    {
	      SDL_Window *Window = SDL_GetWindowFromID(Event->window.windowID);
	      SDL_Renderer *Renderer = SDL_GetRenderer(Window);
	      static bool IsWhite = true;
	      if (IsWhite == true)
		{
		  SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		  IsWhite = false;
		}
	      else
		{
		  SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		  IsWhite = true;
		}
	      SDL_RenderClear(Renderer); // clear the window
	      SDL_RenderPresent(Renderer); // render the window
	    } break;

	  case SDL_WINDOWEVENT_RESIZED:
	    {
	      printf("SDL_WINDOWEVENT_RESIZED (%d, %d)\n",
		     Event->window.data1,
		     Event->window.data2);
	    } break;

	  }
      } break;
    }
  
  return(ShouldQuit);
}


int main(int argc, char *argv[])
{
  //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Manor", "Test", 0);
  
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
      //TODO: SDL_Init didn't work!
    }
  
  // create a variable which is pointer to a struct of type SDL_Window
  // the window handle, with one window not much of a concern.
  SDL_Window *Window; 
  
  // create our window by calling SDL_CreateWindow() which takes args:
  // const char* title - the title of the window: Manor
  // int x - the x coordinate of the window 
  // int y - the y coord of our window zero is at the top here. (top left)
  // int w - width of the window in pixels
  // int h - the height of the window in pixels
  // Uint32 flags: extra options. bitwise OR | options you want. 0 for none
  
  Window = SDL_CreateWindow("Manor",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    640,
			    480,
			    SDL_WINDOW_RESIZABLE);

  // the remainder of the code depends on Window working
  if (Window) {  
    // create a "Renderer" for our window. Accepts three arguments:
    // Pointer to window, underlying driver (-1 for autodetect),
    // SDL_RendererFlags.
    SDL_Renderer *Renderer = SDL_CreateRenderer(Window,
						-1,
						0);
  
    // create a SDL_Event variable, pass a pointer to
    // SDL_WaitEvent. once we get an event this event is stored in
    // Event. then we pass a pointer to Event to our event handle
    // function.
    for(;;)
      {
	SDL_Event Event; 
	SDL_WaitEvent(&Event);
	if (HandleEvent(&Event))
	  {
	    break;
	  }
      }
  }

  else ; // here we would catch an error with Window not being
	 // initialized
  
  SDL_Quit();
  return(0);
}
