#include "DeferredMsaaRenderTechnique.hpp"

#include "DeferredMsaaRenderTechniqueOpaquePass.hpp"

#include <Engine.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>
#include <Cache/ShaderCache.hpp>
#include <Cache/TargetCache.hpp>

#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Technique/ForwardRenderTechniquePass.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Log/Logger.hpp>

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

#define DEBUG_BUFFERS 0

using namespace Castor;
using namespace Castor3D;

namespace deferred_msaa
{
	namespace
	{
		String GetTextureName( DsTexture p_texture )
		{
			static std::array< String, size_t( DsTexture::eCount ) > Values
			{
				{
					cuT( "c3d_mapPosition" ),
					cuT( "c3d_mapDiffuse" ),
					cuT( "c3d_mapNormals" ),
					cuT( "c3d_mapTangent" ),
					cuT( "c3d_mapSpecular" ),
					cuT( "c3d_mapEmissive" ),
					cuT( "c3d_mapInfos" ),
				}
			};

			return Values[size_t( p_texture )];
		}

		PixelFormat GetTextureFormat( DsTexture p_texture )
		{
			static std::array< PixelFormat, size_t( DsTexture::eCount ) > Values
			{
				{
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
				}
			};

			return Values[size_t( p_texture )];
		}

		AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture )
		{
			static std::array< AttachmentPoint, size_t( DsTexture::eCount ) > Values
			{
				{
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
				}
			};

			return Values[size_t( p_texture )];
		}

		uint32_t GetTextureAttachmentIndex( DsTexture p_texture )
		{
			static std::array< uint32_t, size_t( DsTexture::eCount ) > Values
			{
				{
					0,
					1,
					2,
					3,
					4,
					5,
					6,
				}
			};

			return Values[size_t( p_texture )];
		}

		int & GetSamplesCountParam( Parameters const & p_params, int & p_count )
		{
			String l_count;

			if ( p_params.Get( cuT( "samples_count" ), l_count ) )
			{
				p_count = string::to_int( l_count );
			}

			if ( p_count <= 1 )
			{
				p_count = 0;
			}

			return p_count;
		}

		static constexpr uint32_t VertexCount = 6u;
	}

	String const RenderTechnique::Type = cuT( "deferred_msaa" );
	String const RenderTechnique::Name = cuT( "Deferred Lighting MSAA Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "deferred_msaa" )
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< OpaquePass >( p_renderTarget, *this )
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "deferred_msaa_transparent" ), p_renderTarget, *this, false, GetSamplesCountParam( p_params, m_samplesCount ) > 1 )
			, p_params
			, GetSamplesCountParam( p_params, m_samplesCount ) > 1 )
		, m_viewport{ *p_renderSystem.GetEngine() }
		, m_matrixUbo{ ShaderProgram::BufferMatrix, p_renderSystem }
		, m_sceneUbo{ ShaderProgram::BufferScene, p_renderSystem }
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
		UniformBuffer::FillSceneBuffer( m_sceneUbo );

		m_projectionUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );

		m_sceneNode = std::make_unique< SceneRenderNode >( m_sceneUbo );
		Logger::LogInfo( StringStream() << cuT( "Using Deferred MSAA, " ) << m_samplesCount << cuT( " samples" ) );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		return DoCreateDeferred() && DoCreateMsaa();
	}

	void RenderTechnique::DoDestroy()
	{
		DoDestroyMsaa();
		DoDestroyDeferred();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		return DoInitialiseDeferred( p_index ) && DoInitialiseMsaa();
	}

	void RenderTechnique::DoCleanup()
	{
		DoCleanupMsaa();
		DoCleanupDeferred();
	}

	void RenderTechnique::DoRenderOpaque( uint32_t & p_visible )
	{
		m_opaquePass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		GetEngine()->SetPerObjectLighting( false );
		m_geometryPassFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_geometryPassFrameBuffer->Clear();
		m_opaquePass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_geometryPassFrameBuffer->Unbind();
		// Render the light pass.

#if DEBUG_BUFFERS

		int l_width = int( m_size.width() );
		int l_height = int( m_size.height() );
		int l_thirdWidth = int( l_width / 3.0f );
		int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
		int l_thirdHeight = int( l_height / 3.0f );
		int l_twothirdHeight = int( 2.0f * l_height / 3.0f );
		m_geometryPassTexAttachs[size_t( DsTexture::ePosition )]->Blit( m_msaaFrameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_thirdHeight, l_thirdWidth, l_twothirdHeight ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eDiffuse )]->Blit( m_msaaFrameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_twothirdHeight, l_thirdWidth, l_height ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eNormals )]->Blit( m_msaaFrameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_thirdHeight ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eTangent )]->Blit( m_msaaFrameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_thirdHeight, l_twoThirdWidth, l_twothirdHeight ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eSpecular )]->Blit( m_msaaFrameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_twothirdHeight, l_twoThirdWidth, l_height ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eEmissive )]->Blit( m_msaaFrameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, 0, l_width, l_thirdHeight ), InterpolationMode::eLinear );

