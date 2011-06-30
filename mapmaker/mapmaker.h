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

#ifndef MAPMAKER_HEADER_GUARD
#define MAPMAKER_HEADER_GUARD

#include <vector>
#include <glm/glm.hpp>
#include <cstdio>
#include <string>
#include "map.h"
#include "fixed_pipeline.h"
#include "input.h"

struct spriteView{
	Sprite		spr;
	bgmf_sprite	*ptr;
};

struct MapMaker
{
public:
	MapMaker();

public:
	struct change_struct;

	void Step();
    bool Pick(change_struct *ch, size_t range);
    void ApplyChange(change_struct *ch, bool leftdown);
	void Init();

public:
	////////////////////////////////////////////////////////////
	/// Save map file
	////////////////////////////////////////////////////////////
    bool saveMap(const char *fpath)
    {
        bgmfsave(map, fpath);

        return true;
    }

	////////////////////////////////////////////////////////////
	/// Load map file
	////////////////////////////////////////////////////////////
	bool loadMap(const char *fpath)
    {
        if(map){
            bgmfdelete(map);
            map = NULL;
        }
        map = bgmfopen(fpath);
        if(!map->error){
            printf("Loaded %s\n",fpath);
            printf("Poly count: %d\n",map->header.pc);
        }
		else
		{
			//error with opening the map...
			return false;
		}

        return true;
    }

	////////////////////////////////////////////////////////////
	/// This will find a vertex in 10px by 10px area
    ////////////////////////////////////////////////////////////
	inline glm::vec3* findVertex(float& mx, float &my,glm::vec3* ignore=NULL)
    {
        static float area = 10.0f;
        glm::vec3 *v = NULL;
        glm::vec3 *final = NULL;
        sf::Rect<float> pick;

        glm::vec3 a;
        for(bgmf_poly *p = &map->poly[0];
            p != &map->poly[map->header.pc]; p++)
        {
            a = p->data[0];
            pick = sf::Rect<float>(a.x-area,a.y-area,a.x+area,a.y+area);
            if(pick.Contains(mx,my))
                v = &p->data[0];;

            a = p->data[1];
            pick = sf::Rect<float>(a.x-area,a.y-area,a.x+area,a.y+area);
            if(pick.Contains(mx,my))
                v = &p->data[1];;

            a = p->data[2];
            pick = sf::Rect<float>(a.x-area,a.y-area,a.x+area,a.y+area);
            if(pick.Contains(mx,my))
                v = &p->data[2];;

            if(ignore != v)
                final = v;
        }

        return final;
    }

	////////////////////////////////////////////////////////////
	/// Change property struct
	////////////////////////////////////////////////////////////
    struct change_struct{
	public:
		////////////////////////////////////////////////////////////
		/// ctor
		////////////////////////////////////////////////////////////
		change_struct() { this->Clear(); }

	public:
		////////////////////////////////////////////////////////////
		/// Clear all the data
		////////////////////////////////////////////////////////////
		inline void Clear(void)
		{
			poly		= NULL;
			vertex		= NULL;
			vertex_color = NULL;
			spawn		= NULL;
			pPolygon	= NULL;
			sprite		= NULL;
			move		= false;
			color		= false;
			remove		= false;
			pick_vertex	= false;
			pick_poly	= false;
			pick_spawn	= false;
			pick_sprite	= false;
			place		= false;
			select		= false;
			picked		= false;
			spawna		= false;
			spawnb		= false;
			index		= 0;
		}

	public:
		////////////////////////////////////////////////////////////
		/// data
		////////////////////////////////////////////////////////////
        bgmf_poly   	*poly;
        glm::vec3   	*vertex;
        glm::vec4   	*vertex_color;
		glm::vec2		*spawn;
		spriteView		*sprite;
		bgmf_vert		pVert;
		bgmf_poly_view	*pPolygon;
        bool 			move;
        bool 			color;
        bool 			remove;
        bool 			pick_vertex;
        bool 			pick_poly;
		bool			pick_spawn;
		bool			pick_sprite;
		bool			select;
		bool 			picked;
		bool 			spawna;
		bool 			spawnb;
		bool			place;

        size_t      	index;
    };

	change_struct	change;

public:
	////////////////////////////////////////////////////////////
	/// Texture handling
	////////////////////////////////////////////////////////////
	void	loadTextureData();
	void	loadSpriteData();

	inline void FixTC(int i, size_t pTexID){
		float tsz		= 1.0f / map->texpath.size();

		glm::vec2	swap;
		int sz	= map->poly[i].data.size();
		map->texcoord[i].data.clear();

		for(int k=0;k<sz;k++){
			swap = _DefaultTC[k%4];
			swap.x = swap.x * tsz + pTexID * tsz;
			map->texcoord[i].data.push_back(swap);
		}
	}

public:
	/// Graphics data
	struct GfxData {
		GLuint	Texture;
		std::vector<GLuint>	SpriteTextures;
		std::vector<bgmf_poly_tex>	tc;
		std::vector<spriteView>	Scenery;
	};

	GfxData	gfx;
	GLuint	SpawnPointTexture;
	Sprite	*SpawnPoint;

public:
    /// Data
	glm::vec3 grid[128*76];
	int count;
	bool drawgrid;
    bool load;
    bool vertex_snap;
	std::vector<glm::vec3> vertex;
	std::vector<Vertex>		nverts;
	std::vector<GLushort>	nindx;
	bgmf_poly_tex texcoord;
	glm::vec2				_DefaultTC[4];
	bgmf_poly	 polynew;
    glm::vec3 mouse;
    glm::vec3 lastmouse;
	std::string		pWorkingDir;
    bgmf *map;

public:
	/// Events & IO
    Input				*input;
    gfx::Core			*display;
	EventQueue			pEvents;
};



#endif //MAPMAKER_HEADER_GUARD
