#include "SpotLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	//*********************************************************************************************

	namespace
	{
		static constexpr uint32_t VertexCount = 6u;

		float DoCalcSpotLightBCone( const Castor3D::SpotLight & p_light )
		{
			float l_maxChannel = fmax( fmax( p_light.GetColour()[0], p_light.GetColour()[1] )
				, p_light.GetColour()[2] );

			auto & l_const = p_light.GetAttenuation()[0];
			auto & l_linear = p_light.GetAttenuation()[1];
			auto & l_quadr = p_light.GetAttenuation()[1];

			return ( -l_linear + sqrtf( l_linear * l_linear -
					4 * l_quadr * ( l_const - 256.0f * l_maxChannel * p_light.GetDiffuseIntensity() ) ) )
				/ ( 2 * l_quadr );
		}
	}

	//*********************************************************************************************
	
	void SpotLightPass::Program::Create( Scene const & p_scene
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo
		, UniformBuffer & p_modelMatrixUbo
		, String const & p_vtx
		, String const & p_pxl
		, uint16_t p_fogType )
	{
		DoCreate( p_scene, p_matrixUbo, p_sceneUbo, p_vtx, p_pxl, p_fogType );
		m_viewUniform = p_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxView );
		m_modelUniform = p_modelMatrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxModel );
		m_lightDirection = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Direction" ), ShaderType::ePixel );
		m_lightTransform = m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_mtxLightSpace" ), ShaderType::ePixel );
		m_lightPosition = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Position" ), ShaderType::ePixel );
		m_lightAttenuation = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Attenuation" ), ShaderType::ePixel );
		m_lightExponent = m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_fExponent" ), ShaderType::ePixel );
		m_lightCutOff = m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_fCutOff" ), ShaderType::ePixel );

		DepthStencilState l_dsstate1;
		l_dsstate1.SetDepthTest( false );
		l_dsstate1.SetDepthMask( WritingMask::eZero );
		m_firstPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate1 )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );

		DepthStencilState l_dsstate2;
		l_dsstate2.SetDepthTest( false );
		l_dsstate2.SetDepthMask( WritingMask::eZero );
		BlendState l_blstate;
		l_blstate.EnableBlend( true );
		l_blstate.SetRgbBlendOp( BlendOperation::eAdd );
		l_blstate.SetRgbSrcBlend( BlendOperand::eOne );
		l_blstate.SetRgbDstBlend( BlendOperand::eOne );
		m_blendPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate2 )
			, RasteriserState{}
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void SpotLightPass::Program::Destroy()
	{
		m_lightDirection = nullptr;
		m_lightTransform = nullptr;
		m_lightAttenuation = nullptr;
		m_lightPosition = nullptr;
		m_lightExponent = nullptr;
		m_lightCutOff = nullptr;
		m_modelUniform = nullptr;
		m_viewUniform = nullptr;
		DoDestroy();
	}

	void SpotLightPass::Program::Initialise( VertexBuffer & p_vbo
		, IndexBufferSPtr p_ibo
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo
		, UniformBuffer & p_modelMatrixUbo )
	{
		DoInitialise( p_matrixUbo, p_sceneUbo );
		m_geometryBuffers = m_program->GetRenderSystem()->CreateGeometryBuffers( Topology::eTriangles, *m_program );
		m_geometryBuffers->Initialise( { p_vbo }, p_ibo );
		m_firstPipeline->AddUniformBuffer( p_modelMatrixUbo );
		m_blendPipeline->AddUniformBuffer( p_modelMatrixUbo );
	}

	void SpotLightPass::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		DoCleanup();
	}

	void SpotLightPass::Program::Render( Size const & p_size
		, Castor3D::SpotLight const & p_light
		, Matrix4x4r const & p_projection
		, Matrix4x4r const & p_view
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo
		, UniformBuffer & p_modelMatrixUbo
		, bool p_first )
	{
		DoBind( p_size, p_light, p_projection, p_first );
		m_viewUniform->SetValue( p_view );
		auto l_position = p_light.GetLight().GetParent()->GetDerivedPosition();
		auto l_scale = DoCalcSpotLightBCone( p_light );
		Matrix4x4r l_model{ 1.0f };
		matrix::set_transform( l_model, l_position, Point3f{ l_scale, l_scale, l_scale }, Quaternion::identity() );
		m_modelUniform->SetValue( l_model );
		m_lightAttenuation->SetValue( p_light.GetAttenuation() );
		m_lightPosition->SetValue( p_light.GetLight().GetParent()->GetDerivedPosition() );
		m_lightExponent->SetValue( p_light.GetExponent() );
		m_lightCutOff->SetValue( p_light.GetCutOff().cos() );
		m_lightDirection->SetValue( p_light.GetDirection() );
		m_lightTransform->SetValue( p_light.GetLightSpaceTransform() );
		m_currentPipeline->Apply();
		p_matrixUbo.Update();
		p_sceneUbo.Update();
		p_modelMatrixUbo.Update();
		m_geometryBuffers->Draw( VertexCount, 0 );
	}

	//*********************************************************************************************

	SpotLightPass::SpotLightPass( Engine & p_engine
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
		: LightPass{ p_engine, p_matrixUbo, p_sceneUbo }
		, m_modelMatrixUbo{ ShaderProgram::BufferModelMatrix, *p_engine.GetRenderSystem() }
	{
		UniformBuffer::FillModelMatrixBuffer( m_modelMatrixUbo );
	}

	void SpotLightPass::Create( Castor3D::Scene const & p_scene )
	{
		uint16_t l_fogType{ 0u };

		for ( auto & l_program : m_programs )
		{
			SceneFlags l_sceneFlags{ p_scene.GetFlags() };
			RemFlag( l_sceneFlags, SceneFlag::eFogSquaredExponential );
			AddFlag( l_sceneFlags, SceneFlag( l_fogType ) );
			l_program.Create( p_scene
				, m_matrixUbo
				, m_sceneUbo
				, m_modelMatrixUbo
				, DoGetVertexShaderSource( l_sceneFlags )
				, DoGetPixelShaderSource( l_sceneFlags )
				, l_fogType++ );
		}

		auto l_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
		} );

		real l_data[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, l_declaration );
		m_vertexBuffer->Resize( sizeof( l_data ) );
		std::memcpy( m_vertexBuffer->data(), l_data, sizeof( l_data ) );
		
		//m_indexBuffer = std::make_shared< IndexBuffer >( m_engine );
	}

	void SpotLightPass::Destroy()
	{
		m_indexBuffer.reset();
		m_vertexBuffer.reset();

		for ( auto & l_program : m_programs )
		{
			l_program.Destroy();
		}
	}

	void SpotLightPass::Initialise()
	{
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		//m_indexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		for ( auto & l_program : m_programs )
		{
			l_program.Initialise( *m_vertexBuffer
				, m_indexBuffer
				, m_matrixUbo
				, m_sceneUbo
				, m_modelMatrixUbo );
		}
	}

	void SpotLightPass::Cleanup()
	{
		for ( auto & l_program : m_programs )
		{
			l_program.Cleanup();
		}

		//m_indexBuffer->Cleanup();
		m_vertexBuffer->Cleanup();
		m_modelMatrixUbo.Cleanup();
	}

	void SpotLightPass::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::SpotLight const & p_light
		, Castor3D::Camera const & p_camera
		, uint16_t p_fogType
		, bool p_first )
	{
		DoBeginRender( p_size, p_gp );
		auto & l_program = m_programs[uint16_t( GetFogType( p_fogType ) )];
		l_program.Render( p_size
			, p_light
			, p_camera.GetViewport().GetProjection()
			, p_camera.GetView()
			, m_matrixUbo
			, m_sceneUbo
			, m_modelMatrixUbo
			, p_first );
		DoEndRender( p_gp );
	}
	
	String SpotLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		auto vertex = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex.x(), vertex.y(), 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String SpotLightPass::DoGetPixelShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

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

	//*********************************************************************************************
}
