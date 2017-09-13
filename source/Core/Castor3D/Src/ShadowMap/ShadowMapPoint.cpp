#include "ShadowMapPoint.hpp"

#include "Engine.hpp"
#include "Cache/SamplerCache.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "ShadowMap/ShadowMapPassPoint.hpp"
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
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_worldLightPosition" );
		static String const FarPlane = cuT( "c3d_farPlane" );

		TextureUnit doInitialisePoint( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Point" );
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
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				sampler->setBorderColour( Colour::fromPredefined( PredefinedColour::eOpaqueWhite ) );
				//sampler->setComparisonMode( ComparisonMode::eRefToTexture );
				//sampler->setComparisonFunc( ComparisonFunc::eLEqual );
			}

			TextureUnit unit{ engine };
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eL32F
				, size );
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}
	}

	ShadowMapPoint::ShadowMapPoint( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialisePoint( engine, Size{ 1024, 1024 } )
			, std::make_shared< ShadowMapPassPoint >( engine, scene, *this ) }
	{
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}

	void ShadowMapPoint::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_pass->update( camera, queues, light, index );
	}

	void ShadowMapPoint::render()
	{
		uint32_t face = 0u;

		for ( auto & attach : m_colourAttach )
		{
			m_frameBuffer->bind( FrameBufferTarget::eDraw );
			attach->attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->setDrawBuffer( attach );
			m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
			m_pass->render( face++ );
			m_frameBuffer->unbind();
		}
	}

	void ShadowMapPoint::debugDisplay( castor::Size const & size, uint32_t index )
	{
		Size displaySize{ 128u, 128u };
		Position position{ int32_t( displaySize.getWidth() * 4 * index), int32_t( displaySize.getHeight() * 4 ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderDepthCube( position
			, displaySize
			, *m_shadowMap.getTexture() );
	}

	void ShadowMapPoint::doInitialise()
	{
		constexpr float component = std::numeric_limits< float >::max();
		m_frameBuffer->setClearColour( component, component, component, component );
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

	void ShadowMapPoint::doCleanup()
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

	void ShadowMapPoint::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapPoint );
	}

	glsl::Shader ShadowMapPoint::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapUbo, 8u };
		auto c3d_wordLightPosition( shadowMap.declMember< Vec3 >( WorldLightPosition ) );
		auto c3d_farPlane( shadowMap.declMember< Float >( FarPlane ) );
		shadowMap.end();

		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );

		auto materials = doCreateMaterials( writer, passFlags );
		materials->declare();

		// Fragment Outputs
		auto pxl_depth = writer.declFragData< Float >( cuT( "pxl_depth" ), 0u );

		auto main = [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, length( vtx_position - c3d_wordLightPosition ) );
			pxl_depth = depth / c3d_farPlane;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
