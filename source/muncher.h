#ifndef _MUNCHER_H
#define _MUNCHER_H

class Muncher;

#include "unit.h"

class Muncher : public Unit {

	public:
		Muncher(Player* owner, CIwFVec2 position, Game* game);

		Muncher(const Muncher& newMuncher);
	
		~Muncher() {};
    
		virtual void update();
        
		virtual unit_type getType();
	
		virtual Unit* spawnCopy();
};

#endif
