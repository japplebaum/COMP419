#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#include "attackingunit.h"

class Projectile : public AttackingUnit {
	
	public:
		Projectile(Player* owner, CIwFVec2 position, Game* game);
	
		virtual void update();
	
		virtual unit_type getType();
	
		virtual void attack();

	private:
		CIwFVec2 worldRad;
		
		float minTargetDist;
	
};

#endif