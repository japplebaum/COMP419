#include "spreader.h"

Spreader::Spreader(Player* owner, CIwFVec2 position, Game* game)
        : Unit(250.0f, 0.0f, owner, position, game) {
}

Spreader::Spreader(const Spreader& newSpreader) : Unit(newSpreader) {
}

void Spreader::update(){
    
    if(curFrame % spreadDelay == 0){
        // Do stuff
    }
    
	curFrame = (curFrame + 1) % numFrames;
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
