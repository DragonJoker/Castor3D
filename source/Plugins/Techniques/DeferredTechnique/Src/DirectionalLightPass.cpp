#include "DirectionalLightPass.hpp"

#include <Engine.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/DirectionalLight.hpp>
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
	}

	//*********************************************************************************************

	DirectionalLightPass::Program::Program( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl )
		: LightPass::Program{ p_scene, p_vtx, p_pxl }
	{
		m_lightDirection = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Direction" ), ShaderType::ePixel );
		m_lightTransform = m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_mtxLightSpace" ), ShaderType::ePixel );

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

	DirectionalLightPass::Program::~Program()
	{
		m_lightDirection = nullptr;
		m_lightTransform = nullptr;
	}

	void DirectionalLightPass::Program::DoBind( Light const & p_light )
	{
		auto & l_light = *p_light.GetDirectionalLight();
		m_lightDirection->SetValue( l_light.GetDirection() );
		m_lightTransform->SetValue( l_light.GetLightSpaceTransform() );
	}

	//*********************************************************************************************

	DirectionalLightPass::DirectionalLightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, RenderBufferAttachment & p_depthAttach
		, bool p_shadows )
		: LightPass{ p_engine, p_frameBuffer, p_depthAttach, p_shadows }
		, m_viewport{ p_engine }
	{
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
		uint32_t l_stride = l_declaration.stride();
		m_vertexBuffer->Resize( sizeof( l_data ) );
		uint8_t * l_buffer = m_vertexBuffer->data();
		std::memcpy( l_buffer, l_data, sizeof( l_data ) );
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_projectionUniform = m_matrixUbo.GetUniform< UniformType::eMat4x4f >( RenderPipeline::MtxProjection );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_viewport.Initialise();
	}

	DirectionalLightPass::~DirectionalLightPass()
	{
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_viewport.Cleanup();
		m_projectionUniform = nullptr;
		m_matrixUbo.Cleanup();
	}

	void DirectionalLightPass::Initialise( Scene const & p_scene
		, UniformBuffer & p_sceneUbo )
	{
		DoInitialise( p_scene
			, LightType::eDirectional
			, *m_vertexBuffer
			, nullptr
			, p_sceneUbo
			, nullptr );
	}

	void DirectionalLightPass::Cleanup()
	{
		DoCleanup();
	}

	uint32_t DirectionalLightPass::GetCount()const
	{
		return VertexCount;
	}

	void DirectionalLightPass::DoUpdate( Size const & p_size
		, Light const & p_light
		, Camera const & p_camera )
	{
		m_viewport.Resize( p_size );
		m_viewport.Update();
		m_projectionUniform->SetValue( m_viewport.GetProjection() );
		m_matrixUbo.Update();
	}

	String DirectionalLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

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

	LightPass::ProgramPtr DirectionalLightPass::DoCreateProgram( Scene const & p_scene
		, String const & p_vtx
		, String const & p_pxl )const
	{
		return std::make_unique< Program >( p_scene, p_vtx, p_pxl );
	}
}
