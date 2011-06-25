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

#include "player.h"

////////////////////////////////////////////////////////////
/// Default ctor
////////////////////////////////////////////////////////////
Player::Player()
{
	pTime.Current	= 0.0f;
	pTime.Jump		= ATimer();
	pTime.Move		= ATimer();
	pTime.Jet		= ATimer();
	pTime.Shoot		= ATimer();

	mRunningSprite	= Sprite("assets/sprite/merc/run.sprh");
	mIdleSprite		= Sprite("assets/sprite/merc/still.sprh");
	mJetSprite		= Sprite("assets/sprite/merc/jet.sprh");	
	mJetFlameSprite = Sprite("assets/sprite/merc/jetflame.sprh");

	mBody			= NULL;
	mSparks			= NULL;

	pWeaponPos = pBarrelPos = pAngle = pPos = pIPos = glm::vec3(0.f,0.f,0.f);

	pWeaponID		= 0;
	pWeapon			= object::Weapon();
	pHasWeapon		= false;

	mFlip			= 1;
	mVelocity		= glm::vec3(0.f,0.f,0.f);
	mAngleCursor	= 0.0f;
	mAngleR			= 0.0f;

	mShoot			= false;
	mJumpState		= false;
	mRunning		= false;
	pSpawned		= false;
	mJetAmmount		= 100;
	pJetCounter		= 0;

	pHealth			= 100.0f;

	MercObject.Initialize();
}

////////////////////////////////////////////////////////////
///Constructor
////////////////////////////////////////////////////////////
Player::Player(GLuint *Texture)
{
	pTime.Current	= 0.0f;
	pTime.Jump		= ATimer();
	pTime.Move		= ATimer();
	pTime.Jet		= ATimer();
	pTime.Shoot		= ATimer();

	mRunningSprite	= Sprite("assets/sprite/merc/run.sprh", Texture[PLAYER_LEGS]);
	mIdleSprite		= Sprite("assets/sprite/merc/still.sprh", Texture[PLAYER_TORSO]);
	mJetSprite		= Sprite("assets/sprite/merc/jet.sprh", Texture[PLAYER_JETS]);	
	mJetFlameSprite = Sprite("assets/sprite/merc/jetflame.sprh", Texture[JET_FLAME]);

	mBody			= &mIdleSprite;
	mSparks			= &mJetFlameSprite;


	pAngle			= glm::vec3(0.f,0.f,0.f); 


	pWeaponID		= 0;
	pWeapon			= object::Weapon();
	pHasWeapon		= false;

	mFlip			= 1;
	mVelocity		= glm::vec3(0.f,0.f,0.f);
	mAngleCursor	= 0.0f;
	mAngleR			= 0.0f;

	mShoot			= false;
	mJumpState		= false;
	mRunning		= false;
	pSpawned		= false;
	pHealth			= 100.0f;
	mJetAmmount		= 100;
	pJetCounter		= 0;

	mJetFlames.assign(10, FlameData());
	mFC = 0;

	MercObject.Initialize();
}

