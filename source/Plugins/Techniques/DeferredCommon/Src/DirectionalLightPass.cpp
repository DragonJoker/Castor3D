#include "DirectionalLightPass.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred_common
{
	//*********************************************************************************************

	namespace
	{
		static constexpr uint32_t VertexCount = 6u;
	}

	//*********************************************************************************************

	DirectionalLightPass::Program::Program( Engine & p_engine
		, String const & p_vtx
		, String const & p_pxl )
		: LightPass::Program{ p_engine, p_vtx, p_pxl }
		, m_lightIntensity{ m_program->CreateUniform< UniformType::eVec2f >( cuT( "light.m_lightBase.m_intensity" ), ShaderType::ePixel ) }
		, m_lightDirection{ m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_direction" ), ShaderType::ePixel ) }
		, m_lightTransform{ m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_transform" ), ShaderType::ePixel ) }
	{
	}

	DirectionalLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr DirectionalLightPass::Program::DoCreatePipeline( bool p_blend )
	{
		DepthStencilState l_dsstate;
		l_dsstate.SetDepthTest( false );
		l_dsstate.SetDepthMask( WritingMask::eZero );
		BlendState l_blstate;

		if ( p_blend )
		{
			l_blstate.EnableBlend( true );
			l_blstate.SetBlendOp( BlendOperation::eAdd );
			l_blstate.SetSrcBlend( BlendOperand::eOne );
			l_blstate.SetDstBlend( BlendOperand::eOne );
		}

		return m_program->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate )
			, RasteriserState{}
			, std::move( l_blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void DirectionalLightPass::Program::DoBind( Light const & p_light )
	{
		auto & l_light = *p_light.GetDirectionalLight();
		m_lightIntensity->SetValue( l_light.GetIntensity() );
		m_lightDirection->SetValue( l_light.GetDirection() );
		m_lightTransform->SetValue( l_light.GetLightSpaceTransform() );
	}

	//*********************************************************************************************

	DirectionalLightPass::DirectionalLightPass( Engine & p_engine
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach
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
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_viewport.Initialise();
	}

	DirectionalLightPass::~DirectionalLightPass()
	{
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_viewport.Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
	}

	void DirectionalLightPass::Initialise( Scene const & p_scene
		, SceneUbo & p_sceneUbo )
	{
		DoInitialise( p_scene
			, LightType::eDirectional
			, *m_vertexBuffer
			, nullptr
			, p_sceneUbo
			, nullptr );
		m_viewport.Update();
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
		m_matrixUbo.Update( p_camera.GetView(), m_viewport.GetProjection() );
	}

	String DirectionalLightPass::DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		UBO_GPINFO( l_writer );
		auto vertex = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * vec4( vertex, 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	LightPass::ProgramPtr DirectionalLightPass::DoCreateProgram( String const & p_vtx
		, String const & p_pxl )const
	{
		return std::make_unique< Program >( m_engine, p_vtx, p_pxl );
	}
}
