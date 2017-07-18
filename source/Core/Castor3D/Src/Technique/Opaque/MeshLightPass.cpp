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

namespace Castor3D
{
	//*********************************************************************************************

	MeshLightPass::Program::Program( Engine & p_engine
		, GLSL::Shader const & p_vtx
		, GLSL::Shader const & p_pxl )
		: LightPass::Program{ p_engine, p_vtx, p_pxl }
		, m_lightIntensity{ m_program->CreateUniform< UniformType::eVec2f >( cuT( "light.m_lightBase.m_intensity" ), ShaderType::ePixel ) }
	{
	}

	MeshLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr MeshLightPass::Program::DoCreatePipeline( bool p_blend )
	{
		RasteriserState rsstate;
		rsstate.SetCulledFaces( Culling::eFront );

		DepthStencilState dsstate;
		dsstate.SetDepthTest( false );
		dsstate.SetDepthMask( WritingMask::eZero );
		dsstate.SetStencilTest( true );
		dsstate.SetStencilFunc( StencilFunc::eNEqual );
		dsstate.SetStencilRef( 0u );

		BlendState blstate;

		if ( p_blend )
		{
			blstate.EnableBlend( true );
			blstate.SetRgbBlendOp( BlendOperation::eAdd );
			blstate.SetRgbSrcBlend( BlendOperand::eOne );
			blstate.SetRgbDstBlend( BlendOperand::eOne );
		}

		return m_program->GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
			, std::move( rsstate )
			, std::move( blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	//*********************************************************************************************

	MeshLightPass::MeshLightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach
		, LightType p_type
		, bool p_shadows )
		: LightPass{ p_engine, p_frameBuffer, p_depthAttach, p_shadows }
		, m_modelMatrixUbo{ p_engine }
		, m_stencilPass{ p_frameBuffer, p_depthAttach, m_matrixUbo, m_modelMatrixUbo }
		, m_type{ p_type }
	{
	}

	MeshLightPass::~MeshLightPass()
	{
		m_stencilPass.Cleanup();
		m_indexBuffer->Cleanup();
		m_vertexBuffer->Cleanup();
		m_modelMatrixUbo.GetUbo().Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
		m_indexBuffer.reset();
		m_vertexBuffer.reset();
	}

	void MeshLightPass::Initialise( Scene const & p_scene
		, SceneUbo & p_sceneUbo )
	{
		auto declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
		} );

		auto data = DoGenerateVertices();
		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, declaration );
		auto size = data.size() * sizeof( *data.data() );
		m_vertexBuffer->Resize( uint32_t( size ) );
		std::memcpy( m_vertexBuffer->GetData()
			, data.data()->const_ptr()
			, size );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );

		auto faces = DoGenerateFaces();
		m_indexBuffer = std::make_shared< IndexBuffer >( m_engine );
		m_indexBuffer->Resize( uint32_t( faces.size() ) );
		std::memcpy( m_indexBuffer->GetData()
			, faces.data()
			, faces.size() * sizeof( *faces.data() ) );
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
		auto model = DoComputeModelMatrix( p_light, p_camera );
		m_matrixUbo.Update( p_camera.GetView(), p_camera.GetViewport().GetProjection() );
		m_modelMatrixUbo.Update( model );
		p_camera.Apply();
		m_stencilPass.Render( m_indexBuffer->GetSize() );
	}
	
	GLSL::Shader MeshLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_MODEL_MATRIX( writer );
		UBO_GPINFO( writer );
		auto vertex = writer.DeclAttribute< Vec3 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * c3d_mtxView * c3d_mtxModel * vec4( vertex, 1.0_f );
		} );

		return writer.Finalise();
	}

	//*********************************************************************************************
}
