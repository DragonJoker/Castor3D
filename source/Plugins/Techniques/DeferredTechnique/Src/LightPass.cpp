#include "LightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
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
				cuT( "c3d_mapAmbient" ),
				cuT( "c3d_mapSpecular" ),
				cuT( "c3d_mapEmissive" ),
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

	void LightPass::Program::DoCreate( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl
		, uint16_t p_fogType )
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
	}

	void LightPass::Program::DoDestroy()
	{
		m_firstPipeline->Cleanup();
		m_blendPipeline->Cleanup();
		m_currentPipeline.reset();
		m_firstPipeline.reset();
		m_blendPipeline.reset();
		m_geometryBuffers.reset();
		m_lightColour = nullptr;
		m_lightIntensity = nullptr;
		m_renderSize = nullptr;
		m_program.reset();
	}

	void LightPass::Program::DoInitialise( UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
	{
		m_program->Initialise();
		m_firstPipeline->AddUniformBuffer( p_matrixUbo );
		m_firstPipeline->AddUniformBuffer( p_sceneUbo );
		m_blendPipeline->AddUniformBuffer( p_matrixUbo );
		m_blendPipeline->AddUniformBuffer( p_sceneUbo );
	}

	void LightPass::Program::DoCleanup()
	{
		m_program->Cleanup();
	}

	void LightPass::Program::DoBind( Size const & p_size
		, Castor3D::LightCategory const & p_light
		, bool p_first )
	{
		m_renderSize->SetValue( Point2f( p_size.width(), p_size.height() ) );
		m_lightColour->SetValue( p_light.GetColour() );
		m_lightIntensity->SetValue( p_light.GetIntensity() );

		if ( p_first )
		{
			m_currentPipeline = m_firstPipeline;
		}
		else
		{
			m_currentPipeline = m_blendPipeline;
		}
	}

	//************************************************************************************************

	LightPass::LightPass( Engine & p_engine )
		: m_engine{ p_engine }
		, m_matrixUbo{ ShaderProgram::BufferMatrix, *p_engine.GetRenderSystem() }
	{
		UniformBuffer::FillMatrixBuffer( m_matrixUbo );
	}

	void LightPass::DoBeginRender( Size const & p_size
		, GeometryPassResult const & p_gp )
	{
		p_gp[size_t( DsTexture::ePosition )]->Bind();
		p_gp[size_t( DsTexture::eDiffuse )]->Bind();
		p_gp[size_t( DsTexture::eNormals )]->Bind();
		p_gp[size_t( DsTexture::eAmbient )]->Bind();
		p_gp[size_t( DsTexture::eSpecular )]->Bind();
		p_gp[size_t( DsTexture::eEmissive )]->Bind();
	}

	void LightPass::DoEndRender( GeometryPassResult const & p_gp )
	{
		p_gp[size_t( DsTexture::eEmissive )]->Unbind();
		p_gp[size_t( DsTexture::eSpecular )]->Unbind();
		p_gp[size_t( DsTexture::eAmbient )]->Unbind();
		p_gp[size_t( DsTexture::eNormals )]->Unbind();
		p_gp[size_t( DsTexture::eDiffuse )]->Unbind();
		p_gp[size_t( DsTexture::ePosition )]->Unbind();
	}

	String LightPass::DoGetPixelShaderSource( SceneFlags const & p_sceneFlags
		, LightType p_type )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_SCENE( l_writer );
		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::ePosition ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormals ) );
		auto c3d_mapAmbient = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eAmbient ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eSpecular ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );
		auto gl_FragCoord = l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

		std::unique_ptr< LightingModel > l_lighting;

		switch ( p_type )
		{
		case LightType::eDirectional:
			{
				l_lighting = l_writer.CreateDirectionalLightingModel( PhongLightingModel::Name
					, ShadowType::eNone );
				auto light = l_writer.GetUniform< GLSL::DirectionalLight >( cuT( "light" ) );
			}
			break;

		case LightType::ePoint:
			{
				l_lighting = l_writer.CreatePointLightingModel( PhongLightingModel::Name
					, ShadowType::eNone );
				auto light = l_writer.GetUniform< GLSL::PointLight >( cuT( "light" ) );
			}
			break;

		case LightType::eSpot:
			{
				l_lighting = l_writer.CreateSpotLightingModel( PhongLightingModel::Name
					, ShadowType::eNone );
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
			auto l_v4Ambient = l_writer.GetLocale( cuT( "l_v4Tangent" ), texture( c3d_mapAmbient, l_texCoord ) );
			auto l_v4Specular = l_writer.GetLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, l_texCoord ) );
			auto l_v4Emissive = l_writer.GetLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, l_texCoord ) );
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), l_v4Normal.xyz() );
			auto l_iShadowReceiver = l_writer.GetLocale( cuT( "l_receiver" ), l_writer.Cast< Int >( l_v4Position.w() ) );
			auto l_v3MapAmbient = l_writer.GetLocale( cuT( "l_v3MapAmbient" ), l_v4Ambient.xyz() );
			auto l_v3MapDiffuse = l_writer.GetLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
			auto l_v3MapSpecular = l_writer.GetLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
			auto l_v3MapEmissive = l_writer.GetLocale( cuT( "l_v3MapEmissive" ), l_v4Emissive.xyz() );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
			auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), l_v4Position.xyz() );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4AmbientLight.xyz() );
			auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), c3d_v3CameraPosition );
			auto l_dist = l_writer.GetLocale( cuT( "l_dist" ), l_v4Diffuse.w() );
			auto l_y = l_writer.GetLocale( cuT( "l_y" ), l_v4Emissive.w() );

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

			pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( l_v3Ambient * l_v3MapAmbient.xyz() )
				+ l_writer.Paren( l_v3Diffuse * l_v3MapDiffuse.xyz() )
				+ l_writer.Paren( l_v3Specular * l_v3MapSpecular.xyz() )
				+ l_v3MapEmissive ), 1.0 );

			if ( GetFogType( p_sceneFlags ) != GLSL::FogType::eDisabled )
			{
				l_fog.ApplyFog( pxl_v4FragColor, l_dist, l_y );
			}
		} );

		return l_writer.Finalise();
	}

	//************************************************************************************************
}
