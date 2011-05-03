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
cpBool
begin(cpArbiter *arb, cpSpace *space, void *ignore)
{
    CP_ARBITER_GET_SHAPES(arb, a, b);
    GameObject::Status *player = &((GameObject*)a->data)->myStatus;

    cpVect n = cpvneg(cpArbiterGetNormal(arb,0));
    if(n.y > 0.0f){
        cpArrayPush(player->groundShapes, b);
    }
    return cpTrue;
}

////////////////////////////////////////////////////////////
cpBool
preSolve(cpArbiter *arb, cpSpace *space, void *ignore)
{
    CP_ARBITER_GET_SHAPES(arb, a, b);
    GameObject::Status *player = &((GameObject*)a->data)->myStatus;
    if(cpArbiterIsFirstContact(arb)){
        a->u = player->u;

        cpVect n = cpvneg(cpArbiterGetNormal(arb, 0));
        if(n.y >= player->groundNormal.y){
            player->groundNormal = n;
        }
        if(n.y > 0.50f){
            //0.67f is about as steep a wall that you can jump off
            player->Airborne = false;
            player->DoubleJump = false;
        }
    }
    return cpTrue;
}

////////////////////////////////////////////////////////////
void
separate(cpArbiter *arb, cpSpace *space, void *ignore)
{
    CP_ARBITER_GET_SHAPES(arb, a, b);
    GameObject::Status *player = &((GameObject*)a->data)->myStatus;

    cpArrayDeleteObj(player->groundShapes, b);

    if(player->groundShapes->num == 0){
        a->u = 0.0f;
        player->groundNormal = cpvzero;
        player->Airborne = true;
    }

}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Merc::Initialize(void)
{
    direction = 0;
    lastdirection = 0;
    myBody.BuildCircle(20.0f,1.0f,myStatus.pos.x,myStatus.pos.y);
    myBody.Spawn(myStatus.pos);
	//myBody.BuildRect(50,50,1.0f,myStatus.pos.x,myStatus.pos.y);
    myBody.SetGroup(0x01);
    myBody.SetCollisionType(MERC);
    myBody.SetShapeData(this);
    myBody.GetBodyDef().velocity_func = mercUpdateVelocity;

    myStatus.shape = myBody.__dbg_get_shape();
    myStatus.val = GameObject::Status::Active;
    myStatus.Airborne = true;
    myStatus.DoubleJump = true;
	myStatus.groundNormal = cpvzero;
    applyjump = false;
	jets = false;
    jumpheight = 0;
	dbljumptimer = 0;

	coast = 0;
    id = 2;
    myStatus.groundShapes = cpArrayNew(0);
    cpSpaceAddCollisionHandler(	myBody.__dbg_get_space(),
								MERC,
								MAPPOLYGON,
								begin,preSolve,
								NULL,
								separate,
								NULL );
}

////////////////////////////////////////////////////////////
/// Update function
////////////////////////////////////////////////////////////
GameObject::Status& Merc::Update(float dt)
{
    myStatus.pos = myBody.GetLocation();
	//TODO: get rid of the old Generic Vector template
	myStatus.v = myBody.GetVelocity();

    cpBody *body = &myBody.GetBodyDef();
    cpVect groundNormal = myStatus.groundNormal;

	//Apply jets
	if(jets){
		body->v.y += 15.0f;
		myStatus.Airborne = true;
	}
	jets = false;

	//Handling of directional keys, jump state

	//Jump state
    if(applyjump){
		//The below line is from "Player.cpp" demo in chipmunks
		//source code. Seems to work well so im keeping it here
        body->v = cpvadd(body->v, cpvmult(cpvslerp(groundNormal, cpv(0.0f,1.0f),0.75),0.4f*jumpheight));

		body->v.x += 15.0f*direction;

		//Set some jump flags
		myStatus.Airborne = true;
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
			body->v.x = 0.0f;

			//ground Normal with y less than about .6f is a steep wall
			//this is a diferent case and one would expect to slide of
			//something like this
			if(groundNormal.y > 0.5f)
				myStatus.shape->surface_v = cpvzero;
		}
	}
	//Directional key states
	else {
		if((groundNormal.y > 0.0f)){
			//Ground velocity is accumulated
			cpVect r = cpvmult(cpvperp(groundNormal), MERC_HORIZONTALSURFACEMAG*direction);
			myStatus.shape->surface_v = myStatus.shape->surface_v + r;
			body->v = -myStatus.shape->surface_v;
		}
		else{
			//This little bit of code creates a nice effect i like
			//this makes the player 'slide' just a little bit after they
			//land providing for a smooth transition between flying and
			//landing/running
			myStatus.shape->surface_v = -body->v; //Yes, velocity has to be negative
		}

		//While in air its usefull to make the player loose some of
		//the controll they are used to while on the ground (1/5th)
		if(myStatus.Airborne){
			//NOTE: body velocity is pretty different from surface_v(keep in mind)
			body->v.x += 5.0f*direction;
		}
	}

	//Reset flags
    direction = 0;
	jets = false;

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
    if(!myStatus.Airborne)
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
