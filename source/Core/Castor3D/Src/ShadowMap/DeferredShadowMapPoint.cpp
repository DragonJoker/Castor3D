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
			auto l_sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Point" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );
			TextureUnit l_unit{ p_engine };
			auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eCube,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eL32F,
				p_size );
			l_unit.SetTexture( l_texture );
			l_unit.SetSampler( l_sampler );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			return l_unit;
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
		for ( auto & l_pass : m_passes )
		{
			l_pass.second->Update( p_queues, 0 );
		}
	}

	void DeferredShadowMapPoint::Render( PointLight const & p_light )
	{
		auto l_it = m_passes.find( &p_light.GetLight() );
		REQUIRE( l_it != m_passes.end() && "Light not found, call AddLight..." );
		uint32_t l_face = 0u;

		for ( auto & l_attach : m_colourAttach )
		{
			m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			l_attach->Attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->SetDrawBuffer( l_attach );
			m_frameBuffer->Clear( BufferComponent::eDepth | BufferComponent::eColour );
			l_it->second->Render( l_face++ );
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
		constexpr float l_component = std::numeric_limits< float >::max();
		m_frameBuffer->SetClearColour( l_component, l_component, l_component, l_component );
		auto l_texture = m_shadowMap.GetTexture();
		l_texture->Initialise();
		int i = 0;

		for ( auto & l_attach : m_colourAttach )
		{
			l_attach = m_frameBuffer->CreateAttachment( l_texture, CubeMapFace( i++ ) );
			l_attach->SetTarget( TextureType::eTwoDimensions );
		}

		m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_depthBuffer->Create();
		m_depthBuffer->Initialise( l_texture->GetDimensions() );

		m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
		m_frameBuffer->Unbind();
	}

	void DeferredShadowMapPoint::DoCleanup()
	{
		m_depthAttach.reset();

		for ( auto & l_attach : m_colourAttach )
		{
			l_attach.reset();
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

	void DeferredShadowMapPoint::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapPoint );
	}

	GLSL::Shader DeferredShadowMapPoint::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags
		, ComparisonFunc p_alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		Ubo l_shadowMap{ l_writer, ShadowMapUbo };
		auto c3d_v3WordLightPosition( l_shadowMap.DeclMember< Vec3 >( WorldLightPosition ) );
		auto c3d_fFarPlane( l_shadowMap.DeclMember< Float >( FarPlane ) );
		l_shadowMap.End();

		auto vtx_position = l_writer.DeclInput< Vec3 >( cuT( "vtx_position" ) );
		auto vtx_texture = l_writer.DeclInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( l_writer.DeclUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::eOpacity ) ) );

		// Fragment Outputs
		auto pxl_fFragColor = l_writer.DeclFragData< Float >( cuT( "pxl_fFragColor" ), 0u );

		auto l_main = [&]()
		{
			if ( CheckFlag( p_textureFlags, TextureChannel::eOpacity ) )
			{
				auto l_alpha = l_writer.DeclLocale( cuT( "l_alpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

				IF( l_writer, l_alpha < 0.2_f )
				{
					l_writer.Discard();
				}
				FI;
			}

			auto l_distance = l_writer.DeclLocale( cuT( "l_distance" ), length( vtx_position - c3d_v3WordLightPosition ) );
			pxl_fFragColor = l_distance / c3d_fFarPlane;
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}
}
