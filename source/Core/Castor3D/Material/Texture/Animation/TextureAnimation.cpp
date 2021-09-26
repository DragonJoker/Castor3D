#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"

using namespace castor;

namespace castor3d
{
	TextureAnimation::TextureAnimation( TextureUnit & texture
		, String const & name )
		: AnimationT< Engine >{ AnimationType::eTexture
			, texture
			, name }
		, m_translate{}
	{
		m_length = castor::Milliseconds{ std::numeric_limits< int64_t >::max() };
	}

	castor::Point3f TextureAnimation::getTranslate( castor::Milliseconds const & time )const
	{
		return castor::Point3f{ m_translate.getDistance( time ) };
	}

	castor::Angle TextureAnimation::getRotate( castor::Milliseconds const & time )const
	{
		return m_rotate.getDistance( time );
	}

	castor::Point3f TextureAnimation::getScale( castor::Milliseconds const & time )const
	{
		if ( m_scale.getValue() != castor::Point2f{ 0, 0 } )
		{
			return castor::Point3f{ m_scale.getDistance( time ) };
		}

		return castor::Point3f{ 1.0f, 1.0f, 1.0f };
	}
}
