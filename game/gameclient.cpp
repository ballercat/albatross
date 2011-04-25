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
#include "assets.h"
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include "SimpleIni.h"

static sf::Clock g_timer;

////////////////////////////////////////////////////////////
/// Load & apply some settings
////////////////////////////////////////////////////////////
void MainClient::_loadGameSettings(void)
{
	CSimpleIniA ini;
	ini.LoadFile("settings.ini");

	//Input .ini file settings
	info.mapfile 		= ini.GetValue("Settings", "map");

	info.fullscreen 	= ini.GetBoolValue("Window", "fullscreen");
	info.window_width	= ini.GetLongValue("Window", "width");
	info.window_height	= ini.GetLongValue("Window", "height");
	info.cursorfile 	= ini.GetValue("Window", "cursor");

	info.fpslimit 		= ini.GetLongValue("Graphics", "fpslimit");
	info.frameskip 		= ini.GetLongValue("Graphics", "frameskip");

} //void MainClient::_loadGameSettings(void)

////////////////////////////////////////////////////////////
///Constructor
////////////////////////////////////////////////////////////
MainClient::MainClient(void) :
    mPhysics(&physics::Simulator::Instance())
{
	//Parse the settings file
	_loadGameSettings();


    //Create the display
	display = new gfx::FixedPipeline(	info.window_width,
										info.window_height,
										"v0.0.4a(Finback)",
										info.fullscreen	);

    GLuint *Texture = display->Texture;
	display->clearColor(0.5f, 0.7f, 0.8f, 1.0f);

    mInput = new InputHandlerMixin<Input>(&mMessageQueue);
    mInput->Window = display->Window;

    mIP = network::GetLocalIP();

    //Quick and dirty texture loading
    LoadTextures("textures.ini", Texture);

	//Display settings
	display->setCursor(info.cursorfile.c_str());
	display->Window->SetFramerateLimit(info.fpslimit);


	//Physics
    cpShape *shape;
	cpSpace* space = mPhysics->GetWorldHandle();
    cpBody *staticBody = &space->staticBody;

    //Bind bullet collision handlers
    cpSpaceAddCollisionHandler(space,0,1,b2w_begin,NULL,NULL,NULL,this);

	//Load the map, plug in polygon values into the game
	map = bgmfopen(info.mapfile.c_str());
    glm::vec3 v0,v1,v2;
    for(bgmf_poly *p = &map->poly[0];p!=&map->poly[map->header.pc];p++)
    {
        v0 = p->data[0];
        v1 = p->data[1];
        v2 = p->data[2];

        //Add map polygon to physics world
        shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(v0.x,v0.y),cpv(v1.x,v1.y),0.0f));
        shape->e = 1.0f; shape->u = 1.0f;
        shape->collision_type = 0;
        shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(v1.x,v1.y),cpv(v2.x,v2.y),0.0f));
        shape->e = 1.0f; shape->u = 1.0f;
        shape->collision_type = 0;
        shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(v2.x,v2.y),cpv(v0.x,v0.y),0.0f));
        shape->e = 1.0f; shape->u = 1.0f;
        shape->collision_type = 0;
    }

    //Create a player object
    mPlayer = new MercPlayer(glm::vec3(500,200,0),Texture);

	//Timing(NOTE: improve/move this)
    lastupdate = g_timer.GetElapsedTime();
    lastbullet = lastupdate;
	objectupdate = lastupdate;

	//Sprite hack
	Tree = new Sprite("assets/sprite/tree/tree1.sprh",Texture[TREE_PLAIN]);
	Tree->pos = glm::vec3(200,300,0);
} //MainClient::MainClient

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
MainClient::~MainClient(void)
{
    if(display)
    {
        delete display;
        display = NULL;
    }
} //MainClient::~MainClient(void)

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
int MainClient::Connect(std::string server_ip)
{
    if(server_ip == network::GetLocalIP())
    {
        return 1;
    }

    return 0;
}



