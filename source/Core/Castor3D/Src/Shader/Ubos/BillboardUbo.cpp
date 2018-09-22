#include "BillboardUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	const uint32_t BillboardUbo::BindingPoint = 9u;
	String const BillboardUbo::BufferBillboard = cuT( "Billboard" );
	String const BillboardUbo::Dimensions = cuT( "c3d_dimensions" );

	BillboardUbo::BillboardUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
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
			auto & device = getCurrentDevice( m_engine );
			m_ubo = ashes::makeUniformBuffer< Configuration >( device
				, 1u
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
		}
	}

	void BillboardUbo::cleanup()
	{
		m_ubo.reset();
	}

	void BillboardUbo::update( Point2f const & dimensions )const
	{
		REQUIRE( m_ubo );
		m_ubo->getData( 0u ).dimensions = dimensions;
		m_ubo->upload();
	}
}
