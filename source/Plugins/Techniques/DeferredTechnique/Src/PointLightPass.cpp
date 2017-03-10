#include "PointLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light/PointLight.hpp>
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
		float DoCalcPointLightBSphere( const Castor3D::PointLight & p_light )
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

	void PointLightPass::Program::Create( Scene const & p_scene
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo
		, UniformBuffer & p_modelMatrixUbo
		, String const & p_vtx
		, String const & p_pxl
		, uint16_t p_fogType )
	{
		DoCreate( p_scene, p_matrixUbo, p_sceneUbo, p_vtx, p_pxl, p_fogType );
		m_viewUniform = p_matrixUbo.GetUniform< UniformType::eMat4x4f > (RenderPipeline::MtxView);
		m_modelUniform = p_modelMatrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxModel );
		m_lightPosition = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Position" ), ShaderType::ePixel );
		m_lightAttenuation = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Attenuation" ), ShaderType::ePixel );

		DepthStencilState l_dsstate1;
		l_dsstate1.SetDepthTest( false );
		l_dsstate1.SetDepthMask( WritingMask::eZero );
		//l_dsstate1.SetStencilTest( true );
		//l_dsstate1.SetStencilFrontFunc( StencilFunc::eAlways );
		//l_dsstate1.SetStencilBackFunc( StencilFunc::eAlways );
		//l_dsstate1.SetStencilBackFailOp( StencilOp::eKeep );
		//l_dsstate1.SetStencilBackDepthFailOp( StencilOp::eIncrWrap );
		//l_dsstate1.SetStencilBackPassOp( StencilOp::eKeep );
		//l_dsstate1.SetStencilFrontFailOp( StencilOp::eKeep );
		//l_dsstate1.SetStencilFrontDepthFailOp( StencilOp::eDecrWrap );
		//l_dsstate1.SetStencilFrontPassOp( StencilOp::eKeep );
		RasteriserState l_rsstate1;
		//l_rsstate1.SetCulledFaces( Culling::eNone );
		m_firstPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate1 )
			, std::move( l_rsstate1 )
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );

		DepthStencilState l_dsstate2;
		l_dsstate2.SetDepthTest( false );
		l_dsstate2.SetDepthMask( WritingMask::eZero );
		//l_dsstate2.SetStencilTest( true );
		//l_dsstate2.SetStencilFrontFunc( StencilFunc::eAlways );
		//l_dsstate2.SetStencilBackFunc( StencilFunc::eAlways );
		//l_dsstate2.SetStencilBackFailOp( StencilOp::eKeep );
		//l_dsstate2.SetStencilBackDepthFailOp( StencilOp::eIncrWrap );
		//l_dsstate2.SetStencilBackPassOp( StencilOp::eKeep );
		//l_dsstate2.SetStencilFrontFailOp( StencilOp::eKeep );
		//l_dsstate2.SetStencilFrontDepthFailOp( StencilOp::eDecrWrap );
		//l_dsstate2.SetStencilFrontPassOp( StencilOp::eKeep );
		RasteriserState l_rsstate2;
		//l_rsstate2.SetCulledFaces( Culling::eNone );
		BlendState l_blstate;
		l_blstate.EnableBlend( true );
		l_blstate.SetRgbBlendOp( BlendOperation::eAdd );
		l_blstate.SetRgbSrcBlend( BlendOperand::eOne );
		l_blstate.SetRgbDstBlend( BlendOperand::eOne );
		m_blendPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate2 )
			, std::move( l_rsstate2 )
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void PointLightPass::Program::Destroy()
	{
		m_lightAttenuation = nullptr;
		m_lightPosition = nullptr;
		m_modelUniform = nullptr;
		m_viewUniform = nullptr;
		DoDestroy();
	}

	void PointLightPass::Program::Initialise( VertexBuffer & p_vbo
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

	void PointLightPass::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		DoCleanup();
	}

	void PointLightPass::Program::Render( Size const & p_size
		, Castor3D::PointLight const & p_light
		, Matrix4x4r const & p_projection
		, Matrix4x4r const & p_view
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo
		, UniformBuffer & p_modelMatrixUbo
		, uint32_t p_count
		, bool p_first )
	{
		DoBind( p_size, p_light, p_projection, p_first );
		m_viewUniform->SetValue( p_view );
		auto l_position = p_light.GetLight().GetParent()->GetDerivedPosition();
		auto l_scale = DoCalcPointLightBSphere( p_light );
		Matrix4x4r l_model{ 1.0f };
		matrix::set_transform( l_model, l_position, Point3f{ l_scale, l_scale, l_scale }, Quaternion::identity() );
		m_modelUniform->SetValue( l_model );
		m_lightAttenuation->SetValue( p_light.GetAttenuation() );
		m_lightPosition->SetValue( p_light.GetLight().GetParent()->GetDerivedPosition() );
		m_currentPipeline->Apply();
		p_matrixUbo.Update();
		p_sceneUbo.Update();
		p_modelMatrixUbo.Update();
		m_geometryBuffers->Draw( p_count, 0 );
	}

	//*********************************************************************************************

	PointLightPass::PointLightPass( Engine & p_engine
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
		: LightPass{ p_engine, p_matrixUbo, p_sceneUbo }
		, m_modelMatrixUbo{ ShaderProgram::BufferModelMatrix, *p_engine.GetRenderSystem() }
	{
		UniformBuffer::FillModelMatrixBuffer( m_modelMatrixUbo );
	}

	void PointLightPass::Create( Castor3D::Scene const & p_scene )
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
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
		} );
		
		uint32_t l_faceCount = 20u;
		Angle l_angle = Angle::from_degrees( 360.0f / l_faceCount );
		std::vector< Point2f > l_arc{ l_faceCount + 1 };
		Angle l_alpha;
		std::vector< Point3f > l_data;
		std::vector< uint32_t > l_faces;
		l_data.reserve( l_faceCount * l_faceCount * 4 );
		l_faces.reserve( l_faceCount * l_faceCount * 6 );

		for ( uint32_t i = 0; i <= l_faceCount; i++ )
		{
			float l_x = +l_alpha.sin();
			float l_y = -l_alpha.cos();
			l_arc[i][0] = l_x;
			l_arc[i][1] = l_y;
			l_alpha += l_angle / 2;
		}

		Angle l_iAlpha;
		Point3f l_pos;

		for ( uint32_t k = 0; k < l_faceCount; ++k )
		{
			auto l_ptT = l_arc[k + 0];
			auto l_ptB = l_arc[k + 1];

			if ( k == 0 )
			{
				// Calcul de la position des points du haut
				for ( uint32_t i = 0; i <= l_faceCount; l_iAlpha += l_angle, ++i )
				{
					auto l_cos = l_iAlpha.cos();
					auto l_sin = l_iAlpha.sin();
					l_data.emplace_back( l_ptT[0] * l_cos, l_ptT[1], l_ptT[0] * l_sin );
				}
			}

			// Calcul de la position des points
			l_iAlpha = 0.0_radians;

			for ( uint32_t i = 0; i <= l_faceCount; l_iAlpha += l_angle, ++i )
			{
				auto l_cos = l_iAlpha.cos();
				auto l_sin = l_iAlpha.sin();
				l_data.emplace_back( l_ptB[0] * l_cos, l_ptB[1], l_ptB[0] * l_sin );
			}
		}

		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, l_declaration );
		auto l_size = l_data.size() * sizeof( *l_data.data() );
		m_vertexBuffer->Resize( uint32_t( l_size ) );
		std::memcpy( m_vertexBuffer->data()
			, l_data.data()->const_ptr()
			, l_size );

		// Reconstition des faces
		uint32_t l_cur = 0;
		uint32_t l_prv = 0;

		for ( uint32_t k = 0; k < l_faceCount; ++k )
		{
			if ( k == 0 )
			{
				for ( uint32_t i = 0; i <= l_faceCount; ++i )
				{
					l_cur++;
				}
			}

			for ( uint32_t i = 0; i < l_faceCount; ++i )
			{
				l_faces.push_back( l_prv + 0 );
				l_faces.push_back( l_cur + 0 );
				l_faces.push_back( l_prv + 1 );
				l_faces.push_back( l_cur + 0 );
				l_faces.push_back( l_cur + 1 );
				l_faces.push_back( l_prv + 1 );
				l_prv++;
				l_cur++;
			}

			l_prv++;
			l_cur++;
		}

		m_indexBuffer = std::make_shared< IndexBuffer >( m_engine );
		m_indexBuffer->Resize( uint32_t( l_faces.size() ) );
		std::memcpy( m_indexBuffer->data()
			, l_faces.data()
			, l_faces.size() * sizeof( *l_faces.data() ) );
	}

	void PointLightPass::Destroy()
	{
		m_indexBuffer.reset();
		m_vertexBuffer.reset();

		for ( auto & l_program : m_programs )
		{
			l_program.Destroy();
		}
	}

	void PointLightPass::Initialise()
	{
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_indexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		for ( auto & l_program : m_programs )
		{
			l_program.Initialise( *m_vertexBuffer
				, m_indexBuffer
				, m_matrixUbo
				, m_sceneUbo
				, m_modelMatrixUbo );
		}
	}

	void PointLightPass::Cleanup()
	{
		for ( auto & l_program : m_programs )
		{
			l_program.Cleanup();
		}

		m_indexBuffer->Cleanup();
		m_vertexBuffer->Cleanup();
		m_modelMatrixUbo.Cleanup();
	}

	void PointLightPass::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::PointLight const & p_light
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
			, m_indexBuffer->GetSize()
			, p_first );
		DoEndRender( p_gp );
	}
	
	String PointLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
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

	String PointLightPass::DoGetPixelShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

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

	//*********************************************************************************************
}
