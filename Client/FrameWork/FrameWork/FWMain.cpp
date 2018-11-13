#include "FWMain.h"
FWMain*	 FWMain::instance = NULL;
FWMain::FWMain() : FWWindows("Galaga",1024,720)
{
	Entry(0, new Menu);
	Entry(1, new Game);
	Warp(1);
}
FWMain::~FWMain()
{

}
