#include "ShadowMapSpot.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseSpot( Engine & engine, Size const & p_size )
		{
			SamplerSPtr sampler;
			String const name = cuT( "ShadowMap_Spot" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
				sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
				sampler->setComparisonMode( ComparisonMode::eRefToTexture );
				sampler->setComparisonFunc( ComparisonFunc::eLEqual );
			}

			auto texture = engine.getRenderSystem()->createTexture(
				TextureType::eTwoDimensionsArray,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F,
				Point3ui{ p_size.getWidth(), p_size.getHeight(), GLSL::SpotShadowMapCount } );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}
	}

	ShadowMapSpot::ShadowMapSpot( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMap{ doInitialiseSpot( engine, Size{ 1024, 1024 } ) }
	{
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
	}
	
	void ShadowMapSpot::update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			const int32_t max = doGetMaxPasses();
			m_sorted.clear();

			for ( auto & it : m_passes )
			{
				m_sorted.emplace( point::distanceSquared( p_camera.getParent()->getDerivedPosition()
					, it.first->getParent()->getDerivedPosition() )
					, it.second );
			}

			auto it = m_sorted.begin();

			for ( int32_t i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				it->second->update( p_queues, i );
			}
		}
	}

	void ShadowMapSpot::render()
	{
		if ( !m_sorted.empty() )
		{
			m_frameBuffer->bind( FrameBufferTarget::eDraw );
			auto it = m_sorted.begin();
			const int32_t max = doGetMaxPasses();

			for ( int32_t i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				m_depthAttach[i]->attach( AttachmentPoint::eDepth );
				m_frameBuffer->clear( BufferComponent::eDepth );
				it->second->render();
				m_depthAttach[i]->detach();
			}

			m_frameBuffer->unbind();
		}
	}

	int32_t ShadowMapSpot::doGetMaxPasses()const
	{
		return int32_t( m_shadowMap.getTexture()->getLayersCount() );
	}

	Size ShadowMapSpot::doGetSize()const
	{
		return m_shadowMap.getTexture()->getDimensions();
	}

	void ShadowMapSpot::doInitialise()
	{
		m_shadowMap.initialise();
		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );

		auto texture = m_shadowMap.getTexture();
		m_depthAttach.resize( doGetMaxPasses() );
		int i = 0;

		for ( auto & attach : m_depthAttach )
		{
			attach = m_frameBuffer->createAttachment( texture );
			attach->setTarget( texture->getType() );
			attach->setLayer( i++ );
		}
	}

	void ShadowMapSpot::doCleanup()
	{
		for ( auto & attach : m_depthAttach )
		{
			attach.reset();
		}

		m_shadowMap.cleanup();
	}

	ShadowMapPassSPtr ShadowMapSpot::doCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassSpot >( *getEngine(), p_light, *this );
	}

	void ShadowMapSpot::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapSpot );
	}

	GLSL::Shader ShadowMapSpot::doGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( writer.declFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.declLocale( cuT( "alpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

				IF( writer, alpha < 0.2_f )
				{
					writer.discard();
				}
				FI;
			}

			pxl_fFragDepth = gl_FragCoord.z();
		} );

		return writer.finalise();
	}
}
