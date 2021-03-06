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
#include"types.h"
#include<SFML/Graphics.hpp>
#include<cstdlib>
#include<iostream>
#include<cmath>

sf::Clock   gclock;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
BulletPlain::BulletPlain()
{	
    Type = BULLET;
	myBody = new physics::Rectangle;
    Status = BulletStatus::Active;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
BulletPlain::~BulletPlain(void)
{
    delete myBody;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void BulletPlain::Initialize(void)
{
    myBody->BuildRect(1.0f,1.0f,1.0f,pos);
	myBody->Spawn(pos);
    myBody->SetShapeData(this);
    myBody->SetGroup(0x03);
    myBody->SetCollisionType(BULLET);

    cpVect s = cpv(pos.x,pos.y);
    cpVect e = cpv(des.x,des.y);
    cpFloat d = cpvdist(s,e);
	glm::vec3 test = des - pos;
	cpFloat t = 0;
	if(abs(startV.x) > 0.99f && startV.x/test.x > 0.0f)
		t = d/(pSpeed+abs(startV.x/1.5f));
	else
		t = d/pSpeed;

    myBody->Move(des, t);

    myStatus.val = BulletStatus::Active;

	glm::vec3 dn = pos - des;
	pAngle = -glm::degrees(-glm::atan(dn.y, dn.x));

    myStatus.lastup = gclock.GetElapsedTime();
    lastpos = pos;

}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
const BulletStatus& BulletPlain::Update(void)
{
    float t = gclock.GetElapsedTime();
    myDuration -= t - myStatus.lastup;
    if(myDuration < 0){
        myStatus.val = BulletStatus::Dead;
    }

	lastpos = pos;
    pos = myBody->GetLocation();

	pVelocity = myBody->GetVelocity();

	glm::vec3 dn = lastpos - pos;
    pAngle = -glm::degrees(-glm::atan(dn.y, dn.x));

	myStatus.lastup = t;

    return myStatus;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
Explosive::Explosive(void)
{
	Type = EXPLOSIVE;
	m_isExploded = false;

	m_phProjectile = NULL;
	m_phExplosion = NULL;

	m_phProjectile = new physics::Rectangle;

	Status = BulletStatus::Active;

	explosion_pos = glm::vec3(0,0,0);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
Explosive::~Explosive(void)
{
	delete m_phProjectile;
	delete m_phExplosion;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Explosive::Initialize(void)
{
	m_phProjectile->BuildRect(3.0f, 3.0f, 1000.0f, pos);
	m_phProjectile->Spawn(pos);
	m_phProjectile->SetShapeData(this);
	m_phProjectile->SetGroup(0x03);
	m_phProjectile->SetCollisionType(EXPLOSIVE);

	cpVect s = cpv(pos.x, pos.y);
	cpVect e = cpv(des.x, des.y);
	cpFloat d = cpvdist(s, e);
	glm::vec3 test = des - pos;
	cpFloat t = 0;
	if(abs(startV.x) > 0.99f && startV.x/test.x > 0.0f)
		t = d/(pSpeed+abs(startV.x/1.5f));
	else
		t = d/pSpeed;


	cpBodySetVel(&m_phProjectile->GetBodyDef(), cpv(startV.x,startV.y));
	m_phProjectile->Move(des, t);

	myStatus.val = BulletStatus::Active;

	myStatus.lastup = gclock.GetElapsedTime();
	lastpos = pos;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
const BulletStatus& Explosive::Update(void)
{
	pos = m_phProjectile->GetLocation();
	
	//if exploded kill self
	if(m_isExploded){
		Status = BulletStatus::Dead;
		return myStatus;
	}

	pVelocity = m_phProjectile->GetVelocity();


	//cpBodySetVel(&m_phProjectile->GetBodyDef(), cpv(pVelocity.x, pVelocity.y));

	pAngle = R2D(m_phProjectile->GetBodyDef().a);

	return myStatus;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Explosive::Explode(void)
{
	if(m_isExploded)
		return;
	
	m_phExplosion = physics::Static::Object::Circle(pos, 50.0f);
	m_phExplosion->SetShapeData(this);
	m_phExplosion->SetCollisionType(EXPLOSION);
	m_phExplosion->SetGroup(0x02);

	explosion_pos = pos;
	m_isExploded = true;
}

