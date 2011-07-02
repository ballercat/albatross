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

#include "gameclient.h"
#include "SimpleIni.h"
#include "collision.h"
#include <string>

////////////////////////////////////////////////////////////
/// Load & apply some settings
////////////////////////////////////////////////////////////
void MainClient::_loadGameSettings(void)
{
	CSimpleIniA ini;
	if(	ini.LoadFile("settings.ini") < 0 ){
		throw "Could not open the settings file!";
	}

	//Input .ini file settings
	info.debug			= ini.GetBoolValue("Settings", "debug");
	info.mapfile 		= ini.GetValue("Settings", "map");
	info.demoFile		= ini.GetValue("Settings", "demo");
	info.demoPlay		= ini.GetBoolValue("Settings", "play");
	info.demoRecord		= ini.GetBoolValue("Settings", "record");
	info.texturefile	= ini.GetValue("Settings", "textures");

	info.fullscreen 	= ini.GetBoolValue("Window", "fullscreen");
	info.window_width	= ini.GetLongValue("Window", "width");
	info.window_height	= ini.GetLongValue("Window", "height");
	info.cursorfile 	= ini.GetValue("Window", "cursor");

	info.fpslimit 		= ini.GetLongValue("Graphics", "fpslimit");
	info.frameskip 		= ini.GetLongValue("Graphics", "frameskip");

	/* Load the weapon list */
	CSimpleIniA weaponlist;
	weaponlist.LoadFile("assets/weaponlist.ini");

	CSimpleIniA::TNamesDepend keys;
	weaponlist.GetAllKeys("Weapons", keys);

	//Build Weapon definitions
	CSimpleIniA::TNamesDepend::const_iterator i;
	for(i = keys.begin(); i != keys.end(); ++i){
		//corresponds to the weapon select menu
		WeaponList.push_back(weaponlist.GetValue("Weapons", i->pItem));

		{
			//Push back a weapon info
			object::Weapon::Info winfo;
			std::string	path = WeaponList.back() + "/weapon.ini";

			object::Weapon::Info::Load(path.c_str(), &winfo);

			WeaponInfo.push_back(winfo);
		}
	}

} //void MainClient::_loadGameSettings(void)

void MainClient::_loadObjectSprites(void)
{
	gs.MercTexture.resize(mercGFX::COUNT, 0);
    glGenTextures(mercGFX::COUNT, &gs.MercTexture[0]);

	CSimpleIniA texini;
	texini.LoadFile(info.texturefile.c_str());

	std::string texpath;

	//Manualy load merc sprites into one default mercGFX object
	LoadTex(gs.MercTexture[mercGFX::IDLE], texini.GetValue("Merc", "still"));
	LoadTex(gs.MercTexture[mercGFX::RUN], texini.GetValue("Merc", "run"));
	LoadTex(gs.MercTexture[mercGFX::JET], texini.GetValue("Merc", "jets"));
	LoadTex(gs.MercTexture[mercGFX::JETFLAME], texini.GetValue("Merc", "jetflame"));
	LoadTex(gs.MercTexture[mercGFX::SHOOT], texini.GetValue("Merc", "shoot"));

	gs.MercSprite.Idle = Sprite("assets/sprite/merc/still.sprh", gs.MercTexture[mercGFX::IDLE]);
	gs.MercSprite.Run = Sprite("assets/sprite/merc/run.sprh", gs.MercTexture[mercGFX::RUN]);
	gs.MercSprite.Jet = Sprite("assets/sprite/merc/jet.sprh", gs.MercTexture[mercGFX::JET]);
	gs.MercSprite.JetFlame = Sprite("assets/sprite/merc/jetflame.sprh", gs.MercTexture[mercGFX::JETFLAME]);
}
////////////////////////////////////////////////////////////
/// Load HUD sprites
////////////////////////////////////////////////////////////
void MainClient::_loadHudSprites(void)
{
	gs.hud.Texture.resize(GameSprites::HUD::COUNT, 0);
	glGenTextures(GameSprites::HUD::COUNT, &gs.hud.Texture[0]);

	LoadTex(gs.hud.Texture[GameSprites::HUD::FRAME],	"assets/interface/frame.png");
	LoadTex(gs.hud.Texture[GameSprites::HUD::HEALTH],	"assets/interface/health.png");
	LoadTex(gs.hud.Texture[GameSprites::HUD::AMMO],		"assets/interface/ammo.png");
	LoadTex(gs.hud.Texture[GameSprites::HUD::CIRCLEBAR],"assets/interface/circlebar.png");

	gs.hud.Object.push_back(Sprite(gs.hud.Texture[GameSprites::HUD::FRAME]));
	gs.hud.Object.push_back(Sprite(gs.hud.Texture[GameSprites::HUD::HEALTH]));
	gs.hud.Object.push_back(Sprite(gs.hud.Texture[GameSprites::HUD::AMMO]));
	gs.hud.Object.push_back(Sprite(gs.hud.Texture[GameSprites::HUD::CIRCLEBAR]));
	gs.hud.Object[GameSprites::HUD::CIRCLEBAR].pInfo.pivot = glm::vec3(0.0f);
	gfx::Link::Instance().spriteBuild(&gs.hud.Object[GameSprites::HUD::CIRCLEBAR]);

	gs.hud.Object[GameSprites::HUD::HEALTH].pInfo.pos = glm::vec3(-50.0f, -250.0f, 0.0f);
	gs.hud.Object[GameSprites::HUD::AMMO].pInfo.pos = glm::vec3(-50.0f, -275.0f, 0.0f);
}

