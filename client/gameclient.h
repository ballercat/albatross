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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef GAME_CLIENT_HEADER_GUARD
#define GAME_CLIENT_HEADER_GUARD

#include "fixed_pipeline.h"
#include "client.h"
#include "input.h"
#include "inputhandler.h"
#include "network.h"
#include "player.h"
#include "bullets.h"
#include "weapon.h"
#include "map.h"
#include <vector>
#include <list>

struct GameSprites{
	//Sprites used in-game
	std::vector<Sprite>	Bullet;
	std::vector<Sprite>	Weapon;
	std::vector<Sprite>	Expl;

	//Underlying OpenGL textures
	std::vector<GLuint>	bulletTex;
	std::vector<GLuint>	weaponTex;
	std::vector<GLuint> explTex;
};

////////////////////////////////////////////////////////////
///Main game Client
////////////////////////////////////////////////////////////
class MainClient : public Client
{
public:
	MainClient();
	virtual ~MainClient();

	virtual int Connect(std::string server_ip);

	void DebugDrawPhysics(void);

	//Game-step... deprecated
	//int Execute(void);

	///Main game loop
	void Run(void);

	static int BeginCollision(cpArbiter *arb,cpSpace *space,void *unused);
	static cpBool w2p_beginCollision(cpArbiter *arb, cpSpace *space, void *p_Client);

public:
	////////////////////////////////////////////////////////////
	/// Game info struct
	////////////////////////////////////////////////////////////
	struct GameInfo
	{
		bool debug;
		
		std::string mapfile;	//bgmf file path
		std::string cursorfile;	//cursor .png file path

		int			fpslimit;	//Frame Rate Limit(0 to disable)
		int			frameskip;	//Number skipping(0 to disable)
		bool		fullscreen;	//Fullscreen(0 to disable)
		unsigned	window_height;
		unsigned	window_width;

	}; //struct GameInfo

	GameInfo	info; //Game information

private:
	///Member Data
	InputHandlerMixin<Input>*       mInput;
	gfx::FixedPipeline*    			display;
	Player*		                    mPlayer;
	physics::PhysicsSimulator*      mPhysics;
	GenericHeap<Bullet>             Bullets;
	bgmf    *map;

	float lastupdate;
	float lastbullet;
	float inputupdate;
	float objectupdate;

	Sprite* Tree;
	Sprite* Explosion;
	bool	expld;

	GameSprites		gs; //THE ALMIGHTY GAME SPRITE
private:
	////////////////////////////////////////////////////////////
	/// Weapons
	////////////////////////////////////////////////////////////
	void _drawWeapons(void);

	std::vector<std::string> WeaponList;

	//Weapon definitions
	std::vector<object::Weapon::Info> WeaponInfo;

private:
	////////////////////////////////////////////////////////////
	/// Bullet drawing
	////////////////////////////////////////////////////////////
	void _drawBullets(float& delta);

	//Bullet Textures
	std::vector<GLuint>	mBulletTex;

private:
	////////////////////////////////////////////////////////////
	/// Parse and draw Hit sprites
	////////////////////////////////////////////////////////////
	void _drawHits(void);

	////////////////////////////////////////////////////////////
	/// Hit & Explosion handling. Sprites
	////////////////////////////////////////////////////////////
	std::list<Sprite>	bulletHits;

private:
	////////////////////////////////////////////////////////////
	/// Draw with interpolation
	////////////////////////////////////////////////////////////
	inline void drawSpriteInter(Sprite* p_Sprite,
								glm::vec3& p_Pos,
								float& p_Delta,
								glm::vec3& p_Velocity)
	{
		p_Sprite->pos = p_Pos + (p_Delta * p_Velocity);
		p_Sprite->Draw();
	}

private:
	////////////////////////////////////////////////////////////
	/// Load settings
	////////////////////////////////////////////////////////////
	void _loadGameSettings(void);
	void _populateSprites(void);

private:
	////////////////////////////////////////////////////////////		
	/// Set up Collision handlers	
	////////////////////////////////////////////////////////////	
	void _initCollisionHandlers(cpSpace *space);

private:
		///Private functions
		bool _handleMessages(void);
};

#endif //GAME_CLIENT_HEADER_GUARD
