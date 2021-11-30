#include "Castor3D/Material/Texture/Animation/TextureAnimationKeyFrame.hpp"

namespace castor3d
{
	TextureAnimationKeyFrame::TextureAnimationKeyFrame( TextureAnimation & parent
		, castor::Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< TextureAnimation >{ parent }
	{
	}
}