////////////////////////////////////////////////////////////
/// Populate all the required game sprites
////////////////////////////////////////////////////////////
void MainClient::_populateSprites(void)
{
	//Weapon loading
	{
		std::string path;

		size_t sz = WeaponList.size();

		//resize texture vectors
		gs.bulletTex.resize(sz);
		gs.weaponTex.resize(sz);

		//Generate Textures
		glGenTextures(sz, &gs.bulletTex[0]);
		glGenTextures(sz, &gs.weaponTex[0]);

		//Populate sprites
		for(size_t k = 0; k < sz; k++){
			//Bullet
			{
				path = WeaponList[k] + "/bullet.png";
				if(!LoadTex(gs.bulletTex[k], path.c_str()))
					throw "Could not load bullet texture...";

				path = WeaponList[k] + "/bullet.sprh";
				gs.Bullet.push_back(Sprite(path.c_str(), gs.bulletTex[k]));
			}

			//Weapon
			{
				path = WeaponList[k] + "/weapon.png";
				if(!LoadTex(gs.weaponTex[k], path.c_str()))
					throw "Could not load weapon texture...";

				path = WeaponList[k] + "/weapon.sprh";
				gs.Weapon.push_back(Sprite(path.c_str(), gs.weaponTex[k]));
			}
		}
	}

	//Bullet, Hits, Explosions Sprites
	{
		gs.HitTexture.resize(2, 0);

		glGenTextures(2, &gs.HitTexture[0]);

		LoadTex(gs.HitTexture[1], "assets/explosions/default.png");
		gs.BulletHit.push_back(Sprite());
		gs.BulletHit.push_back(Sprite("assets/explosions/default.sprh", gs.HitTexture[1]));
	}
}