////////////////////////////////////////////////////////////
///Step
/// @param cursor : vec3 location of the cursor on screen
////////////////////////////////////////////////////////////
void Player::Step(glm::vec3& cursor, float& p_Time)
{
	if(!pSpawned)
		return;

	// Update internal state, get position
	GameObject::Status stat = MercObject.Update(p_Time - pTime.Current);
	pPos = stat.pPos;
	pPos.z = 0;
	mVelocity = stat.pVelocity;
	pHealth = stat.pHealth;

	// Adjust sprite angles
	pAngle = glm::vec3(0,0,0);

	// The player is drawn at 0,0(ALWAYS*) so its pretty
	// easy to get the angle here
	//(*not in the menu)
	pAngle.z = -glm::atan(cursor.y,cursor.x);
	mAngleR = pAngle.z;

	pAngle.z  = glm::degrees(pAngle.z);
	// Flip flags, adjust angle depending on flip
	if(cursor.x > 0){
		pAngle.x = 180;
		mFlip = -1;
		pAngle.z = -pAngle.z;
	} else {
		mFlip = 1;
		//float x = cursor.x + d*2;
		pAngle.z = 180-pAngle.z;//(glm::degrees(-glm::atan(cursor.y,x)));
	}

	mAngleCursor = pAngle.z;

	//Timer updates, and flag updates
	pTime.Current = p_Time;
	mShoot = false;

	if(mJumpState){
		if((pTime.Current - pTime.Jump.Stamp) >= 0.45f){
			MercObject.Jump( (pTime.Current - pTime.Jump.Stamp) * 1500.0f );
			mJumpState = false;
		}
	}

	if(stat.pVelocity.x == 0.0f){
		mRunning = false;
	}

	pBarrelPos.x = 23.0f*(glm::cos(-mAngleR)) + pPos.x;
	pBarrelPos.y = 23.0f*(glm::sin(-mAngleR)) + pPos.y;

	if(!pWeapon.pInfo.Clip){
		if(pTime.Diff(pTime.Reload.Stamp) >= pWeapon.pInfo.Reload){
			pTime.Reload.Stamp = 0.0f;
			pWeapon.Reload();
		}
	}

	//Set Apropriate sprite
	switch(pAction()){
		case ActState::IDLE:
			mBody = &mIdleSprite;
			break;
		case ActState::RUNNING:
		{
			mBody = &mRunningSprite;
			//Adjust running sprite speed
			float r = (MercObject.myStatus.pVelocity.x > 0) ? 1 : -1;
			mBody->mSpeed = (r * 15.0f)/(MercObject.myStatus.pVelocity.x);
			break;
		}
		case ActState::JETTING:
			mBody = &mJetSprite;
			break;
	}
}

////////////////////////////////////////////////////////////
///Draw self
/// @param interpolate: float, time to interpolate
////////////////////////////////////////////////////////////
void Player::Draw(float interpolate)
{
	if(!pSpawned)
		return;

	float dt = pTime() - pTime.Flame.Timer;
	pTime.Flame.Timer = pTime();

	for(size_t k = 0; k < 10; k++){
		if(mJetFlames[k].Alpha > 0.0f){
			mJetFlameSprite.color.a = mJetFlames[k].Alpha;
			mJetFlameSprite.pos 	= mJetFlames[k].Pos;
			mJetFlameSprite.angle.z	+= dt*80.0f;
			mJetFlameSprite.Draw();

			mJetFlameSprite.pos.x += 3*mFlip;
			mJetFlameSprite.pos.y += 8.0f;
			mJetFlameSprite.Draw();

			mJetFlames[k].Update(dt, dt*50.0f);
		}
	}
	
	
	//Interpolate
	if(interpolate > 0.0f){
		pIPos = pPos + (mVelocity * interpolate);
		_updatePositions(pIPos);
	}
	else {
		_updatePositions(pPos);
	}

	//Set sprite position, angle. Draw
	mBody->pos = pIPos;
	mBody->angle.x = pAngle.x;
	mBody->Step();
	mBody->Draw();

}

////////////////////////////////////////////////////////////
///Jets pass trough
////////////////////////////////////////////////////////////
void Player::Jet(void)
{
	// Apply jets if timer allows it 
	if((pTime.Current - pTime.Jet.Stamp > 0.001)){
		if(pJetCounter > 0){
			MercObject.Jet();
			mJetSprite.Step();
			pAction[ActState::JETTING] = true;
		}
		pTime.Jet.Stamp = pTime.Current;

		pJetCounter--;
		if(pJetCounter < 0)
			pJetCounter = 0;
	}

	// Jet flame timer
	if(pJetCounter && pTime.Current - pTime.Flame.Stamp > 0.03){
		//Set jet position, alpha
		mJetFlames[mFC] = FlameData(1.0f, glm::vec3(pPos.x+(12*mFlip),pPos.y-15,0.0f));
		mFC = ((mFC+1) > 9) ? 0 : (mFC+1);

		//Timestamp the jetflame
		pTime.Flame.Stamp = pTime.Current;
	}
}

