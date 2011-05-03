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
#include<iostream>

sf::Clock   gclock;

Bullet9mm::Bullet9mm(GLuint *Texture)
{
	Type = BULLET;
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
    myBody->SetCollisionType(BULLET);

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

Explosive::Explosive(GLuint *Texture)
{
	Type = EXPLOSIVE;
	myDamage = 100.0f;
	myDuration = 5.0f;
	mySpeed = 410.0f;

	m_isExploded = false;

	m_phProjectile = NULL;
	m_phExplosion = NULL;

	m_phProjectile = new physics::Rectangle;
	sprite = new Sprite("assets/sprite/merc/jetflame.sprh", Texture[JET_FLAME]);
	status = Bullet::Status::Active;

	explosion_pos = glm::vec3(0,0,0);
}

Explosive::~Explosive(void)
{
	delete m_phProjectile;
	delete m_phExplosion;
	delete sprite;
}

void Explosive::Initialize(void)
{
	m_phProjectile->BuildRect(3.0f, 3.0f, 10.0f, pos);
	m_phProjectile->Spawn(pos);
	m_phProjectile->SetShapeData(this);
	m_phProjectile->SetGroup(0x01);
	m_phProjectile->SetCollisionType(EXPLOSIVE);

	cpVect s = cpv(pos.x, pos.y);
	cpVect e = cpv(des.x, des.y);
	cpFloat d = cpvdist(s, e);
	cpFloat t = d/mySpeed;

	m_phProjectile->Move(des, t);

	myStatus.val = Bullet::Status::Active;

	myStatus.lastup = gclock.GetElapsedTime();
	lastpos = pos;
	
}

const Bullet::Status& Explosive::Update(void)
{
	pos = m_phProjectile->GetLocation();
	
	//if exploded kill self
	if(m_isExploded){
		status = Bullet::Status::Dead;
		return myStatus;
	}
	sprite->pos = pos;

	return myStatus;
}

void Explosive::Explode(void)
{
	if(m_isExploded)
		return;
	

	m_phExplosion = physics::Static::Object::Circle(pos, 50.0f);
	m_phExplosion->SetShapeData(this);
	m_phExplosion->SetCollisionType(EXPLOSION);
	m_phExplosion->SetGroup(0x02);

	/*
	m_phAOA->BuildCircle(50.0f, 10.0f, pos.x, pos.y);
	m_phAOA->Spawn(pos);
	m_phAOA->SetShapeData(this);
	m_phAOA->SetGroup(0x02);
	m_phAOA->SetCollisionType(EXPLOSION);
	*/

	explosion_pos = pos;
	m_isExploded = true;
}

