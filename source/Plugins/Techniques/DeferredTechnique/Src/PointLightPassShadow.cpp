#include "PointLightPassShadow.hpp"

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
		float DoCalcPointLightBSphere( const Castor3D::PointLight & p_light
			, float p_max )
		{
			return GetMaxDistance( p_light
				, p_light.GetAttenuation()
				, p_max );
		}
	}

	//*********************************************************************************************

	void PointLightPassShadow::Program::Create( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl
		, uint16_t p_fogType )
	{
		DoCreate( p_scene, p_vtx, p_pxl, p_fogType );
		m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowPoint, ShaderType::ePixel )->SetValue( int( DsTexture::eCount ) );
		m_lightPosition = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Position" ), ShaderType::ePixel );
		m_lightAttenuation = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Attenuation" ), ShaderType::ePixel );

		RasteriserState l_rsstate1;
		l_rsstate1.SetCulledFaces( Culling::eFront );
		DepthStencilState l_dsstate1;
		l_dsstate1.SetDepthTest( false );
		l_dsstate1.SetDepthMask( WritingMask::eZero );
		l_dsstate1.SetStencilWriteMask( 0xFFFFFFFFu );
		l_dsstate1.SetStencilBackFunc( StencilFunc::eNEqual );
		l_dsstate1.SetStencilBackRef( 0u );
		l_dsstate1.SetStencilFrontFunc( StencilFunc::eNEqual );
		l_dsstate1.SetStencilFrontRef( 0u );
		m_firstPipeline = m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate1 )
			, std::move( l_rsstate1 )
			, BlendState{}
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );

		RasteriserState l_rsstate2;
		l_rsstate2.SetCulledFaces( Culling::eFront );
		DepthStencilState l_dsstate2;
		l_dsstate2.SetDepthTest( false );
		l_dsstate2.SetDepthMask( WritingMask::eZero );
		l_dsstate2.SetStencilWriteMask( 0xFFFFFFFFu );
		l_dsstate2.SetStencilBackFunc( StencilFunc::eNEqual );
		l_dsstate2.SetStencilBackRef( 0u );
		l_dsstate2.SetStencilFrontFunc( StencilFunc::eNEqual );
		l_dsstate2.SetStencilFrontRef( 0u );
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

	void PointLightPassShadow::Program::Destroy()
	{
		m_lightAttenuation = nullptr;
		m_lightPosition = nullptr;
		DoDestroy();
	}

	void PointLightPassShadow::Program::Initialise( VertexBuffer & p_vbo
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

	void PointLightPassShadow::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		DoCleanup();
	}

	void PointLightPassShadow::Program::Render( Size const & p_size
		, Castor3D::PointLight const & p_light
		, uint32_t p_count
		, bool p_first )
	{
		DoBind( p_size, p_light, p_first );
		m_lightAttenuation->SetValue( p_light.GetAttenuation() );
		m_lightPosition->SetValue( p_light.GetLight().GetParent()->GetDerivedPosition() );
		m_currentPipeline->Apply();
		m_geometryBuffers->Draw( p_count, 0 );
	}

	//*********************************************************************************************

	PointLightPassShadow::PointLightPassShadow( Engine & p_engine )
		: LightPassShadow{ p_engine }
		, m_modelMatrixUbo{ ShaderProgram::BufferModelMatrix, *p_engine.GetRenderSystem() }
		, m_shadowMap{ p_engine }
	{
		UniformBuffer::FillModelMatrixBuffer( m_modelMatrixUbo );
	}

	void PointLightPassShadow::Create( Castor3D::Scene const & p_scene )
	{
		uint16_t l_fogType{ 0u };

		for ( auto & l_program : m_programs )
		{
			SceneFlags l_sceneFlags{ p_scene.GetFlags() };
			RemFlag( l_sceneFlags, SceneFlag::eFogSquaredExponential );
			AddFlag( l_sceneFlags, SceneFlag( l_fogType ) );
			l_program.Create( p_scene
				, DoGetVertexShaderSource( l_sceneFlags )
				, DoGetPixelShaderSource( l_sceneFlags, LightType::ePoint )
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

	void PointLightPassShadow::Destroy()
	{
		m_indexBuffer.reset();
		m_vertexBuffer.reset();

		for ( auto & l_program : m_programs )
		{
			l_program.Destroy();
		}
	}

	void PointLightPassShadow::Initialise( UniformBuffer & p_sceneUbo )
	{
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_indexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		for ( auto & l_program : m_programs )
		{
			l_program.Initialise( *m_vertexBuffer
				, m_indexBuffer
				, m_matrixUbo
				, p_sceneUbo
				, m_modelMatrixUbo );
		}

		m_stencilPass.Initialise( *m_vertexBuffer
			, m_indexBuffer );
		m_projectionUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );
		m_viewUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxView );
		m_modelUniform = m_modelMatrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxModel );
	}

	void PointLightPassShadow::Cleanup()
	{
		m_modelUniform = nullptr;
		m_viewUniform = nullptr;
		m_projectionUniform = nullptr;
		m_stencilPass.Cleanup();

		for ( auto & l_program : m_programs )
		{
			l_program.Cleanup();
		}

		m_indexBuffer->Cleanup();
		m_vertexBuffer->Cleanup();
		m_modelMatrixUbo.Cleanup();
		m_matrixUbo.Cleanup();
	}

	void PointLightPassShadow::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::PointLight const & p_light
		, Castor3D::Camera const & p_camera
		, uint16_t p_fogType
		, bool p_first )
	{
		auto l_lightPos = p_light.GetLight().GetParent()->GetDerivedPosition();
		auto l_camPos = p_camera.GetParent()->GetDerivedPosition();
		auto l_far = p_camera.GetViewport().GetFar();
		auto l_scale = DoCalcPointLightBSphere( p_light
			, float( l_far - point::distance( l_lightPos, l_camPos ) - ( l_far / 50.0f ) ) );
		m_projectionUniform->SetValue( p_camera.GetViewport().GetProjection() );
		m_viewUniform->SetValue( p_camera.GetView() );
		Matrix4x4r l_model{ 1.0f };
		matrix::set_transform( l_model
			, p_light.GetLight().GetParent()->GetDerivedPosition()
			, Point3f{ l_scale, l_scale, l_scale }
		, Quaternion::identity() );
		m_modelUniform->SetValue( l_model );
		m_matrixUbo.Update();
		m_modelMatrixUbo.Update();
		m_stencilPass.Render( m_indexBuffer->GetSize() );
		DoBeginRender( p_size, p_gp );
		auto & l_program = m_programs[uint16_t( GetFogType( p_fogType ) )];
		l_program.Render( p_size
			, p_light
			, m_indexBuffer->GetSize()
			, p_first );
		DoEndRender( p_gp );
	}
	
	String PointLightPassShadow::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		auto vertex = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0 );
		} );

		return l_writer.Finalise();
	}

	//*********************************************************************************************
}
