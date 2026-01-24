#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationKeyFrame.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

CU_ImplementSmartPtr( c3d, TextureAnimation )

namespace c3d
{
	TextureAnimation::TextureAnimation( Engine & engine
		, String const & name )
		: Animation{ engine
			, AnimationType::eTexture
			, name }
	{
		m_length = Milliseconds{ std::numeric_limits< int64_t >::max() };
	}

	void TextureAnimation::setAnimable( TextureUnit & unit )
	{
		for ( auto pending : m_pending )
		{
			static_cast< AnimatedTexture & >( *pending ).setTextureUnit( unit );
			pending->addAnimation( unit.getAnimation().getName() );
		}
	}

	void TextureAnimation::initialiseTiles( TextureUnit const & unit )
	{
		if ( m_tileAnim )
		{
			m_length = 0_ms;
			auto tiles = std::max( unit.getConfiguration().tiles
				, unit.getTextureImageTiles()->z );
			auto tileSet = unit.getConfiguration().tileSet;
			Milliseconds timeIndex{};
			Milliseconds timeStep{ 25_ms };
			uint32_t tile = 0u;

			for ( uint32_t y = 0u; y < tileSet->w; ++y )
			{
				for ( uint32_t x = 0u; x < tileSet->z; ++x )
				{
					if ( tile < tiles )
					{
						auto kf = makeUnique< TextureAnimationKeyFrame >( *this, timeIndex );
						kf->setTile( { x, y } );
						addKeyFrame( ptrRefCast< AnimationKeyFrame >( kf ) );
						timeIndex += timeStep;
					}

					++tile;
				}
			}
		}
	}

	Point3f TextureAnimation::getTranslate( Milliseconds const & time )const
	{
		return Point3f{ m_translate.getDistance( time ) };
	}

	Angle TextureAnimation::getRotate( Milliseconds const & time )const
	{
		return m_rotate.getDistance( time );
	}

	Point3f TextureAnimation::getScale( Milliseconds const & time )const
	{
		if ( m_scale.getValue() != Point2f{ 0, 0 } )
		{
			return Point3f{ m_scale.getDistance( time ) };
		}

		return Point3f{ 1.0f, 1.0f, 1.0f };
	}

	bool TextureAnimation::isTransformAnimated()const
	{
		return m_translate.getValue() != Point2f{ 0.0f, 0.0f }
			|| m_rotate.getValue() != Angle::fromDegrees( 0.0f )
			|| ( m_scale.getValue() != Point2f{ 0.0f, 0.0f }
				&& m_scale.getValue() != Point2f{ 1.0f, 1.0f } );
	}

	void TextureAnimation::doCloneInto( Animation & output )const
	{
		auto & texAnim = static_cast< TextureAnimation & >( output );
		texAnim.m_translate = m_translate;
		texAnim.m_rotate = m_rotate;
		texAnim.m_scale = m_scale;
		texAnim.m_tileAnim = m_tileAnim;
	}
}
