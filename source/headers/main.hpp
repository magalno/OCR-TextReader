
#ifndef MAIN_HPP
#define MAIN_HPP


/*Included headers*/
/*---------------------------------------------*/
#include "errorlogger.hpp"
/*---------------------------------------------*/

/*Included dependencies*/
/*---------------------------------------------*/
#include <string>
#include <iostream>
#include <SDL2/SDL.h>
#include <fstream>
#include <unistd.h>
/*---------------------------------------------*/

namespace SDLconsts {
	const unsigned int window_length = 500;
	const unsigned int window_height = 500;
}

/*Header content*/
/*=============================================*/
void pollevent(bool& var);

void waitForEvent();
/*=============================================*/

#endif