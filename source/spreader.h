#ifndef _SPREADER_H
#define _SPREADER_H

#include "unit.h"

class Spreader : public Unit {
    private:
        int framesUntilUpdate;
    
	public:
		Spreader(Player* owner, Game* game, float x, float y);
		~Spreader(){};

		virtual bool update();
        virtual const char* getTextureName();
    
};

#endif