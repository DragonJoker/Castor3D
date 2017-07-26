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

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		static constexpr uint32_t VertexCount = 6u;
	}

	//*********************************************************************************************

	DirectionalLightPass::Program::Program( Engine & engine
		, GLSL::Shader const & vtx
		, GLSL::Shader const & pxl )
		: LightPass::Program{ engine, vtx, pxl }
		, m_lightIntensity{ m_program->CreateUniform< UniformType::eVec2f >( cuT( "light.m_lightBase.m_intensity" ), ShaderType::ePixel ) }
		, m_lightDirection{ m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_direction" ), ShaderType::ePixel ) }
		, m_lightTransform{ m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_transform" ), ShaderType::ePixel ) }
	{
	}

	DirectionalLightPass::Program::~Program()
	{
	}

	RenderPipelineUPtr DirectionalLightPass::Program::DoCreatePipeline( bool blend )
	{
		DepthStencilState dsstate;
		dsstate.SetDepthTest( false );
		dsstate.SetDepthMask( WritingMask::eZero );
		BlendState blstate;

		if ( blend )
		{
			blstate.EnableBlend( true );
			blstate.SetBlendOp( BlendOperation::eAdd );
			blstate.SetSrcBlend( BlendOperand::eOne );
			blstate.SetDstBlend( BlendOperand::eOne );
		}

		return m_program->GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
			, RasteriserState{}
			, std::move( blstate )
			, MultisampleState{}
			, *m_program
			, PipelineFlags{} );
	}

	void DirectionalLightPass::Program::DoBind( Light const & light )
	{
		auto & directionalLight = *light.GetDirectionalLight();
		m_lightIntensity->SetValue( directionalLight.GetIntensity() );
		m_lightDirection->SetValue( directionalLight.GetDirection() );
		m_lightTransform->SetValue( directionalLight.GetLightSpaceTransform() );
	}

	//*********************************************************************************************

	DirectionalLightPass::DirectionalLightPass( Engine & engine
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, GpInfoUbo & gpInfoUbo
		, bool hasShadows )
		: LightPass{ engine, frameBuffer, depthAttach, gpInfoUbo, hasShadows }
		, m_viewport{ engine }
	{
		auto declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
		} );

		real data[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( m_engine, declaration );
		uint32_t stride = declaration.stride();
		m_vertexBuffer->Resize( sizeof( data ) );
		uint8_t * buffer = m_vertexBuffer->GetData();
		std::memcpy( buffer, data, sizeof( data ) );
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

	void DirectionalLightPass::Initialise( Scene const & scene
		, SceneUbo & sceneUbo )
	{
		DoInitialise( scene
			, LightType::eDirectional
			, *m_vertexBuffer
			, nullptr
			, sceneUbo
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

	void DirectionalLightPass::DoUpdate( Size const & size
		, Light const & light
		, Camera const & camera )
	{
		m_viewport.Resize( size );
		m_matrixUbo.Update( camera.GetView(), m_viewport.GetProjection() );
	}

	GLSL::Shader DirectionalLightPass::DoGetVertexShaderSource( SceneFlags const & sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter writer = m_engine.GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( writer );
		UBO_GPINFO( writer );
		auto vertex = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );

		// Shader outputs
		auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

		writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			gl_Position = c3d_mtxProjection * vec4( vertex, 0.0, 1.0 );
		} );

		return writer.Finalise();
	}

	LightPass::ProgramPtr DirectionalLightPass::DoCreateProgram( GLSL::Shader const & vtx
		, GLSL::Shader const & pxl )const
	{
		return std::make_unique< Program >( m_engine, vtx, pxl );
	}
}
