#ifndef _THROWER_H
#define _THROWER_H

#include "attackingunit.h"

class Thrower : public AttackingUnit {	
	public:
		Thrower(Player* owner, CIwFVec2 position, Game* game);
	
		Thrower(const Thrower& newThrower);
	
		~Thrower() { };
        
        virtual void attack();

        virtual void update();

		virtual unit_type getType();
	
		virtual Unit* spawnCopy();
};

#endif
