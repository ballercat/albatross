#include "player.h"

////////////////////////////////////////////////////////////
///Constructor
////////////////////////////////////////////////////////////
Player::Player(glm::vec3 loc,GLuint *Texture)
{
	MercObject.Spawn(loc);
	MercObject.Initialize();
	MercObject.id = 2;

	rawsprite.push_back(new Sprite("assets/sprite/merc/still.sprh",Texture[PLAYER_TORSO]));
	rawsprite.push_back(new Sprite("assets/sprite/merc/run.sprh", Texture[PLAYER_LEGS]));
	rawsprite.push_back(new Sprite("assets/sprite/merc/jet.sprh", Texture[PLAYER_JETS]));
	rawsprite.push_back(new Sprite("assets/sprite/m203/m203.sprh", Texture[AK47]));

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

	pHasWeapon = true;

	//Jet flames
	jflame_alpha.assign(10,0.0f);
	jflame_pos.assign(10,glm::vec3());
	jflame_count = 0;

	//Timing
	Time = 0.0f;
	jflame_eraser = Time;
	jettimer = Time;
	flametimer = Time;
	jumptimer = Time;
	jumptime = Time;
	jumpstate = false;
	wps = glm::vec3(0,0,0);
}

////////////////////////////////////////////////////////////
///Step
/// @param cursor : vec3 location of the cursor on screen
////////////////////////////////////////////////////////////
void Player::Step(glm::vec3& cursor, float& p_Time)
{
	// Update internal state, get position
	GameObject::Status stat = MercObject.Update(p_Time - Time);
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

	if(pHasWeapon){
		weapon->angle.x = angle.x;
		weapon->angle.z = angle.z;
	}

	ps.x = int(ps.x);
	ps.y = int(ps.y);

	//Timer updates, and flag updates
	Time = p_Time;
	shoot = false;

	if(jumpstate){
		if((Time - jumptime) >= 0.45f){
			MercObject.Jump( (Time - jumptime) * 1500.0f );
			jumpstate = false;
		}
	}

	barrel.x = 23.0f*(glm::cos(-angleR)) + ps.x;
	barrel.y = 23.0f*(glm::sin(-angleR)) + ps.y;
}

////////////////////////////////////////////////////////////
///Draw self
/// @param interpolate: float, time to interpolate
////////////////////////////////////////////////////////////
void Player::Draw(float interpolate)
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

	//weapon->Draw();
	torso->Step();

	if(sparkcounter > 0.0f){
		sparks->Draw();
		sparkcounter -= Time - sparktimer;
	}

	//Reset sprite pointers
	rawsprite[1]->pos = legs->pos;
	legs = rawsprite[1];

	if(MercObject.myStatus.v.x == 0){
		running = false;

	}
	else{
		float r = (MercObject.myStatus.v.x > 0) ? 1 : -1;
		legs->mSpeed = (r * 15.0f)/(MercObject.myStatus.v.x);
		legs->Step();
	}
}

////////////////////////////////////////////////////////////
///Jets pass trough
////////////////////////////////////////////////////////////
void Player::Jet(void)
{
	rawsprite[2]->pos = legs->pos;
	legs = rawsprite[2];
	legs->Step();
	running = true;

	if(Time - jettimer > 0.001){
		MercObject.Jet();
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
void Player::Right(void)
{
	legs->Step();
	running = true;

	if(Time - movetimer > 0.001f){
		MercObject.Right();
		movetimer = Time;
	}
}

////////////////////////////////////////////////////////////
///Left move pass trough
////////////////////////////////////////////////////////////
void Player::Left(void)
{
	legs->Step();
	running = true;

	if(Time - movetimer > 0.001f){
		MercObject.Left();
		movetimer = Time;
	}
}

////////////////////////////////////////////////////////////
///Jump
////////////////////////////////////////////////////////////
void Player::jumpBegin()
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
void Player::jumpEnd()
{
	if(jumpstate){
		if((Time - jumptime) >= 0.030f){
			MercObject.Jump( (Time - jumptime) * 1000.0f );
		}
	}

	jumpstate = false;
}
////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////
Bullet* Player::Shoot(glm::vec3& p_Dest)
{
	if(!pHasWeapon)
		return NULL;
	sparktimer = Time;
	sparkcounter = 0.35f;

	Bullet *bullet = m_Weapon->Shoot();
	bullet->pos = barrel;
	bullet->des = barrel + p_Dest;
	bullet->startV = mVelocity;
	bullet->pID = WeaponID;
	bullet->Initialize();

	return bullet;
}

////////////////////////////////////////////////////////////
/// Throw whats in your hands
////////////////////////////////////////////////////////////
void Player::Throw(void)
{
	if(!pHasWeapon)
		return;
	glm::vec3 spawn = ipos + glm::vec3(-30*flip,30,0);
	m_Weapon->Spawn(spawn);
	glm::vec3 impulse(-1*flip, 1, 0);
	if(flip < 0)
		m_Weapon->Impulse(impulse/4.0f,15,5);
	else
		m_Weapon->Impulse(impulse,1*flip,1*flip);
	pHasWeapon = false;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Player::Pickup(object::Weapon *weapon)
{
	if(pHasWeapon)
		return;
	m_Weapon = weapon;
	pHasWeapon = true;
}

////////////////////////////////////////////////////////////
/// Pick a weapon
////////////////////////////////////////////////////////////
void Player::PickWeapon(object::Weapon::Info& p_Info, int p_ID)
{
	WeaponID = p_ID;

	m_Weapon->pInfo = p_Info;

	pHasWeapon = true;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Player::_updatePositions(glm::vec3& npos)
{
	//Adjust position
	if(pHasWeapon){
		wps = npos + (glm::vec3(2*flip,-8,0));
	}
	else {
		m_Weapon->Update(0.0f);
		weapon->pos = m_Weapon->GetStatus().pos;
		weapon->angle.z = m_Weapon->rot_dAngle;
	}

	legs->pos = npos; legs->pos.y += 3;
	torso->pos = npos; torso->pos.y += 3;

	sparks->pos = barrel;
}
