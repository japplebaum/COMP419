#include "attackingunit.h"

AttackingUnit::AttackingUnit(float hp, float cost, float speed, float range, Player* owner, CIwFVec2 position, Game* game): 
	Unit(hp, cost, speed, owner, position, game), range(range) { }

Unit* AttackingUnit::getTarget() { return target; }

void AttackingUnit::setTarget(Unit* unit){
    if(unit != NULL && unit->getHp() <= 0) {
        target = NULL;
	} 
	else {
		target = unit;
	}
	if(target == NULL) {
		setIdleSprite();
	}
}

float AttackingUnit::distToTarget(){    
    return (target->getPosition() - getPosition()).GetLength();
}

bool AttackingUnit::targetInRange() {
	return distToTarget() <= range;
}

bool AttackingUnit::canAttack() {
	return target != NULL && targetInRange();
}


void AttackingUnit::detectEnemy() {
    /*std::list<Unit*>* units = game->getUnits();
	 CIwFVec2 position = (*unit_itr)->getPosition() + (*unit_itr)->getVelocity();
	 CIwFVec2 otherPos = CIwFVec2::g_Zero;
	 
	 float sq_dist = 0;
	 float closest_distance = SQ((*unit_itr)->getSight());
	 float max_dist = closest_distance;
	 Unit* closest = (*unit_itr)->getTarget();
	 
	 bool foundTarget = false;*/
    
    /**
	 * In order to avoid brute-force distance calculations, we take advantage of
	 * the fact that the units are sorted by their theta values. We begin our distance
	 * checking at the given unit's position in the sorted container, then at each
	 * step, check the unit with the next nearest theta, and see if it's close enough.
	 * 
	 * We stop once we've reached a unit that is completely outside the sight range,
	 * We do the same thing in both directions to find the closest unit.
	 */
	
	/*
	 std::list<Unit*>::iterator incr_theta_itr = unit_itr;
	 std::list<Unit*>::iterator decr_theta_itr = unit_itr;
	 while(incr_theta_itr != units->end() && sq_dist <= max_dist) {
	 // Look up theta, which means we're moving to the BACK of the container
	 if(&(*incr_theta_itr)->getOwner() != &(*unit_itr)->getOwner()) {
	 otherPos = (*incr_theta_itr)->getPosition();
	 sq_dist = SQ(position.x - otherPos.x) + SQ(position.y - otherPos.y);
	 
	 if(sq_dist < closest_distance && (*decr_theta_itr)->getHp() > 0 && (*incr_theta_itr)->getType() != PROJECTILE) {
	 closest_distance = sq_dist;
	 closest = *(incr_theta_itr);
	 foundTarget = true;
	 }
	 }
	 ++incr_theta_itr;
	 }
	 
	 // Must reset the distance here since we're switching directions.
	 sq_dist = 0.0f;
	 while(decr_theta_itr != units->begin() && sq_dist <= max_dist) {
	 // Look down theta, which means we're moving to the FRONT of the container. 
	 if(&(*decr_theta_itr)->getOwner() != &(*unit_itr)->getOwner()) {
	 otherPos = (*decr_theta_itr)->getPosition();
	 sq_dist = SQ(position.x - otherPos.x) + SQ(position.y - otherPos.y);
	 
	 if(sq_dist < closest_distance && (*decr_theta_itr)->getHp() > 0 && (*decr_theta_itr)->getType() != PROJECTILE) {
	 closest_distance = sq_dist;
	 closest = *(decr_theta_itr);
	 foundTarget = true;
	 }
	 }
	 --decr_theta_itr;
	 }
	 
	 
	 target = closest;
	 
	 if (foundTarget) {
	 pathMode = OBJECTIVE;
	 }*/
}