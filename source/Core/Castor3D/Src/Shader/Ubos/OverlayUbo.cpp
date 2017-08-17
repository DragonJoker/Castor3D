#include "OverlayUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	OverlayUbo::OverlayUbo( Engine & engine )
		: m_ubo{ ShaderProgram::BufferOverlay
			, *engine.getRenderSystem()
			, OverlayUbo::BindingPoint }
		, m_position{ *m_ubo.createUniform< UniformType::eVec2i >( ShaderProgram::OvPosition ) }
		, m_material{ *m_ubo.createUniform< UniformType::eInt >( ShaderProgram::MaterialIndex ) }
	{
	}

	OverlayUbo::~OverlayUbo()
	{
	}

	void OverlayUbo::setPosition( castor::Position const & p_position )
	{
		m_position.setValue( Point2i{ p_position[0], p_position[1] } );
	}

	void OverlayUbo::update( int p_materialIndex )const
	{
		m_material.setValue( p_materialIndex - 1 );
		m_ubo.update();
		m_ubo.bindTo( OverlayUbo::BindingPoint );
	}
}
