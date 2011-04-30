#include "collision.h"
#include "bullets.h"

using namespace collision;

////////////////////////////////////////////////////////////
/// Projectiles
////////////////////////////////////////////////////////////
cpBool Begin::BulletWorld(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);

	Bullet *bullet = (Bullet*)b->data;
	//mark the bullet object as dead
	bullet->status = Bullet::Status::Dead;

	return cpTrue;
}

cpBool Begin::ExplosiveWorld(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);

	Explosive *explosive = (Explosive*)a->data;

	explosive->Explode();

	return cpTrue;
}

cpBool Begin::ExplosionObject(BEGINVARS)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);
	Explosive *explosive = (Explosive*)a->data;
	GameObject *object = (GameObject*)b->data;

	glm::vec3 p_obj = object->GetStatus().pos;
	glm::vec3 p_aoa = explosive->pos;
	
	float dx = p_aoa.x - p_obj.x;
	float dy = p_aoa.y - p_obj.y;
	
	cpContactPointSet cnt = cpArbiterGetContactPointSet(arb);
	
	cpVect bloc;
	
	bloc.x = cnt.points[0].point.x - p_obj.x;
	bloc.y = cnt.points[0].point.y - p_obj.y;
		
	object->Impulse(glm::vec3((-dx),(-dy), 0), bloc.x, bloc.y );

	explosive->status = Bullet::Status::Dead;

	return cpFalse;
}

////////////////////////////////////////////////////////////
/// Weapon
////////////////////////////////////////////////////////////
cpBool Begin::WeaponWorld(BEGINVARS)
{
	return cpTrue;
}

cpBool Begin::WeaponPlayer(BEGINVARS)
{
	return cpTrue;
}

////////////////////////////////////////////////////////////
///Player
////////////////////////////////////////////////////////////
cpBool Begin::PlayerWorld(BEGINVARS)
{

	return cpTrue;
}



///PreSolve handlers

cpBool PreSolve::PlayerWorld(PRESOLVEVARS)
{
	return cpTrue;
}


///Separate Handlers

//Player
void Separate::PlayerWorld(SEPARATEVARS)
{

}



