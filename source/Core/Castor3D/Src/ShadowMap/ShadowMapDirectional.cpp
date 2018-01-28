#include "ShadowMapDirectional.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Miscellaneous/GaussianBlur.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/UniformBuffer.hpp"
#include "ShadowMap/ShadowMapPassDirectional.hpp"
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
		TextureUnit doInitialiseVariance( Engine & engine, Size const & p_size )
		{
			String const name = cuT( "ShadowMap_Directional_Variance" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToBorder );
				sampler->setWrapT( renderer::WrapMode::eClampToBorder );
				sampler->setWrapR( renderer::WrapMode::eClampToBorder );
				sampler->setBorderColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueWhite ) );
			}

			auto texture = engine.getRenderSystem()->createTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eAL32F, p_size );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		TextureUnit doInitialiseDepth( Engine & engine, Size const & p_size )
		{
			String const name = cuT( "ShadowMap_Directional_Depth" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			}

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

	ShadowMapDirectional::ShadowMapDirectional( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize } )
			, doInitialiseDepth( engine, Size{ ShadowMapPassDirectional::TextureSize, ShadowMapPassDirectional::TextureSize } )
			, std::make_shared< ShadowMapPassDirectional >( engine, scene, *this ) }
	{
	}

	ShadowMapDirectional::~ShadowMapDirectional()
	{
	}

	void ShadowMapDirectional::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_pass->update( camera, queues, light, index );
	}

	void ShadowMapDirectional::render()
	{
		m_pass->startTimer();
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
		m_pass->render( 0u );
		m_frameBuffer->unbind();

		m_blur->blur( m_shadowMap.getTexture() );
		m_pass->stopTimer();
	}

	void ShadowMapDirectional::debugDisplay( castor::Size const & size, uint32_t index )
	{
		Size displaySize{ 256u, 256u };
		Position position{ int32_t( displaySize.getWidth() * index * 3 ), int32_t( displaySize.getHeight() * 3u ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderVariance( position
			, displaySize
			, *m_shadowMap.getTexture() );
		position = Position{ int32_t( displaySize.getWidth() * ( 2 + index * 3 ) ), int32_t( displaySize.getHeight() * 3u ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderDepth( position
			, displaySize
			, *m_linearMap.getTexture() );
	}

	void ShadowMapDirectional::doInitialise()
	{
		m_frameBuffer->setClearColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueBlack ) );
		m_varianceAttach = m_frameBuffer->createAttachment( m_shadowMap.getTexture() );
		m_linearAttach = m_frameBuffer->createAttachment( m_linearMap.getTexture() );

		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_linearAttach, m_linearMap.getTexture()->getType() );
		m_frameBuffer->attach( AttachmentPoint::eColour, 0u, m_varianceAttach, m_shadowMap.getTexture()->getType() );
		ENSURE( m_frameBuffer->isComplete() );
		m_frameBuffer->setDrawBuffers();
		m_frameBuffer->unbind();

		m_blur = std::make_unique< GaussianBlur >( *getEngine()
			, m_shadowMap.getTexture()->getDimensions()
			, m_shadowMap.getTexture()->getPixelFormat()
			, 5u );
	}

	void ShadowMapDirectional::doCleanup()
	{
		m_blur.reset();
		m_linearAttach.reset();
		m_varianceAttach.reset();
	}

	void ShadowMapDirectional::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapDirectional );
	}

	glsl::Shader ShadowMapDirectional::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassDirectional::ShadowMapUbo, ShadowMapPassDirectional::UboBindingPoint };
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassDirectional::FarPlane ) );
		shadowMap.end();

		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
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
			pxl_linear = vtx_viewPosition.z() / c3d_farPlane;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( depth ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( depth ) );
			pxl_depth.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		} );

		return writer.finalise();
	}
}
