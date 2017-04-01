#include "MeshLightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
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

namespace deferred_common
{
	//*********************************************************************************************

	MeshLightPass::Program::Program( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl
		, bool p_ssao )
		: LightPass::Program{ p_scene, p_vtx, p_pxl, p_ssao }
	{
	}

	MeshLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr MeshLightPass::Program::DoCreatePipeline( bool p_blend )
	{
		RasteriserState l_rsstate;
		l_rsstate.SetCulledFaces( Culling::eFront );

		DepthStencilState l_dsstate;
		l_dsstate.SetDepthTest( false );
		l_dsstate.SetDepthMask( WritingMask::eZero );
		l_dsstate.SetStencilTest( true );
		l_dsstate.SetStencilFunc( StencilFunc::eNEqual );
		l_dsstate.SetStencilRef( 0u );

		BlendState l_blstate;

		if ( p_blend )
		{
			l_blstate.EnableBlend( true );
			l_blstate.SetRgbBlendOp( BlendOperation::eAdd );
			l_blstate.SetRgbSrcBlend( BlendOperand::eOne );
			l_blstate.SetRgbDstBlend( BlendOperand::eOne );
		}

		return m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
			, std::move( l_rsstate )
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	//*********************************************************************************************

	MeshLightPass::MeshLightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach
		, LightType p_type
		, bool p_ssao
		, bool p_shadows )
		: LightPass{ p_engine, p_frameBuffer, p_depthAttach, p_ssao, p_shadows }
		, m_modelMatrixUbo{ ShaderProgram::BufferModelMatrix, *p_engine.GetRenderSystem() }
		, m_stencilPass{ p_frameBuffer, p_depthAttach, m_matrixUbo, m_modelMatrixUbo }
		, m_type{ p_type }
	{
		UniformBuffer::FillModelMatrixBuffer( m_modelMatrixUbo );
		m_modelUniform = m_modelMatrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxModel );
	}

	MeshLightPass::~MeshLightPass()
	{
		m_modelUniform = nullptr;
		m_viewUniform = nullptr;
		m_projectionUniform = nullptr;
		m_stencilPass.Cleanup();
		m_indexBuffer->Cleanup();
		m_vertexBuffer->Cleanup();
		m_modelMatrixUbo.Cleanup();
		m_matrixUbo.Cleanup();
		m_indexBuffer.reset();
		m_vertexBuffer.reset();
	}

	void MeshLightPass::Initialise( Scene const & p_scene
		, UniformBuffer & p_sceneUbo )
	{
		auto l_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
		} );

		auto l_data = DoGenerateVertices();
		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, l_declaration );
		auto l_size = l_data.size() * sizeof( *l_data.data() );
		m_vertexBuffer->Resize( uint32_t( l_size ) );
		std::memcpy( m_vertexBuffer->data()
			, l_data.data()->const_ptr()
			, l_size );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		auto l_faces = DoGenerateFaces();
		m_indexBuffer = std::make_shared< IndexBuffer >( m_engine );
		m_indexBuffer->Resize( uint32_t( l_faces.size() ) );
		std::memcpy( m_indexBuffer->data()
			, l_faces.data()
			, l_faces.size() * sizeof( *l_faces.data() ) );
		m_indexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		m_stencilPass.Initialise( *m_vertexBuffer
			, m_indexBuffer );

		DoInitialise( p_scene
			, m_type
			, *m_vertexBuffer
			, m_indexBuffer
			, p_sceneUbo
			, &m_modelMatrixUbo );
	}

	void MeshLightPass::Cleanup()
	{
		DoCleanup();
	}

	uint32_t MeshLightPass::GetCount()const
	{
		return m_indexBuffer->GetSize();
	}

	void MeshLightPass::DoUpdate( Size const & p_size
		, Light const & p_light
		, Camera const & p_camera )
	{
		auto l_model = DoComputeModelMatrix( p_light, p_camera );
		m_projectionUniform->SetValue( p_camera.GetViewport().GetProjection() );
		m_viewUniform->SetValue( p_camera.GetView() );
		m_modelUniform->SetValue( l_model );
		m_matrixUbo.Update();
		m_modelMatrixUbo.Update();
		p_camera.Apply();
		m_stencilPass.Render( m_indexBuffer->GetSize() );
	}
	
	String MeshLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_MODEL_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_GPINFO( l_writer );
		auto vertex = l_writer.GetAttribute< Vec3 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0_f );
		} );

		return l_writer.Finalise();
	}

	//*********************************************************************************************
}
