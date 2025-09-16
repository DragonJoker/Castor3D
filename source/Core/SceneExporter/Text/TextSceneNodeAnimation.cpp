#include "TextSceneNodeAnimation.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextQuaternion.hpp>

namespace c3d
{
	template<>
	class TextWriter< SceneNodeAnimationKeyFrame >
		: public TextWriterT< SceneNodeAnimationKeyFrame >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< SceneNodeAnimationKeyFrame >{ tabs }
		{
		}

		bool operator()( SceneNodeAnimationKeyFrame const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, cuT( "keyframe" ) ) } )
			{
				result = write( file, cuT( "index " ), object.getTimeIndex().count() )
					&& writeNamedSub( file, cuT( "position " ), object.getPosition() )
					&& writeNamedSub( file, cuT( "rotation " ), object.getRotation() )
					&& writeNamedSub( file, cuT( "scale " ), object.getScale() );
			}

			return result;
		}
	};

	TextWriter< SceneNodeAnimation >::TextWriter( String const & tabs )
		: TextWriterT< SceneNodeAnimation >{ tabs }
	{
	}

	bool TextWriter< SceneNodeAnimation >::operator()( SceneNodeAnimation const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing SceneNodeAnimation " ) << object.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "node_animation" ), object.getName() ) } )
		{
			result = true;
			TextWriter< SceneNodeAnimationKeyFrame > writer{ tabs() };
			for ( auto const & keyframe : object )
				result = result && writer( static_cast< SceneNodeAnimationKeyFrame const & >( *keyframe ), file );
		}

		return result;
	}
}
