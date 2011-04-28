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

#include"bullets.h"
#include"assets.h"
#include<SFML/Graphics.hpp>
#include<cstdlib>

sf::Clock   gclock;

cpBool b2w_begin(cpArbiter *arb,cpSpace *space,void *data)
{
    CP_ARBITER_GET_SHAPES(arb, a, b);
    Bullet *bullet = (Bullet*)b->data;
    //mark the bullet as dead
    bullet->status = Bullet::Status::Dead;

    return cpTrue;
}

Bullet9mm::Bullet9mm(GLuint *Texture)
{
    myDamage = 10.0f;
    myDuration = 5.0f;
    mySpeed = 700.0f;
    myBody = new physics::Rectangle;
    sprite = new Sprite("assets/sprite/ak47/ak47_bullet.sprh",Texture[AK47_BULLET]);
    status = Bullet::Status::Active;
}
Bullet9mm::~Bullet9mm(void)
{
    delete myBody;
}
void Bullet9mm::Initialize(void)
{
    myBody->BuildRect(1.0f,1.0f,1.0f,pos);
	myBody->Spawn(pos);
    myBody->SetShapeData(this);
    myBody->SetGroup(0x01);
    myBody->SetCollisionType(BULLETOBJECT_TYPE);

    cpVect s = cpv(pos.x,pos.y);
    cpVect e = cpv(des.x,des.y);
    cpFloat d = cpvdist(s,e);
    cpFloat t = d/mySpeed;

    //cpBodyApplyForce(&myBody.GetBodyDef(),cpv(0,0),v*10000.0f);

    myBody->Move(des, t);

    myStatus.val = Bullet::Status::Active;

    myStatus.lastup = gclock.GetElapsedTime();
    lastpos = pos;
	sprite->pos = pos;
}
const Bullet::Status& Bullet9mm::Update(void)
{
    float t = gclock.GetElapsedTime();
    myDuration -= t - myStatus.lastup;
    if(myDuration < 0){
        myStatus.val = Bullet::Status::Dead;
    }
    myStatus.lastup = t;
    pos = myBody->GetLocation();

    float a;
    glm::vec3 dn;
    dn = pos - lastpos;
    a = glm::degrees(-glm::atan(dn.y,dn.x));
    pos.x -= 2;
    pos.y += 5;

    sprite->angle.z = -a;

    sprite->pos = pos;
    return myStatus;
}
