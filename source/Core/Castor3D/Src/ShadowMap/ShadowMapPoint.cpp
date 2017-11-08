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
		TextureUnit doInitialisePointShadow( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Point_Shadow" );
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
				sampler->setBorderColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueWhite ) );
			}

			TextureUnit unit{ engine };
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eAL32F
				, size );
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		TextureUnit doInitialisePointDepth( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Point_Depth" );
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
				sampler->setBorderColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueWhite ) );
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
			, doInitialisePointShadow( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
			, doInitialisePointDepth( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
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
		m_pass->startTimer();

		for ( uint32_t face = 0u; face < 6u; ++face )
		{
			m_frameBuffer->bind( FrameBufferTarget::eDraw );
			m_colourAttach[face]->attach( AttachmentPoint::eColour, 0u );
			m_linearAttach[face]->attach( AttachmentPoint::eColour, 1u );
			REQUIRE( m_frameBuffer->isComplete() );
			m_frameBuffer->setDrawBuffers( { m_colourAttach[face], m_linearAttach[face] } );
			m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
			m_pass->render( face );
			m_frameBuffer->unbind();
		}

		m_pass->stopTimer();
	}

	void ShadowMapPoint::debugDisplay( castor::Size const & size, uint32_t index )
	{
		Size displaySize{ 128u, 128u };
		Position position{ int32_t( displaySize.getWidth() * 4 * index), int32_t( displaySize.getHeight() * 4 ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderVarianceCube( position
			, displaySize
			, *m_shadowMap.getTexture() );
		position = Position{ int32_t( displaySize.getWidth() * 4 * ( index + 2 ) ), int32_t( displaySize.getHeight() * 4 ) };
		getEngine()->getRenderSystem()->getCurrentContext()->renderDepthCube( position
			, displaySize
			, *m_linearMap.getTexture() );
	}

	void ShadowMapPoint::doInitialise()
	{
		constexpr float component = std::numeric_limits< float >::max();
		m_frameBuffer->setClearColour( component, component, component, component );
		auto texture = m_shadowMap.getTexture();
		int i = 0;

		for ( auto & attach : m_colourAttach )
		{
			attach = m_frameBuffer->createAttachment( texture, CubeMapFace( i++ ) );
			attach->setTarget( TextureType::eTwoDimensions );
		}

		i = 0;
		texture = m_linearMap.getTexture();

		for ( auto & attach : m_linearAttach )
		{
			attach = m_frameBuffer->createAttachment( texture, CubeMapFace( i++ ) );
			attach->setTarget( TextureType::eTwoDimensions );
		}
	}

	void ShadowMapPoint::doCleanup()
	{
		for ( auto & attach : m_colourAttach )
		{
			attach.reset();
		}

		for ( auto & attach : m_linearAttach )
		{
			attach.reset();
		}
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
		Ubo shadowMap{ writer, ShadowMapPassPoint::ShadowMapUbo, ShadowMapPassPoint::UboBindingPoint };
		auto c3d_wordLightPosition( shadowMap.declMember< Vec3 >( ShadowMapPassPoint::WorldLightPosition ) );
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassPoint::FarPlane ) );
		shadowMap.end();

		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" ) );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" ) );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( ShaderProgram::MapOpacity
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		// Fragment Outputs
		auto pxl_depth = writer.declFragData< Vec2 >( cuT( "pxl_depth" ), 0u );
		auto pxl_linear = writer.declFragData< Float >( cuT( "pxl_linear" ), 1u );

		auto main = [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, length( vtx_worldPosition - c3d_wordLightPosition ) );
			pxl_linear = depth / c3d_farPlane;
			pxl_depth.x() = pxl_linear;
			pxl_depth.y() = pxl_linear * pxl_linear;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( pxl_linear ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( pxl_linear ) );
			pxl_depth.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
