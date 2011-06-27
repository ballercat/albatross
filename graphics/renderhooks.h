#ifndef RENDERHOOKS_HEADER_GUARD
#define RENDERHOOKS_HEADER_GUARD

#include"generics.h"
#include"sprite.h"

namespace gfx
{
	struct renderFunctionPointers
	{
		void (*spriteBuild)(Sprite *);
		void (*spriteDraw)(Sprite *);
	};

	typedef GenericSingleton<renderFunctionPointers> Link;
}

#endif //RENDERHOOKS_HEADER_GUARD
