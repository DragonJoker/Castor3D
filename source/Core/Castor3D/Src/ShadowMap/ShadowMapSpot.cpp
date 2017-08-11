#include "ShadowMapSpot.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/ShaderProgram.hpp>
#include <ShadowMap/ShadowMapPassSpot.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseSpot( Engine & engine
			, Size const & size )
		{
			auto sampler = engine.getSamplerCache().add( cuT( "ShadowMap_Spot" ) );
			sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			sampler->setComparisonMode( ComparisonMode::eRefToTexture );
			sampler->setComparisonFunc( ComparisonFunc::eLEqual );

			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eD32F
				, size );
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

	ShadowMapSpot::ShadowMapSpot( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialiseSpot( engine, Size{ 1024, 1024 } )
			, std::make_shared< ShadowMapPassSpot >( engine, scene, *this ) }
	{
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
	}
	
	void ShadowMapSpot::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_pass->update( camera, queues, light, index );
	}

	void ShadowMapSpot::render()
	{
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eDepth );
		m_pass->render();
		m_frameBuffer->unbind();
	}

	void ShadowMapSpot::doInitialise()
	{
		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );
		auto texture = m_shadowMap.getTexture();
		m_depthAttach = m_frameBuffer->createAttachment( texture );
		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach, m_shadowMap.getTexture()->getType() );
		ENSURE( m_frameBuffer->isComplete() );
		m_frameBuffer->unbind();
	}

	void ShadowMapSpot::doCleanup()
	{
		m_depthAttach.reset();
	}

	void ShadowMapSpot::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapSpot );
	}

	glsl::Shader ShadowMapSpot::doGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
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
