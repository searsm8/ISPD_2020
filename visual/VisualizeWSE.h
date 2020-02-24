#ifndef _VisualizeWSE
#define _VisualizeWSE

#include <SDL2/SDL.h>
#include <stdio.h>
#include "../kernel/Kernels.h"
#include <vector>

//drawWSE
//draw the Wafer Scale Engine
//given a vector of kernels, draw SDL rectangles for each one
//
SDL_Window* createWSE(int WSE_width=633, int WSE_height=633)
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
//    SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );

    // Clear winow
//    SDL_RenderClear( renderer );

   return window;
}

void drawBackground(SDL_Window* window)
{
    SDL_Renderer* renderer = SDL_GetRenderer(window);
	
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    SDL_Rect b;
    b.x = 0;
    b.y = 0;
    b.w = width;
    b.h = height;

    //Render background rect
    //blue background
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 155 );
    SDL_RenderFillRect( renderer, &b );
    
}

void drawRect(SDL_Renderer* renderer, vector<int> rect, vector<int> colors)
{
    SDL_Rect r;
    r.x = rect[0];
    r.y = rect[1];
    r.w = rect[2];
    r.h = rect[3];

    // Set render color to white fill 
    SDL_SetRenderDrawColor( renderer, colors[0], colors[1], colors[2], 255);
    SDL_RenderFillRect( renderer, &r );
    // Draw black border 
    SDL_SetRenderDrawColor( renderer, 0,0,0,255);
    SDL_RenderDrawRect( renderer, &r );
}


//draw a line representing the connection between two kernels
void drawConnection(SDL_Renderer* renderer, Kernel* k)
{
	for(int i = 0; i < k->getNextKernels().size(); i++)
	{
		Kernel* next = k->getNextKernels()[i];
		if(next == NULL) break;

		pair<double, double> c1 = k->getCenter();
		pair<double, double> c2 = next->getCenter();
		
		//draw the connecting line
		SDL_RenderDrawLine(renderer, c1.first, c1.second, c2.first, c2.second);

		//draw small squares at each endpoint
		SDL_Rect r;
		r.x = c1.first;
		r.y = c1.second;
		r.w = 3;
		r.h = 3;

		// Set render color to white fill 
		SDL_SetRenderDrawColor( renderer, 0,0,0,255);
		SDL_RenderFillRect( renderer, &r );
	}

}



void updateWSE(SDL_Window* window, vector<Kernel*> kernels_to_draw, int iteration=-1)
{
    string title = "WSE Visualization -- Iteration " + to_string(iteration);
    //update the window title
    SDL_SetWindowTitle(window, title.c_str());

    //wipe the window with a new background
    drawBackground(window);

    SDL_Renderer* renderer = SDL_GetRenderer(window);
    //draw all the Kernels
    for(Kernel* k : kernels_to_draw)
    {
	    auto rects = k->getRectangles();
	    for(vector<int> rect : rects) 
	    {
		    drawRect(renderer, rect, k->getColors());		
	    }
    }
    // Render the rects to the screen
    SDL_RenderPresent(renderer);

    //draw all the connections as lines
    for(Kernel* k : kernels_to_draw)
    {
	    auto rects = k->getRectangles();
	
	    for(vector<int> rect : rects) 
	    {
		    drawConnection(renderer, k);
	    }
    }

    // Render the lines to the screen
    SDL_RenderPresent(renderer);
}

#endif
