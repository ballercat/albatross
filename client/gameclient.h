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
#include "demo.h"
#include "timing.h"
#include <vector>
#include <list>

////////////////////////////////////////////////////////////
/// Sprites required for the game
////////////////////////////////////////////////////////////
struct GameSprites{
	//Sprites used in-game
	std::vector<Sprite>	Bullet;
	std::vector<Sprite>	Weapon;
	std::vector<Sprite>	Expl;

	//HUD sprites
	struct MenuSprites{
		Sprite	StartGame;
		Sprite	ExitGame;
	} menu;

	//Map
	struct MapData{
		GLuint				Texture; //polygon textures
		std::vector<Sprite>	Scenery;	 //static sprites
		std::vector<Sprite> AnimScenery; //animated sprties
	} map;

	//Underlying OpenGL textures
	std::vector<GLuint>	bulletTex;
	std::vector<GLuint>	weaponTex;
	std::vector<GLuint> explTex;
};

////////////////////////////////////////////////////////////
/// In-game HUD, menu and menu input
////////////////////////////////////////////////////////////
struct GameHUD
{

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

	////////////////////////////////////////////////////////////
	///Main game loop
	////////////////////////////////////////////////////////////
	void Run(const char *p_DemoFile = NULL);

	////////////////////////////////////////////////////////////
	/// Run main menu
	////////////////////////////////////////////////////////////
	void MainMenu();

	////////////////////////////////////////////////////////////
	/// Collision handlers NOTE:DEPRECTATED
	////////////////////////////////////////////////////////////
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
		std::string	demoFile;	//bgdf file path
		bool		demoPlay;
		bool		demoRecord;

		int			fpslimit;	//Frame Rate Limit(0 to disable)
		int			frameskip;	//Number skipping(0 to disable)
		bool		fullscreen;	//Fullscreen(0 to disable)
		unsigned	window_height;
		unsigned	window_width;

	}; //struct GameInfo

	GameInfo	info; //Game information

private:
	////////////////////////////////////////////////////////////
	/// Load settings
	////////////////////////////////////////////////////////////
	void _loadGameSettings(void);
	void _populateSprites(void);
	void _loadMap(const char *p_MapPath);


////////////////////////////////////////////////////////////
///Member Data
////////////////////////////////////////////////////////////
private:
	//Demo data
	GameDemo						Demo;

private:
	//IO
	InputHandlerMixin<Input>*       mInput;
	gfx::FixedPipeline*    			display;

private:
	//Physics
	physics::PhysicsSimulator*      mPhysics;

private:
	//Main player data
	Player*		                    mPlayer;
	GenericHeap<Bullet>             Bullets;

private:
	//Timing
	float							currentTime;
	sf::Clock*						mTime; //pointer to the global time

private:
	//Map data
	bgmf*							map;

private:
	//NOTE: Remove these
	float lastupdate;
	float lastbullet;
	float inputupdate;
	float objectupdate;

private:
	//Hacked sprites
	Sprite* Tree;
	Sprite* Explosion;

private:
	//More hacking in of features
	bool	expld;

private:
	//Graphics
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
	/// Set up Collision handlers	
	////////////////////////////////////////////////////////////	
	void _initCollisionHandlers(cpSpace *space);

private:
		///Private functions
		bool _handleMessages(void);
};

#endif //GAME_CLIENT_HEADER_GUARD
