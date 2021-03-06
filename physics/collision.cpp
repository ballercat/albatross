/*
    ********************************************
    *   Albatross - A 2D multiplayer shooter   *
    ********************************************
    Copyright (C) 2011  Arthur Buldauskas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "collision.h"
#include "bullets.h"
#include <SFML/Graphics.hpp>
#include <iostream>

static sf::Clock _cTimer;

using namespace collision;

////////////////////////////////////////////////////////////
/// Projectiles
////////////////////////////////////////////////////////////
cpBool Begin::BulletWorld(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);

	Bullet *bullet = (Bullet*)b->data;
	//mark the bullet object as dead
	bullet->Status = BulletStatus::Dead;

	bullet->Hit = Bullet::HitSpot( bullet->pos,
							_cTimer.GetElapsedTime(),
							bullet->Type );

	return cpTrue;
}

cpBool Begin::BulletObject(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);

	Bullet *bullet = (Bullet*)a->data;
	GameObject *object = (GameObject*)b->data;

	object->Damage(bullet->pDamage);

	return cpFalse;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
cpBool Begin::ExplosiveWorld(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);

	Explosive *explosive = (Explosive*)a->data;

	explosive->Hit = Bullet::HitSpot( explosive->pos ,
							  _cTimer.GetElapsedTime(),
							  explosive->Type );

	cpSpaceAddPostStepCallback(space, PostStep::Explode, explosive, NULL);

	return cpTrue;
}

cpBool Begin::ExplosiveObject(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);

	Explosive *explosive = (Explosive*)a->data;

	explosive->Hit = Bullet::HitSpot( explosive->pos,
									  _cTimer.GetElapsedTime(),
									  explosive->Type );

	cpSpaceAddPostStepCallback(space, PostStep::Explode, explosive, NULL);

	return cpTrue;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
cpBool Begin::ExplosionObject(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);
	Explosive *explosive = (Explosive*)a->data;
	GameObject *object = (GameObject*)b->data;

	glm::vec3 p_obj = object->GetStatus().pPos;
	glm::vec3 p_aoa = explosive->explosion_pos;

	cpContactPointSet cnt = cpArbiterGetContactPointSet(arb);

	cpVect bloc;

	bloc.x = cnt.points[0].point.x - p_obj.x;
	bloc.y = cnt.points[0].point.y - p_obj.y;

	float dx = cnt.points[0].point.x - p_aoa.x ;
	float dy = cnt.points[0].point.y - p_aoa.y ;
	float damage = 100.0f - glm::distance(p_obj, p_aoa);//250.0f;

	if(glm::abs(dx) > 40.0f)
		dx = (glm::abs(dx)/dx * 52.0f) - dx;

	if(glm::abs(dy) > 40.0f)
		dy = (glm::abs(dy)/dy * 52.0f) - dy;
	else
		dy *= 2.0f;

	object->Impulse(glm::vec3((dx*10),(dy*10), 0), bloc.x, bloc.y );
	object->Damage(damage);

	explosive->Status = BulletStatus::Dead;

	return cpFalse;
}

////////////////////////////////////////////////////////////
/// Weapon
////////////////////////////////////////////////////////////
cpBool Begin::WeaponWorld(BEGINVARS)
{
	return cpTrue;
}

cpBool Begin::WeaponPlayer(BEGINVARS)
{
	return cpTrue;
}

////////////////////////////////////////////////////////////
///Player
////////////////////////////////////////////////////////////
cpBool Begin::PlayerWorld(BEGINVARS)
{
    CP_ARBITER_GET_SHAPES(arb, a, b);
    GameObject::Status *player = &((GameObject*)a->data)->myStatus;

    cpVect n = cpvneg(cpArbiterGetNormal(arb,0));
    if(n.y > 0.0f){
       player->pGroundShapes++;
    }

	return cpTrue;
}



///PreSolve handlers

cpBool PreSolve::PlayerWorld(PRESOLVEVARS)
{
    CP_ARBITER_GET_SHAPES(arb, a, b);
    GameObject::Status *player = &((GameObject*)a->data)->myStatus;
    if(cpArbiterIsFirstContact(arb)){
        a->u = player->u;

        cpVect n = cpvneg(cpArbiterGetNormal(arb, 0));
        if(n.y >= player->pGroundNormal.y){
            player->pGroundNormal = n;
        }
        if(n.y > 0.50f){
            //0.67f is about as steep a wall that you can jump off
            player->pAirborne = false;
            player->pDoubleJump = false;
        }
    }
    return cpTrue;
}

void Post::BulletWorld(POSTSOLVEVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);

	Bullet *bullet = (Bullet*)b->data;
	//mark the bullet object as dead
	cpVect imp = cpArbiterGetNormal(arb, 0);

	//Get bullet vector
	glm::vec3 h = bullet->lastpos - bullet->pos;
	//Convert normal vector
	glm::vec3 n = glm::vec3(imp.x, imp.y, 0);
	//find theta of ground normal and bullet vector
	float t = glm::degrees(glm::atan(n.y, n.x) - glm::atan(h.y, h.x));
	//Apply state to the bullet
	if(glm::abs(t) < 84.0f)
		bullet->Status = BulletStatus::Dead;

	bullet->Hit = Bullet::HitSpot( bullet->pos,
							_cTimer.GetElapsedTime(),
							bullet->Type );

	cpVect i = cpArbiterTotalImpulse(arb);

}

///Separate Handlers

//Player
void Separate::PlayerWorld(SEPARATEVARS)
{
    CP_ARBITER_GET_SHAPES(arb, a, b);
    GameObject::Status *player = &((GameObject*)a->data)->myStatus;

    player->pGroundShapes--;

    if(player->pGroundShapes <= 0){
		player->pGroundShapes = 0;
        a->u = 0.0f;
        player->pGroundNormal = cpvzero;
        player->pAirborne = true;
    }
}

void PostStep::Explode(POSTSTEPVARS)
{
	Explosive *explosive = (Explosive*)(obj);
	explosive->Explode();
}
