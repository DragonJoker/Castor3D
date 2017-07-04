#include "OverlayUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	OverlayUbo::OverlayUbo( Engine & p_engine )
		: m_ubo{ ShaderProgram::BufferOverlay, *p_engine.GetRenderSystem() }
		, m_position{ *m_ubo.CreateUniform< UniformType::eVec2i >( ShaderProgram::OvPosition ) }
		, m_material{ *m_ubo.CreateUniform< UniformType::eInt >( ShaderProgram::MaterialIndex ) }
	{
	}

	OverlayUbo::~OverlayUbo()
	{
	}

	void OverlayUbo::SetPosition( Castor::Position const & p_position )
	{
		m_position.SetValue( Point2i{ p_position[0], p_position[1] } );
	}

	void OverlayUbo::Update( int p_materialIndex )const
	{
		m_material.SetValue( p_materialIndex - 1 );
		m_ubo.Update();
		m_ubo.BindTo( OverlayUbo::BindingPoint );
	}
}
