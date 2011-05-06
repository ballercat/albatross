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
