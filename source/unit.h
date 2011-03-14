#ifndef _UNIT_H
#define _UNIT_H

class Unit;

#include <string>
#include <vector>
#include <map>
#include <math.h>

#include "worldobject.h"
#include "game.h"
#include "player.h"
#include "IwDebugPrim.h"

class Leader;

#define UNIT_SPRITE_SIZE 256
#define REPEL_FACTOR 8000000
#define CIRCLE_SPRING .0015f
#define NAV_ATTRACT_FACTOR 1000
#define FORCE_THRESHOLD SQ((NAV_ATTRACT_FACTOR/64))
#define NORMAL_FORCE_THRESHOLD 100

static float statAttacks[7][7] = {
	// Muncher | Shooter | Spreader | Wrecker | Thrower | Leader | Projectile
	{0		   , 20	     ,	0		,	30	  ,     50  ,     0  ,  0    }, // Muncher
	{0		   , 10	     ,	0		,	20	  ,     30  ,     0  ,  0    }, // Shooter
	{0		   , 10	     ,	0		,	20	  ,     30  ,     0  ,  0    }, // Spreader
	{10		   , 15	     ,	0		,	20	  ,     10  ,     0  ,  0    }, // Wrecker
	{0		   , 15	     ,	0		,	30	  ,     20  ,     0  ,  0    }, // Thrower
	{0		   , 20	     ,	0		,	4	  ,     12  ,     0  ,  0    }, // Leader
	{0		   , 0	     ,	0		,	0	  ,     0   ,     0  ,  0    }, // Projectile
};

static int unitCosts[7] = {1, 1, 1, 1, 1, 1, 1};

/**
This lets us quickly determine a unit's type at run time.
*/
enum unit_type {
	MUNCHER, SHOOTER, SPREADER, WRECKER, THROWER, LEADER, PROJECTILE
};

/**
Pathing modes that a unit can be in
 
TOLEADER: all is well - move toward the enemy leader and do circular pathing
ESCAPE: fell into equilibrium - ignore enemy leader and circular pathing, 
		go to a wall
OBJECTIVE: trying to go somewhere/chase something - ignore enemy leader, 
		   but not circular pathing
*/
enum path_mode {
	TOLEADER, ESCAPE, OBJECTIVE
};

class Unit : public WorldObject {
    
	private:	
		/** Get the attractive force exerted by the enemy leader */
		CIwFVec2 getLeaderForce();
	
		/** Calculate the sum of all repulsive forces on this unit */
		CIwFVec2 getRepulsiveForce();
	
		/** Get the "spring force" that drives the unit toward pathing
		 in a circular fashion" */
		CIwFVec2 getCircularForce();
	
		/** Get the repulsive force exhbited by the nearest wall on this
		 unit. If we're not at a wall, then it's just the zero vector */
		CIwFVec2 getWallForce(CIwFVec2 sumForces);
	
		/** Get the attractive force exhbited by the navigation target
		 for escape/objective pathing */
		CIwFVec2 getNavTargetForce();
	
		/** Create a target (the inner or outer wall) for escape pathing */
		void createEscapeTarget();
	
		/** Move the escape target while escape pathing so that we can
		 "drag" the unit out forward of its predicament */
		void updateEscapeTarget(CIwFVec2 sumForces);
	
		/** Based on this unit's current position, determine the angular 
		 direction toward the the enemy leader */
		int dirToEnemyAngular();
	
	protected:	
		//info for sprite animation
		int numFrames;
		int curFrame;

		CIwFVec2 navTarget;	// a target the unit will move toward when it's stuck
		CIwFVec2 navTargetPolar; // the same target, but in polar coordinates
		path_mode pathMode; // the current pathing mode that we're in
	
		CIwFVec2 enemyLeaderPos;
	
		std::string unitType;

    public:
		Player *owner;
		CIwFVec2 velocity;
		float speed;
		float hp;
		float scale;
		int uid;
		
		Unit(float hp, float speed, Player* owner, CIwFVec2 position, Game* game);

		Unit(const Unit& newUnit);

		virtual ~Unit() { };
    	
		virtual unit_type getType() = 0;
		
		void setOwner(Player* p);
		
        float getSize();

		/** Create a copy of this Unit without having it's header file - no circular includes! */
		virtual Unit* spawnCopy() { return NULL; }
	
		virtual void update() = 0;
	
        void display();
	
        void displayOnScreen(CIwSVec2 pos);
		
        void receiveDamage(float amount); 
	
        int getDamage(Unit* unit);
    
        float getAngle();
    
		void path();
};

#endif
