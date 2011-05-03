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

#ifndef BULLETS_HEADER_GUARD
#define BULLETS_HEADER_GUARD

#include "objects.h"
#include "physics.h"
#include "sprite.h"

class Bullet
{
public:
    struct Status
    {
        enum
        {
            Unknown = 0,
            Active,
            Inactive,
            Dead
        };
        glm::vec3   des; ///> destination
        glm::vec3   pos; ///> postion
        int         val; ///> status value
        int         parentid;

        ///Default constructor
        Status(void)
        {
            val = Unknown;
        }

        float       lastup;
    };

    virtual ~Bullet() {};
    virtual void Initialize(void) = 0;
    virtual const Bullet::Status& Update(void) = 0;
    virtual void Draw(void)
    {
        bSprite->Draw();
    }

    void Spawn(int parent,int x, int y, glm::vec3 d)
    {
        myStatus.pos = glm::vec3(x,y,0);
        myStatus.des = d;
        myStatus.parentid = parent;
    }

    inline void SetStatusValue(int status)
    {
        myStatus.val = status;
    }

    inline void SetPosition(int x,int y)
    {
        myStatus.pos = glm::vec3(x,y,0);
    }
    inline void SetPosition(glm::vec3 ps)
    {
        myStatus.pos = ps;
    }
    const glm::vec3& GetPosition(void) const
    {
        return myStatus.pos;
    }

    /* Bullet Types */
    //TODO: replace static types with dynamic instantiation
    enum
    {
        TBlank = 0,
        T9mm
    };
public:
	////////////////////////////////////////////////////////////
	/// Hit position of the bullet
	////////////////////////////////////////////////////////////
	struct HitSpot{
	public:
		////////////////////////////////////////////////////////////
		/// Constructor
		////////////////////////////////////////////////////////////
		HitSpot(){
			Pos = glm::vec3();
			Timestamp = 0.0f;
			Drawstamp = 0.0f;
		}

		HitSpot(glm::vec3& p_Pos, float p_Time, int& p_Type)
		{
			Pos = p_Pos;
			Timestamp = p_Time;
			Drawstamp = p_Time;
			Type = p_Type;
		}

		//Data
		glm::vec3 	Pos;
		float		Timestamp;
		float 		Drawstamp;
		int			Type;
	};

	//Hit position
	HitSpot Hit;

public:
	////////////////////////////////////////////////////////////
	/// Data
	////////////////////////////////////////////////////////////
    glm::vec3 pos;
    glm::vec3 lastpos;
    glm::vec3 des;
    glm::vec3 damage;
	glm::vec3 Velocity;
    int       status;
	int 		Type;
	Sprite*     bSprite;

protected:
    Bullet::Status  myStatus;
    float       myDuration;
    float       mySpeed;
    float       myDamage;
};

class Bullet9mm :
    public Bullet
{
public:
    Bullet9mm(GLuint *Texture);
    virtual ~Bullet9mm(void);
    virtual void Initialize(void);
    virtual const Bullet::Status& Update(void);

private:
    physics::Rectangle  *myBody;
};

////////////////////////////////////////////////////////////
/// Explosive
////////////////////////////////////////////////////////////
class Explosive : public Bullet
{
public:
	////////////////////////////////////////////////////////////	
	/// Constructor 			
	////////////////////////////////////////////////////////////		
	Explosive(GLuint *Texture);

public:
	////////////////////////////////////////////////////////////	
	/// Destructor	
	////////////////////////////////////////////////////////////	
	~Explosive(void);

public:
	////////////////////////////////////////////////////////////		
	/// Initializer
	////////////////////////////////////////////////////////////		
	virtual void Initialize(void);

	////////////////////////////////////////////////////////////	
	/// Update function
	////////////////////////////////////////////////////////////	
	virtual const Bullet::Status& Update(void);

	void Explode(void);

public:
	glm::vec3 explosion_pos;
	
private:
	////////////////////////////////////////////////////////////	
	/// Physics body	
	////////////////////////////////////////////////////////////
	physics::Rectangle *m_phProjectile; //Projectile object
	physics::Static::Object		*m_phExplosion; //Explosion

	//Explosion state
	bool m_isExploded;
};

#endif //BULLETS_HEADER_GUARD
