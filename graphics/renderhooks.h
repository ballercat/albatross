#ifndef RENDERHOOKS_HEADER_GUARD
#define RENDERHOOKS_HEADER_GUARD

#include"generics.h"
#include"sprite.h"
#include<string>
#include<vector>

namespace gfx
{
	////////////////////////////////////////////////////////////
	/// Global render pointers
	////////////////////////////////////////////////////////////
	struct renderPointers
	{
		void (*spriteBuild)(Sprite *);
		void (*spriteDraw)(Sprite *);

		struct globalShaders_t{
			Shader	*sprDefault;
			Shader	*sprAnimated;
			Shader	*Default;
		};

		globalShaders_t glsl;
	};

	typedef GenericSingleton<renderPointers> Link;
}

#endif //RENDERHOOKS_HEADER_GUARD
