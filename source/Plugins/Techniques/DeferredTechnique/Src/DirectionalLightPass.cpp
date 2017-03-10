#include "DirectionalLightPass.hpp"

#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	namespace
	{
		static constexpr uint32_t VertexCount = 6u;
	}

	DirectionalLightPass::DirectionalLightPass( Engine & p_engine
		, UniformBuffer & p_matrixUbo
		, UniformBuffer & p_sceneUbo )
		: LightPass{ p_engine, p_matrixUbo, p_sceneUbo }
	{
	}

	void DirectionalLightPass::Create( Castor3D::ShaderProgramSPtr p_program
		, Scene const & p_scene )
	{
		DoCreate( p_program, p_scene );
		m_lightDirection = m_program->CreateUniform< UniformType::eVec3f >( cuT( "light.m_v3Direction" ), ShaderType::ePixel );
		m_lightTransform = m_program->CreateUniform< UniformType::eMat4x4f >( cuT( "light.m_mtxLightSpace" ), ShaderType::ePixel );

		m_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
			BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 ),
		} );

		real l_data[] =
		{
			0, 0, 0, 0,
			1, 1, 1, 1,
			0, 1, 0, 1,
			0, 0, 0, 0,
			1, 0, 1, 0,
			1, 1, 1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( *p_program->GetRenderSystem()->GetEngine(), m_declaration );
		uint32_t l_stride = m_declaration.stride();
		m_vertexBuffer->Resize( sizeof( l_data ) );
		uint8_t * l_buffer = m_vertexBuffer->data();
		std::memcpy( l_buffer, l_data, sizeof( l_data ) );
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
	}

	void DirectionalLightPass::Destroy()
	{
		m_vertexBuffer.reset();
		m_lightDirection = nullptr;
		m_lightTransform = nullptr;
		DoDestroy();
	}

	void DirectionalLightPass::Initialise()
	{
		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		DoInitialise( *m_vertexBuffer );
	}

	void DirectionalLightPass::Cleanup()
	{
		DoCleanup();
		m_vertexBuffer->Cleanup();
	}

	void DirectionalLightPass::Render( Size const & p_size
		, GeometryPassResult const & p_gp
		, Castor3D::DirectionalLight const & p_light
		, bool p_first )
	{
		DoBeginRender( p_size, p_gp, p_light, p_first );
		m_lightDirection->SetValue( p_light.GetDirection() );
		m_lightTransform->SetValue( p_light.GetLightSpaceTransform() );
		m_currentPipeline->Apply();
		m_geometryBuffers->Draw( VertexCount, 0 );
		DoEndRender( p_gp, p_light );
	}
}
