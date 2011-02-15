#ifndef _ATTACKING_UNIT_H_
#define _ATTACKING_UNIT_H_

#include "unit.h"

class AttackingUnit : public Unit {
	public:
		AttackingUnit(float hp, float cost, float speed, float range, Player* owner, CIwFVec2 position, Game* game);
	
		virtual ~AttackingUnit() { };
	
		virtual Unit* spawnCopy() = 0;
		
		Unit* getTarget();
	
		virtual void update() = 0;
	
		virtual unit_type getType() = 0;
			
		void setTarget(Unit* unit);
	
		float distToTarget();
	
	protected:
		Unit* target;
	
		float range;
	
	private:	
		void detectEnemy();
	
		/** 
		Set the new sprite for this state and move the unit to the correct bucket in
		the render-ordering structure.
		 
		We make this a separate function so it can be called from base-class code. 
		We want to do this as little as possible and immediately after knowing we 
		need to. It's an expensive operation.
		*/
		virtual void setAttackSprite() { };
		
		virtual void setIdleSprite() { };
	
		/** Determine whether the target is within attacking range */
		bool targetInRange();
	
		/** Determine whether we can attack right now */
		bool canAttack();
	
		virtual void attack() = 0;
};

#endif