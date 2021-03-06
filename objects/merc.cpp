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

#include "merc.h"
#include <SFML/System/Clock.hpp>
#include<cmath>
#include<stdlib.h>
#include<iostream>

static int lastdirection;

sf::Clock timer;
static float dbljumptimer;

////////////////////////////////////////////////////////////
void mercUpdateVelocity(cpBody* body,cpVect gravity,cpFloat damping,cpFloat dt)
{
    cpBodyUpdateVelocity(body,gravity,damping,dt);
    //body->v.y = cpfmin(body->v.y, 70);
    //body->v.y = cpfclamp(body->v.y,-600,150);
    body->v.x = cpfclamp(body->v.x,-600,600);
}

////////////////////////////////////////////////////////////
/// Initialize
/// Why is this even needed
////////////////////////////////////////////////////////////
void Merc::Initialize(void)
{
    direction = 0;
    lastdirection = 0;
    myBody.BuildCircle(20.0f,1.0f,myStatus.pPos.x,myStatus.pPos.y);

	//myBody.BuildRect(50,50,1.0f,myStatus.pos.x,myStatus.pos.y);
    myBody.SetGroup(0x01);
    myBody.SetCollisionType(MERC);
    myBody.SetShapeData(this);
    myBody.SetVelFunc(mercUpdateVelocity);

	myStatus.pShape = NULL;
    myStatus.pGroundShapes = NULL;
	myStatus.pValue = GameObject::Status::Dead;
}

////////////////////////////////////////////////////////////
/// Spawn self
////////////////////////////////////////////////////////////
void Merc::Spawn(const glm::vec3 &p_Pos)
{
	myBody.Spawn(p_Pos);
	myStatus.pShape = myBody.GetShape();

	myStatus.pValue = GameObject::Status::Active;
    myStatus.pAirborne = true;
    myStatus.pDoubleJump = true;
	myStatus.pGroundNormal = cpvzero;
	myStatus.pGroundShapes = 0;
	myStatus.pHealth = 100.0f;

	applyjump = false;
	jets = false;
    jumpheight = 0;
	dbljumptimer = 0;
	coast = 0;
}

////////////////////////////////////////////////////////////
/// Kill self
////////////////////////////////////////////////////////////
void Merc::Kill(void)
{
	myBody.Kill();
	myStatus.pValue = GameObject::Status::Dead;
	myStatus.pShape = NULL;
	myStatus.pGroundNormal = cpvzero;
	myStatus.pGroundShapes = 0;
}

////////////////////////////////////////////////////////////
/// Update function
////////////////////////////////////////////////////////////
GameObject::Status& Merc::Update(float dt)
{
    myStatus.pPos = myBody.GetLocation();
	//TODO: get rid of the old Generic Vector template
	myStatus.pVelocity = myBody.GetVelocity();

    cpBody *body = &myBody.GetBodyDef();
    cpVect groundNormal = myStatus.pGroundNormal;
	cpVect vel = cpBodyGetVel(body);

	//Apply jets
	if(jets){
		vel.y += 15.0f;
		myStatus.pAirborne = true;
	}
	jets = false;

	//Handling of directional keys, jump state

	//Jump state
    if(applyjump){
		//The below line is from "Player.cpp" demo in chipmunks
		//source code. Seems to work well so im keeping it here
        vel = cpvadd(vel, cpvmult(cpvslerp(groundNormal, cpv(0.0f,1.0f),0.75),0.4f*jumpheight));

		vel.x += 15.0f*direction;

		//Set some jump flags
		myStatus.pAirborne = true;
        applyjump = false;
    }
	else if(!direction && (groundNormal.y > 0.0f)){
		if(coast > 0.0f){
			coast-= dt;
		}
		else{
			//The controls have to be tight so
			//changing the speed to zero while not pressing
			//down any keys is a good way to keep that 'tight' feeling
			vel.x = 0.0f;

			//ground Normal with y less than about .6f is a steep wall
			//this is a diferent case and one would expect to slide of
			//something like this
			if(groundNormal.y > 0.5f)
				myStatus.pShape->surface_v = cpvzero;
		}
	}
	//Directional key states
	else {
		if((groundNormal.y > 0.0f)){
			//Ground velocity is accumulated
			cpVect r = cpvmult(cpvperp(groundNormal), MERC_HORIZONTALSURFACEMAG*direction);
			cpVect v = myStatus.pShape->surface_v + r;
			vel.x = cpfclamp(v.x, -400, 400);
			myStatus.pShape->surface_v = v;
			vel = -myStatus.pShape->surface_v;
		}
		else{
			//This little bit of code creates a nice effect i like
			//this makes the player 'slide' just a little bit after they
			//land providing for a smooth transition between flying and
			//landing/running
			myStatus.pShape->surface_v = -vel; //Yes, velocity has to be negative
		}

		//While in air its usefull to make the player loose some of
		//the controll they are used to while on the ground (1/5th)
		if(myStatus.pAirborne){
			//NOTE: body velocity is pretty different from surface_v(keep in mind)
			vel.x += 5.0f*direction;
		}
	}

	//Reset flags
    direction = 0;
	jets = false;

	cpBodySetVel(body, vel);

	//Done
    return myStatus;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Merc::Move(glm::vec3 pos,float time)
{
    glm::vec3 v = myBody.GetVelocity();
    if(v.x < max_v::x && v.y < max_v::y )
        myBody.Move(pos,time);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Merc::Impulse(glm::vec3 i, float p_x, float p_y)
{
	cpBodyApplyImpulse(&myBody.GetBodyDef(), cpv(i.x,i.y),cpv(p_x,p_y));
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Merc::Jump(float howhigh)
{
    if(!myStatus.pAirborne)
    {
		applyjump = true;
		doublejump = true;
        jumpheight = howhigh;
		dbljumptimer = timer.GetElapsedTime();
    } else if(doublejump && timer.GetElapsedTime() - dbljumptimer > 0.5f){
		//Double jump logic
		cpBody *body = &myBody.GetBodyDef();
		float dt = body->v.y;

		float jmod = 0.0f; //jump modifier
		int dir = (body->v.y > 0.0f) ? 1 : -1;

		dt = cpfclamp(dt, -100, 100);

		jmod = (100*dir) / dt;

		//Perfect double jump
		if(dt > -15.0f && dt < 15.0f)
			jmod = 0.7f;

		if(jmod < 2.5f)
		{
			applyjump = true;
			doublejump = false;
			jumpheight = jumpheight/jmod;
		}
	}
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Merc::Right()
{
    direction = 1;
	coast = 0.3f;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Merc::Left()
{
    direction = -1;
	coast = 0.3f;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Merc::Jet()
{
    jets = true;
}
