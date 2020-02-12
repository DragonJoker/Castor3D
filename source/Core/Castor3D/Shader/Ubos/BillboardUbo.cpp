#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	const uint32_t BillboardUbo::BindingPoint = 11u;
	castor::String const BillboardUbo::BufferBillboard = cuT( "Billboard" );
	castor::String const BillboardUbo::Dimensions = cuT( "c3d_dimensions" );

	BillboardUbo::BillboardUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise();
		}
	}

	BillboardUbo::~BillboardUbo()
	{
	}

	void BillboardUbo::initialise()
	{
		if ( !m_ubo )
		{
			m_ubo = makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "BillboardUbo" );
		}
	}

	void BillboardUbo::cleanup()
	{
		m_ubo.reset();
	}

	void BillboardUbo::update( castor::Point2f const & dimensions )const
	{
		CU_Require( m_ubo );
		m_ubo->getData( 0u ).dimensions = dimensions;
		m_ubo->upload();
	}
}
