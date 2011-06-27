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
#include<cstring>

//Map polygon masks
#define POLY_VISIBLE 	0x01
#define POLY_SOLID 		0x00
#define POLY_HOLLOW		0x02
#define POLY_DEFAULT	0x01

#define BGMF_VERSION	0x01

struct MapVertex
{
	MapVertex() :
		v(glm::vec3()),
		t(glm::vec2()),
		c(glm::vec4())
		{

		}
	MapVertex(glm::vec3 &p_Vert, glm::vec2 &p_Tc, glm::vec4 &p_Col) :
		v(p_Vert),
		t(p_Tc),
		c(p_Col)
		{

		}

	glm::vec3	v;
	glm::vec2	t;
	glm::vec4	c;
};

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
	bgmf_poly(const glm::vec3 *p_Data, uint32_t p_Count){
		data.resize(p_Count, glm::vec3(0.0f, 0.0f, 0.0f));
		memcpy(&data[0], p_Data, sizeof(glm::vec3)*p_Count);
	}

    bool operator ==(const bgmf_poly &test) const
    {
		if(test.data.size() != data.size())
			return false;

		for(unsigned i =0;i<data.size();i++){
			if(data[i] != test.data[i])
				return false;
		}

        return true;
    }

	bgmf_poly& operator=(const bgmf_poly &p_Swap)
	{
		data.clear();
		data = p_Swap.data;

		return *this;
	}

	std::vector<glm::vec3> data;
};

////////////////////////////////////////////////////////////
/// 3xglm::vec2(u,v) Polygon-texture struct
////////////////////////////////////////////////////////////
struct bgmf_poly_tex
{
    bgmf_poly_tex(){}
    bgmf_poly_tex(const glm::vec2 *p_Data, size_t p_Count)
    {
        data.resize(p_Count, glm::vec2(0.0f, 0.0f));
		memcpy(&data[0], p_Data, p_Count*sizeof(glm::vec2));
    }

	bgmf_poly_tex& operator=(const bgmf_poly_tex& p_Swap)
	{
		data.clear();
		data = p_Swap.data;

		return *this;
	}

    std::vector<glm::vec2> data;
};

////////////////////////////////////////////////////////////
/// 3xglm::vec4(r,g,b,a) Polygon-color struct
////////////////////////////////////////////////////////////
struct bgmf_color
{
    bgmf_color(){}
    bgmf_color(const glm::vec4 *p_Data, size_t p_Count)
    {
        data.resize(p_Count, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		memcpy(&data[0], p_Data, sizeof(glm::vec4)*p_Count);
    }

	bgmf_color& operator=(const bgmf_color& p_Swap)
	{
		data.clear();
		data = p_Swap.data;

		return *this;
	}

    std::vector<glm::vec4>	data;
};

////////////////////////////////////////////////////////////
/// Polygon mask union
////////////////////////////////////////////////////////////
union bgmf_pmask
{
	bgmf_pmask(uint32_t p_Value)
	{
		full = p_Value;
	}

	struct value{
		unsigned visible 	: 1;
		unsigned hollow		: 1;
		unsigned unused		: 22;
		unsigned customid	: 8;
	};
	value bit;

	bgmf_pmask& operator=(const bgmf_pmask& p_Swap)
	{
		full = p_Swap.full;

		return *this;
	}

	uint32_t	full;
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
	bgmf_pmask			*pM;	//mask
	unsigned int		*pT;	//texture #
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
	bgmf_sprite(glm::vec3 p, unsigned int tid) :
		pos(p),
		id(tid)
	{}

	glm::vec3 pos;
	unsigned int id;
};

////////////////////////////////////////////////////////////
/// Errors
////////////////////////////////////////////////////////////
enum{
	BGMFERR_NONE = 0,
	BGMFERR_VERSION,
	BGMFERR_HEADER,
	BGMFERR_FILE,
	BGMFERR_UNKNOWN
};

union bgmf_version
{
	struct info{
		unsigned	number : 8;
		unsigned	split_data : 1;
		unsigned	complex_polygons : 1;
		unsigned 	unused : 22;
	};

	info		field;
	uint32_t	value;
};
////////////////////////////////////////////////////////////
/// Map header as contained in the actual .bgmf file
////////////////////////////////////////////////////////////
struct bgmf_header
{
    char 			BGMF[4]; // 'B' 'G' 'M' 'F'
    bgmf_version 	version; //simply the size of the header
    uint32_t 		pc;
	uint8_t			rsc;
	uint8_t			bsc;
	uint32_t		sprc;
	char			X; //end
    /*NOTE: (this is old)  bgmf format is:
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

struct bgmf_newheader
{
    char 		BGMF[4]; // 'B' 'G' 'M' 'F'
    uint32_t 	version; //just the size of the header
    uint32_t 	pc;
	uint8_t		rsc;
	uint8_t		bsc;
	uint32_t	sprc; //sprite count;
	char		X; //end
};

////////////////////////////////////////////////////////////
/// Binary Game Map Format struct
////////////////////////////////////////////////////////////
struct bgmf
{
    bgmf_header header;
	bgmf_newheader nheader;

	///Polygon Data
	std::vector<std::string>	texpath;
	std::vector<std::string>	sprpath;
	std::vector<std::string>	sprheader;
	std::vector<bgmf_sprite>	sprite;
	std::vector<unsigned int>	texture; //>Map texture paths
    std::vector<bgmf_pmask>     mask; //>polygon masks
    std::vector<bgmf_poly_tex>  texcoord; //>polygon texture coords
    std::vector<bgmf_color>     color; //>polygon colors(per vertex)
    std::vector<bgmf_poly>      poly; //>polygon vertexes(TRIANGLES)
	std::vector<bgmf_poly_view>	Polygon;
	uint32_t					hpc; //>Hollow Polygon count
	uint32_t					hpos; //pc - hpc
	float	texlength;

	std::vector<glm::vec2>		redspawn;
	std::vector<glm::vec2>		bluespawn;

	uint32_t					error;
	uint32_t					vertcount;

	std::vector<MapVertex>		Data;
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
extern bgmf *bgmfnew(void);
extern void bgmfsave(bgmf *map, const char *fpath);
extern void bgmfdelete(bgmf *map);
extern void bgmfaddpolygon(bgmf *map, bgmf_poly p);
extern void bgmfremovepoly(bgmf *map, bgmf_poly p);
extern void bgmfappend(bgmf *map, bgmf_poly &p, bgmf_poly_tex &t);
extern void bgmfsort(bgmf *map);
extern void bgmfswap(bgmf *map, int left, int right);

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
