#ifndef _UNIT_H
#define _UNIT_H

class Unit;

#include <string>

#include "worldobject.h"
#include "game.h"
#include "player.h"
#include "IwDebugPrim.h"
#include <string>
#include <vector>
#include <math.h>

#define UNIT_SPRITE_SIZE 256

#define PATH_THETA_RANGE PI
#define THETA_DIFF(X, Y) (min(abs((X)-(Y)), 2*PI - abs((X) - (Y))))
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
	    float hp;
		float cost;
		float speed;
        float scale;
		float repulsion_factor;

		int uid;

		bool localPlayedOwnsThis;
	
		//info for sprite animation
		int numFrames;
		int curFrame;

		CIwFVec2 velocity;	
		CIwFVec2 navTarget;	//a target the unit will move toward when it's stuck
		CIwFVec2 navTargetPolar; //polar navigation target
		path_mode pathMode; //the current pathing mode that we're in
	
		CIwFVec2 enemyLeaderPos;
		Player *owner;
	
		std::string unitType;
	
		/** 
		 Calculate a target (the inner or outer wall) for escape pathing
		 @param toLeader vector pointing from this unit to the enemy leader
		 @param the sum of the forces currently acting on this unit
		 */
		void setEscapeTarget(CIwFVec2 toLeader, CIwFVec2 force);

    public:
	
		Unit(float hp, float cost, float attack, float speed, 
				float munch_speed, float range, float sight,
				float spread_speed, float spread_radius, Player* owner,
				Game* game);

		Unit(const Unit& newUnit);

		virtual ~Unit() {};
    	
		virtual unit_type getType() = 0;

        void setVelocity(const CIwFVec2& velocity);
        void setVelocity(float xv, float yv);
    
        CIwFVec2 getVelocity();

    
		float getSpeed(); 
		float getRange();
        float getSize();
		int getId();
		void setId(int uuid);
        
		Player& getOwner();
		void setOwner(Player* p);
			
		Unit* getTarget();
		void setTarget(Unit* unit);
		
        bool hasTarget();
        
		float getHp();
		void setHp(float f);

		/*
		 Create a copy of this Unit. This is here so that you can make copies of Units without including
		 their header files - it's a workaround for doing the mirror opponent, but could be useful
		 in the future. 
		 */
		virtual Unit* spawnCopy() { return NULL; };
	
		virtual bool update(std::list<Unit*>::iterator itr) = 0;
	
        virtual void display();
        void displayOnScreen(int x, int y);
		
		virtual void attack();
	
        void receiveDamage(float amount); 
	
        int getDamage(Unit* unit);


		// Set the new sprite for this state
		// and move the unit to the correct bucket in
		// the render-ordering structure.
		//
		// We make this a separate function so it can be called
		// from base-class code. We want to do this as little as
		// possible and immediately after knowing we need to. it's
		// an expensive operation.
		virtual void setAttackSprite(){};
		virtual void setIdleSprite(){};
    
        virtual float getAngle();
    
        float distToTarget();
	
		void path(std::list<Unit*>::iterator itr);
	
		void detectEnemy(std::list<Unit*>::iterator unit_itr);
};

#endif
