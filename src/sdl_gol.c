/* 
   weird fork
   Stephen Duncanson 
   Fall 2021 -
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static SDL_Texture* Texture;
static void* BitmapMemory;
static int BitmapWidth;
static int BitmapHeight;
const int BytesPerPixel = 4;

static int current_board[480][640] = {0};
static int theNext_board[480][640] = {0};

const int rows = 480;
const int cols = 640;

void initBoard() {
  for (int i=0;i<rows;i++) {
    for (int j=0; j<cols;j++){
      
      if (i==j || i==j+1 ) { current_board[i][j]=1; }
      else { current_board[i][j]=0; }
    }
  }
  return;
}

unsigned int count8(int board[rows][cols], int i, int j) {
  unsigned int neighbors = 0;
  /* Assumes board is a 2d array where each element is alive 1 or dead 0. 
     ......... where C is the cell denoted by board[i][j]
     ..xxx.... and each x is one of the neighbors. Depending on our location on
     ..xCx.... the board we may not have a neighbor on each side. 
     ..xxx.... For example on the first or last row of the grid.
     ......... First check if we can have the neighbor, and add its value to count. */
  neighbors += ((i > 1 && j > 1) ? board[i-1][j-1] : 0);	/* NW */
  neighbors += ((i > 1) ? board[i-1][j] : 0);	                /* N */
  neighbors += ((i > 1 && j < cols-2) ? board[i-1][j+1] : 0);	/* NE */
  neighbors += ((j < cols-2) ? board[i][j+1] : 0);        	/* E */
  neighbors += ((i < rows-2 && j < cols-2) ? board[i+1][j+1] : 0);/* SE */
  neighbors += ((i < rows-2) ? board[i+1][j] : 0);      	/* S */
  neighbors += ((i < rows-2 && j > 1) ? board[i+1][j-1] : 0);	/* SW */
  neighbors += ((j > 0) ?  board[i][j-1] : 0);	/* W */
  return neighbors;
}

void updateBoard(int board_curr[rows][cols], int board_next[rows][cols]) {
  /* replace board_curr[] with board_next[] and refresh stdout for the
     new board_current */
  unsigned int adjacent_count = 0;
  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols; j++) {
      adjacent_count = count8(board_curr, i, j);
      if (adjacent_count == 3) {  board_next[i][j] = 1; }
      else if (adjacent_count == 2 && board_curr[i][j])  { board_next[i][j] = 1; }
      else { board_next[i][j] = 0; }
    }
  }

  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols; j++) {
      board_curr[i][j] = board_next[i][j];
    }
  }

}



static void RenderLifeGame(void) {
  int Width = BitmapWidth;
  int Height = BitmapHeight;
  int Pitch = Width * BytesPerPixel; /* amount of memory per row (width-wise) */
  
  uint8_t* Row = (uint8_t* )BitmapMemory;

  for (int y=0; y<BitmapHeight; y++) {
    uint32_t* Pixel = (uint32_t* )Row;
    for (int x=0; x<BitmapWidth; x++) {
      *Pixel++ = (current_board[y][x]) ? 0xFFFFFFFF : 0x00000000;
    }
    Row+=Pitch;
  }
}


static void RenderGradient(int BlueOffset, int GreenOffset) {
  int Width = BitmapWidth;
  int Height = BitmapHeight;
  int Pitch = Width * BytesPerPixel;
  
  uint8_t* Row = (uint8_t* )BitmapMemory;
  
  for (int y=0; y<BitmapHeight; y++) {
    uint32_t* Pixel = (uint32_t *)Row;

    for (int x=0; x<BitmapWidth; x++) {
      uint8_t Blue = (x + BlueOffset);
      uint8_t Green = (y + GreenOffset);
      *Pixel++ = ((Green << 8) | Blue);
    }
    Row+=Pitch;
  }
}

void SDLResizeTexture(SDL_Renderer* Renderer, int Width, int Height) {
  if (BitmapMemory) { free(BitmapMemory); }
  if (Texture) { SDL_DestroyTexture(Texture); }
     
  Texture = SDL_CreateTexture(Renderer, 
			      SDL_PIXELFORMAT_ARGB8888,
			      SDL_TEXTUREACCESS_STREAMING,
			      Width, Height);
  BitmapWidth = Width;
  BitmapHeight = Height;
  BitmapMemory = malloc(Width * Height * BytesPerPixel);
}

void SDLUpdateWindow(SDL_Window* Window, SDL_Renderer* Renderer) {
  SDL_UpdateTexture(Texture, 0, BitmapMemory, BitmapWidth * BytesPerPixel);
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
  

  SDL_Window* Window = SDL_CreateWindow("Manor",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    640, 480,
			    SDL_WINDOW_RESIZABLE);

  if (Window) {
    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, 0);

    if (Renderer) {

      initBoard();

      int running = 1;
      
      int Width, Height;
      SDL_GetWindowSize(Window, &Width, &Height);
      SDLResizeTexture(Renderer, Width, Height);
      int XOffset = 0;
      int YOffset = 0;
      while ( running ) {
	SDL_Event Event;
	while ( SDL_PollEvent(&Event) ) {
	  if (HandleEvent(&Event)) { running = 0; }
	}
	RenderLifeGame();
	

	/* RenderGradient(XOffset, YOffset); */
	SDLUpdateWindow(Window, Renderer);

	/* ++XOffset; */
	/* YOffset +=2; */
	updateBoard(current_board, theNext_board);
      }
    }
    else { 
      /* TODO(Stephen) logging if no renderer */
    }
  }
  else {
    /* TODO(Stephen) logging if no window */
  }
  SDL_Quit();
  return 0;
}


