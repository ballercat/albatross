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

#include"gfxmanager.h"
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
#include "humanoid.h"
#include <vector>
#include <list>

//Timing constants
#define FPS100			(0.01f)
#define FPS60			(0.016666666666666666666666666666667f)
#define FPS50			(0.02f)
#define FPS45			(0.022222222222222222222222222222222f)
#define FPS40			(0.025f)
#define FPS25			(0.04f)
#define FPS20			(0.05f)
#define FRAME_TIME		FPS40
#define PHYSICS_STEPS	(40)
#define PHYSICS_DELTA	FPS40
#define PHYSICS_PERSTEP	(0.000625f)
#define INPUT_LATENCY	(FPS50)
//NOTE: begin test code
#define SPAWN_TIME	3.0f;
//NOTE: end test code

////////////////////////////////////////////////////////////
/// Sprites required for the game
////////////////////////////////////////////////////////////
struct GameSprites{
	//Sprites used in-game
	std::vector<Sprite>	Bullet;
	std::vector<Sprite>	Weapon;
	std::vector<Sprite>	BulletHit;
	mercGFX				MercSprite;

	//HUD sprites
	struct HUD{
		enum{
			FRAME = 0,
			HEALTH,
			AMMO,
			CIRCLEBAR,
			COUNT
		};
		std::vector<Sprite>	Object;
		std::vector<GLuint>	Texture;
	};

	HUD hud;

	//Map
	struct MapData{
		GLuint				Texture; //polygon textures
		std::vector<GLuint>	SceneryTextures; //scenery textures
		std::vector<Sprite>	Scenery;	 //sprites
		std::vector<GLushort> Index;
		GLuint				VAO;
		GLuint				VBO;
		GLuint				IVBO;
		Shader				*Shdr;
	};
	MapData map;

	//Underlying OpenGL textures
	std::vector<GLuint>	bulletTex;
	std::vector<GLuint>	weaponTex;
	std::vector<GLuint> HitTexture;
	std::vector<GLuint>	MercTexture;
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
		std::string texturefile; //thexture.ini file path


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
	void _loadObjectSprites(void);
	void _loadHudSprites(void);
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
	gfx::Core						*display;

private:
	//Physics
	physics::PhysicsSimulator*      mPhysics;

private:
	//Main player data
	Player*		                    mPlayer;
	Player							*mRobot;
	GenericHeap<Bullet>             Bullets;
	//NOTE: test code ONLY. REMOVE LATER
	float							mSpawnTimer;
	//NOTE: end test code

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
	float wepswitchtimer;
	float inputupdate;
	float objectupdate;
	float jets;

private:
	//Hacked sprites
	//Sprite* Explosion;

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
		p_Sprite->pInfo.pos = p_Pos + (p_Delta * p_Velocity);
		p_Sprite->Draw();
	}

private:
	////////////////////////////////////////////////////////////
	/// Draw HUD
	////////////////////////////////////////////////////////////
	void _drawHud(void);

private:
	////////////////////////////////////////////////////////////
	/// Set up Collision handlers
	////////////////////////////////////////////////////////////
	void _initCollisionHandlers(cpSpace *space);

private:
	///Private functions
	bool _handleMessages(void);
	void _handleAgentMessages(ai::Agent*);
};

#endif //GAME_CLIENT_HEADER_GUARD
