#include "Castor3D/Material/Texture/Animation/TextureAnimationKeyFrame.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

CU_ImplementSmartPtr( castor3d, TextureAnimationKeyFrame )

namespace castor3d
{
	TextureAnimationKeyFrame::TextureAnimationKeyFrame( TextureAnimation & parent
		, castor::Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< TextureAnimation >{ parent }
	{
	}

	AnimationKeyFrameUPtr TextureAnimationKeyFrame::clone( AnimationT< Engine > & parent )const
	{
		auto result = castor::makeUniqueDerived< AnimationKeyFrame, TextureAnimationKeyFrame >( static_cast< TextureAnimation & >( parent )
			, getTimeIndex() );
		doCloneInto( *result );
		return result;
	}
}
