#include "Castor3D/Shader/Ubos/OverlayUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	uint32_t const OverlayUbo::BindingPoint = 3u;
	String const OverlayUbo::BufferOverlayName = cuT( "Overlay" );
	String const OverlayUbo::BufferOverlayInstance = cuT( "overlay" );
	String const OverlayUbo::PositionRatio = cuT( "c3d_positionRatio" );
	String const OverlayUbo::RenderSizeIndex = cuT( "c3d_renderSizeIndex" );

	OverlayUbo::OverlayUbo( Engine & engine )
		: m_engine{ engine }
		, m_ubo{ ashes::makeUniformBuffer< Configuration >( getCurrentDevice( engine )
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible ) }
	{
		getCurrentDevice( engine ).debugMarkerSetObjectName(
			{
				ashes::DebugReportObjectType::eBuffer,
				&m_ubo->getUbo().getBuffer(),
				"OverlayUbo"
			} );
	}

	OverlayUbo::~OverlayUbo()
	{
	}

	void OverlayUbo::setPosition( castor::Point2d const & position
		, castor::Size const & renderSize
		, castor::Point2f const & renderRatio )
	{
		m_ubo->getData( 0u ).positionRatio = Point4f{ position[0], position[1], renderRatio[0], renderRatio[1] };
		m_ubo->getData( 0u ).renderSizeIndex[0] = renderSize[0];
		m_ubo->getData( 0u ).renderSizeIndex[1] = renderSize[1];
	}

	void OverlayUbo::update( int materialIndex )
	{
		m_ubo->getData( 0u ).renderSizeIndex[2] = materialIndex - 1;
	}
}
