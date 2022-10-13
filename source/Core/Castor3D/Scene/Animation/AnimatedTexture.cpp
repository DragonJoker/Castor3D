#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Scene/Animation/Texture/TextureAnimationInstance.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

namespace castor3d
{
	namespace anmtex
	{
		static castor::String writeMask( castor::String const & name
			, TextureFlagConfiguration const & config )
		{
			auto stream = castor::makeStringStream();
			stream << name
				<< cuT( "_" ) << config.flag
				<< cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << config.componentsMask;
			return stream.str();
		}

		static castor::String write( castor::String const & name, float v )
		{
			auto stream = castor::makeStringStream();
			stream << name << std::setprecision( 2 ) << std::fixed << v;
			return stream.str();
		}

		static castor::String write( castor::String const & name, uint32_t v )
		{
			auto stream = castor::makeStringStream();
			stream << name << v;
			return stream.str();
		}

		static castor::String getTexName( TextureSourceInfo const & sourceInfo
			, TextureConfiguration const & configuration )
		{
			castor::String result{ sourceInfo.relative() };
			result += writeMask( cuT( "_c0" ), configuration.components[0] );
			result += writeMask( cuT( "_c1" ), configuration.components[1] );
			result += writeMask( cuT( "_c2" ), configuration.components[2] );
			result += writeMask( cuT( "_c3" ), configuration.components[3] );
			result += write( cuT( "_nf" ), configuration.normalFactor );
			result += write( cuT( "_ng" ), configuration.normalGMultiplier );
			result += write( cuT( "_hf" ), configuration.heightFactor );
			result += write( cuT( "_yv" ), configuration.needsYInversion );
			return result;
		}
	}

	AnimatedTexture::AnimatedTexture( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
		: AnimatedObject{ AnimationType::eTexture
			, pass.getOwner()->getName()
				+ cuT( "_" ) + castor::string::toString( pass.getId() )
				+ cuT( "_" ) + anmtex::getTexName( sourceInfo, config ) }
		, m_pass{ pass }
	{
	}

	void AnimatedTexture::update( castor::Milliseconds const & elpased )
	{
		if ( m_playingAnimation )
		{
			m_playingAnimation->update( elpased );
		}
	}

	void AnimatedTexture::fillBuffer( TextureAnimationData * buffer )const
	{
		if ( m_playingAnimation )
		{
			auto & transform = m_playingAnimation->getTransform();
			auto & data = *buffer;
			data.translate = transform.translate;
			data.rotate = { transform.rotate.cos(), transform.rotate.sin(), 0.0f, 0.0f };
			data.scale = transform.scale;

			if ( !m_playingAnimation->getTextureAnimation().isEmpty() )
			{
				auto tile = m_playingAnimation->getTile();
				data.tileSet = { tile->x
					, tile->y
					, m_texture->getConfiguration().tileSet->z
					, m_texture->getConfiguration().tileSet->w };
			}
			else
			{
				data.tileSet = { 0.0f
					, 0.0f
					, 1.0f
					, 1.0f };
			}
		}
	}

	void AnimatedTexture::doAddAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			if ( m_texture
				&& m_texture->hasAnimation() )
			{
				auto & animation = m_texture->getAnimation();
				auto instance = std::make_unique< TextureAnimationInstance >( *this, animation );
				m_animations.emplace( name, std::move( instance ) );
				m_pass.getOwner()->getEngine()->getMaterialCache().registerTexture( *this );
				startAnimation( animation.getName() );
			}
		}
	}

	void AnimatedTexture::doStartAnimation( AnimationInstance & animation )
	{
		CU_Require( m_playingAnimation == nullptr );
		m_playingAnimation = &static_cast< TextureAnimationInstance & >( animation );
	}

	void AnimatedTexture::doStopAnimation( AnimationInstance & animation )
	{
		CU_Require( m_playingAnimation == &animation );
		m_playingAnimation = nullptr;
	}

	void AnimatedTexture::doClearAnimations()
	{
		m_playingAnimation = nullptr;
	}
}
