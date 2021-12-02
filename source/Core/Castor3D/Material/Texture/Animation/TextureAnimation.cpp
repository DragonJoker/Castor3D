#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationKeyFrame.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

namespace castor3d
{
	TextureAnimation::TextureAnimation( Engine & engine
		, castor::String const & name )
		: AnimationT< Engine >{ engine
			, AnimationType::eTexture
			, name }
		, m_translate{}
	{
		m_length = castor::Milliseconds{ std::numeric_limits< int64_t >::max() };
	}

	void TextureAnimation::setAnimable( TextureUnit & unit )
	{
		for ( auto pending : m_pending )
		{
			static_cast< AnimatedTexture & >( *pending ).setTexture( unit );
			pending->addAnimation( unit.getAnimation().getName() );
		}
	}

	void TextureAnimation::initialiseTiles( TextureUnit const & unit )
	{
		if ( m_tileAnim )
		{
			m_length = 0_ms;
			auto tiles = unit.getTexture()->getImage().getPxBuffer().getTiles()->z;
			auto tileSet = unit.getConfiguration().tileSet;
			castor::Milliseconds timeIndex{};
			castor::Milliseconds timeStep{ 25_ms };
			uint32_t tile = 0u;

			for ( uint32_t y = 0u; y < tileSet->w; ++y )
			{
				for ( uint32_t x = 0u; x < tileSet->z; ++x )
				{
					if ( tile < tiles )
					{
						auto kf = std::make_unique< TextureAnimationKeyFrame >( *this, timeIndex );
						kf->setTile( { x, y } );
						addKeyFrame( std::move( kf ) );
						timeIndex += timeStep;
					}

					++tile;
				}
			}

			
		}
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

	bool TextureAnimation::isTransformAnimated()const
	{
		return m_translate.getValue() != castor::Point2f{ 0.0f, 0.0f }
			|| m_rotate.getValue() != castor::Angle::fromDegrees( 0.0f )
			|| ( m_scale.getValue() != castor::Point2f{ 0.0f, 0.0f }
				&& m_scale.getValue() != castor::Point2f{ 1.0f, 1.0f } );
	}
}
