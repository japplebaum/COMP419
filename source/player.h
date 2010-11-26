#ifndef _PLAYER_H
#define _PLAYER_H

#include "IwColour.h"

class Player {
	private:
	
        CIwColour color; 
	
	public:
        Player(CIwColour& col) { color = col; }
        ~Player() {}
		CIwColour& getColor();
};

#endif