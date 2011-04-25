#ifndef ANIMATION_HEADER_GUARD
#define ANIMATION_HEADER_GUARD

#include <vector>
#include <glm/glm.hpp>
#include "assets.h"
#include <cstdio>

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace animation
{
	#define JOINTCOLOR_PLAIN	0.0f, 0.6f, 0.6f, 1.0f
	#define JOINTCOLOR_SELECT	0.0f, 1.0f, 0.0f, 1.0f
	#define BONECOLOR_PLAIN		0.0f, 0.0f, 0.0f, 1.0f

	////////////////////////////////////////////////////////////
	///bone struct
	////////////////////////////////////////////////////////////
	struct Bone_t{
	public:
		///bone joint
		struct _joint_t{
			glm::vec3 	p; //position
			float		a; //angle
		};
	public:
		///Default Constructor
		Bone_t(void){}

		///
		Bone_t(Bone_t *parent){

		}

		Bone_t(glm::vec3 pA, glm::vec3 pB, float plen, unsigned id){
			len = plen;
			A.p = pA;
			A.a = 0.0f;
			B.p = pB;
			B.a = 0.0f;

			id = id;

			front = &B;
			back = &A;

			center = ((B.p - A.p) / 2.0f) + A.p;
		}

		Bone_t(Bone_t *p,glm::vec3 pA, glm::vec3 pB, unsigned id){
			parent = p;

			A.p = p->B.p;
			A.a = 0.0f;
			B.p = pB;
			B.a = 0.0f;

			id = id;

			front = &B;
			back = &A;

			len = glm::distance(A.p, pB);
			center = ((B.p - A.p) / 2.0f) + A.p;
		}

		///Constructor
		Bone_t(float& plength, glm::vec3& ppos,_joint_t *parent=NULL){
			back = parent;
			front = NULL;

			len = plength;
			pos = ppos;
		}
	public:
		///Helper functions

		///Set anchor
		void setAnchor(glm::vec3 pA){
			glm::vec3 diff = A.p - pA;
			A.p = pA;

			if(!child.empty()){
				printf("Propagate id: %i\n",this->id);
				// Propagate the change trough all the children
				for(Bone_t* cp = child[0]; cp != child[child.size()]; cp ++){
					cp->setAnchor((cp->A.p + diff));
				}
			}
		}

		///Set the z-angle
		void setAngle(float pz){
			B.a = pz;

			if(!child.empty()){
				for(Bone_t* cp = child[0]; cp != child[child.size()]; cp++){
					cp->setAngle(pz);
				}
			}
		}

	public:
		///operators
		bool operator==(const Bone_t& compare){
			if(compare.id == id)
				return true;
			return false;
		}

	public:
		///Data
		_joint_t	A;
		_joint_t	B;

		_joint_t	*front;
		_joint_t	*back;

		float		len; //length
		glm::vec3 	pos; //position
		glm::vec3	center;

		Bone_t *parent;
		unsigned id;

		std::vector<Bone_t*>	child;
	};


	////////////////////////////////////////////////////////////
	/// Skeletal Animation class
	////////////////////////////////////////////////////////////
	class Skeletal_t{
	public:
		///Constructor
		Skeletal_t(float plen);

	public:
		///Member functions
		Bone_t* addBone(float plen, Bone_t *parent = NULL);
		void removeBone(Bone_t& pbone);

		void draw();
		void buildDefaultBody();
		void step(float pdt);

	public:
		///Data
		float 		len; //Length of the animation
		unsigned 	num_frames;
		std::vector<glm::vec3>	vdata;
		std::vector<glm::vec4>	cdata;

		size_t count;
		std::vector<Bone_t>	_bones;

	private:
		///Private Data
		std::vector<std::vector<float> > _frame;
	};

	extern void drawBone(Bone_t *bone);

} //namespace animation

#endif //#define ANIMATION_HEADER_GUARD
