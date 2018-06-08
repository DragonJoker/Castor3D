#include "OverlayUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	uint32_t const OverlayUbo::BindingPoint = 2u;
	String const OverlayUbo::BufferOverlayName = cuT( "Overlay" );
	String const OverlayUbo::BufferOverlayInstance = cuT( "overlay" );
	String const OverlayUbo::Position = cuT( "c3d_position" );
	String const OverlayUbo::RenderSize = cuT( "c3d_renderSize" );
	String const OverlayUbo::RenderRatio = cuT( "c3d_renderRatio" );
	String const OverlayUbo::MaterialIndex = cuT( "c3d_materialIndex" );

	OverlayUbo::OverlayUbo( Engine & engine )
		: m_engine{ engine }
		, m_ubo{ renderer::makeUniformBuffer< Configuration >( getCurrentDevice( engine )
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible ) }
	{
	}

	OverlayUbo::~OverlayUbo()
	{
	}

	void OverlayUbo::setPosition( castor::Point2d const & position
		, castor::Size const & renderSize
		, castor::Point2f const & renderRatio )
	{
		m_ubo->getData( 0u ).position = Point2f{ position };
		m_ubo->getData( 0u ).renderSize = Point2i{ renderSize };
		m_ubo->getData( 0u ).renderRatio = renderRatio;
	}

	void OverlayUbo::update( int materialIndex )
	{
		m_ubo->getData( 0u ).materialIndex = materialIndex - 1;
	}
}