////////////////////////////////////////////////////////////
/// Load a map
////////////////////////////////////////////////////////////
void MainClient::_loadMap(const char *p_MapPath)
{
	if(map != NULL){
		bgmfdelete(map);
		map = NULL;

		mPhysics->remAllStaticShapes();
	}

	//Load the map
	map = bgmfopen(p_MapPath);

	//Map textures
	{
		//Generate polygon texture(s)
		float	tsz	= map->texpath.size();
		float tw	= tsz*128.0f;
		float th	= 128.0f;
		sf::Image	texturedata(tw, th);

		//Create the actual OpenGL texture
		{
			sf::Image	rawImg;
			std::string	fpath;

			for(size_t i=0;i<tsz;i++){
				fpath = "assets/texture/" + map->texpath[i];
				rawImg.LoadFromFile(fpath);
				texturedata.Copy(rawImg, 128*i, 0);
			}

			glGenTextures(1, &gs.map.Texture);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, gs.map.Texture);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th,
						   0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata.GetPixelsPtr());

			glDisable(GL_TEXTURE_2D);
		}
	}

	//Load Map Scenery
	{
		if(!gs.map.SceneryTextures.empty()){
			glDeleteTextures(gs.map.SceneryTextures.size(), &gs.map.SceneryTextures[0]);

			gs.map.SceneryTextures.clear();
			gs.map.Scenery.clear();
		}

		int i = map->sprpath.size();

		if(i){
			gs.map.SceneryTextures.resize(i);
			glGenTextures(i, &gs.map.SceneryTextures[0]);

			std::string fpath;

			//Load the actual texture data
			for(int k=0;k<i;k++){
				fpath = "assets/scenery/" + map->sprpath[k];
				LoadTex(gs.map.SceneryTextures[k], fpath.c_str());
			}

			//Fling sprite objects into existance
			for(unsigned k=0;k<map->header.sprc;k++){
				i = map->sprite[k].id;
				fpath = "assets/scenery/" + map->sprheader[i];
				Sprite spr(fpath.c_str(), gs.map.SceneryTextures[i]);
				spr.pInfo.pos = map->sprite[k].pos;
				gs.map.Scenery.push_back(spr);
			}
		}
	}

	//Polygon vertexes
	glm::vec2 _DefaultTC[4];
	_DefaultTC[0] = glm::vec2(0.0f, 0.0f);
	_DefaultTC[1] = glm::vec2(0.0f, 1.0f);
	_DefaultTC[2] = glm::vec2(1.0f, 0.0f);
	_DefaultTC[3] = glm::vec2(1.0f, 1.0f);
	glm::vec3 v0,v1,v2;
	float texstep = 1.0f / map->texpath.size();
	glm::vec2 swap;

	size_t i = 0;
	GLushort indeces = 0;

	gs.map.Index.clear();
	map->hpc = 0;

	//Add polygons to the space
	for(bgmf_poly *p = &map->poly[0];p!=&map->poly[map->header.pc];p++)
	{


		v0 = p->data[0];
		v1 = p->data[1];
		v2 = p->data[2];

		if(!map->mask[i].bit.hollow){
			mPhysics->addStaticSegmentShape(v0, v1, MAPPOLYGON);
			mPhysics->addStaticSegmentShape(v1, v2, MAPPOLYGON);
			mPhysics->addStaticSegmentShape(v2, v0, MAPPOLYGON);
		} else if(map->mask[i].bit.hollow && !map->hpc){
			map->hpc = indeces;
		}

		swap = _DefaultTC[0];
		swap.x = swap.x * texstep + map->texture[i] * texstep;
		map->texcoord[i].data[0] = (swap);
		swap = _DefaultTC[1];
		swap.x = swap.x * texstep + map->texture[i] * texstep;
		map->texcoord[i].data[1] = (swap);
		swap = _DefaultTC[2];
		swap.x = swap.x * texstep + map->texture[i] * texstep;
		map->texcoord[i].data[2] = (swap);

		map->Data.push_back(MapVertex(v0, map->texcoord[i].data[0], map->color[i].data[0]));
		map->Data.push_back(MapVertex(v1, map->texcoord[i].data[1], map->color[i].data[1]));
		map->Data.push_back(MapVertex(v2, map->texcoord[i].data[2], map->color[i].data[2]));

		gs.map.Index.push_back(indeces);
		gs.map.Index.push_back(indeces+1);
		gs.map.Index.push_back(indeces+2);
		indeces += 3;

		for(unsigned k=3;k<p->data.size();k++){
			v0 = p->data[k];
			v1 = p->data[k-1];
			v2 = p->data[k-2];

			if(!map->mask[i].bit.hollow){
				mPhysics->addStaticSegmentShape(v0, v1, MAPPOLYGON);
				mPhysics->addStaticSegmentShape(v1, v2, MAPPOLYGON);
				mPhysics->addStaticSegmentShape(v2, v0, MAPPOLYGON);
			}

			swap = _DefaultTC[k%4];
			swap.x = swap.x * texstep + map->texture[i] * texstep;
			map->texcoord[i].data[k] = (swap);

			map->Data.push_back(MapVertex(v1, map->texcoord[i].data[k-1], map->color[i].data[k-1]));
			map->Data.push_back(MapVertex(v2, map->texcoord[i].data[k-2], map->color[i].data[k-2]));
			map->Data.push_back(MapVertex(v0, swap, map->color[i].data[k]));

			gs.map.Index.push_back(indeces);
			gs.map.Index.push_back(indeces+1);
			gs.map.Index.push_back(indeces+2);
			indeces += 3;
		}

		i++;
	}

	//Set up the map VBO
	{
		gs.map.Shdr = new Shader("assets/shader/default.vert", "assets/shader/default.frag");

		glGenVertexArrays(1, &gs.map.VAO);
		glBindVertexArray(gs.map.VAO);

		glGenBuffers(1, &gs.map.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, gs.map.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(map->Data[0])*map->Data.size(), &map->Data[0], GL_STATIC_DRAW);

		glGenBuffers(1, &gs.map.IVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gs.map.IVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*gs.map.Index.size(), &gs.map.Index[0], GL_STATIC_DRAW);

		glVertexAttribPointer(gs.map.Shdr->GetAttrib("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(MapVertex), BUFFER_OFFSET(0));
		glEnableVertexAttribArray(gs.map.Shdr->GetAttrib("vPosition"));

		glVertexAttribPointer(gs.map.Shdr->GetAttrib("vUV"), 2, GL_FLOAT, GL_FALSE, sizeof(MapVertex), BUFFER_OFFSET(sizeof(glm::vec3)));
		glEnableVertexAttribArray(gs.map.Shdr->GetAttrib("vUV"));

		glVertexAttribPointer(gs.map.Shdr->GetAttrib("vColor"), 4, GL_FLOAT, GL_FALSE, sizeof(MapVertex), BUFFER_OFFSET(sizeof(glm::vec3)+sizeof(glm::vec2)));
		glEnableVertexAttribArray(gs.map.Shdr->GetAttrib("vColor"));
	}
}

////////////////////////////////////////////////////////////
/// Collision handlers
////////////////////////////////////////////////////////////
void MainClient::_initCollisionHandlers(cpSpace *space)
{
	mPhysics->AddCollision(MAPPOLYGON, BULLET, this, NULL, NULL, collision::Post::BulletWorld);
	mPhysics->AddCollision(EXPLOSIVE, MAPPOLYGON, this, collision::Begin::ExplosiveWorld);
	mPhysics->AddCollision(EXPLOSION, PLAYER, this, collision::Begin::ExplosionObject);
	mPhysics->AddCollision(EXPLOSIVE, PLAYER, this, collision::Begin::ExplosiveObject);

	//mPhysics->AddCollision(WEAPON, PLAYER, this, w2p_beginCollision);

	//Player World Collisions
	mPhysics->AddCollision(MERC, MAPPOLYGON, this, 	collision::Begin::PlayerWorld,
													collision::PreSolve::PlayerWorld,
													NULL,
													collision::Separate::PlayerWorld);
}
