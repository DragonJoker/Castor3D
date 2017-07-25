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

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	namespace
	{
		static String const ShadowMapUbo = cuT( "ShadowMap" );
		static String const WorldLightPosition = cuT( "c3d_v3WorldLightPosition" );
		static String const FarPlane = cuT( "c3d_fFarPlane" );

		TextureUnit DoInitialisePoint( Engine & p_engine, Size const & p_size )
		{
			auto sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Point" ) );
			sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
			TextureUnit unit{ p_engine };
			auto texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eCube,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eL32F,
				p_size );
			unit.SetTexture( texture );
			unit.SetSampler( sampler );

			for ( auto & image : *texture )
			{
				image->InitialiseSource();
			}

			return unit;
		}
	}

	DeferredShadowMapPoint::DeferredShadowMapPoint( Engine & p_engine )
		: Castor3D::ShadowMap{ p_engine }
		, m_shadowMap{ DoInitialisePoint( p_engine, Size{ 1024, 1024 } ) }
	{
	}

	DeferredShadowMapPoint::~DeferredShadowMapPoint()
	{
	}
	
	void DeferredShadowMapPoint::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		for ( auto & pass : m_passes )
		{
			pass.second->Update( p_queues, 0 );
		}
	}

	void DeferredShadowMapPoint::Render( PointLight const & p_light )
	{
		auto it = m_passes.find( &p_light.GetLight() );
		REQUIRE( it != m_passes.end() && "Light not found, call AddLight..." );
		uint32_t face = 0u;

		for ( auto & attach : m_colourAttach )
		{
			m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			attach->Attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->SetDrawBuffer( attach );
			m_frameBuffer->Clear( BufferComponent::eDepth | BufferComponent::eColour );
			it->second->Render( face++ );
			m_frameBuffer->Unbind();
		}
	}

	int32_t DeferredShadowMapPoint::DoGetMaxPasses()const
	{
		return 1;
	}

	Size DeferredShadowMapPoint::DoGetSize()const
	{
		return m_shadowMap.GetTexture()->GetDimensions();
	}

	void DeferredShadowMapPoint::DoInitialise()
	{
		constexpr float component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( component, component, component, component );
		auto texture = m_shadowMap.GetTexture();
		texture->Initialise();
		int i = 0;

		for ( auto & attach : m_colourAttach )
		{
			attach = m_frameBuffer->CreateAttachment( texture, CubeMapFace( i++ ) );
			attach->SetTarget( TextureType::eTwoDimensions );
		}

		m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_depthBuffer->Create();
		m_depthBuffer->Initialise( texture->GetDimensions() );

		m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->Unbind();
	}

	void DeferredShadowMapPoint::DoCleanup()
	{
		m_depthAttach.reset();

		for ( auto & attach : m_colourAttach )
		{
			attach.reset();
		}

		m_depthBuffer->Cleanup();
		m_depthBuffer->Destroy();
		m_depthBuffer.reset();

		m_shadowMap.Cleanup();
	}

	ShadowMapPassSPtr DeferredShadowMapPoint::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassPoint >( *GetEngine(), p_light, *this );
	}

	void DeferredShadowMapPoint::DoUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		AddFlag( programFlags, ProgramFlag::eShadowMapPoint );
	}

	GLSL::Shader DeferredShadowMapPoint::DoGetPixelShaderSource( TextureChannels const & textureFlags
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

		auto vtx_position = writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_texture = writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
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
