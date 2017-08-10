#include "DeferredShadowMapPoint.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>

#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/BillboardList.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <ShadowMap/ShadowMapPassPoint.hpp>
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
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );

		TextureUnit doInitialisePoint( Engine & engine, Size const & p_size )
		{
			auto sampler = engine.getSamplerCache().add( cuT( "ShadowMap_Point" ) );
			sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			sampler->setComparisonMode( ComparisonMode::eRefToTexture );
			sampler->setComparisonFunc( ComparisonFunc::eLEqual );
			TextureUnit unit{ engine };
			auto texture = engine.getRenderSystem()->createTexture(
				TextureType::eCube,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eL32F,
				p_size );
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}
	}

	DeferredShadowMapPoint::DeferredShadowMapPoint( Engine & engine )
		: castor3d::ShadowMap{ engine }
		, m_shadowMap{ doInitialisePoint( engine, Size{ 1024, 1024 } ) }
	{
	}

	DeferredShadowMapPoint::~DeferredShadowMapPoint()
	{
	}
	
	void DeferredShadowMapPoint::update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		for ( auto & pass : m_passes )
		{
			pass.second->update( p_queues, 0 );
		}
	}

	void DeferredShadowMapPoint::render( PointLight const & p_light )
	{
		auto it = m_passes.find( &p_light.getLight() );
		REQUIRE( it != m_passes.end() && "Light not found, call addLight..." );
		uint32_t face = 0u;

		for ( auto & attach : m_colourAttach )
		{
			m_frameBuffer->bind( FrameBufferTarget::eDraw );
			attach->attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->setDrawBuffer( attach );
			m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
			it->second->render( face++ );
			m_frameBuffer->unbind();
		}
	}

	int32_t DeferredShadowMapPoint::doGetMaxPasses()const
	{
		return 1;
	}

	Size DeferredShadowMapPoint::doGetSize()const
	{
		return m_shadowMap.getTexture()->getDimensions();
	}

	void DeferredShadowMapPoint::doInitialise()
	{
		constexpr float PixelComponents = std::numeric_limits< float >::max();
		m_frameBuffer->setClearColour( PixelComponents, PixelComponents, PixelComponents, PixelComponents );
		auto texture = m_shadowMap.getTexture();
		texture->initialise();
		int i = 0;

		for ( auto & attach : m_colourAttach )
		{
			attach = m_frameBuffer->createAttachment( texture, CubeMapFace( i++ ) );
			attach->setTarget( TextureType::eTwoDimensions );
		}

		m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_depthBuffer->create();
		m_depthBuffer->initialise( texture->getDimensions() );

		m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->unbind();
	}

	void DeferredShadowMapPoint::doCleanup()
	{
		m_depthAttach.reset();

		for ( auto & attach : m_colourAttach )
		{
			attach.reset();
		}

		m_depthBuffer->cleanup();
		m_depthBuffer->destroy();
		m_depthBuffer.reset();

		m_shadowMap.cleanup();
	}

	ShadowMapPassSPtr DeferredShadowMapPoint::doCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassPoint >( *getEngine(), p_light, *this );
	}

	void DeferredShadowMapPoint::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapPoint );
	}

	GLSL::Shader DeferredShadowMapPoint::doGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapUbo, 8u };
		auto c3d_v3WordLightPosition( shadowMap.declMember< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( shadowMap.declMember< Float >( FarPlane ) );
		shadowMap.end();

		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );

		// Fragment Outputs
		auto pxl_fFragColor = writer.declFragData< Float >( cuT( "pxl_fFragColor" ), 0u );

		auto main = [&]()
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

			auto distance = writer.declLocale( cuT( "distance" ), length( vtx_position - c3d_v3WordLightPosition ) );
			pxl_fFragColor = distance / c3d_fFarPlane;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
