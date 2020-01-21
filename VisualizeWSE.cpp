//VisualizeWSE.cpp
//utility functions to display graphics representing a Wafter Scale Engine (WSE) solution
//draws rectangles based on the characteristics of kernels
//

#include <graphics.h>

initializeGraphics()
{
	//gm is Graphics mode using computer display
	int gd = DETECT, gm;

	initgraph(&gd, &gm, "");
}

displayKernel()
{
	line(150, 150, 450, 150);
	line(250, 150, 400, 300);

	getch(); //wait for any key
	closegraph();
}

int main()
{
	initializeGraphics();
	displayKernel();
}
