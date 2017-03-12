#include "SpotLightPassShadow.hpp"

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
		Point2f DoCalcSpotLightBCone( const Castor3D::SpotLight & p_light
			, float p_max )
		{
			auto l_length = GetMaxDistance( p_light
				, p_light.GetAttenuation()
				, p_max );
			auto l_width = p_light.GetCutOff().radians() / Angle::from_degrees( 22.5f ).radians();
			return Point2f{ l_length * l_width, l_length };
		}
	}

	//*********************************************************************************************
	
	void SpotLightPassShadow::Program::Create( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl
		, uint16_t p_fogType )
	{
		DoCreate( p_scene, p_vtx, p_pxl, p_fogType );
		m_program->CreateUniform< UniformType::eSampler >( GLSL::Shadow::MapShadowSpot, ShaderType::ePixel )->SetValue( int( DsTexture::eCount ) );
		m_lightDirection = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Direction" ), ShaderType::ePixel );
		m_lightTransform = m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_mtxLightSpace" ), ShaderType::ePixel );
		m_lightPosition = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Position" ), ShaderType::ePixel );
		m_lightAttenuation = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Attenuation" ), ShaderType::ePixel );
		m_lightExponent = m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_fExponent" ), ShaderType::ePixel );
		m_lightCutOff = m_program->CreateUniform< UniformType::eFloat >( cuT( "light.m_fCutOff" ), ShaderType::ePixel );

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

	void SpotLightPassShadow::Program::Destroy()
	{
		m_lightDirection = nullptr;
		m_lightTransform = nullptr;
		m_lightAttenuation = nullptr;
		m_lightPosition = nullptr;
		m_lightExponent = nullptr;
		m_lightCutOff = nullptr;
		DoDestroy();
	}

	void SpotLightPassShadow::Program::Initialise( VertexBuffer & p_vbo
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

	void SpotLightPassShadow::Program::Cleanup()
	{
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		DoCleanup();
	}

	void SpotLightPassShadow::Program::Render( Size const & p_size
		, Castor3D::SpotLight const & p_light
		, uint32_t p_count
		, bool p_first )
	{
		DoBind( p_size, p_light, p_first );
		m_lightAttenuation->SetValue( p_light.GetAttenuation() );
		m_lightPosition->SetValue( p_light.GetLight().GetParent()->GetDerivedPosition() );
		m_lightExponent->SetValue( p_light.GetExponent() );
		m_lightCutOff->SetValue( p_light.GetCutOff().cos() );
		m_lightDirection->SetValue( p_light.GetDirection() );
		m_lightTransform->SetValue( p_light.GetLightSpaceTransform() );
		m_currentPipeline->Apply();
		m_geometryBuffers->Draw( p_count, 0 );
	}

	//*********************************************************************************************

	SpotLightPassShadow::SpotLightPassShadow( Engine & p_engine )
		: LightPassShadow{ p_engine }
		, m_modelMatrixUbo{ ShaderProgram::BufferModelMatrix, *p_engine.GetRenderSystem() }
		, m_shadowMap{ p_engine }
	{
		UniformBuffer::FillModelMatrixBuffer( m_modelMatrixUbo );
	}

	void SpotLightPassShadow::Create( Castor3D::Scene const & p_scene )
	{
		uint16_t l_fogType{ 0u };

		for ( auto & l_program : m_programs )
		{
			SceneFlags l_sceneFlags{ p_scene.GetFlags() };
			RemFlag( l_sceneFlags, SceneFlag::eFogSquaredExponential );
			AddFlag( l_sceneFlags, SceneFlag( l_fogType ) );
			l_program.Create( p_scene
				, DoGetVertexShaderSource( l_sceneFlags )
				, DoGetPixelShaderSource( l_sceneFlags, LightType::eSpot )
				, l_fogType++ );
		}

		auto l_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
		} );

		uint32_t l_nbFaces = 8;
		Point3fArray l_data;
		Angle l_alpha;
		auto l_angle = Angle::from_degrees( 360.0f / l_nbFaces );

		l_data.emplace_back( 0.0f, 0.0f, 0.0f );
		l_data.emplace_back( 0.0f, 0.0f, 1.0f );

		for ( auto i = 0u; i < l_nbFaces; l_alpha += l_angle, ++i )
		{
			l_data.emplace_back( l_alpha.cos() / 2.0f, l_alpha.sin() / 2.0f, 1.0f );
		}

		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, l_declaration );
		auto l_size = l_data.size() * sizeof( *l_data.data() );
		m_vertexBuffer->Resize( uint32_t( l_size ) );
		std::memcpy( m_vertexBuffer->data()
			, l_data.data()->const_ptr()
			, l_size );

		UIntArray l_faces;

		for ( auto i = 0u; i < l_nbFaces - 1; i++ )
		{
			// Side
			l_faces.push_back( 0u );
			l_faces.push_back( i + 3u );
			l_faces.push_back( i + 2u );
			// Base
			l_faces.push_back( 1u );
			l_faces.push_back( i + 2u );
			l_faces.push_back( i + 3u );
		}

		// Side last face
		l_faces.push_back( 0u );
		l_faces.push_back( 2u );
		l_faces.push_back( l_nbFaces + 1 );
		// Base last face
		l_faces.push_back( 1u );
		l_faces.push_back( l_nbFaces + 1 );
		l_faces.push_back( 2u );

		m_indexBuffer = std::make_shared< IndexBuffer >( m_engine );
		m_indexBuffer->Resize( uint32_t( l_faces.size() ) );
		std::memcpy( m_indexBuffer->data()
			, l_faces.data()
			, l_faces.size() * sizeof( *l_faces.data() ) );
	}

	void SpotLightPassShadow::Destroy()
	{
		m_indexBuffer.reset();
		m_vertexBuffer.reset();

		for ( auto & l_program : m_programs )
		{
			l_program.Destroy();
		}
	}

	void SpotLightPassShadow::Initialise( Castor3D::UniformBuffer & p_sceneUbo )
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

	void SpotLightPassShadow::Cleanup()
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

	void SpotLightPassShadow::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::SpotLight const & p_light
		, Castor3D::Camera const & p_camera
		, uint16_t p_fogType
		, bool p_first )
	{
		m_projectionUniform->SetValue( p_camera.GetViewport().GetProjection() );
		m_viewUniform->SetValue( p_camera.GetView() );
		auto l_lightPos = p_light.GetLight().GetParent()->GetDerivedPosition();
		auto l_camPos = p_camera.GetParent()->GetDerivedPosition();
		auto l_far = p_camera.GetViewport().GetFar();
		auto l_scale = DoCalcSpotLightBCone( p_light
			, float( l_far - point::distance( l_lightPos, l_camPos ) - ( l_far / 50.0f ) ) );
		Matrix4x4r l_model{ 1.0f };
		matrix::set_transform( l_model
			, p_light.GetLight().GetParent()->GetDerivedPosition()
			, Point3f{ l_scale[0], l_scale[0], l_scale[1] }
			, p_light.GetLight().GetParent()->GetDerivedOrientation() );
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
	
	String SpotLightPassShadow::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
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
