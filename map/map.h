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

#ifndef MAP_HEADER_GUARD
#define MAP_HEADER_GUARD

#include<glm/glm.hpp>
#include<stdint.h>
#include<vector>
#include<string>

//Map polygon masks
#define POLY_VISIBLE 	0x01
#define POLY_SOLID 		0x02
#define POLY_HOLLOW		0x01
#define POLY_DEFAULT	0x04

////////////////////////////////////////////////////////////
/// glm::vec3(x,y,unused) Polygon-Vertex
/// Not actually used in the map representation
/// Used for reperesenting vertexes in the mapmaker etc.,
////////////////////////////////////////////////////////////
struct bgmf_vert
{
	bgmf_vert() :
		pPos(NULL),
		pCol(NULL)
	{
		//void
	}

	bgmf_vert(glm::vec3 *p_Position, glm::vec4 *p_Color) :
		pPos(p_Position),
		pCol(p_Color)
	{
		//void
	}

	glm::vec3	*pPos;
	glm::vec4	*pCol;
};

////////////////////////////////////////////////////////////
/// 3xglm::vec3(x,y,unused) Map-polygon struct
////////////////////////////////////////////////////////////
struct bgmf_poly
{
    bgmf_poly(){}
    bgmf_poly(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
    {
        data[0] = v0;
        data[1] = v1;
        data[2] = v2;
    }

    bool operator ==(const bgmf_poly &test) const
    {
        if(data[0] == test.data[0] && data[1] == test.data[1] &&
           data[2] == test.data[2] ){
               return true;
           }
        return false;
    }

    glm::vec3 data[3];
};

////////////////////////////////////////////////////////////
/// 3xglm::vec2(u,v) Polygon-texture struct
////////////////////////////////////////////////////////////
struct bgmf_poly_tex
{
    bgmf_poly_tex(){}
    bgmf_poly_tex(glm::vec2 t0, glm::vec2 t1, glm::vec2 t2)
    {
        data[0] = t0;
        data[1] = t1;
        data[2] = t2;
    }

    bool operator ==(const bgmf_poly_tex &test) const
    {
        if(data[0] == test.data[0] && data[1] == test.data[1] &&
           data[2] == test.data[2] ){
               return true;
           }
        return false;
    }

    glm::vec2 data[3];
};

////////////////////////////////////////////////////////////
/// 3xglm::vec4(r,g,b,a) Polygon-color struct
////////////////////////////////////////////////////////////
struct bgmf_color
{
    bgmf_color(){}
    bgmf_color(glm::vec4 v0, glm::vec4 v1, glm::vec4 v2)
    {
        data[0] = v0;
        data[1] = v1;
        data[2] = v2;
    }
    glm::vec4 data[3];
};

////////////////////////////////////////////////////////////
/// Helper map polygon struct
////////////////////////////////////////////////////////////
struct bgmf_poly_view
{
public:
	////////////////////////////////////////////////////////////
	/// ctor
	////////////////////////////////////////////////////////////
	bgmf_poly_view() :
		pID(0),
		pP(NULL),
		pTC(NULL),
		pC(NULL)
	{
		//void
	}

public:
	unsigned int		pID;	//Polygon ID
	unsigned int		pM;		//mask
	unsigned int		*pT;		//texture #
	bgmf_poly			*pP;	//vertex pointer
	bgmf_poly_tex		*pTC;	//texture coord pointer
	bgmf_color			*pC;	//color pointer
};

////////////////////////////////////////////////////////////
/// Map-sprite struct
////////////////////////////////////////////////////////////
struct bgmf_sprite
{
	bgmf_sprite(){}
	bgmf_sprite(glm::vec3 p, unsigned int tid){
		pos = p;
		texid = tid;
	}

	glm::vec3 pos;
	unsigned int texid;
};

////////////////////////////////////////////////////////////
/// Map-object struct
////////////////////////////////////////////////////////////
struct bgmf_object
{
	unsigned id;
	glm::vec3 pos;
};

////////////////////////////////////////////////////////////
/// Map header as contained in the actual .bgmf file
////////////////////////////////////////////////////////////
struct bgmf_header
{
    char BGMF[4]; // 'B' 'G' 'M' 'F'
    uint32_t poly_offset;
    uint32_t pc;
    /*  bgmf format is:
            Header : BGMF.....sizeof(header)
            Mask: sizeof(uint32_t) * poly_count
            Texture coords: sizeof(glm::vec2) * poly_count * 3
            Polygons: sizeof(glm::vec3) * poly_count * 3
			Sprites: sizeof(bgmf_sprite) * sprite_count
    */
	/*Notes on texture encodings:
	 * I've decided to put texture encodings right after
	 * the map header in the file. It starts out as a 8-bit
	 * length number followed by the actuall string. The way
	 * its read from the file is just like that with the
	 * reading loop being terminated by a 0x00 length byte */
};

////////////////////////////////////////////////////////////
/// Binary Game Map Format struct
////////////////////////////////////////////////////////////
struct bgmf
{
    bgmf_header header;

	///Polygon Data
	std::vector<std::string>	texpath;
	std::vector<unsigned int>	texture; //>Map texture paths
    std::vector<uint32_t>       mask; //>polygon masks
    std::vector<bgmf_poly_tex>  texcoord; //>polygon texture coords
    std::vector<bgmf_color>     color; //>polygon colors(per vertex)
    std::vector<bgmf_poly>      poly; //>polygon vertexes(TRIANGLES)
	std::vector<bgmf_poly_view>	Polygon;
/*
	///Static sprites/scenery data
	std::vector<bgmf_sprite>	sprite;

	///Map unique data
	std::vector<glm::vec3>	player_spawn;
	unsigned 				jetsupply;
*/
};

////////////////////////////////////////////////////////////
/// BGMF helper api
////////////////////////////////////////////////////////////
extern bgmf *bgmfopen(const char *fpath);
extern void bgmfsave(bgmf *map, const char *fpath);
extern void bgmfdelete(bgmf *map);
extern void bgmfaddpolygon(bgmf *map, bgmf_poly p);
extern void bgmfremovepoly(bgmf *map, bgmf_poly p);
extern void bgmfappend(bgmf *map, uint32_t &mask, bgmf_poly_tex &t, bgmf_color &color, bgmf_poly &p);

extern bgmf_poly_tex *bgmfnewptc(float u0, float v0, float u1, float v1, float u2, float v2);
extern bgmf_poly *bgmfnewpoly(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

////////////////////////////////////////////////////////////
///helper macros
////////////////////////////////////////////////////////////
#define bgmftexpolyzero bgmf_poly_tex(glm::vec2(0,0),glm::vec2(0,0),glm::vec2(0,0))
#define bgmfplaintpc bgmfnewptc(0.0f,0.0f,0.0f,1.0f,1.0f,1.0f);
#define bgmfpolyzero bgmf_poly(glm::vec3(0,0,0),glm::vec3(0,0,0),glm::vec3(0,0,0))
#define bgmfdefaulttc bgmf_poly_tex(glm::vec2(0.0f,0.0f),glm::vec2(0.0f,1.0f),glm::vec2(1.0f,1.0f))
#define bgmfdefaultcol bgmf_color(glm::vec4(1.0f,1.0f,1.0f,1.0f), \
                                  glm::vec4(1.0f,1.0f,1.0f,1.0f), \
                                  glm::vec4(1.0f,1.0f,1.0f,1.0f))
#define bgmfsetcolorall(color) bgmf_color((color),(color),(color))

#endif //MAP_HEADER_GUARD
