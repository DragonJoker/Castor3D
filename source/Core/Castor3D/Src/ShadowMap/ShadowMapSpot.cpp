#include "ShadowMapSpot.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseVariance( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Spot_Variance" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( InterpolationMode::eLinear );
				sampler->setMagFilter( InterpolationMode::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToBorder );
				sampler->setWrapT( renderer::WrapMode::eClampToBorder );
				sampler->setWrapR( renderer::WrapMode::eClampToBorder );
				sampler->setBorderColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueWhite ) );
			}

			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eAL32F
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

		TextureUnit doInitialiseLinearDepth( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Spot_Depth" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( InterpolationMode::eLinear );
				sampler->setMagFilter( InterpolationMode::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			}

			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eL32F
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
			, doInitialiseVariance( engine, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } )
			, doInitialiseLinearDepth( engine, Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } )
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
		m_pass->startTimer();
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
		m_pass->render();
		m_frameBuffer->unbind();

		m_blur->blur( m_shadowMap.getTexture() );
		m_pass->stopTimer();
	}

	void ShadowMapSpot::debugDisplay( castor::Size const & size, uint32_t index )
	{
		Size displaySize{ 256u, 256u };
		Position position{ int32_t( ( displaySize.getWidth() + 1 ) * index * 3u ), int32_t( displaySize.getHeight() * 2u ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderVariance( position
			, displaySize
			, *m_shadowMap.getTexture() );
		position.offset( 2 * ( displaySize.getWidth() + 1 ), 0 );
		getEngine()->getRenderSystem()->getCurrentContext()->renderTexture( position
			, displaySize
			, *m_linearMap.getTexture() );
	}

	void ShadowMapSpot::doInitialise()
	{
		m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD24 );
		m_depthBuffer->create();
		m_depthBuffer->initialise( Size{ ShadowMapPassSpot::TextureSize, ShadowMapPassSpot::TextureSize } );

		m_frameBuffer->setClearColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueBlack ) );
		m_varianceAttach = m_frameBuffer->createAttachment( m_shadowMap.getTexture() );
		m_linearAttach = m_frameBuffer->createAttachment( m_linearMap.getTexture() );
		m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );

		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->attach( AttachmentPoint::eColour, 0u, m_varianceAttach, m_shadowMap.getTexture()->getType() );
		m_frameBuffer->attach( AttachmentPoint::eColour, 1u, m_linearAttach, m_linearMap.getTexture()->getType() );
		ENSURE( m_frameBuffer->isComplete() );
		m_frameBuffer->setDrawBuffers();
		m_frameBuffer->unbind();

		m_blur = std::make_unique< GaussianBlur >( *getEngine()
			, m_shadowMap.getTexture()->getDimensions()
			, m_shadowMap.getTexture()->getPixelFormat()
			, 5u );
	}

	void ShadowMapSpot::doCleanup()
	{
		m_blur.reset();
		m_depthAttach.reset();
		m_linearAttach.reset();
		m_varianceAttach.reset();
		m_depthBuffer->cleanup();
		m_depthBuffer->destroy();
		m_depthBuffer.reset();
	}

	void ShadowMapSpot::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapSpot );
	}

	glsl::Shader ShadowMapSpot::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassSpot::ShadowMapUbo, ShadowMapPassSpot::UboBindingPoint };
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassSpot::FarPlane ) );
		shadowMap.end();

		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ) );
		auto vtx_viewPosition = writer.declInput< Vec3 >( cuT( "vtx_viewPosition" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, MinTextureIndex
			, 0u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		// Fragment Outputs
		auto pxl_depth( writer.declFragData< Vec2 >( cuT( "pxl_depth" ), 0 ) );
		auto pxl_linear( writer.declFragData< Float >( cuT( "pxl_linear" ), 1 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, gl_FragCoord.z() );
			pxl_depth.x() = depth;
			pxl_depth.y() = pxl_depth.x() * pxl_depth.x();
			pxl_linear = length( vtx_viewPosition ) / c3d_farPlane;
			//pxl_linear = vtx_viewPosition.z()/* / c3d_farPlane*/;
			pxl_linear = abs( vtx_viewPosition.z() ) / c3d_farPlane;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( depth ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( depth ) );
			pxl_depth.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		} );

		return writer.finalise();
	}
}
