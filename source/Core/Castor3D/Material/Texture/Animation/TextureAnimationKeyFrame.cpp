#include "Castor3D/Material/Texture/Animation/TextureAnimationKeyFrame.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

CU_ImplementSmartPtr( c3d, TextureAnimationKeyFrame )

namespace c3d
{
	TextureAnimationKeyFrame::TextureAnimationKeyFrame( TextureAnimation & parent
		, Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< TextureAnimation >{ parent }
	{
	}

	AnimationKeyFrameUPtr TextureAnimationKeyFrame::clone( Animation & parent )const
	{
		auto result = makeUniqueDerived< AnimationKeyFrame, TextureAnimationKeyFrame >( static_cast< TextureAnimation & >( parent )
			, getTimeIndex() );
		doCloneInto( *result );
		return result;
	}
}
