#include "worldobject.h"

WorldObject::WorldObject(Game* game) :
game(game), current_texture_index(0)
{ }

WorldObject::WorldObject(CIwFVec2 position, Game* game) 
	: game(game), current_texture_index(0)
{ 
    setPosition(position);
}

WorldObject::WorldObject(const WorldObject& newObj) 
	: game(newObj.game), r(newObj.r), theta(newObj.theta), current_texture_index(newObj.current_texture_index),
	texture_names(newObj.texture_names)
{
    setPosition(newObj.position);
}

unsigned int WorldObject::getTextureName() {
	return texture_names[current_texture_index];
}

bool WorldObject::operator<(const WorldObject& u) const{
	return theta < u.theta;
}

void WorldObject::setPosition(const CIwFVec2& newPosition) {
	position = newPosition;
	CIwFVec2 polar(position);
	polarize(polar);
	r = polar.x;
	theta = polar.y;
}

CIwFVec2 WorldObject::getPosition() {
	return position; 
}

void WorldObject::setPolarPosition(float _r, float _theta){
	r = _r;
	theta = _theta;
    while(theta > 2 * PI) theta -= 2*PI;
	
	CIwFVec2 v = CIwFVec2(r, theta);
	polarToXY(v);
    
	position.x = v.x;
	position.y = v.y;
}


float WorldObject::getR() {
	return r;
}

float WorldObject::getTheta() {
	return theta;
}

Game* WorldObject::getGame() {
	return game;
}

