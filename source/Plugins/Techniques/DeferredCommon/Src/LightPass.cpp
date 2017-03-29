#include "LightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <Render/RenderPipeline.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred_common
{
	//************************************************************************************************

	String GetTextureName( DsTexture p_texture )
	{
		static std::array< String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "c3d_mapPosition" ),
				cuT( "c3d_mapDiffuse" ),
				cuT( "c3d_mapNormals" ),
				cuT( "c3d_mapSpecular" ),
				cuT( "c3d_mapEmissive" ),
				cuT( "c3d_mapDepth" ),
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
				PixelFormat::eL16F32F,
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
			}
		};

		return Values[size_t( p_texture )];
	}

	float GetMaxDistance( LightCategory const & p_light
		, Point3f const & p_attenuation
		, float p_max )
	{
		constexpr float l_threshold = 0.000001f;
		auto l_const = std::abs( p_attenuation[0] );
		auto l_linear = std::abs( p_attenuation[1] );
		auto l_quadr = std::abs( p_attenuation[2] );
		float l_result = p_max;

		if ( l_const >= l_threshold
			|| l_linear >= l_threshold
			|| l_quadr >= l_threshold )
		{
			float l_maxChannel = std::max( std::max( p_light.GetColour()[0]
				, p_light.GetColour()[1] )
				, p_light.GetColour()[2] );
			auto l_c = 256.0f * l_maxChannel * p_light.GetDiffuseIntensity();

			if ( l_quadr >= l_threshold )
			{
				if ( l_linear < l_threshold )
				{
					REQUIRE( l_c >= l_const );
					l_result = sqrtf( ( l_c - l_const ) / l_quadr );
				}
				else
				{
					auto l_delta = l_linear * l_linear - 4 * l_quadr * ( l_const - l_c );
					REQUIRE( l_delta >= 0 );
					l_result = ( -l_linear + sqrtf( l_delta ) ) / ( 2 * l_quadr );
				}
			}
			else if ( l_linear >= l_threshold )
			{
				l_result = ( l_c - l_const ) / l_linear;
			}
		}

		return std::min( p_max, l_result );
	}

	//************************************************************************************************

	LightPass::Program::Program( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl
		, bool p_ssao )
	{
		auto & l_engine = *p_scene.GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		ShaderModel l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();

		m_program = l_engine.GetShaderProgramCache().GetNewProgram( false );
		m_program->CreateObject( ShaderType::eVertex );
		m_program->CreateObject( ShaderType::ePixel );
		m_program->SetSource( ShaderType::eVertex, l_model, p_vtx );
		m_program->SetSource( ShaderType::ePixel, l_model, p_pxl );

		m_renderSize = m_program->CreateUniform< UniformType::eVec2f >( cuT( "c3d_renderSize" ), ShaderType::ePixel );
		m_lightColour = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_v3Colour" ), ShaderType::ePixel );
		m_lightIntensity = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_lightBase.m_v3Intensity" ), ShaderType::ePixel );

		for ( int i = 0; i < int( DsTexture::eCount ); i++ )
		{
			m_program->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture( i ) ), ShaderType::ePixel )->SetValue( i );
		}

		if ( p_ssao )
		{
			m_program->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapSsao" ), ShaderType::ePixel )->SetValue( int( DsTexture::eCount ) );
		}
	}

	LightPass::Program::~Program()
	{
		m_firstPipeline->Cleanup();
		m_blendPipeline->Cleanup();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_lightColour = nullptr;
		m_lightIntensity = nullptr;
		m_renderSize = nullptr;
		m_program.reset();
	}

	void LightPass::Program::Initialise( VertexBuffer & p_vbo
		, IndexBufferSPtr p_ibo
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo
		, UniformBuffer * p_modelMatrixUbo )
	{
		m_program->Initialise();
		m_firstPipeline = DoCreatePipeline( false );
		m_blendPipeline = DoCreatePipeline( true );
		m_firstPipeline->AddUniformBuffer( p_matrixUbo );
		m_firstPipeline->AddUniformBuffer( p_sceneUbo );
		m_blendPipeline->AddUniformBuffer( p_matrixUbo );
		m_blendPipeline->AddUniformBuffer( p_sceneUbo );

		if ( p_modelMatrixUbo )
		{
			m_firstPipeline->AddUniformBuffer( *p_modelMatrixUbo );
			m_blendPipeline->AddUniformBuffer( *p_modelMatrixUbo );
		}

		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { p_vbo }, p_ibo );
	}

	void LightPass::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_program->Cleanup();
	}

	void LightPass::Program::Render( Size const & p_size
		, Castor3D::Light const & p_light
		, uint32_t p_count
		, bool p_first )
	{
		m_renderSize->SetValue( Point2f( p_size.width(), p_size.height() ) );
		m_lightColour->SetValue( p_light.GetColour() );
		m_lightIntensity->SetValue( p_light.GetIntensity() );
		DoBind( p_light );

		if ( p_first )
		{
			m_firstPipeline->Apply();
		}
		else
		{
			m_blendPipeline->Apply();
		}

		m_geometryBuffers->Draw( p_count, 0 );
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, RenderBufferAttachment & p_depthAttach
		, bool p_ssao
		, bool p_shadows )
		: m_engine{ p_engine }
		, m_shadows{ p_shadows }
		, m_matrixUbo{ ShaderProgram::BufferMatrix, *p_engine.GetRenderSystem() }
		, m_frameBuffer{ p_frameBuffer }
		, m_depthAttach{ p_depthAttach }
		, m_ssao{ p_ssao }
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
		m_projectionUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );
		m_viewUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxView );
	}

	void LightPass::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Light const & p_light
		, Camera const & p_camera
		, GLSL::FogType p_fogType
		, Castor3D::TextureUnit const * p_ssao
		, bool p_first )
	{
		DoUpdate( p_size
			, p_light
			, p_camera );

		DoRender( p_size
			, p_gp
			, p_light
			, p_fogType
			, p_ssao
			, p_first );
	}

	void LightPass::DoInitialise( Scene const & p_scene
		, LightType p_type
		, VertexBuffer & p_vbo
		, IndexBufferSPtr p_ibo
		, UniformBuffer & p_sceneUbo
		, UniformBuffer * p_modelMatrixUbo )
	{
		uint16_t l_fogType{ 0u };

		for ( auto & l_program : m_programs )
		{
			SceneFlags l_sceneFlags{ p_scene.GetFlags() };
			RemFlag( l_sceneFlags, SceneFlag::eFogSquaredExponential );
			AddFlag( l_sceneFlags, SceneFlag( l_fogType ) );
			l_program = DoCreateProgram( p_scene
				, DoGetVertexShaderSource( l_sceneFlags )
				, DoGetPixelShaderSource( l_sceneFlags, p_type ) );
			l_program->Initialise( p_vbo
				, p_ibo
				, m_matrixUbo
				, p_sceneUbo
				, p_modelMatrixUbo );
			l_fogType++;
		}
	}

	void LightPass::DoCleanup()
	{
		for ( auto & l_program : m_programs )
		{
			l_program->Cleanup();
			l_program.reset();
		}
	}

	void LightPass::DoRender( Castor::Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::Light const & p_light
		, GLSL::FogType p_fogType
		, Castor3D::TextureUnit const * p_ssao
		, bool p_first )
	{
		m_frameBuffer.Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.SetDrawBuffers();
		p_gp[size_t( DsTexture::ePosition )]->Bind();
		p_gp[size_t( DsTexture::eDiffuse )]->Bind();
		p_gp[size_t( DsTexture::eNormals )]->Bind();
		p_gp[size_t( DsTexture::eSpecular )]->Bind();
		p_gp[size_t( DsTexture::eEmissive )]->Bind();
		p_gp[size_t( DsTexture::eDepth )]->Bind();

		if ( p_ssao && m_ssao )
		{
			p_ssao->GetTexture()->Bind( size_t( DsTexture::eCount ) );
			p_ssao->GetSampler()->Bind( size_t( DsTexture::eCount ) );
		}

		auto & l_program = *m_programs[uint16_t( p_fogType )];
		l_program.Render( p_size
			, p_light
			, GetCount()
			, p_first );

		if ( p_ssao && m_ssao )
		{
			p_ssao->GetSampler()->Unbind( size_t( DsTexture::eCount ) );
			p_ssao->GetTexture()->Unbind( size_t( DsTexture::eCount ) );
		}

		p_gp[size_t( DsTexture::eDepth )]->Unbind();
		p_gp[size_t( DsTexture::eEmissive )]->Unbind();
		p_gp[size_t( DsTexture::eSpecular )]->Unbind();
		p_gp[size_t( DsTexture::eNormals )]->Unbind();
		p_gp[size_t( DsTexture::eDiffuse )]->Unbind();
		p_gp[size_t( DsTexture::ePosition )]->Unbind();
		m_frameBuffer.Unbind();
	}

	String LightPass::DoGetPixelShaderSource( SceneFlags const & p_sceneFlags
		, LightType p_type )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::ePosition ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormals ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eSpecular ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
		auto c3d_mapDepth = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
		auto c3d_mapSsao = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapSsao" ), m_ssao );
		auto gl_FragCoord = l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		std::unique_ptr< LightingModel > l_lighting;

		switch ( p_type )
		{
		case LightType::eDirectional:
			{
				l_lighting = l_writer.CreateDirectionalLightingModel( PhongLightingModel::Name
					, m_shadows ? GetShadowType( p_sceneFlags ) : ShadowType::eNone );
				auto light = l_writer.GetUniform< GLSL::DirectionalLight >( cuT( "light" ) );
			}
			break;

		case LightType::ePoint:
			{
				l_lighting = l_writer.CreatePointLightingModel( PhongLightingModel::Name
					, m_shadows ? GetShadowType( p_sceneFlags ) : ShadowType::eNone );
				auto light = l_writer.GetUniform< GLSL::PointLight >( cuT( "light" ) );
			}
			break;

		case LightType::eSpot:
			{
				l_lighting = l_writer.CreateSpotLightingModel( PhongLightingModel::Name
					, m_shadows ? GetShadowType( p_sceneFlags ) : ShadowType::eNone );
				auto light = l_writer.GetUniform< GLSL::SpotLight >( cuT( "light" ) );
			}
			break;
		}

		GLSL::Fog l_fog{ GetFogType( p_sceneFlags ), l_writer };
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
			auto l_v4Position = l_writer.GetLocale( cuT( "l_v4Position" ), texture( c3d_mapPosition, l_texCoord ) );
			auto l_v4Diffuse = l_writer.GetLocale( cuT( "l_v4Diffuse" ), texture( c3d_mapDiffuse, l_texCoord ) );
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, l_texCoord ) );
			auto l_v4Specular = l_writer.GetLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, l_texCoord ) );
			auto l_v4Emissive = l_writer.GetLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, l_texCoord ) );
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), l_v4Normal.xyz() );
			auto l_iShadowReceiver = l_writer.GetLocale( cuT( "l_receiver" ), l_writer.Cast< Int >( l_v4Position.w() ) );
			auto l_v3MapDiffuse = l_writer.GetLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
			auto l_v3MapSpecular = l_writer.GetLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
			auto l_v3MapEmissive = l_writer.GetLocale( cuT( "l_v3MapEmissive" ), l_v4Emissive.xyz() );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
			auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), l_v4Position.xyz() );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), c3d_v3CameraPosition );
			auto l_ambientOcclusion = l_writer.GetLocale( cuT( "l_ambientOcclusion" ), m_ssao, texture( c3d_mapSsao, l_texCoord ).r() );

			OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };

			switch ( p_type )
			{
			case LightType::eDirectional:
				{
					auto light = l_writer.GetBuiltin< GLSL::DirectionalLight >( cuT( "light" ) );
					l_lighting->ComputeDirectionalLight( light
						, l_worldEye
						, l_fMatShininess
						, l_iShadowReceiver
						, FragmentInput( l_v3Position, l_v3Normal )
						, l_output );
				}
				break;

			case LightType::ePoint:
				{
				auto light = l_writer.GetBuiltin< GLSL::PointLight >( cuT( "light" ) );
					l_lighting->ComputeOnePointLight( light
						, l_worldEye
						, l_fMatShininess
						, l_iShadowReceiver
						, FragmentInput( l_v3Position, l_v3Normal )
						, l_output );
				}
				break;

			case LightType::eSpot:
				{
				auto light = l_writer.GetBuiltin< GLSL::SpotLight >( cuT( "light" ) );
					l_lighting->ComputeOneSpotLight( light
						, l_worldEye
						, l_fMatShininess
						, l_iShadowReceiver
						, FragmentInput( l_v3Position, l_v3Normal )
						, l_output );
				}
				break;
			}

			if ( m_ssao )
			{
				l_v3Ambient *= l_ambientOcclusion;
			}

			pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( l_v3Ambient )
				+ l_writer.Paren( l_v3Diffuse * l_v3MapDiffuse.xyz() )
				+ l_writer.Paren( l_v3Specular * l_v3MapSpecular.xyz() )
				+ l_v3MapEmissive ), 1.0 );

			if ( GetFogType( p_sceneFlags ) != GLSL::FogType::eDisabled )
			{
				auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" )
					, l_writer.Paren( c3d_mtxView * vec4( l_v3Position, 1.0 ) ).xyz() );
				l_fog.ApplyFog( pxl_v4FragColor, length( l_wvPosition ), l_wvPosition.z() );
			}
		} );

		return l_writer.Finalise();
	}

	//************************************************************************************************
}
