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
#include "Texture/TextureImage.hpp"
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
		TextureUnit doInitialiseSpot( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Spot" );
			SamplerSPtr sampler;

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
				sampler->setBorderColour( Colour::fromPredefined( PredefinedColour::eOpaqueWhite ) );
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
		m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
		m_pass->render();
		m_frameBuffer->unbind();

		m_blur->blur( m_shadowMap.getTexture() );
	}

	void ShadowMapSpot::debugDisplay( castor::Size const & size, uint32_t index )
	{
		Size displaySize{ 256u, 256u };
		Position position{ int32_t( displaySize.getWidth() * index ), int32_t( displaySize.getHeight() * 2u ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderVariance( position
			, displaySize
			, *m_shadowMap.getTexture() );
	}

	void ShadowMapSpot::doInitialise()
	{
		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );
		auto texture = m_shadowMap.getTexture();
		m_varianceAttach = m_frameBuffer->createAttachment( texture );

		m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_depthBuffer->create();
		m_depthBuffer->initialise( texture->getDimensions() );
		m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );

		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->attach( AttachmentPoint::eColour, m_varianceAttach, m_shadowMap.getTexture()->getType() );
		ENSURE( m_frameBuffer->isComplete() );
		m_frameBuffer->setDrawBuffers();
		m_frameBuffer->unbind();
	}

	void ShadowMapSpot::doCleanup()
	{
		m_depthAttach.reset();
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
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		// Fragment Outputs
		auto pxl_depth( writer.declFragData< Vec2 >( cuT( "pxl_depth" ), 0 ) );

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
			pxl_depth.y() = depth * depth;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( depth ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( depth ) );
			pxl_depth.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		} );

		return writer.finalise();
	}
}
