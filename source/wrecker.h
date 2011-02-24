#ifndef _WRECKER_H
#define _WRECKER_H

#include "attackingunit.h"

class Wrecker : public AttackingUnit {
	
	public:
		Wrecker(Player* owner, CIwFVec2 position, Game* game);
	
		Wrecker(const Wrecker& wrecker);
	
		~Wrecker(){ };

		virtual void update();
	
		virtual unit_type getType();
	
		virtual Unit* spawnCopy();
	
        virtual void attack();

		virtual void setAttackSprite();
	
		virtual void setIdleSprite();
};

#endif
