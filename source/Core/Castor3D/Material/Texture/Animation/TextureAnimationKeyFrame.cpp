#include "Castor3D/Material/Texture/Animation/TextureAnimationKeyFrame.hpp"

CU_ImplementSmartPtr( castor3d, TextureAnimationKeyFrame )

namespace castor3d
{
	TextureAnimationKeyFrame::TextureAnimationKeyFrame( TextureAnimation & parent
		, castor::Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< TextureAnimation >{ parent }
	{
	}
}
