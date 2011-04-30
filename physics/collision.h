#ifndef COLLISION_HEADER_GUARD
#define COLLISION_HEADER_GUARD

#include "physics.h"

#define BEGINVARS	cpArbiter *arb, cpSpace *space, void *data
#define PRESOLVEVARS BEGINVARS
#define SEPARATEVARS BEGINVARS

namespace collision
{

	//Begin collision handlers
	struct Begin
	{
		//Projectiles
		static cpBool BulletWorld(BEGINVARS);
		static cpBool ExplosiveWorld(BEGINVARS);
		static cpBool ExplosionObject(BEGINVARS);

		//Weapon
		static cpBool WeaponWorld(BEGINVARS);
		static cpBool WeaponPlayer(BEGINVARS);

		//Player
		static cpBool PlayerWorld(BEGINVARS);
	};
	
	struct PreSolve
	{
		//Player
		static cpBool PlayerWorld(PRESOLVEVARS);
	};

	struct Separate
	{	
		//Player
		static void PlayerWorld(SEPARATEVARS);
	};

} //namespace collision

#endif //COLLISION_HEADER_GUARD
