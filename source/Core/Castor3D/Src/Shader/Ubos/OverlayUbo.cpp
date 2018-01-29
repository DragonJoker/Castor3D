#include "OverlayUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const OverlayUbo::BufferOverlayName = cuT( "Overlay" );
	String const OverlayUbo::BufferOverlayInstance = cuT( "overlay" );
	String const OverlayUbo::Position = cuT( "c3d_position" );
	String const OverlayUbo::RenderSize = cuT( "c3d_renderSize" );
	String const OverlayUbo::RenderRatio = cuT( "c3d_renderRatio" );
	String const OverlayUbo::MaterialIndex = cuT( "c3d_materialIndex" );

	namespace
	{
		renderer::PushConstantArray doGetVariables()
		{
			return
			{
				{ 0u, 0u, renderer::AttributeFormat::eVec2f },
				{ 1u, 8u, renderer::AttributeFormat::eVec2i },
				{ 2u, 16u, renderer::AttributeFormat::eVec2f },
				{ 3u, 24u, renderer::AttributeFormat::eInt },
			};
		}
	}

	OverlayUbo::OverlayUbo( Engine & engine )
		: m_engine{ engine }
		, m_pcb{ renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment
			, doGetVariables() }
	{
	}

	OverlayUbo::~OverlayUbo()
	{
	}

	void OverlayUbo::setPosition( castor::Point2d const & position
		, castor::Size const & renderSize
		, castor::Point2f const & renderRatio )
	{
		m_pcb.getData()->position = Point2f{ position };
		m_pcb.getData()->renderSize = Point2i{ renderSize };
		m_pcb.getData()->renderRatio = renderRatio;
	}

	void OverlayUbo::update( int materialIndex )
	{
		m_pcb.getData()->materialIndex = materialIndex - 1;
	}
}
