#include "AI.h"
  
AI::AI(Game* game):game(game){
    worldRad = game->getWorldRadius();
}

/*
 The AI logic for a Unit
 =======================
 
 get best target
 
 if no target
    move
 else if can attack
    attack
 else
    pursue
*/

void AI::updateAI(Unit* unit){
    unit->setTarget(detectEnemy(unit));
    
    if(!unit->hasTarget())
        doIdle(unit);
    else if(unit->getRange() >= unit->distToTarget())
        unit->attack();
    else
        doPursue(unit);
}

void AI::doIdle(Unit *unit){
    CIwFVec2 position = unit->getPosition();
    
    float r     = unit->getR();
	float theta = unit->getTheta();
	float speed = unit->getSpeed();
	
	float diff = theta - (unit->isLocal() ? 0 : PI);
	int direction = diff < PI && diff > 0 ? 1 : -1;
    
	float thetaChange = direction * speed / r; 
    
    CIwFVec2 coords(r + getRChange(unit), theta + thetaChange);
    polarToXY(coords);
        
    // We want to move along (coords - position) by speed
    coords -= position;
    coords.Normalise();
    coords *= speed;
        
    unit->setPosition(position + coords);
	unit->setVelocity(coords);
}

float AI::getRChange(Unit* unit) {
    CIwFVec2 position = unit->getPosition();
    float unitR = unit->getR();
    
	std::list<Unit*>* units = game->getUnits();
	float rChange = 0.0;
	
    // THIS IS TOO SLOW. Also, what are the magic numbers about?
	for (std::list<Unit*>::iterator itr = units->begin(); itr != units->end(); ++itr) {
		
		Unit* otherUnit = *(itr);
		
		if(otherUnit != unit) {
			
			CIwFVec2 otherPos = otherUnit->getPosition();
			float otherR = otherUnit->getR();
			float otherTheta = otherUnit->getTheta();
			float distSquare = abs((otherPos-position).GetLengthSquared());
			
			
			float rDiff = unitR - otherR;
			float dir = rDiff > 0 ? 1 : -1;
			
			rChange += dir*15000.0/distSquare;
		}
	}
	    
	rChange += 5000.0/SQ(worldRad.x - unitR);
	rChange -= 5000.0/SQ(worldRad.y - unitR); 
	
    return rChange;
}

void AI::doPursue(Unit* unit) {
    CIwFVec2 position = unit->getPosition();
    float speed = unit->getSpeed();
    float range = unit->getRange();
    
    Unit *pursuing = unit->getTarget();
    CIwFVec2 old_position = unit->getPosition();
    CIwFVec2 pursuingPos = pursuing->getPosition();
    
    // The following logic should probably be altered as to
    // only do one polar-to-xy conversion, not two.
    
    CIwFVec2 pursuitVector = pursuingPos - unit->getPosition();
    pursuitVector.Normalise();
    pursuitVector *= speed;
    polarize(pursuitVector);
    
    pursuitVector.x += getRChange(unit);
    
    polarToXY(pursuitVector);
    pursuitVector.Normalise();
    pursuitVector *= speed;
    
    unit->setPosition(position + pursuitVector);
	unit->setVelocity(pursuitVector);
}



Unit* AI::detectEnemy(Unit* unit){
    std::list<Unit*>* units = game->getUnits();
    CIwFVec2 position = unit->getPosition() + unit->getVelocity();
    CIwFVec2 otherPos = CIwFVec2::g_Zero;
    
    float sq_dist = 0;
    float closest_distance = SQ(unit->getSight());
    Unit* closest = unit->getTarget(); // Chase em down
    
    // THIS IS TOO SLOW
    
    // Just treat sight as a radius for now and return the closest enemy unit within it.
    for(std::list<Unit*>::iterator itr = units->begin(); itr != units->end(); ++itr) {
        if(&(*itr)->getOwner() != &unit->getOwner()) {
			otherPos = (*itr)->getPosition();
			sq_dist = SQ(position.x - otherPos.x) + SQ(position.y - otherPos.y);
            
            // If this unit is closer and alive, it's the best for now
			if(sq_dist < closest_distance && (*itr)->getHp() > 0) {
                closest_distance = sq_dist;
                closest = *(itr);
			}
		}
	}
    
    return closest;
}

// THIS NEEDS TO BE FASTER
template<typename OutputIterator> void AI::collide(OutputIterator out, Unit* unit)
{
	std::list<Unit*>* units = game->getUnits();
	
	CIwFVec2 unitPos = unit->getPosition();
	
	Unit* collideUnit; //unit that we're colliding with
	float collideRad; //radius of circle containing the colliding unit
	float unitRad = unit->getSize()/2.0;
    CIwFVec2 collidePos = CIwFVec2::g_Zero;
    CIwFVec2 collideDir = CIwFVec2::g_Zero;
    CIwFVec2 collideRadPoint = CIwFVec2::g_Zero;
    float collideT = 0.0f;
	
	for(std::list<Unit*>::iterator itr = units->begin(); itr != units->end(); ++itr) {
		
		if((*itr) != unit && (*itr)->getHp() >= 0) {
			collideUnit = *(itr);
			collideRad = collideUnit->getSize()/2.0;
			
			collidePos = collideUnit->getPosition(); //position of unit we're colliding with
			collideDir = (collidePos - unitPos); //normalized vector pointing from the unit toward the one being collided with
			collideDir.Normalise();
			
			collideRadPoint = (-1*collideDir)*collideRad + collidePos; //point on edge of colliding unit's bounding circle closest to our unit
				
			collideT = (collideRadPoint.x - unitPos.x)/collideDir.x;
			
			if (collideT <= unitRad) {
				*(out++) = collideUnit;
			}
		}
	}
}
