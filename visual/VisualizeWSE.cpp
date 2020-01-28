#include "VisualizeWSE.h"

int main (int argc, char** argv)
{

	Dblock k1 = Dblock(56, 56, 256);
	k1.setEP(4, 4, 8, 8, 8, 16, 8, 16);
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

	Dblock k3 = Dblock(28, 28, 256);
       	k3.setEP(4, 4, 8, 8, 8, 32, 8, 8);
	k3.x = 72;
	k3.y = 0;
	k3.computePerformance();
	k3.printParameters();
	k3.printPerformance();

/*	k1.conv1.printperformance();
	k1.conv2.printperformance();
	k1.conv3.printperformance();

	k2.conv1.printperformance();
	k2.conv2.printperformance();
	k2.conv3.printperformance();
	k2.conv3.printperformance();

	k3.conv1.printperformance();
	k3.conv2.printperformance();
	k3.conv3.printperformance();
*/	
	
	int WSE_width = 633;
	int WSE_height= 633;
	int interval = 3000;
	int iteration = 0;
	SDL_Window* window = createWSE(WSE_width, WSE_height) ;
	vector<Kernel*> kernels_to_draw;

	updateWSE(window, kernels_to_draw, iteration++);
	SDL_Delay(interval);

	k1.setX(0);
	k1.setY(0);
	k2.setX(200);
	k2.setY(0);
	k3.setX(400);
	k3.setY(0);
	kernels_to_draw.push_back(&k1.conv1);
	kernels_to_draw.push_back(&k1.conv2);
	kernels_to_draw.push_back(&k1.conv3);
	kernels_to_draw.push_back(&k2.conv1);
	kernels_to_draw.push_back(&k2.conv2);
	kernels_to_draw.push_back(&k2.conv3);
	kernels_to_draw.push_back(&k2.conv4);
	kernels_to_draw.push_back(&k3.conv1);
	kernels_to_draw.push_back(&k3.conv2);
	kernels_to_draw.push_back(&k3.conv3);

	updateWSE(window, kernels_to_draw, iteration++);
	SDL_Delay(interval);

	kernels_to_draw.clear();
	k1.conv1.setY(100);
	k1.conv2.setY(100);
	k1.conv3.setY(100);
	kernels_to_draw.push_back(&k1.conv1);
	kernels_to_draw.push_back(&k1.conv2);
	kernels_to_draw.push_back(&k1.conv3);
	
	updateWSE(window, kernels_to_draw, iteration++);
	SDL_Delay(interval);
}

