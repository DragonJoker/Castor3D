#include "OverlayUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	String const OverlayUbo::BufferOverlay = cuT( "Overlay" );
	String const OverlayUbo::Position = cuT( "c3d_position" );
	String const OverlayUbo::RenderSize = cuT( "c3d_renderSize" );
	String const OverlayUbo::RenderRatio = cuT( "c3d_renderRatio" );
	String const OverlayUbo::MaterialIndex = cuT( "c3d_materialIndex" );

	OverlayUbo::OverlayUbo( Engine & engine )
		: m_ubo{ OverlayUbo::BufferOverlay
			, *engine.getRenderSystem()
			, OverlayUbo::BindingPoint }
		, m_position{ *m_ubo.createUniform< UniformType::eVec2f >( OverlayUbo::Position ) }
		, m_size{ *m_ubo.createUniform< UniformType::eVec2i >( OverlayUbo::RenderSize ) }
		, m_ratio{ *m_ubo.createUniform< UniformType::eVec2f >( OverlayUbo::RenderRatio ) }
		, m_material{ *m_ubo.createUniform< UniformType::eInt >( OverlayUbo::MaterialIndex ) }
	{
	}

	OverlayUbo::~OverlayUbo()
	{
	}

	void OverlayUbo::setPosition( castor::Point2d const & p_position
		, castor::Size const & renderSize
		, castor::Point2f const & renderRatio )
	{
		m_position.setValue( Point2f{ p_position[0], p_position[1] } );
		m_size.setValue( Point2i{ renderSize[0], renderSize[1] } );
		m_ratio.setValue( renderRatio );
	}

	void OverlayUbo::update( int p_materialIndex )const
	{
		m_material.setValue( p_materialIndex - 1 );
		m_ubo.update();
		m_ubo.bindTo( OverlayUbo::BindingPoint );
	}
}
