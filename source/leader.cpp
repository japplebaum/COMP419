#include "leader.h"

Leader::Leader(Player* owner, CIwFVec2 position, Game* game)
		: Unit(1000.0f, 0.0f, owner, position, game) {
    numFrames = 3;
    scale = 0.5f;
	framesUntilUpdate = 0;
			
	texture_names.push_back(IwHashString("leader_sprite_sheet"));
}

void Leader::update() {
	if (framesUntilUpdate == 0) {
		curFrame = IwRandMinMax(0, numFrames-1);
		framesUntilUpdate = 24;
	} else {
		--framesUntilUpdate;
	}
}

unit_type Leader::getType(){
	return LEADER;
}

float Leader::getAngle(){
	return PI/2;
}