////////////////////////////////////////////////////////////
///Right move pass trough
////////////////////////////////////////////////////////////
void Player::Right(void)
{
	mRunningSprite.Step();
	pAction[ActState::RUNNING] = true;

	if(pTime.Diff(pTime.Move.Stamp) > 0.001f){
		MercObject.Right();
		pTime.Move.Stamp = pTime.Current;
	}
}

////////////////////////////////////////////////////////////
///Left move pass trough
////////////////////////////////////////////////////////////
void Player::Left(void)
{
	mRunningSprite.Step();
	pAction[ActState::RUNNING] = true;
	
	if(pTime.Diff(pTime.Move.Stamp) > 0.001f){
		MercObject.Left();
		pTime.Move.Stamp = pTime.Current;
	}
}

////////////////////////////////////////////////////////////
///Jump
////////////////////////////////////////////////////////////
void Player::jumpBegin()
{
	if((pTime.Current - pTime.Jump.Timer) >= 0.001f){
		if(!mJumpState){
			mJumpState = true;
			pTime.Jump.Stamp = pTime.Current;
		}
	}

	pTime.Jump.Timer = pTime.Current;
}

////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////
void Player::jumpEnd()
{
	if(mJumpState){
		if((pTime.Diff(pTime.Jump.Stamp)) >= 0.020f){
			MercObject.Jump((pTime.Diff(pTime.Jump.Stamp)) * 1500.0f );
		}
	}

	mJumpState = false;
}
////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////
Bullet* Player::Shoot(glm::vec3& p_Dest)
{
	if(!pHasWeapon)
		return NULL;

	if(pTime.Current - pTime.Shoot.Timer < pWeapon.pInfo.Rate)
		return NULL;

	pTime.Shoot.Timer = pTime.Current;

	Bullet *bullet = pWeapon.Shoot();

	if(bullet){
		bullet->pos = pBarrelPos;
		bullet->des = pBarrelPos + p_Dest;
		bullet->startV = mVelocity;
		bullet->pID = pWeaponID;
		bullet->Initialize();
	}

	if(!pWeapon.pInfo.Clip){
		if(0.0f == pTime.Reload.Stamp){
			pTime.Reload.Stamp = pTime();
		}
		else if(pTime.Diff(pTime.Reload.Stamp) >= pWeapon.pInfo.Reload){
			pTime.Reload.Stamp = 0.0f;
			pWeapon.Reload();
		}
	}

	return bullet;
}

////////////////////////////////////////////////////////////
/// Throw whats in your hands
////////////////////////////////////////////////////////////
void Player::Throw(void)
{
	if(!pHasWeapon)
		return;
	glm::vec3 spawn = pIPos + glm::vec3(-30*mFlip,30,0);
	pWeapon.Spawn(spawn);
	glm::vec3 impulse(-1*mFlip, 1, 0);
	if(mFlip < 0)
		pWeapon.Impulse(impulse/4.0f,15,5);
	else
		pWeapon.Impulse(impulse,1*mFlip,1*mFlip);
		
	pHasWeapon = false;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Player::Pickup(object::Weapon weapon)
{
	if(pHasWeapon)
		return;
		
	pWeapon.pInfo = weapon.pInfo;
	pHasWeapon = true;
}

////////////////////////////////////////////////////////////
/// Pick a weapon
////////////////////////////////////////////////////////////
void Player::PickWeapon(object::Weapon::Info& p_Info, int p_ID)
{
	pWeaponID = p_ID;

	pWeapon.pInfo = p_Info;

	pHasWeapon = true;
}

////////////////////////////////////////////////////////////
/// Damage the player
////////////////////////////////////////////////////////////
void Player::Damage(float damage)
{
	MercObject.Damage(damage);
}

////////////////////////////////////////////////////////////
/// Kill the player
////////////////////////////////////////////////////////////
void Player::Kill()
{
	if(!pSpawned)
		return;

	MercObject.Kill();
	pSpawned = false;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Player::_updatePositions(glm::vec3& npos)
{
	//Adjust position	
	pWeaponPos = npos + (glm::vec3(5*mFlip,-10,0));
}
