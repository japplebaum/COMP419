#include "spreader.h"

Spreader::Spreader(Player* owner, Game* game, float x, float y)
        : Unit(250.0f, 200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 200.0f, 7.0f, 5.0f, owner, game){
            
}

Spreader::Spreader(const Spreader& newSpreader) : Unit(newSpreader) {
}

bool Spreader::shouldAIUpdate() {
    return false;
}

bool Spreader::update(std::list<Unit*>::iterator itr){
    
    if(curFrame % spreadDelay == 0){
        // Do stuff
    }

    
	curFrame = (curFrame + 1) % numFrames;
    return true;
}

Spreader::~Spreader() {
}

unit_type Spreader::getType() {
	return SPREADER;
}

Unit* Spreader::spawnCopy() {
	return new Spreader(*this);
}

int Spreader::nearbyIceCount(CIwFVec2 loc){
    return 0;
}

int Spreader::useNearbyIce(CIwFVec2 loc, int max){
    return 0;
}

CIwFVec2 *closestIce(CIwFVec2 loc){
    return NULL;
}