#else

		m_msaaFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		auto & l_scene = *m_renderTarget.GetScene();
		auto & l_camera = *m_renderTarget.GetCamera();
		m_msaaFrameBuffer->SetClearColour( l_scene.GetBackgroundColour() );
		m_msaaFrameBuffer->Clear();

		auto & l_program = m_lightPassShaderPrograms[uint16_t( GetFogType( l_scene.GetFlags() ) )];

		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_projectionUniform->SetValue( m_viewport.GetProjection() );

		if ( l_program.m_camera )
		{
			auto & l_fog = l_scene.GetFog();
			m_sceneNode->m_fogType.SetValue( int( l_fog.GetType() ) );

			if ( l_fog.GetType() != GLSL::FogType::eDisabled )
			{
				m_sceneNode->m_fogDensity.SetValue( l_fog.GetDensity() );
			}

			auto & l_cache = l_scene.GetLightCache();
			m_sceneNode->m_ambientLight.SetValue( rgba_float( l_scene.GetAmbientLight() ) );

			{
				auto l_lock = make_unique_lock( l_cache );
				m_sceneNode->m_lightsCount.GetValue( 0 )[size_t( LightType::eDirectional )] = l_cache.GetLightsCount( LightType::eDirectional );
				m_sceneNode->m_lightsCount.GetValue( 0 )[size_t( LightType::ePoint )] = l_cache.GetLightsCount( LightType::ePoint );
				m_sceneNode->m_lightsCount.GetValue( 0 )[size_t( LightType::eSpot )] = l_cache.GetLightsCount( LightType::eSpot );
			}

			m_sceneNode->m_backgroundColour.SetValue( rgba_float( l_scene.GetBackgroundColour() ) );
			m_sceneNode->m_cameraPos.SetValue( l_camera.GetParent()->GetDerivedPosition() );
			l_scene.GetLightCache().BindLights();
			m_matrixUbo.Update();
			m_sceneUbo.Update();

			m_lightPassTextures[size_t( DsTexture::ePosition )]->Bind();
			m_lightPassTextures[size_t( DsTexture::eDiffuse )]->Bind();
			m_lightPassTextures[size_t( DsTexture::eNormals )]->Bind();
			m_lightPassTextures[size_t( DsTexture::eTangent )]->Bind();
			m_lightPassTextures[size_t( DsTexture::eSpecular )]->Bind();
			m_lightPassTextures[size_t( DsTexture::eEmissive )]->Bind();
			m_lightPassTextures[size_t( DsTexture::eInfos )]->Bind();
			DoBindDepthMaps( uint32_t( DsTexture::eInfos ) + 2 );

			l_program.m_pipeline->Apply();
			l_program.m_geometryBuffers->Draw( VertexCount, 0 );

			DoUnbindDepthMaps( uint32_t( DsTexture::eInfos ) + 2 );
			m_lightPassTextures[size_t( DsTexture::eInfos )]->Unbind();
			m_lightPassTextures[size_t( DsTexture::eEmissive )]->Unbind();
			m_lightPassTextures[size_t( DsTexture::eSpecular )]->Unbind();
			m_lightPassTextures[size_t( DsTexture::eTangent )]->Unbind();
			m_lightPassTextures[size_t( DsTexture::eNormals )]->Unbind();
			m_lightPassTextures[size_t( DsTexture::eDiffuse )]->Unbind();
			m_lightPassTextures[size_t( DsTexture::ePosition )]->Unbind();

			l_scene.GetLightCache().UnbindLights();
		}

		m_msaaFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->BlitInto( *m_msaaFrameBuffer, m_rect, uint32_t( BufferComponent::eDepth ) );
		m_msaaFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );

