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

#define PATH_THETA_RANGE PI
#define REPEL_FACTOR 8000000
#define LEADER_ATTRACTION 1000
#define CIRCLE_SPRING .0015f
#define FORCE_THRESHOLD 12000
#define NAV_ATTRACT_FACTOR 1000
#define NORMAL_FORCE_THRESHOLD 40000

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
 
NORMAL: all is well - move toward the enemy leader and do circular pathing
ESCAPE: fell into equilibrium - ignore enemy leader and circular pathing, 
		go to a wall
OBJECTIVE: trying to go somewhere/chase something - ignore enemy leader, 
		   but not circular pathing
*/
enum path_mode {
	NORMAL, ESCAPE, OBJECTIVE
};

class Unit : public WorldObject {
    
	protected:
		bool localPlayedOwnsThis;
	
		//info for sprite animation
		int numFrames;
		int curFrame;

		CIwFVec2 navTarget;	//a target the unit will move toward when it's stuck
		CIwFVec2 navTargetPolar; //polar navigation target
		path_mode pathMode; //the current pathing mode that we're in
	
		CIwFVec2 enemyLeaderPos;
	
		std::string unitType;
	
		/** 
		 Calculate a target (the inner or outer wall) for escape pathing
		 @param toLeader vector pointing from this unit to the enemy leader
		 @param the sum of the forces currently acting on this unit
		 */
		void setEscapeTarget(CIwFVec2 toLeader, CIwFVec2 force);

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
