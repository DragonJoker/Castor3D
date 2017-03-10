#include "DeferredRenderTechnique.hpp"

#include "DeferredRenderTechniqueOpaquePass.hpp"

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
#include <Scene/Light/Light.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Technique/RenderTechniquePass.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Log/Logger.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

#define DEBUG_DEFERRED_BUFFERS 0

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	//************************************************************************************************

	String const RenderTechnique::Type = cuT( "deferred" );
	String const RenderTechnique::Name = cuT( "Deferred Lighting Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( RenderTechnique::Type
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< OpaquePass >( p_renderTarget, *this )
			, std::make_unique< RenderTechniquePass >( cuT( "deferred_transparent" ), p_renderTarget, *this, false, false )
			, p_params )
		, m_matrixUbo{ ShaderProgram::BufferMatrix, p_renderSystem }
		, m_sceneUbo{ ShaderProgram::BufferScene, p_renderSystem }
		, m_directionalLightPassShaderPrograms
		{
			{
				DirectionalLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				DirectionalLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				DirectionalLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				DirectionalLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
			}
		}
		, m_pointLightPassShaderPrograms
		{
			{
				PointLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				PointLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				PointLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				PointLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
			}
		}
		, m_spotLightPassShaderPrograms
		{
			{
				SpotLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				SpotLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				SpotLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
				SpotLightPass{ *p_renderTarget.GetEngine(), m_matrixUbo, m_sceneUbo },
			}
		}
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
		UniformBuffer::FillSceneBuffer( m_sceneUbo );

		m_sceneNode = std::make_unique< SceneRenderNode >( m_sceneUbo );
		Logger::LogInfo( cuT( "Using deferred shading" ) );
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
		bool l_return = DoCreateGeometryPass();

		if ( l_return )
		{
			l_return = DoCreateLightPass();
		}

		return l_return;
	}

	void RenderTechnique::DoDestroy()
	{
		DoDestroyLightPass();
		DoDestroyGeometryPass();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = DoInitialiseLightPass( p_index );

		if ( l_return )
		{
			l_return = DoInitialiseGeometryPass();
		}

		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		DoCleanupGeometryPass();
		DoCleanupLightPass();
	}

	void RenderTechnique::DoBeginRender()
	{
	}

	void RenderTechnique::DoBeginOpaqueRendering()
	{
		GetEngine()->SetPerObjectLighting( false );
		m_geometryPassFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_geometryPassFrameBuffer->Clear();
	}

	void RenderTechnique::DoEndOpaqueRendering()
	{
		m_geometryPassFrameBuffer->Unbind();
		// Render the light pass.

#if DEBUG_DEFERRED_BUFFERS

		int l_width = int( m_size.width() );
		int l_height = int( m_size.height() );
		int l_thirdWidth = int( l_width / 3.0f );
		int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
		int l_thirdHeight = int( l_height / 3.0f );
		int l_twothirdHeight = int( 2.0f * l_height / 3.0f );
		auto l_size = Size( l_thirdWidth, l_thirdHeight );
		auto & l_context = *m_renderSystem.GetCurrentContext();
		m_frameBuffer.m_frameBuffer->Bind();
		l_context.RenderTexture( Position{ 0, 0 }, l_size, *m_lightPassTextures[size_t( DsTexture::ePosition )]->GetTexture() );
		l_context.RenderTexture( Position{ 0, l_thirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eDiffuse )]->GetTexture() );
		l_context.RenderTexture( Position{ 0, l_twothirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eNormals )]->GetTexture() );
		l_context.RenderTexture( Position{ l_thirdWidth, 0 }, l_size, *m_lightPassTextures[size_t( DsTexture::eTangent )]->GetTexture() );
		l_context.RenderTexture( Position{ l_thirdWidth, l_thirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eSpecular )]->GetTexture() );
		l_context.RenderTexture( Position{ l_thirdWidth, l_twothirdHeight }, l_size, *m_lightPassTextures[size_t( DsTexture::eEmissive )]->GetTexture() );

