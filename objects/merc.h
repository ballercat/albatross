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

#ifndef MERC_HEADER_GUARD
#define MERC_HEADER_GUARD

#include "objects.h"

#define MERC_HORIZONTALSURFACEMAG	10.0f
#define MERC_HORIZONTALAIRMAG		5.00f

class Merc :
    public GameObject
{
public:
    virtual void Initialize(void);
    virtual GameObject::Status& Update(float dt);

    //Movement & Velocity
    virtual void Move(glm::vec3 pos,float time);
    virtual void Impulse(glm::vec3 i, float p_x, float p_y);
    virtual void Jump(float);
    virtual void Right();
    virtual void Left();
    virtual void Jet(void);

	virtual void Spawn(const glm::vec3 &p_Pos);
	virtual void Kill(void);

public:
	float	jumpheight;
	bool	applyjump;
    bool    doublejump;
	bool	jets;
	float 	direction;

private:
	physics::Circle myBody;

	
private:
    float   airtime;
	float	coast;
private:
    struct max_v
    {
        enum{ x = 10, y = 5 };
    };
};

#endif //MERC_HEADER_GUARD
