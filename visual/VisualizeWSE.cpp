#include <SDL2/SDL.h>
#include <stdio.h>
#include "../kernel/Kernels.h"
#include <vector>

//drawWSE
//draw the Wafer Scale Engine
//given a vector of kernels, draw SDL rectangles for each one
//
int drawWSE(vector<Kernel> kernels_to_draw, int WSE_width=633, int WSE_height=633)
{
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        "WSE Visualization", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WSE_width,
        WSE_height,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );

    // Clear winow
    SDL_RenderClear( renderer );

    // Creat a rect at pos ( 50, 50 ) that's 50 pixels wide and 50 pixels high.
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = 633;
    r.h = 633;

    // Set render color to blue ( rect will be rendered in this color )
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );

    // Render rect
    SDL_RenderFillRect( renderer, &r );


    //draw all the Kernels
    //
    for(Kernel k : kernels_to_draw)
    { 
    r.x = k.x; //add 100 to match the background
    r.y = k.y;
    r.w = k.width;
    r.h = k.height;

    // Set render color to white fill 
    SDL_SetRenderDrawColor( renderer, 255,255,255,255);
    SDL_RenderFillRect( renderer, &r );
    // Draw black border 
    SDL_SetRenderDrawColor( renderer, 0,0,0,0);
    SDL_RenderDrawRect( renderer, &r );
    }


    // Render the rect to the screen
    SDL_RenderPresent(renderer);
    SDL_Delay(5000);	
    return 1;
}

int main (int argc, char** argv)
{

	Dblock k1 = Dblock(56, 56, 256);
	k1.setEP(4, 4, 8, 8, 8, 64, 8, 16);
	k1.x = 0;
	k1.y = 148;
	k1.computePerformance();
	
	k1.printParameters();
	k1.printPerformance();
//	k1.conv1.printPerformance();
//	k1.conv1.printParameters();
//	k1.conv2.printPerformance();
//	k1.conv2.printParameters();
//	k1.conv3.printPerformance();
//	k1.conv3.printParameters();

	Cblock k2 = Cblock(56, 56, 512);
	k2.setEP(4, 4, 8, 32, 8, 32, 8, 8, 8, 19);
	k2.x = 150;
	k2.y = 0;
	k2.computePerformance();
	k2.printParameters();
	k2.printPerformance();

	Dblock k3 = Dblock(28, 28, 256); k3.setEP(4, 4, 8, 8, 8, 32, 8, 8);
	k3.x = 72;
	k3.y = 0;
	k3.computePerformance();
	k3.printParameters();
	k3.printPerformance();

	vector<Kernel> kernels_to_draw;
	kernels_to_draw.push_back(k1);
	kernels_to_draw.push_back(k2);
	kernels_to_draw.push_back(k3);

	int WSE_width = 633;
	int WSE_height= 633;
	drawWSE(kernels_to_draw, WSE_width, WSE_height) ;
}
