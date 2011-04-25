#ifndef ANIMATOR_HEADER_GUARD
#define ANIMATOR_HEADER_GUARD

#include "animation.h"
#include <vector>
#include <glm/glm.hpp>
#include <cstdio>
#include <string>
#include <SFML/Graphics.hpp>

////////////////////////////////////////////////////////////
///Animator
////////////////////////////////////////////////////////////
class Animator
{
public:
	static animation::Bone_t*
	findBJoint(glm::vec3& ms, animation::Bone_t* bones, unsigned size,float range)
	{
		animation::Bone_t *bn;
		glm::vec3 v;
		sf::Rect<float> pick;
		for(size_t i = 0; i < size; i++){
			bn = &bones[i];

			v = bn->B.p;
			pick = sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
			if(pick.Contains(ms.x,ms.y)){
				return bn;
			}
		}

		return NULL;
	}

	static animation::Bone_t*
	findAnchor(glm::vec3& ms, animation::Bone_t* bones, unsigned size, float range)
	{
		glm::vec3 v;
		sf::Rect<float> pick;
		for(animation::Bone_t* bn = &bones[0]; bn != &bones[size]; bn++){

			v = bn->A.p;
			pick = sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
			if(pick.Contains(ms.x, ms.y)){
				return bn;
			}
		}
		return NULL;
	}

	animation::Skeletal_t body;
};




#endif //#define ANIMATOR_HEADER_GUARD
