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

////////////////////////////////////////////////////////////
/// Open a .bgmf file format, returns a new struct
////////////////////////////////////////////////////////////
bgmf* bgmfopen(const char *fpath)
{
	bgmf *map = new bgmf;
    FILE *mapfile;

    mapfile = fopen(fpath, "rb");
    if(!mapfile){
        fprintf(stderr,"Could not read .bgmf file: %s\n",fpath);
        map->error = BGMFERR_FILE;
		return map;
    }


    fread(map, sizeof(bgmf_header), 1, mapfile);

    if(map->header.BGMF[0] != 'B' || map->header.BGMF[1] != 'G' ||
       map->header.BGMF[2] != 'M' || map->header.BGMF[3] != 'F'){

        fprintf(stderr, "Corrupted map header\n");
        fclose(mapfile);

        map->error = BGMFERR_HEADER;
        return map;
    }

	//Version mismatch check
	if(map->header.version.field.number != BGMF_VERSION ){
		//NOTE: Do something more usefull here here
		map->error = BGMFERR_VERSION;
		//return map;
	}

/*
	//Used for swaping in new header formats
	map->nheader.BGMF[0] = 'B';
	map->nheader.BGMF[1] = 'G';
	map->nheader.BGMF[2] = 'M';
	map->nheader.BGMF[3] = 'F';
	map->nheader.poly_offset = map->header.poly_offset;
	map->nheader.pc = map->header.pc;
	map->nheader.rsc = 1;
	map->nheader.bsc = 0;
	map->nheader.sprc = 0;
	map->nheader.X = 'X';
*/

	map->redspawn.resize(map->header.rsc, glm::vec2(0,0));
	map->bluespawn.resize(map->header.bsc, glm::vec2(0,0));
	map->sprite.resize(map->header.sprc, bgmf_sprite());
    map->poly.resize(map->header.pc,bgmf_poly());
    map->texcoord.resize(map->header.pc,bgmf_poly_tex());
    map->color.resize(map->header.pc,bgmf_color());
    map->mask.resize(map->header.pc,bgmf_pmask(0));
	map->texture.resize(map->header.pc,0);

	/*NOTE: Not reading/writing the whole map struct at once
	 * 		makes it simpler to add new features to the map */

	//Read in spawn locations
	fread(&map->redspawn[0], sizeof(glm::vec2), map->header.rsc, mapfile);
	fread(&map->bluespawn[0], sizeof(glm::vec2), map->header.bsc, mapfile);

	uint32_t temp = 0;
	//Read in texture names
	{
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

		//Read in sprite names
		temp = 0;
		std::string sprite_header_path;

		fread(&temp, sizeof(uint32_t), 1, mapfile);

		while(temp != 0){
			for(size_t i=0;i<temp;i++){
				fread(&buffer, sizeof(char), 1, mapfile);
				if(buffer == '.'){
					sprite_header_path = file + ".sprh";
					map->sprheader.push_back(sprite_header_path);
				}
				file += buffer;
			}

			map->sprpath.push_back(file);

			fread(&temp, sizeof(uint32_t), 1, mapfile);
			file = "";
		}
	}

	fread(&map->sprite[0], sizeof(bgmf_sprite) * map->header.sprc, 1, mapfile);
	fread(&map->texture[0], sizeof(uint32_t) * map->header.pc, 1, mapfile);
    fread(&map->mask[0], sizeof(bgmf_pmask) * map->header.pc, 1, mapfile);
	for(int i=0;i<map->header.pc;i++){
		fread(&temp, sizeof(uint32_t), 1, mapfile);
		map->color[i].data.resize(temp, glm::vec4());
		fread(&map->color[i].data[0], sizeof(glm::vec4), temp, mapfile);
	}
    //fread(&map->color[0], sizeof(bgmf_color) * map->header.pc, 1, mapfile);
	for(int i=0;i<map->header.pc;i++){
		fread(&temp, sizeof(uint32_t), 1, mapfile);
		map->poly[i].data.resize(temp, glm::vec3());
		fread(&map->poly[i].data[0], sizeof(glm::vec3), temp, mapfile);

		//Resize the tex coordinates while at it
		map->texcoord[i].data.resize(temp, glm::vec2(0.0f,0.0f));

		map->vertcount += temp;
	}
    //fread(&map->poly[0], sizeof(bgmf_poly) * map->header.pc, 1, mapfile);

	map->hpc = 0;

	//Populate Polygon vector
	{
		bgmf_poly_view	ViewPolygon;
		for(size_t i = 0;i < map->header.pc;i++){
			ViewPolygon.pM	= &map->mask[i];
			ViewPolygon.pC	= &map->color[i];
			ViewPolygon.pP	= &map->poly[i];
			ViewPolygon.pTC = &map->texcoord[i];
			ViewPolygon.pID = i;
			ViewPolygon.pT 	= &map->texture[i];

			map->Polygon.push_back(ViewPolygon);

			if(map->mask[i].bit.hollow) map->hpc++;
		}
	}

	map->hpos = map->header.pc - map->hpc;

    fclose(mapfile);

	map->error = BGMFERR_NONE;
    return map;
}

