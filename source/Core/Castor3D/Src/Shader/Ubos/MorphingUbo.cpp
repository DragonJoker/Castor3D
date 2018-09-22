#include "MorphingUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const MorphingUbo::BindingPoint = 7u;
	String const MorphingUbo::BufferMorphing = cuT( "Morphing" );
	String const MorphingUbo::Time = cuT( "c3d_time" );

	MorphingUbo::MorphingUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
		{
			initialise();
		}
	}

	MorphingUbo::~MorphingUbo()
	{
	}

	void MorphingUbo::initialise()
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

	void MorphingUbo::cleanup()
	{
		m_ubo.reset();
	}

	void MorphingUbo::update( float time )const
	{
		REQUIRE( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.time = time;
		m_ubo->upload();
	}
}