#else

		m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		auto & l_scene = *m_renderTarget.GetScene();
		auto & l_camera = *m_renderTarget.GetCamera();
		m_frameBuffer.m_frameBuffer->SetClearColour(l_scene.GetBackgroundColour() );
		m_frameBuffer.m_frameBuffer->Clear();

		auto & l_fog = l_scene.GetFog();
		m_sceneNode->m_fogType.SetValue( int( l_fog.GetType() ) );

		if ( l_fog.GetType() != GLSL::FogType::eDisabled )
		{
			m_sceneNode->m_fogDensity.SetValue( l_fog.GetDensity() );
		}

		auto & l_cache = l_scene.GetLightCache();
		auto l_lock = make_unique_lock( l_cache );
		m_sceneNode->m_ambientLight.SetValue( rgba_float( l_scene.GetAmbientLight() ) );

		m_sceneNode->m_backgroundColour.SetValue( rgba_float( l_scene.GetBackgroundColour() ) );
		m_sceneNode->m_cameraPos.SetValue( l_camera.GetParent()->GetDerivedPosition() );
		m_matrixUbo.Update();
		m_sceneUbo.Update();
		bool l_first{ true };

		if ( l_cache.GetLightsCount( LightType::eDirectional ) )
		{
			auto & l_program = m_directionalLightPassShaderPrograms[uint16_t( GetFogType( l_scene.GetFlags() ) )];

			for ( auto & l_light : l_cache.GetLights( LightType::eDirectional ) )
			{
				l_program.Render( m_size, m_lightPassTextures, *l_light->GetDirectionalLight(), l_first );
				l_first = false;
			}
		}

		if ( l_cache.GetLightsCount( LightType::ePoint ) )
		{
			auto & l_program = m_pointLightPassShaderPrograms[uint16_t( GetFogType( l_scene.GetFlags() ) )];

			for ( auto & l_light : l_cache.GetLights( LightType::ePoint ) )
			{
				l_program.Render( m_size, m_lightPassTextures, *l_light->GetPointLight(), l_first );
				l_first = false;
			}
		}

		if ( l_cache.GetLightsCount( LightType::eSpot ) )
		{
			auto & l_program = m_spotLightPassShaderPrograms[uint16_t( GetFogType( l_scene.GetFlags() ) )];

			for ( auto & l_light : l_cache.GetLights( LightType::eSpot ) )
			{
				l_program.Render( m_size, m_lightPassTextures, *l_light->GetSpotLight(), l_first );
				l_first = false;
			}
		}

		m_frameBuffer.m_frameBuffer->Unbind();
		m_geometryPassFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer, Rectangle{ Position{}, m_size }, uint32_t( BufferComponent::eDepth ) );
		m_frameBuffer.m_frameBuffer->Bind();

