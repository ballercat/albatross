#include "gameclient.h"
#include "SimpleIni.h"
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
		size_t	psz	= map->header.pc;
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

		tsz				= 1.0f / map->texpath.size();
		float left		= 0.0f;
		float right		= 0.0f;

		for(size_t i=0;i<psz;i++){
			// Populate map texture coordinates
			left = map->texture[i] * tsz;
			right = left + tsz;

			map->texcoord[i].data[0] = glm::vec2(left, 0.0f);
			map->texcoord[i].data[1] = glm::vec2(left, 1.0f);
			map->texcoord[i].data[2] = glm::vec2(right, 1.0f);
		}
	}

	//Polygon vertexes
	glm::vec3 v0,v1,v2;

	size_t i = 0;
	//Add polygons to the space
	for(bgmf_poly *p = &map->poly[0];p!=&map->poly[map->header.pc];p++)
	{
		v0 = p->data[0];
		v1 = p->data[1];
		v2 = p->data[2];

		//Only add non-hollow polygons
		if(!map->mask[i].bit.hollow){
			mPhysics->addStaticSegmentShape(v0, v1, MAPPOLYGON);
			mPhysics->addStaticSegmentShape(v1, v2, MAPPOLYGON);
			mPhysics->addStaticSegmentShape(v2, v0, MAPPOLYGON);
		}

		i++;
	}
}
