#ifndef _SHOOTER_H
#define _SHOOTER_H

#include "attackingunit.h" 

class Shooter : public AttackingUnit {
	public:
		Shooter(Player* owner, CIwFVec2 position, Game* game);
	
		Shooter(const Shooter& newShooter);

		virtual void update();

		virtual unit_type getType();
	
		virtual Unit* spawnCopy();

        void attack();
	
	private:
	
		int projectileCount;

};

#endif
