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

static Merc::Creator _soldiercreator;
static object::Weapon::Creator _weaponcreator;

using namespace object;

GameObject::Status& Weapon::Update(float unused)
{
	myStatus.pos = m_phBody.GetLocation();	
    rot_dAngle = m_phBody.GetBodyDef().a;	
}

void Weapon::Initialize(void)
{
	m_phBody.BuildRect(15.0f, 5.0f, 1.0f, myStatus.pos.x, myStatus.pos.y);
	m_phBody.SetGroup(0x01);
	m_phBody.SetCollisionType(WEAPONOBJECT_TYPE);
	
	myStatus.val = GameObject::Status::Active;
}

void Weapon::Spawn(glm::vec3 p_Pos)
{
	m_phBody.Spawn(p_Pos);
	m_phBody.SetShapeData(&myStatus);
}