#endif

	}

	void RenderTechnique::DoBeginTransparentRendering()
	{
		GetEngine()->SetPerObjectLighting( true );
	}

	void RenderTechnique::DoEndTransparentRendering()
	{
		m_frameBuffer.m_frameBuffer->Unbind();
	}

	void RenderTechnique::DoEndRender()
	{
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

		// Shader outputs
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * vec4( vertex.x(), vertex.y(), 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetDirectionalLightPassPixelShaderSource(
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_SCENE( l_writer );
		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::ePosition ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormals ) );
		auto c3d_mapTangent = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eTangent ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eSpecular ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
		auto c3d_mapInfos = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eInfos ) );
		auto gl_FragCoord = l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto l_lighting = l_writer.CreateDirectionalLightingModel( PhongLightingModel::Name
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
		auto light = l_writer.GetUniform< GLSL::DirectionalLight >( cuT( "light" ) );
		auto c3d_renderSize = l_writer.GetUniform< Vec2 >( cuT( "c3d_renderSize" ) );

		// Shader outputs
		auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		auto l_calcTexCoord = l_writer.ImplementFunction< Vec2 >( cuT( "CalcTexCoord" ), [&]()
		{
			l_writer.Return( gl_FragCoord.xy() / c3d_renderSize );
		} );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_texCoord = l_writer.GetLocale( cuT( "l_texCoord" ), l_calcTexCoord() );
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, l_texCoord ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), texture( c3d_mapTangent, l_texCoord ) );
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), l_v4Normal.xyz() );
			auto l_v3Tangent = l_writer.GetLocale( cuT( "l_v3Tangent" ), l_v4Tangent.xyz() );

			IF( l_writer, l_v3Normal != l_v3Tangent )
			{
				auto l_v4Infos = l_writer.GetLocale( cuT( "l_infos" ), texture( c3d_mapInfos, l_texCoord ) );
				auto l_iShadowReceiver = l_writer.GetLocale( cuT( "l_receiver" ), l_writer.Cast< Int >( l_v4Infos.x() ) );
				auto l_v4Position = l_writer.GetLocale( cuT( "l_v4Position" ), texture( c3d_mapPosition, l_texCoord ) );
				auto l_v4Diffuse = l_writer.GetLocale( cuT( "l_v4Diffuse" ), texture( c3d_mapDiffuse, l_texCoord ) );
				auto l_v4Specular = l_writer.GetLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, l_texCoord ) );
				auto l_v4Emissive = l_writer.GetLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, l_texCoord ) );
				auto l_v3MapAmbient = l_writer.GetLocale( cuT( "l_v3MapAmbient" ), vec3( l_v4Position.w(), l_v4Normal.w(), l_v4Tangent.w() ) );
				auto l_v3MapDiffuse = l_writer.GetLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
				auto l_v3MapSpecular = l_writer.GetLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
				auto l_v3MapEmissive = l_writer.GetLocale( cuT( "l_v3MapEmissive" ), l_v4Emissive.xyz() );
				auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
				auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), l_v4Position.xyz() );
				auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
				auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
				auto l_dist = l_writer.GetLocale( cuT( "l_dist" ), l_v4Diffuse.w() );
				auto l_y = l_writer.GetLocale( cuT( "l_y" ), l_v4Emissive.w() );

				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( light
					, l_worldEye
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

	String RenderTechnique::DoGetPointLightPassPixelShaderSource(
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_SCENE( l_writer );
		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::ePosition ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormals ) );
		auto c3d_mapTangent = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eTangent ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eSpecular ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
		auto c3d_mapInfos = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eInfos ) );
		auto gl_FragCoord = l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto l_lighting = l_writer.CreatePointLightingModel( PhongLightingModel::Name
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
		auto light = l_writer.GetUniform< GLSL::PointLight >( cuT( "light" ) );
		auto c3d_renderSize = l_writer.GetUniform< Vec2 >( cuT( "c3d_renderSize" ) );

		// Shader outputs
		auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		auto l_calcTexCoord = l_writer.ImplementFunction< Vec2 >( cuT( "CalcTexCoord" ), [&]()
		{
			l_writer.Return( gl_FragCoord.xy() / c3d_renderSize );
		} );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_texCoord = l_writer.GetLocale( cuT( "l_texCoord" ), l_calcTexCoord() );
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, l_texCoord ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), texture( c3d_mapTangent, l_texCoord ) );
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), l_v4Normal.xyz() );
			auto l_v3Tangent = l_writer.GetLocale( cuT( "l_v3Tangent" ), l_v4Tangent.xyz() );

			IF( l_writer, l_v3Normal != l_v3Tangent )
			{
				auto l_v4Infos = l_writer.GetLocale( cuT( "l_infos" ), texture( c3d_mapInfos, l_texCoord ) );
				auto l_iShadowReceiver = l_writer.GetLocale( cuT( "l_receiver" ), l_writer.Cast< Int >( l_v4Infos.x() ) );
				auto l_v4Position = l_writer.GetLocale( cuT( "l_v4Position" ), texture( c3d_mapPosition, l_texCoord ) );
				auto l_v4Diffuse = l_writer.GetLocale( cuT( "l_v4Diffuse" ), texture( c3d_mapDiffuse, l_texCoord ) );
				auto l_v4Specular = l_writer.GetLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, l_texCoord ) );
				auto l_v4Emissive = l_writer.GetLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, l_texCoord ) );
				auto l_v3MapAmbient = l_writer.GetLocale( cuT( "l_v3MapAmbient" ), vec3( l_v4Position.w(), l_v4Normal.w(), l_v4Tangent.w() ) );
				auto l_v3MapDiffuse = l_writer.GetLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
				auto l_v3MapSpecular = l_writer.GetLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
				auto l_v3MapEmissive = l_writer.GetLocale( cuT( "l_v3MapEmissive" ), l_v4Emissive.xyz() );
				auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
				auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), l_v4Position.xyz() );
				auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
				auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
				auto l_dist = l_writer.GetLocale( cuT( "l_dist" ), l_v4Diffuse.w() );
				auto l_y = l_writer.GetLocale( cuT( "l_y" ), l_v4Emissive.w() );

				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( light
					, l_worldEye
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

	String RenderTechnique::DoGetSpotLightPassPixelShaderSource(
		TextureChannels const & p_textureFlags,
		ProgramFlags const & p_programFlags,
		SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_SCENE( l_writer );
		auto l_texCoord = l_writer.GetInput< Vec2 >( cuT( "l_texCoord" ) );

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
		auto gl_FragCoord = l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		auto l_lighting = l_writer.CreateSpotLightingModel( PhongLightingModel::Name
			, GetShadowType( p_sceneFlags ) );
		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
		auto light = l_writer.GetUniform< GLSL::SpotLight >( cuT( "light" ) );
		auto c3d_renderSize = l_writer.GetUniform< Vec2 >( cuT( "c3d_renderSize" ) );

		// Shader outputs
		auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		auto l_calcTexCoord = l_writer.ImplementFunction< Vec2 >( cuT( "CalcTexCoord" ), [&]()
		{
			l_writer.Return( gl_FragCoord.xy() / c3d_renderSize );
		} );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_texCoord = l_writer.GetLocale( cuT( "l_texCoord" ), l_calcTexCoord() );
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, l_texCoord ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), texture( c3d_mapTangent, l_texCoord ) );
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), l_v4Normal.xyz() );
			auto l_v3Tangent = l_writer.GetLocale( cuT( "l_v3Tangent" ), l_v4Tangent.xyz() );

			IF( l_writer, l_v3Normal != l_v3Tangent )
			{
				auto l_v4Infos = l_writer.GetLocale( cuT( "l_infos" ), texture( c3d_mapInfos, l_texCoord ) );
				auto l_iShadowReceiver = l_writer.GetLocale( cuT( "l_receiver" ), l_writer.Cast< Int >( l_v4Infos.x() ) );
				auto l_v4Position = l_writer.GetLocale( cuT( "l_v4Position" ), texture( c3d_mapPosition, l_texCoord ) );
				auto l_v4Diffuse = l_writer.GetLocale( cuT( "l_v4Diffuse" ), texture( c3d_mapDiffuse, l_texCoord ) );
				auto l_v4Specular = l_writer.GetLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, l_texCoord ) );
				auto l_v4Emissive = l_writer.GetLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, l_texCoord ) );
				auto l_v3MapAmbient = l_writer.GetLocale( cuT( "l_v3MapAmbient" ), vec3( l_v4Position.w(), l_v4Normal.w(), l_v4Tangent.w() ) );
				auto l_v3MapDiffuse = l_writer.GetLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
				auto l_v3MapSpecular = l_writer.GetLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
				auto l_v3MapEmissive = l_writer.GetLocale( cuT( "l_v3MapEmissive" ), l_v4Emissive.xyz() );
				auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
				auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), l_v4Position.xyz() );
				auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
				auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
				auto l_dist = l_writer.GetLocale( cuT( "l_dist" ), l_v4Diffuse.w() );
				auto l_y = l_writer.GetLocale( cuT( "l_y" ), l_v4Emissive.w() );

				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( light
					, l_worldEye
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

	void RenderTechnique::DoBindDepthMaps( uint32_t p_startIndex )
	{
		if ( m_renderTarget.GetScene()->HasShadows() )
		{
			m_directionalShadowMap.GetTexture().GetTexture()->Bind( p_startIndex );
			m_directionalShadowMap.GetTexture().GetSampler()->Bind( p_startIndex++ );
			m_spotShadowMap.GetTexture().GetTexture()->Bind( p_startIndex );
			m_spotShadowMap.GetTexture().GetSampler()->Bind( p_startIndex++ );

			for ( auto & l_map : m_pointShadowMap.GetTextures() )
			{
				l_map.GetTexture()->Bind( p_startIndex );
				l_map.GetSampler()->Bind( p_startIndex++ );
			}
		}
	}

	void RenderTechnique::DoUnbindDepthMaps( uint32_t p_startIndex )const
	{
		if ( m_renderTarget.GetScene()->HasShadows() )
		{
			m_directionalShadowMap.GetTexture().GetTexture()->Unbind( p_startIndex );
			m_directionalShadowMap.GetTexture().GetSampler()->Unbind( p_startIndex++ );
			m_spotShadowMap.GetTexture().GetTexture()->Unbind( p_startIndex );
			m_spotShadowMap.GetTexture().GetSampler()->Unbind( p_startIndex++ );

			for ( auto & l_map : m_pointShadowMap.GetTextures() )
			{
				l_map.GetTexture()->Unbind( p_startIndex );
				l_map.GetSampler()->Unbind( p_startIndex++ );
			}
		}
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
		using PxlShaderGetter = std::function< String( TextureChannels const &, ProgramFlags const &, SceneFlags ) >;
		auto l_createPasses = [this]( auto & p_programs
			, PxlShaderGetter p_pixelShaderGetter )
		{
			ShaderModel l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			auto & l_scene = *m_renderTarget.GetScene();
			ProgramFlags l_programFlags;
			uint16_t l_fog{ 0u };

			for ( auto & l_program : p_programs )
			{
				SceneFlags l_sceneFlags{ l_scene.GetFlags() };
				RemFlag( l_sceneFlags, SceneFlag::eFogSquaredExponential );
				AddFlag( l_sceneFlags, SceneFlag( l_fog ) );
				auto l_shader = GetEngine()->GetShaderProgramCache().GetNewProgram( false );
				l_shader->CreateObject( ShaderType::eVertex );
				l_shader->CreateObject( ShaderType::ePixel );
				l_shader->SetSource( ShaderType::eVertex, l_model, DoGetLightPassVertexShaderSource( 0u, l_programFlags, l_sceneFlags ) );
				l_shader->SetSource( ShaderType::ePixel, l_model, p_pixelShaderGetter( 0u, l_programFlags, l_sceneFlags ) );
				l_program.Create( l_shader, l_scene );
				++l_fog;
			}
		};
		l_createPasses( m_directionalLightPassShaderPrograms
			, std::bind( &RenderTechnique::DoGetDirectionalLightPassPixelShaderSource
				, this
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3 ) );
		l_createPasses( m_pointLightPassShaderPrograms
			, std::bind( &RenderTechnique::DoGetPointLightPassPixelShaderSource
				, this
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3 ) );
		l_createPasses( m_spotLightPassShaderPrograms
			, std::bind( &RenderTechnique::DoGetSpotLightPassPixelShaderSource
				, this
				, std::placeholders::_1
				, std::placeholders::_2
				, std::placeholders::_3 ) );
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
		m_matrixUbo.Cleanup();
		m_sceneUbo.Cleanup();

		auto l_destroyPasses = [this]( auto & p_programs )
		{
			for ( auto & l_program : p_programs )
			{
				l_program.Destroy();
			}
		};

		l_destroyPasses( m_directionalLightPassShaderPrograms );
		l_destroyPasses( m_pointLightPassShaderPrograms );
		l_destroyPasses( m_spotLightPassShaderPrograms );
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
		
		return l_return;
	}

	bool RenderTechnique::DoInitialiseLightPass( uint32_t & p_index )
	{
		bool l_return = true;

		auto l_initPasses = [this]( auto & p_programs )
		{
			Scene & l_scene = *m_renderTarget.GetScene();

			for ( auto & l_program : p_programs )
			{
				l_program.Initialise();
			}
		};

		l_initPasses( m_directionalLightPassShaderPrograms );
		l_initPasses( m_pointLightPassShaderPrograms );
		l_initPasses( m_spotLightPassShaderPrograms );

		for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
		{
			auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, GetTextureFormat( DsTexture( i ) )
				, m_size );
			l_texture->GetImage().InitialiseSource();

			m_lightPassTextures[i] = std::make_unique< TextureUnit >( *GetEngine() );
			m_lightPassTextures[i]->SetIndex( i );
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
		auto l_cleanPasses = [this]( auto & p_programs )
		{
			for ( auto & l_program : p_programs )
			{
				l_program.Cleanup();
			}
		};

		l_cleanPasses( m_directionalLightPassShaderPrograms );
		l_cleanPasses( m_pointLightPassShaderPrograms );
		l_cleanPasses( m_spotLightPassShaderPrograms );

		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
			l_unit.reset();
		}
	}
}