bgmf* bgmfnew(void)
{
	bgmf	*map = new bgmf;

	map->header.BGMF[0] 	= 'B';
	map->header.BGMF[1] 	= 'G';
	map->header.BGMF[2] 	= 'M';
	map->header.BGMF[3] 	= 'F';
	map->header.version.field.number = BGMF_VERSION;
	map->header.version.field.complex_polygons = true;
	map->header.version.field.split_data = true;
	map->header.pc 			= 0;
	map->header.rsc 		= 0;
	map->header.bsc 		= 0;
	map->header.sprc 		= 0;
	map->header.X 			= 'X';

	map->hpc = 0;
	map->hpos = 0;
	map->vertcount = 0;

	return map;
}

////////////////////////////////////////////////////////////
/// Save .bgmf file
////////////////////////////////////////////////////////////
void bgmfsave(bgmf *map, const char *fpath)
{
    FILE *mapfile;

    mapfile = fopen(fpath, "wb");
    if(!mapfile){
        fprintf(stderr, "Could not save to file : %s\n",fpath);
        return;
    }

	//Before any writing sort the polygons
	bgmfsort(map);

	//Write all the data to file
	unsigned int temp = 0;

	//Write map header
	fwrite(map, sizeof(bgmf_header), 1, mapfile);
	//fwrite(&map->nheader, sizeof(bgmf_newheader), 1, mapfile);

	//Write spawn locations
	fwrite(&map->redspawn[0], sizeof(glm::vec2), map->header.rsc, mapfile);
	fwrite(&map->bluespawn[0], sizeof(glm::vec2), map->header.bsc, mapfile);

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

	//write sprite names
	{
		for(size_t i=0;i<map->sprpath.size();i++){
			temp = map->sprpath[i].length();
			fwrite(&temp, sizeof(uint32_t), 1, mapfile);
			fwrite(map->sprpath[i].c_str(), sizeof(char), temp, mapfile);
		}

		temp = 0;
		fwrite(&temp, sizeof(unsigned int), 1, mapfile);
	}

	fwrite(&map->sprite[0], sizeof(bgmf_sprite)*map->header.sprc, 1, mapfile);
	fwrite(&map->texture[0], sizeof(uint32_t)*map->header.pc, 1, mapfile);
    fwrite(&map->mask[0], sizeof(bgmf_pmask)*map->header.pc, 1, mapfile);
	for(int i=0;i<map->header.pc;i++){
		temp = map->color[i].data.size();
		fwrite(&temp, sizeof(uint32_t), 1, mapfile);
		fwrite(&map->color[i].data[0], sizeof(glm::vec4)*temp, 1, mapfile);
	}
    //fwrite(&map->color[0], sizeof(bgmf_color)*map->header.pc, 1, mapfile);
    //fwrite(&map->texcoord[0], sizeof(bgmf_poly_tex)*map->header.pc, 1, mapfile);
    for(int i=0;i<map->header.pc;i++){
		temp = map->poly[i].data.size();
		fwrite(&temp, sizeof(uint32_t), 1, mapfile);
		fwrite(&map->poly[i].data[0], sizeof(glm::vec3)*temp, 1, mapfile);
	}
	//fwrite(&map->poly[0], sizeof(bgmf_poly)*map->header.pc, 1, mapfile);

    fclose(mapfile);
}

////////////////////////////////////////////////////////////
/// Swap polygon data
////////////////////////////////////////////////////////////
void bgmfswap(bgmf *map, int left, int right)
{
	std::swap(map->mask[left], map->mask[right]);
	std::swap(map->poly[left], map->poly[right]);
	std::swap(map->color[left], map->color[right]);
	std::swap(map->texture[left], map->texture[right]);
	std::swap(map->texcoord[left], map->texcoord[right]);
}

////////////////////////////////////////////////////////////
/// Sort polygon data by the mask values
////////////////////////////////////////////////////////////
static int sortPartition(bgmf* map, int left, int right)
{
	bgmf_pmask pivot = map->mask[left];
	while(true)
	{
		while(map->mask[left].bit.hollow < pivot.bit.hollow) left++;
		while(map->mask[right].bit.hollow > pivot.bit.hollow) right--;

		if(left < right && map->mask[left].bit.hollow != map->mask[right].bit.hollow){
			bgmfswap(map, left, right);
		}
		else {
			return right;
		}
	}

	return right;
}

