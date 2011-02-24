#include "icing.h"
#include "game.h"

Icing::Icing(const CIwFVec2 position, Game* game, Player* owner) : owner(owner), scale(0.02f), WorldObject(position, game)  { 
	angle = TO_RADIANS(IwRandMinMax(0, 360));
	texture_names.push_back(IwHashString("icing"));
}

Icing::Icing(const Icing& newIcing) : owner(newIcing.owner), scale(0.02f), WorldObject(newIcing) {
}

Player* Icing::getOwner() {
	return owner;
}

void Icing::display() {
    if(scale < ICING_FINAL_SCALE) scale += 0.04f;

	IwGxSetColStream(owner->getColors(), 4);
	renderImageWorldSpace(position, angle, scale, 256, game->getRotation(), 0, 1, .2f);
}
