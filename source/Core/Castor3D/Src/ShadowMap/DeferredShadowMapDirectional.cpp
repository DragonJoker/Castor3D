#include "DeferredShadowMapDirectional.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Miscellaneous/GaussianBlur.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBuffer.hpp>
#include <ShadowMap/ShadowMapPassDirectional.hpp>
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
		TextureUnit doInitialiseDirectional( Engine & engine, Size const & p_size )
		{
			auto sampler = engine.getSamplerCache().add( cuT( "ShadowMap_Directional" ) );
			sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			sampler->setComparisonMode( ComparisonMode::eRefToTexture );
			sampler->setComparisonFunc( ComparisonFunc::eLEqual );

			auto texture = engine.getRenderSystem()->createTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F, p_size );
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

	DeferredShadowMapDirectional::DeferredShadowMapDirectional( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMap{ doInitialiseDirectional( engine, Size{ 4096, 4096 } ) }
	{
	}

	DeferredShadowMapDirectional::~DeferredShadowMapDirectional()
	{
	}

	void DeferredShadowMapDirectional::update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			for ( auto & it : m_passes )
			{
				it.second->update( p_queues, 0u );
			}
		}
	}

	void DeferredShadowMapDirectional::render( DirectionalLight const & p_light )
	{
		auto it = m_passes.find( &p_light.getLight() );
		REQUIRE( it != m_passes.end() && "Light not found, call addLight..." );
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eDepth );
		it->second->render();
		m_frameBuffer->unbind();

		m_blur->blur( m_shadowMap.getTexture() );
	}

	int32_t DeferredShadowMapDirectional::doGetMaxPasses()const
	{
		return 1;
	}

	Size DeferredShadowMapDirectional::doGetSize()const
	{
		return m_shadowMap.getTexture()->getDimensions();
	}

	void DeferredShadowMapDirectional::doInitialise()
	{
		m_shadowMap.initialise();
		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );

		auto texture = m_shadowMap.getTexture();
		m_depthAttach = m_frameBuffer->createAttachment( texture );
		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach, m_shadowMap.getTexture()->getType() );
		ENSURE( m_frameBuffer->isComplete() );
		m_frameBuffer->unbind();

		m_blur = std::make_shared< GaussianBlur >( *getEngine()
			, m_shadowMap.getTexture()->getDimensions()
			, m_shadowMap.getTexture()->getPixelFormat()
			, 5u );
	}

	void DeferredShadowMapDirectional::doCleanup()
	{
		m_blur.reset();
		m_depthAttach.reset();
		m_shadowMap.cleanup();
	}

	ShadowMapPassSPtr DeferredShadowMapDirectional::doCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassDirectional >( *getEngine(), p_light, *this );
	}

	void DeferredShadowMapDirectional::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapDirectional );
	}

	GLSL::Shader DeferredShadowMapDirectional::doGetPixelShaderSource( TextureChannels const & textureFlags
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