//actual sorting
static void sortQuickSort(bgmf *map, int left, int right)
{
	int i = left;
	int k = right;

	if( right - left >= 1){
		bgmf_pmask pivot = map->mask[left];

		while( k > i ){
			while(map->mask[i].bit.hollow <= pivot.bit.hollow &&
					i <= right && k > i )
				i++;
			while(map->mask[k].bit.hollow > pivot.bit.hollow &&
					k >= left && k >= i)
				k--;

			if (k > i)
				bgmfswap(map, i, k);

			sortQuickSort(map, left, k - 1);
			sortQuickSort(map, k + 1, right);
		}
	}
	else {
		return;
	}
}

//Sorting wrapper
void bgmfsort(bgmf *map)
{
	if(!map)
		return;

	sortQuickSort(map, 0, map->poly.size()-1);

	//Fix the polygon view
	for(size_t i=0;i<map->poly.size();i++){
		map->Polygon[i].pID	= i;
		map->Polygon[i].pM	= &map->mask[i];
		map->Polygon[i].pTC	= &map->texcoord[i];
		map->Polygon[i].pT	= &map->texture[i];
		map->Polygon[i].pP	= &map->poly[i];
		map->Polygon[i].pC	= &map->color[i];
	}
}

////////////////////////////////////////////////////////////
/// Delete the map
////////////////////////////////////////////////////////////
void bgmfdelete(bgmf *map)
{
    if(map){
        map->mask.clear();
        map->poly.clear();
        map->texcoord.clear();
		map->texture.clear();
		map->Polygon.clear();
		map->color.clear();
		map->sprite.clear();
		map->Data.clear();
    }

	delete map;
}

////////////////////////////////////////////////////////////
/// Remove a polygon from the map
////////////////////////////////////////////////////////////
void bgmfremovepoly(bgmf *map, bgmf_poly p)
{
    std::vector<bgmf_poly>::iterator it = std::find(map->poly.begin(),map->poly.end(),p);
    size_t index = it - map->poly.begin();

    map->poly.erase(it);
    map->mask.erase(map->mask.begin()+index);
    map->texcoord.erase(map->texcoord.begin()+index);
	map->texture.erase(map->texture.begin()+index);
    map->color.erase(map->color.begin()+index);
	map->Polygon.erase(map->Polygon.begin()+index);

	for(size_t i=0;i<map->poly.size();i++){
		map->Polygon[i].pID	= i;
		map->Polygon[i].pM	= &map->mask[i];
		map->Polygon[i].pTC	= &map->texcoord[i];
		map->Polygon[i].pT	= &map->texture[i];
		map->Polygon[i].pP	= &map->poly[i];
		map->Polygon[i].pC	= &map->color[i];
	}

    map->header.pc--;
}

////////////////////////////////////////////////////////////
/// Append a polygon
////////////////////////////////////////////////////////////
void bgmfappend(bgmf *map, bgmf_poly &p, bgmf_poly_tex& t)
{
    map->mask.push_back(bgmf_pmask(0));
	map->texture.push_back(0);
    map->texcoord.push_back(t);
    map->poly.push_back(p);

	glm::vec4	c(1.0f, 1.0f, 1.0f, 1.0f);
	bgmf_color  color;
	color.data.resize(p.data.size(), c);
    map->color.push_back(color);

	bgmf_poly_view	ViewPolygon;
	ViewPolygon.pM	= &map->mask[map->header.pc];
	ViewPolygon.pC	= &map->color[map->header.pc];
	ViewPolygon.pP	= &map->poly[map->header.pc];
	ViewPolygon.pTC = &map->texcoord[map->header.pc];
	ViewPolygon.pID = map->header.pc;
	ViewPolygon.pT 	= &map->texture[map->header.pc];
	map->Polygon.push_back(ViewPolygon);

	map->header.pc++;

	map->vertcount += p.data.size();
}

////////////////////////////////////////////////////////////
/// ?
////////////////////////////////////////////////////////////
void bgmfaddpolygon(bgmf *map, bgmf_poly poly)
{
	//void ...
}

////////////////////////////////////////////////////////////
/// Helper function, creates a texture coord
////////////////////////////////////////////////////////////
bgmf_poly_tex *bgmfnewptc(float u0, float v0, float u1, float v1, float u2, float v2)
{
    bgmf_poly_tex *texcoord = new bgmf_poly_tex;
    texcoord->data[0].x = u0; texcoord->data[0].y = v0;
    texcoord->data[1].x = u1; texcoord->data[1].y = v1;
    texcoord->data[2].x = u2; texcoord->data[2].y = v2;

    return texcoord;
}

////////////////////////////////////////////////////////////
/// Helper function, creates a new poly struct
////////////////////////////////////////////////////////////
bgmf_poly *bgmfnewpoly(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    bgmf_poly *polygon = new bgmf_poly;
    polygon->data[0] = v0;
    polygon->data[1] = v1;
    polygon->data[2] = v2;

    return polygon;
}
