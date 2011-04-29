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

#ifndef PLAYER_HEADER_GUARD
#define PLAYER_HEADER_GUARD

#include "merc.h"
#include "assets.h"
#include <vector>
#include <iostream>

////////////////////////////////////////////////////////////
///Player handler template-class
/// @param : <class T> can be any controllable object
////////////////////////////////////////////////////////////
template <class T>
class Player :
    public T
{

public:
	////////////////////////////////////////////////////////////
	///Constructor
	////////////////////////////////////////////////////////////	
	Player(glm::vec3 loc,GLuint *Texture)
	{
		T::Spawn(loc);
		T::Initialize();
		T::id = 2;
		
		rawsprite.push_back(new Sprite("assets/sprite/merc/still.sprh",Texture[PLAYER_TORSO]));
		rawsprite.push_back(new Sprite("assets/sprite/merc/run.sprh", Texture[PLAYER_LEGS]));
		rawsprite.push_back(new Sprite("assets/sprite/merc/jet.sprh", Texture[PLAYER_JETS]));
		rawsprite.push_back(new Sprite("assets/sprite/ak47/ak47.sprh", Texture[AK47]));

		jetflame = new Sprite("assets/sprite/merc/jetflame.sprh", Texture[JET_FLAME]);

		sparks = new Sprite("assets/sprite/ak47/ak47fire.sprh", Texture[AK47_FIRE]);

		torso = rawsprite[0];
		legs = rawsprite[1];
		weapon = rawsprite[3];

		shoot = false;
		running = false;
		flip = 0;

		m_Weapon = new object::Weapon();
		m_Weapon->Initialize();
		
		m_hasWeapon = true;

		//Jet flames
		jflame_alpha.assign(10,0.0f);
		jflame_pos.assign(10,glm::vec3());
		jflame_count = 0;
		
		//Timing
		Time = timer.GetElapsedTime();
		jflame_eraser = Time;
		jettimer = Time;
		flametimer = Time;
		jumptimer = Time;
		jumptime = Time;
		jumpstate = false;
	}
	////////////////////////////////////////////////////////////
	///Destructor
	////////////////////////////////////////////////////////////
	~Player()
	{
		if(rawsprite.size())
		{
			for(size_t k;k < rawsprite.size();k++){
				delete rawsprite[k];
				rawsprite[k] = NULL;
			}
		}
	}
public:
	////////////////////////////////////////////////////////////
	///Step
	/// @param cursor : vec3 location of the cursor on screen
	////////////////////////////////////////////////////////////
	void Step(glm::vec3& cursor)
	{
		float t = timer.GetElapsedTime();
		// Update internal state, get position
		GameObject::Status stat = T::Update(t - Time);
		ps = stat.pos;
		ps.z = 0;
		mVelocity = stat.v;

		// Adjust sprite angles
		angle = glm::vec3(0,0,0);

		// The player is drawn at 0,0(ALWAYS) so its pretty
		// easy to get the angle here
		angle.z = -glm::atan(cursor.y,cursor.x);
		angleR = angle.z;

		angle.z  = glm::degrees(angle.z);
		// Flip flags, adjust angle depending on flip
		if(cursor.x > 0){
			angle.x = 180;
			flip = -1;
		} else {
			flip = 1;
			//float x = cursor.x + d*2;
			angle.z = 180-angle.z;//(glm::degrees(-glm::atan(cursor.y,x)));
		}

		angle2cursor = angle.z;

		legs->angle.x = angle.x;
		torso->angle.x = angle.x;
		
		if(m_hasWeapon){
			weapon->angle.x = angle.x;
			weapon->angle.z = angle.z;
		}
		
		ps.x = int(ps.x);
		ps.y = int(ps.y);

		//Timer updates, and flag updates
		Time = t;
		shoot = false;

		if(jumpstate){
			if((Time - jumptime) >= 0.45f){
				T::Jump( (Time - jumptime) * 1500.0f );
				jumpstate = false;
			}
		}
	}

	////////////////////////////////////////////////////////////
	///Draw self
	/// @param interpolate: float, time to interpolate
	////////////////////////////////////////////////////////////
	void Draw(float interpolate)
	{
		float dt = Time - jflame_eraser;
		jflame_eraser = Time;

		for(size_t k = 0; k < 10; k++){
			if(jflame_alpha[k] > 0.0f){
				jetflame->color.a = jflame_alpha[k];
				jetflame->pos = jflame_pos[k];
				jetflame->angle.z += dt*80.0f;
				jetflame->Draw();

				jetflame->pos.x += 3*flip;
				jetflame->pos.y += 8.0f;
				jetflame->Draw();

				jflame_alpha[k] -= dt;
				jflame_pos[k].y -= dt*50;
			}
		}

		//Interpolate
		if(interpolate > 0.0f){
			ipos = ps + (mVelocity * interpolate);
			_updatePositions(ipos);
		}
		else {
			_updatePositions(ps);
		}

		if(running){
			legs->Draw();		
		}
		else{
			torso->Draw();
		}

		weapon->Draw();
		torso->Step();

		if(sparkcounter > 0.0f){
			sparks->Draw();
			sparkcounter -= Time - sparktimer;
		}

		//Reset sprite pointers
		rawsprite[1]->pos = legs->pos;
		legs = rawsprite[1];

		if(T::myStatus.v.x == 0){
			running = false;

		}
		else{
			float r = (T::myStatus.v.x > 0) ? 1 : -1;
			legs->mSpeed = (r * 15.0f)/(T::myStatus.v.x);
			legs->Step();
		}
	}
public:
	////////////////////////////////////////////////////////////
	///Jets pass trough
	////////////////////////////////////////////////////////////
	virtual void Jet(void)
	{
		rawsprite[2]->pos = legs->pos;
		legs = rawsprite[2];
		legs->Step();
		running = true;

		if(Time - jettimer > 0.001){
			T::Jet();
			jettimer = Time;
		}

		if(Time - flametimer > 0.03){
			jflame_count++;
			if(jflame_count > 10){
				jflame_count = 1;
			}
			jflame_alpha[jflame_count-1] = 1.0f;
			jflame_pos[jflame_count-1] = glm::vec3(ps.x+(12*flip),ps.y-15,0.0f);

			flametimer = Time;
		}
	}

	////////////////////////////////////////////////////////////
	///Right move pass trough
	////////////////////////////////////////////////////////////
	virtual void Right(void)
	{
		legs->Step();
		running = true;

		if(Time - movetimer > 0.001f){
			T::Right();
			movetimer = Time;
		}
	}

	////////////////////////////////////////////////////////////
	///Left move pass trough
	////////////////////////////////////////////////////////////
	virtual void Left(void)
	{
		legs->Step();
		running = true;

		if(Time - movetimer > 0.001f){
			T::Left();
			movetimer = Time;
		}
	}

	////////////////////////////////////////////////////////////
	///Jump
	////////////////////////////////////////////////////////////
	void jumpBegin()
	{
		if((Time - jumptimer) >= 0.001f){
			if(!jumpstate){
				jumpstate = true;
				jumptime = Time;
			}
		}

		jumptimer = Time;
	}

	////////////////////////////////////////////////////////////
	///
	////////////////////////////////////////////////////////////
	void jumpEnd()
	{
		if(jumpstate){
			if((Time - jumptime) >= 0.050f){
				T::Jump( (Time - jumptime) * 1000.0f );
			}
		}

		jumpstate = false;
	}
	////////////////////////////////////////////////////////////
	///
	////////////////////////////////////////////////////////////
	void Shoot(void)
	{
		if(!m_hasWeapon)
			return;
		sparktimer = Time;
		sparkcounter = 0.35f;
	}

	////////////////////////////////////////////////////////////
	/// Throw whats in your hands
	////////////////////////////////////////////////////////////
	inline void Throw(void)
	{
		if(!m_hasWeapon)
			return;
		glm::vec3 spawn = ipos + glm::vec3(-30*flip,30,0);
		m_Weapon->Spawn(spawn);
		glm::vec3 impulse(-1*flip, 1, 0);
		if(flip < 0)
			m_Weapon->Impulse(impulse/2.0f,7,2);
		else
			m_Weapon->Impulse(impulse,1*flip,1*flip);	
		m_hasWeapon = false;
	}

	inline void Pickup(object::Weapon *weapon)
	{
		if(m_hasWeapon)
			return;
		m_Weapon = weapon;
		m_hasWeapon = true;
	}

public:
	////////////////////////////////////////////////////////////
	///Sprite Data
	////////////////////////////////////////////////////////////
	Sprite *torso;
	Sprite *legs;
	Sprite *weapon;
	Sprite *sparks;

	std::vector<Sprite*> rawsprite;

public:
	////////////////////////////////////////////////////////////
	///Physical Data
	////////////////////////////////////////////////////////////
	glm::vec3 barrel; //gun barrel
	glm::vec3 angle;
	glm::vec3 ps;
	glm::vec3 ipos;
	bool running;

private:
	////////////////////////////////////////////////////////////
	///Update sprite positions
	////////////////////////////////////////////////////////////
	void _updatePositions(glm::vec3& npos)
	{
		//Adjust position
		if(m_hasWeapon){
			weapon->pos = npos; weapon->pos.y -= 8;
			weapon->pos.x += 2*flip;
		} 
		else {
			m_Weapon->Update(0.0f);
			weapon->pos = m_Weapon->GetStatus().pos;
			weapon->angle.z = m_Weapon->rot_dAngle;
		}
		
		legs->pos = npos; legs->pos.y += 3;
		torso->pos = npos; torso->pos.y += 3;

		barrel.x = 21*(glm::cos(-angleR)) + npos.x;// + 4*flip;
		barrel.y = 21*(glm::sin(-angleR)) + npos.y - 10;

		sparks->pos = barrel;
	}

private:
	////////////////////////////////////////////////////////////
	///Timing
	////////////////////////////////////////////////////////////
	sf::Clock timer;
	float Time;
	float flametimer;
	float movetimer;
	float jettimer;
	float sparktimer;
	float sparkcounter;

private:
	////////////////////////////////////////////////////////////
	///Jump handling
	////////////////////////////////////////////////////////////
	float 	jumptimer;
	float 	jumptime;
	float	jumpcounter;
	bool	jumpstate;

private:
	////////////////////////////////////////////////////////////
	///Data
	////////////////////////////////////////////////////////////
	int flip;
	glm::vec3 mVelocity;
	float angle2cursor; //zangle relative to cursor position
	float angleR; //angle to cursor in radians
	bool shoot;
		
private:
	////////////////////////////////////////////////////////////
	///Jet flames handling data
	////////////////////////////////////////////////////////////
	Sprite *jetflame;
	std::vector<float> jflame_alpha;
	std::vector<glm::vec3> jflame_pos;
	size_t	jflame_count;
	float jflame_eraser;

private:
	////////////////////////////////////////////////////////////
	/// Weapon data
	////////////////////////////////////////////////////////////	
	int m_WeaponID;
	object::Weapon *m_Weapon;
	bool m_hasWeapon;
};

typedef Player<Merc> MercPlayer;
//typedef Player MercPlayer;
#endif //PLAYER_HEADER_GUARD
