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

#include "objects.h"
#include "merc.h"
#include "types.h"

static Merc::Creator _soldiercreator;
static object::Weapon::Creator _weaponcreator;

using namespace object;

GameObject::Status& Weapon::Update(float unused)
{
	myStatus.pos = m_phBody->GetLocation();	
    rot_dAngle = R2D(m_phBody->GetBodyDef().a);	
	
	return myStatus;
}

void Weapon::Initialize(void)
{
	m_phBody = NULL;
}

void Weapon::Spawn(glm::vec3 p_Pos)
{
	delete m_phBody;
	m_phBody = NULL;
	
	m_phBody = new physics::Rectangle;
	m_phBody->BuildRect(15.0f, 5.0f, 0.01f, myStatus.pos.x, myStatus.pos.y);
	myStatus.val = GameObject::Status::Active;
	m_phBody->Spawn(p_Pos);
	m_phBody->SetShapeData(this);
	m_phBody->SetGroup(0x02);
	m_phBody->SetCollisionType(WEAPONOBJECT_TYPE);
}

void Weapon::Impulse(glm::vec3 p_Imp)
{
	cpBodyApplyImpulse(&m_phBody->GetBodyDef(), cpv(p_Imp.x,p_Imp.y), cpv(1,1));	
}

void Weapon::Impulse(glm::vec3 p_Imp, float p_x, float p_y)
{
	cpBodyApplyImpulse(&m_phBody->GetBodyDef(), cpv(p_Imp.x, p_Imp.y), cpv(p_x, p_y));
}
