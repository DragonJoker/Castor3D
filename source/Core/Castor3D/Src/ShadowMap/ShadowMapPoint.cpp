#include "ShadowMapPoint.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPassPoint.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );
		static String const ShadowMatrix = cuT( "c3d_mtxShadowMatrix" );

		std::vector< TextureUnit > doInitialisePoint( Engine & engine, Size const & p_size )
		{
			std::vector< TextureUnit > result;
			String const name = cuT( "ShadowMap_Point_" );

			for ( auto i = 0u; i < GLSL::PointShadowMapCount; ++i )
			{
				SamplerSPtr sampler;

				if ( engine.getSamplerCache().has( name + string::toString( i ) ) )
				{
					sampler = engine.getSamplerCache().find( name + string::toString( i ) );
				}
				else
				{
					sampler = engine.getSamplerCache().add( name + string::toString( i ) );
					sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
					sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
					sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
					sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
					sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
					sampler->setComparisonMode( ComparisonMode::eRefToTexture );
					sampler->setComparisonFunc( ComparisonFunc::eLEqual );
				}

				auto texture = engine.getRenderSystem()->createTexture(
					TextureType::eCube,
					AccessType::eNone,
					AccessType::eRead | AccessType::eWrite,
					PixelFormat::eL32F,
					p_size );
				result.emplace_back( engine );
				TextureUnit & unit = result.back();
				unit.setTexture( texture );
				unit.setSampler( sampler );

				for ( auto & image : *texture )
				{
					image->initialiseSource();
				}
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMaps{ doInitialisePoint( engine, Size{ 1024, 1024 } ) }
	{
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}
	
	void ShadowMapPoint::update( Camera const & p_camera
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

			for ( auto i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				it->second->update( p_queues, i );
			}
		}
	}

	void ShadowMapPoint::render()
	{
		if ( !m_sorted.empty() )
		{
			auto it = m_sorted.begin();
			const int32_t max = doGetMaxPasses();

			for ( int32_t i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				uint32_t face = 0;

				for ( auto & attach : m_colourAttachs[i] )
				{
					m_frameBuffer->bind( FrameBufferTarget::eDraw );
					attach->attach( AttachmentPoint::eColour, 0u );
					m_frameBuffer->setDrawBuffer( attach );
					m_frameBuffer->clear( BufferComponent::eDepth | BufferComponent::eColour );
					it->second->render( face++ );
					m_frameBuffer->unbind();
				}
			}
		}
	}

	int32_t ShadowMapPoint::doGetMaxPasses()const
	{
		return int32_t( m_shadowMaps.size() );
	}

	Size ShadowMapPoint::doGetSize()const
	{
		return m_shadowMaps[0].getTexture()->getDimensions();
	}

	void ShadowMapPoint::doInitialise()
	{
		constexpr float PixelComponents = std::numeric_limits< float >::max();
		m_frameBuffer->setClearColour( PixelComponents, PixelComponents, PixelComponents, PixelComponents );
		m_colourAttachs.resize( doGetMaxPasses() );
		auto it = m_colourAttachs.begin();

		for ( auto & map : m_shadowMaps )
		{
			auto texture = map.getTexture();
			texture->initialise();
			uint32_t i = 0;

			for ( auto & attach : *it )
			{
				attach = m_frameBuffer->createAttachment( texture, CubeMapFace( i++ ) );
				attach->setTarget( TextureType::eTwoDimensions );
			}

			++it;
		}

		m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_depthBuffer->create();
		m_depthBuffer->initialise( m_shadowMaps[0].getTexture()->getDimensions() );

		m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->unbind();
	}

	void ShadowMapPoint::doCleanup()
	{
		m_depthAttach.reset();

		for ( auto & attach : m_colourAttachs )
		{
			for ( auto & face : attach )
			{
				face.reset();
			}
		}

		m_depthBuffer->cleanup();
		m_depthBuffer->destroy();
		m_depthBuffer.reset();

		for ( auto & map : m_shadowMaps )
		{
			map.cleanup();
		}
	}

	ShadowMapPassSPtr ShadowMapPoint::doCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassPoint >( *getEngine(), p_light, *this );
	}

	void ShadowMapPoint::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapPoint );
	}

	GLSL::Shader ShadowMapPoint::doGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapUbo, 8u };
		auto c3d_v3WorldLightPosition( shadowMap.declMember< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( shadowMap.declMember< Float >( FarPlane ) );
		shadowMap.end();

		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
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

			auto distance = writer.declLocale( cuT( "distance" ), length( vtx_position - c3d_v3WorldLightPosition ) );
			pxl_fFragColor = distance / c3d_fFarPlane;
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
