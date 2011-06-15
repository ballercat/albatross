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

#include "map.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

bgmf* bgmfopen(const char *fpath)
{
    FILE *mapfile;

    mapfile = fopen(fpath, "rb");
    if(!mapfile){
        fprintf(stderr,"Could not read .bgmf file: %s\n",fpath);
        return NULL;
    }

    bgmf *map = new bgmf;
    fread(map, sizeof(bgmf_header), 1, mapfile);

    if(map->header.BGMF[0] != 'B' || map->header.BGMF[1] != 'G' ||
       map->header.BGMF[2] != 'M' || map->header.BGMF[3] != 'F'){

        fprintf(stderr, "Corrupted map header\n");
        fclose(mapfile);

        delete map;
        map = NULL;
        return NULL;
    }

    map->poly.resize(map->header.pc,bgmfpolyzero);
    map->texcoord.resize(map->header.pc,bgmftexpolyzero);
    map->color.resize(map->header.pc,bgmf_color());
    map->mask.resize(map->header.pc,POLY_DEFAULT);
	map->texture.resize(map->header.pc,0);

	/*NOTE: Not reading/writing the whole map struct at once
	 * 		makes it simpler to add new features to the map */

	//Read in texture names
	{
		uint32_t temp = 0;
		char buffer;
		std::string file;

		fread(&temp, sizeof(uint32_t), 1, mapfile);

		while(temp != 0){

			for(size_t i=0;i<temp;i++){
				fread(&buffer, sizeof(char), 1, mapfile);
				file += buffer;
			}
			//fread(&buffer, sizeof(char), temp, mapfile);
			map->texpath.push_back(file);

			fread(&temp, sizeof(uint32_t), 1, mapfile);
			file = "";
		}

		//map->texpath.push_back("lumpybark.bmp");
	}
	fread(&map->texture[0], sizeof(uint32_t) * map->header.pc, 1, mapfile);
    fread(&map->mask[0], sizeof(uint32_t) * map->header.pc, 1, mapfile);
    fread(&map->color[0], sizeof(bgmf_color) * map->header.pc, 1, mapfile);
    fread(&map->poly[0], sizeof(bgmf_poly) * map->header.pc, 1, mapfile);

	//Populate Polygon vector
	{
		bgmf_poly_view	ViewPolygon;
		for(size_t i = 0;i < map->header.pc;i++){
			ViewPolygon.pM	= map->mask[i];
			ViewPolygon.pC	= &map->color[i];
			ViewPolygon.pP	= &map->poly[i];
			ViewPolygon.pTC = &map->texcoord[i];
			ViewPolygon.pID = i;
			ViewPolygon.pT 	= &map->texture[i];

			map->Polygon.push_back(ViewPolygon);
		}
	}


    fclose(mapfile);

    return map;
}

void bgmfsave(bgmf *map, const char *fpath)
{
    FILE *mapfile;

    mapfile = fopen(fpath, "wb");
    if(!mapfile){
        fprintf(stderr, "Could not save to file : %s\n",fpath);
        return;
    }
    map->header.poly_offset = sizeof(bgmf_header);

	//Write all the data to file
	unsigned int temp = 0;

	fwrite(map, sizeof(bgmf_header), 1, mapfile);
	//write texture names
	{
		for(size_t i=0;i<map->texpath.size();i++){
			temp = map->texpath[i].length();
			fwrite(&temp, sizeof(unsigned int), 1, mapfile);
			fwrite(map->texpath[i].c_str(), sizeof(char), temp, mapfile);
		}

		//Terminate list with 0
		temp = 0;
		fwrite(&temp, sizeof(unsigned int), 1, mapfile);
	}

	fwrite(&map->texture[0], sizeof(uint32_t)*map->header.pc, 1, mapfile);
    fwrite(&map->mask[0], sizeof(uint32_t)*map->header.pc, 1, mapfile);
    fwrite(&map->color[0], sizeof(bgmf_color)*map->header.pc, 1, mapfile);
    //fwrite(&map->texcoord[0], sizeof(bgmf_poly_tex)*map->header.pc, 1, mapfile);
    fwrite(&map->poly[0], sizeof(bgmf_poly)*map->header.pc, 1, mapfile);

    fclose(mapfile);
}

void bgmfdelete(bgmf *map)
{
    if(map){
        map->mask.clear();
        map->poly.clear();
        map->texcoord.clear();
        delete map;
    }
}

void bgmfremovepoly(bgmf *map, bgmf_poly p)
{
    std::vector<bgmf_poly>::iterator it = std::find(map->poly.begin(),map->poly.end(),p);
    size_t index = it - map->poly.begin();

    map->poly.erase(it);
    map->mask.erase(map->mask.begin()+index);
    map->texcoord.erase(map->texcoord.begin()+index);
	map->texture.erase(map->texture.begin()+index);
    map->color.erase(map->color.begin()+index);

    map->header.pc--;
}

void bgmfappend(bgmf *map, uint32_t &mask, bgmf_poly_tex &t, bgmf_color &color, bgmf_poly &p)
{
    map->mask.push_back(mask);
	map->texture.push_back(0);
    map->texcoord.push_back(t);
    map->poly.push_back(p);
    map->color.push_back(color);

	bgmf_poly_view	ViewPolygon;
	ViewPolygon.pM	= map->mask[map->header.pc];
	ViewPolygon.pC	= &map->color[map->header.pc];
	ViewPolygon.pP	= &map->poly[map->header.pc];
	ViewPolygon.pTC = &map->texcoord[map->header.pc];
	ViewPolygon.pID = map->header.pc;
	ViewPolygon.pT 	= &map->texture[map->header.pc];
	map->Polygon.push_back(ViewPolygon);

	map->header.pc++;
}

void bgmfaddpolygon(bgmf *map, bgmf_poly poly)
{

}

bgmf_poly_tex *bgmfnewptc(float u0, float v0, float u1, float v1, float u2, float v2)
{
    bgmf_poly_tex *texcoord = new bgmf_poly_tex;
    texcoord->data[0].x = u0; texcoord->data[0].y = v0;
    texcoord->data[1].x = u1; texcoord->data[1].y = v1;
    texcoord->data[2].x = u2; texcoord->data[2].y = v2;

    return texcoord;
}

bgmf_poly *bgmfnewpoly(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    bgmf_poly *polygon = new bgmf_poly;
    polygon->data[0] = v0;
    polygon->data[1] = v1;
    polygon->data[2] = v2;

    return polygon;
}
