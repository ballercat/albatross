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


#ifndef SPRITE_HEADER_GUARD
#define SPRITE_HEADER_GUARD
#include "assets.h"
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

////////////////////////////////////////////////////////////
///Sprite class
////////////////////////////////////////////////////////////
struct Sprite
{
public:
	////////////////////////////////////////////////////////////
	///Constructor, from .spr file
	////////////////////////////////////////////////////////////
    Sprite(const char *fpath, GLuint texid);

	////////////////////////////////////////////////////////////
	///Default constructor
	////////////////////////////////////////////////////////////
	Sprite();

	////////////////////////////////////////////////////////////
	///Destructor
	////////////////////////////////////////////////////////////
    virtual ~Sprite();

    inline const void* getPixelpointer(void)
    {
        return NULL;
    }

public:
	////////////////////////////////////////////////////////////
	///Build vertex & texture coordinates
	////////////////////////////////////////////////////////////
	void Build();

	////////////////////////////////////////////////////////////
	///Step trough frames
	////////////////////////////////////////////////////////////
	virtual void Step();

	////////////////////////////////////////////////////////////
	///Draw
	////////////////////////////////////////////////////////////
	void Draw();

public:
    //Sprite & animation information
    uint32_t    height,width;
	glm::vec2	imgd;
    glm::vec3   angle;
    glm::vec3   pos;
    glm::vec3   pivot;
    glm::vec3   off;
    glm::vec3   scale;
    float       xstart;
    glm::vec4   color;

#ifdef SHADER_PIPELINE
public:
	glm::mat4	mvp,proj,model,view;
	GLuint		_mvp;
#endif

public:
    ///Animation controll
    size_t  mLength;
    float   mSpeed;
    size_t  mPosition;
    GLuint  textureid;

	glm::vec2 texdata[6];
	glm::vec3 vertdata[6];

protected:
	////////////////////////////////////////////////////////////
	///Parse .spr file for sprite data
	////////////////////////////////////////////////////////////
	void _parseInfo(const char *fpath);

protected:
	///Data
    float   mLastUpdate;
	sf::Clock mTimer;
#ifdef SHADER_PIPELINE
	glm::vec2 texcoord[6];
	glm::vec3 vcoord[6];
	VertexArray _quad;
#endif
};


#endif //SPRITE_HEADER_GUARD
