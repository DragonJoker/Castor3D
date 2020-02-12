#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	uint32_t const OverlayUbo::BindingPoint = 3u;
	castor::String const OverlayUbo::BufferOverlayName = cuT( "Overlay" );
	castor::String const OverlayUbo::BufferOverlayInstance = cuT( "overlay" );
	castor::String const OverlayUbo::PositionRatio = cuT( "c3d_positionRatio" );
	castor::String const OverlayUbo::RenderSizeIndex = cuT( "c3d_renderSizeIndex" );

	OverlayUbo::OverlayUbo( Engine & engine )
		: m_engine{ engine }
		, m_ubo{ makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "OverlayUbo" ) }
	{
	}

	OverlayUbo::~OverlayUbo()
	{
	}

	void OverlayUbo::setPosition( castor::Point2d const & position
		, castor::Size const & renderSize
		, castor::Point2f const & renderRatio )
	{
		m_ubo->getData( 0u ).positionRatio = castor::Point4f{ position[0], position[1], renderRatio[0], renderRatio[1] };
		m_ubo->getData( 0u ).renderSizeIndex[0] = renderSize[0];
		m_ubo->getData( 0u ).renderSizeIndex[1] = renderSize[1];
	}

	void OverlayUbo::update( int materialIndex )
	{
		m_ubo->getData( 0u ).renderSizeIndex[2] = materialIndex - 1;
	}
}
