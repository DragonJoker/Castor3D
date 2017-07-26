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

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );
		static String const ShadowMatrix = cuT( "c3d_mtxShadowMatrix" );

		std::vector< TextureUnit > DoInitialisePoint( Engine & engine, Size const & p_size )
		{
			std::vector< TextureUnit > result;
			String const name = cuT( "ShadowMap_Point_" );

			for ( auto i = 0u; i < GLSL::PointShadowMapCount; ++i )
			{
				SamplerSPtr sampler;

				if ( engine.GetSamplerCache().Has( name + string::to_string( i ) ) )
				{
					sampler = engine.GetSamplerCache().Find( name + string::to_string( i ) );
				}
				else
				{
					sampler = engine.GetSamplerCache().Add( name + string::to_string( i ) );
					sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
					sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
					sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
					sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
					sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
					sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
					sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
				}

				auto texture = engine.GetRenderSystem()->CreateTexture(
					TextureType::eCube,
					AccessType::eNone,
					AccessType::eRead | AccessType::eWrite,
					PixelFormat::eL32F,
					p_size );
				result.emplace_back( engine );
				TextureUnit & unit = result.back();
				unit.SetTexture( texture );
				unit.SetSampler( sampler );

				for ( auto & image : *texture )
				{
					image->InitialiseSource();
				}
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMaps{ DoInitialisePoint( engine, Size{ 1024, 1024 } ) }
	{
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}
	
	void ShadowMapPoint::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			const int32_t max = DoGetMaxPasses();
			m_sorted.clear();

			for ( auto & it : m_passes )
			{
				m_sorted.emplace( point::distance_squared( p_camera.GetParent()->GetDerivedPosition()
						, it.first->GetParent()->GetDerivedPosition() )
					, it.second );
			}

			auto it = m_sorted.begin();

			for ( auto i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				it->second->Update( p_queues, i );
			}
		}
	}

	void ShadowMapPoint::Render()
	{
		if ( !m_sorted.empty() )
		{
			auto it = m_sorted.begin();
			const int32_t max = DoGetMaxPasses();

			for ( int32_t i = 0; i < max && it != m_sorted.end(); ++i, ++it )
			{
				uint32_t face = 0;

				for ( auto & attach : m_colourAttachs[i] )
				{
					m_frameBuffer->Bind( FrameBufferTarget::eDraw );
					attach->Attach( AttachmentPoint::eColour, 0u );
					m_frameBuffer->SetDrawBuffer( attach );
					m_frameBuffer->Clear( BufferComponent::eDepth | BufferComponent::eColour );
					it->second->Render( face++ );
					m_frameBuffer->Unbind();
				}
			}
		}
	}

	int32_t ShadowMapPoint::DoGetMaxPasses()const
	{
		return int32_t( m_shadowMaps.size() );
	}

	Size ShadowMapPoint::DoGetSize()const
	{
		return m_shadowMaps[0].GetTexture()->GetDimensions();
	}

	void ShadowMapPoint::DoInitialise()
	{
		constexpr float component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( component, component, component, component );
		m_colourAttachs.resize( DoGetMaxPasses() );
		auto it = m_colourAttachs.begin();

		for ( auto & map : m_shadowMaps )
		{
			auto texture = map.GetTexture();
			texture->Initialise();
			uint32_t i = 0;

			for ( auto & attach : *it )
			{
				attach = m_frameBuffer->CreateAttachment( texture, CubeMapFace( i++ ) );
				attach->SetTarget( TextureType::eTwoDimensions );
			}

			++it;
		}

		m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_depthBuffer->Create();
		m_depthBuffer->Initialise( m_shadowMaps[0].GetTexture()->GetDimensions() );

		m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->Unbind();
	}

	void ShadowMapPoint::DoCleanup()
	{
		m_depthAttach.reset();

		for ( auto & attach : m_colourAttachs )
		{
			for ( auto & face : attach )
			{
				face.reset();
			}
		}

		m_depthBuffer->Cleanup();
		m_depthBuffer->Destroy();
		m_depthBuffer.reset();

		for ( auto & map : m_shadowMaps )
		{
			map.Cleanup();
		}
	}

	ShadowMapPassSPtr ShadowMapPoint::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassPoint >( *GetEngine(), p_light, *this );
	}

	void ShadowMapPoint::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		AddFlag( programFlags, ProgramFlag::eShadowMapPoint );
	}

	GLSL::Shader ShadowMapPoint::DoGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapUbo, 8u };
		auto c3d_v3WordLightPosition( shadowMap.DeclMember< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( shadowMap.DeclMember< Float >( FarPlane ) );
		shadowMap.End();

		auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_position = writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto c3d_mapOpacity( writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( textureFlags, TextureChannel::eOpacity ) ) );

		// Fragment Outputs
		auto pxl_fFragColor = writer.DeclFragData< Float >( cuT( "pxl_fFragColor" ), 0u );

		auto main = [&]()
		{
			if ( CheckFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.DeclLocale( cuT( "alpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

				IF( writer, alpha < 0.2_f )
				{
					writer.Discard();
				}
				FI;
			}

			auto distance = writer.DeclLocale( cuT( "distance" ), length( vtx_position - c3d_v3WordLightPosition ) );
			pxl_fFragColor = distance / c3d_fFarPlane;
		};

		writer.ImplementFunction< void >( cuT( "main" ), main );
		return writer.Finalise();
	}
}