#endif
	}

	void RenderTechnique::DoRenderTransparent( uint32_t & p_visible )
	{
		m_msaaFrameBuffer->Unbind();
		GetEngine()->SetPerObjectLighting( true );
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_msaaFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_msaaFrameBuffer->Unbind();
		m_msaaFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer, m_rect, BufferComponent::eColour | BufferComponent::eDepth );
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
	}

	String RenderTechnique::DoGetLightPassVertexShaderSource(
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		auto vertex = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );
		auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );

		// Shader outputs
		auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			vtx_texture = texture;
			gl_Position = c3d_mtxProjection * vec4( vertex.x(), vertex.y(), 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetLightPassPixelShaderSource(
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_SCENE( l_writer );
		auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::ePosition ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormals ) );
		auto c3d_mapTangent = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eTangent ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eSpecular ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
		auto c3d_mapInfos = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eInfos ) );
		
		auto l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };

		// Shader outputs
		auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, vtx_texture ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), texture( c3d_mapTangent, vtx_texture ) );
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), l_v4Normal.xyz() );
			auto l_v3Tangent = l_writer.GetLocale( cuT( "l_v3Tangent" ), l_v4Tangent.xyz() );
			auto l_v4Infos = l_writer.GetLocale( cuT( "l_infos" ), texture( c3d_mapInfos, vtx_texture ) );
			auto l_iShadowReceiver = l_writer.GetLocale( cuT( "l_receiver" ), l_writer.Cast< Int >( l_v4Infos.x() ) );

			IF( l_writer, l_v3Normal != l_v3Tangent )
			{
				auto l_v4Position = l_writer.GetLocale( cuT( "l_v4Position" ), texture( c3d_mapPosition, vtx_texture ) );
				auto l_v4Diffuse = l_writer.GetLocale( cuT( "l_v4Diffuse" ), texture( c3d_mapDiffuse, vtx_texture ) );
				auto l_v4Specular = l_writer.GetLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, vtx_texture ) );
				auto l_v4Emissive = l_writer.GetLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, vtx_texture ) );
				auto l_v3MapAmbient = l_writer.GetLocale( cuT( "l_v3MapAmbient" ), vec3( l_v4Position.w(), l_v4Normal.w(), l_v4Tangent.w() ) );
				auto l_v3MapDiffuse = l_writer.GetLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
				auto l_v3MapSpecular = l_writer.GetLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
				auto l_v3MapEmissive = l_writer.GetLocale( cuT( "l_v3MapEmissive" ), l_v4Emissive.xyz() );
				auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
				auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), l_v4Position.xyz() );
				auto l_v3Bitangent = l_writer.GetLocale( cuT( "l_v3Bitangent" ), cross( l_v3Tangent.xyz(), l_v3Normal.xyz() ) );
				auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
				auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
				auto l_dist = l_writer.GetLocale( cuT( "l_dist" ), l_v4Diffuse.w() );
				auto l_y = l_writer.GetLocale( cuT( "l_y" ), l_v4Emissive.w() );

				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeCombinedLighting( l_worldEye
					, l_fMatShininess
					, l_iShadowReceiver
					, FragmentInput( l_v3Position, l_v3Normal )
					, l_output );

				pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( l_v3Ambient * l_v3MapAmbient.xyz() )
					+ l_writer.Paren( l_v3Diffuse * l_v3MapDiffuse.xyz() )
					+ l_writer.Paren( l_v3Specular * l_v3MapSpecular.xyz() )
					+ l_v3MapEmissive ), 1.0 );

				if ( GetFogType( p_sceneFlags ) != GLSL::FogType::eDisabled )
				{
					l_fog.ApplyFog( pxl_v4FragColor, l_dist, l_y );
				}
			}
			ELSE
			{
				l_writer.Discard();
			}
			FI
		} );

		return l_writer.Finalise();
	}

	bool RenderTechnique::DoCreateDeferred()
	{
		bool l_return = DoCreateGeometryPass();

		if ( l_return )
		{
			l_return = DoCreateLightPass();
		}

		return l_return;
	}

	bool RenderTechnique::DoCreateMsaa()
	{
		m_msaaFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_msaaColorBuffer = m_msaaFrameBuffer->CreateColourRenderBuffer( PixelFormat::eRGBA16F32F );
		m_msaaDepthBuffer = m_msaaFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_msaaColorAttach = m_msaaFrameBuffer->CreateAttachment( m_msaaColorBuffer );
		m_msaaDepthAttach = m_msaaFrameBuffer->CreateAttachment( m_msaaDepthBuffer );

		bool l_bReturn = m_msaaFrameBuffer->Create();
		m_msaaColorBuffer->SetSamplesCount( m_samplesCount );
		m_msaaDepthBuffer->SetSamplesCount( m_samplesCount );
		l_bReturn &= m_msaaColorBuffer->Create();
		l_bReturn &= m_msaaDepthBuffer->Create();
		return l_bReturn;
	}

	void RenderTechnique::DoDestroyDeferred()
	{
		DoDestroyLightPass();
		DoDestroyGeometryPass();
	}

	void RenderTechnique::DoDestroyMsaa()
	{
		m_msaaColorBuffer->Destroy();
		m_msaaDepthBuffer->Destroy();
		m_msaaFrameBuffer->Destroy();
		m_msaaDepthAttach.reset();
		m_msaaColorAttach.reset();
		m_msaaColorBuffer.reset();
		m_msaaDepthBuffer.reset();
		m_msaaFrameBuffer.reset();
	}

	bool RenderTechnique::DoInitialiseDeferred( uint32_t & p_index )
	{
		bool l_return = DoInitialiseLightPass( p_index );

		if ( l_return )
		{
			l_return = DoInitialiseGeometryPass();
		}

		return l_return;
	}

	bool RenderTechnique::DoInitialiseMsaa()
	{
		bool l_bReturn = true;
		m_rect = Castor::Rectangle( Position(), m_size );

		if ( l_bReturn )
		{
			l_bReturn = m_msaaColorBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_msaaDepthBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_msaaFrameBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			m_msaaFrameBuffer->Bind( FrameBufferMode::eConfig );
			m_msaaFrameBuffer->Attach( AttachmentPoint::eColour, m_msaaColorAttach );
			m_msaaFrameBuffer->Attach( AttachmentPoint::eDepth, m_msaaDepthAttach );
			m_msaaFrameBuffer->Unbind();
		}

		return l_bReturn;
	}

	void RenderTechnique::DoCleanupDeferred()
	{
		DoCleanupGeometryPass();
		DoCleanupLightPass();
	}

	void RenderTechnique::DoCleanupMsaa()
	{
		m_msaaFrameBuffer->Bind( FrameBufferMode::eConfig );
		m_msaaFrameBuffer->DetachAll();
		m_msaaFrameBuffer->Unbind();
		m_msaaFrameBuffer->Cleanup();
		m_msaaColorBuffer->Cleanup();
		m_msaaDepthBuffer->Cleanup();
	}

	void RenderTechnique::DoBindDepthMaps( uint32_t p_startIndex )
	{
		//if ( m_renderTarget.GetScene()->HasShadows() )
		//{
		//	m_directionalShadowMap.GetTexture().GetTexture()->Bind( p_startIndex );
		//	m_directionalShadowMap.GetTexture().GetSampler()->Bind( p_startIndex++ );
		//	m_spotShadowMap.GetTexture().GetTexture()->Bind( p_startIndex );
		//	m_spotShadowMap.GetTexture().GetSampler()->Bind( p_startIndex++ );

		//	for ( auto & l_map : m_pointShadowMap.GetTextures() )
		//	{
		//		l_map.GetTexture()->Bind( p_startIndex );
		//		l_map.GetSampler()->Bind( p_startIndex++ );
		//	}
		//}
	}

	void RenderTechnique::DoUnbindDepthMaps( uint32_t p_startIndex )const
	{
		//if ( m_renderTarget.GetScene()->HasShadows() )
		//{
		//	m_directionalShadowMap.GetTexture().GetTexture()->Unbind( p_startIndex );
		//	m_directionalShadowMap.GetTexture().GetSampler()->Unbind( p_startIndex++ );
		//	m_spotShadowMap.GetTexture().GetTexture()->Unbind( p_startIndex );
		//	m_spotShadowMap.GetTexture().GetSampler()->Unbind( p_startIndex++ );

		//	for ( auto & l_map : m_pointShadowMap.GetTextures() )
		//	{
		//		l_map.GetTexture()->Unbind( p_startIndex );
		//		l_map.GetSampler()->Unbind( p_startIndex++ );
		//	}
		//}
	}

	bool RenderTechnique::DoCreateGeometryPass()
	{
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_geometryPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		m_lightPassDepthBuffer = m_geometryPassFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_geometryPassDepthAttach = m_geometryPassFrameBuffer->CreateAttachment( m_lightPassDepthBuffer );
		bool l_return = m_geometryPassFrameBuffer->Create();

		if ( l_return )
		{
			l_return = m_lightPassDepthBuffer->Create();
		}

		return l_return;
	}

	bool RenderTechnique::DoCreateLightPass()
	{
		ShaderModel l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
		auto & l_scene = *m_renderTarget.GetScene();
		uint16_t l_fog{ 0u };
		ProgramFlags l_programFlags;

		for ( auto & l_program : m_lightPassShaderPrograms )
		{
			SceneFlags l_sceneFlags{ l_scene.GetFlags() };
			RemFlag( l_sceneFlags, SceneFlag::eFogSquaredExponential );
			AddFlag( l_sceneFlags, SceneFlag( l_fog ) );
			l_program.m_program = GetEngine()->GetShaderProgramCache().GetNewProgram( false );
			l_program.m_program->CreateObject( ShaderType::eVertex );
			l_program.m_program->CreateObject( ShaderType::ePixel );
			l_program.m_program->SetSource( ShaderType::eVertex, l_model, DoGetLightPassVertexShaderSource( 0u, l_programFlags, l_sceneFlags ) );
			l_program.m_program->SetSource( ShaderType::ePixel, l_model, DoGetLightPassPixelShaderSource( 0u, l_programFlags, l_sceneFlags ) );

			l_program.m_camera = m_sceneUbo.GetUniform< UniformType::eVec3f >( ShaderProgram::CameraPos );
			l_program.m_program->CreateUniform< UniformType::eSampler >( ShaderProgram::Lights, ShaderType::ePixel );

			for ( int i = 0; i < int( DsTexture::eInfos ); i++ )
			{
				l_program.m_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture( i ) ), ShaderType::ePixel )->SetValue( i + 1 );
			}

			if ( GetShadowType( l_scene.GetFlags() ) != GLSL::ShadowType::eNone )
			{
				l_program.m_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eInfos ), ShaderType::ePixel )->SetValue( int( DsTexture::eInfos ) + 1 );
			}

			if ( l_scene.HasShadows() )
			{
				l_program.m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowDirectional, ShaderType::ePixel )->SetValue( uint32_t( DsTexture::eInfos ) + 2u );
				l_program.m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel )->SetValue( uint32_t( DsTexture::eInfos ) + 3u );
				auto l_mapPoint = l_program.m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint, ShaderType::ePixel, 6u );

				for ( int i = 0; i < 6; ++i )
				{
					l_mapPoint->SetValue( uint32_t( DsTexture::eInfos ) + 4u + i, i );
				}
			}

			DepthStencilState l_dsstate;
			l_dsstate.SetDepthTest( false );
			l_dsstate.SetDepthMask( WritingMask::eZero );
			l_program.m_pipeline = GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate ), RasteriserState{}, BlendState{}, MultisampleState{}, *l_program.m_program, PipelineFlags{} );

			++l_fog;
		}

		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
			BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 ),
		} );

		real l_data[] =
		{
			0, 0, 0, 0,
			1, 1, 1, 1,
			0, 1, 0, 1,
			0, 0, 0, 0,
			1, 0, 1, 0,
			1, 1, 1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( *m_renderSystem.GetEngine(), m_declaration );
		uint32_t l_stride = m_declaration.stride();
		m_vertexBuffer->Resize( sizeof( l_data ) );
		uint8_t * l_buffer = m_vertexBuffer->data();
		std::memcpy( l_buffer, l_data, sizeof( l_data ) );

		return true;
	}

	void RenderTechnique::DoDestroyGeometryPass()
	{
		m_lightPassDepthBuffer->Destroy();
		m_lightPassDepthBuffer.reset();
		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
	}

	void RenderTechnique::DoDestroyLightPass()
	{
		m_vertexBuffer.reset();
		m_matrixUbo.Cleanup();
		m_sceneUbo.Cleanup();

		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_pipeline->Cleanup();
			program.m_pipeline.reset();
			program.m_geometryBuffers.reset();
			program.m_program.reset();
		}
	}

	bool RenderTechnique::DoInitialiseGeometryPass()
	{
		bool l_return = m_geometryPassFrameBuffer->Initialise( m_size );

		if ( l_return )
		{
			m_geometryPassFrameBuffer->Bind( FrameBufferMode::eConfig );

			for ( int i = 0; i < size_t( DsTexture::eCount ) && l_return; i++ )
			{
				m_geometryPassFrameBuffer->Attach( GetTextureAttachmentPoint( DsTexture( i ) )
					, GetTextureAttachmentIndex( DsTexture( i ) )
					, m_geometryPassTexAttachs[i]
					, m_lightPassTextures[i]->GetType() );
			}

			m_geometryPassFrameBuffer->Attach( AttachmentPoint::eDepth, m_geometryPassDepthAttach );
			m_geometryPassFrameBuffer->IsComplete();
			m_geometryPassFrameBuffer->Unbind();
		}

		m_viewport.Initialise();
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_program->Initialise();
			program.m_geometryBuffers = m_renderSystem.CreateGeometryBuffers( Topology::eTriangles, *program.m_program );
			program.m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
			program.m_pipeline->AddUniformBuffer( m_matrixUbo );
			program.m_pipeline->AddUniformBuffer( m_sceneUbo );
		}

		return l_return;
	}

	bool RenderTechnique::DoInitialiseLightPass( uint32_t & p_index )
	{
		bool l_return = true;

		for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
		{
			auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, GetTextureFormat( DsTexture( i ) ), m_size );
			l_texture->GetImage().InitialiseSource();

			m_lightPassTextures[i] = std::make_unique< TextureUnit >( *GetEngine() );
			m_lightPassTextures[i]->SetIndex( i + 1 ); // +1 because light texture is at index 0
			m_lightPassTextures[i]->SetTexture( l_texture );
			m_lightPassTextures[i]->SetSampler( GetEngine()->GetLightsSampler() );
			m_lightPassTextures[i]->Initialise();

			m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( l_texture );
			p_index++;
		}

		if ( l_return )
		{
			m_lightPassDepthBuffer->Initialise( m_size );
		}

		return l_return;
	}

	void RenderTechnique::DoCleanupGeometryPass()
	{
		m_geometryPassFrameBuffer->Bind( FrameBufferMode::eConfig );
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();
	}

	void RenderTechnique::DoCleanupLightPass()
	{
		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_geometryBuffers->Cleanup();
			program.m_geometryBuffers.reset();
			program.m_program->Cleanup();
		}

		m_viewport.Cleanup();
		m_vertexBuffer->Cleanup();

		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
			l_unit.reset();
		}
	}
}
