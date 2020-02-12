#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	uint32_t const MorphingUbo::BindingPoint = 9u;
	castor::String const MorphingUbo::BufferMorphing = cuT( "Morphing" );
	castor::String const MorphingUbo::Time = cuT( "c3d_time" );

	MorphingUbo::MorphingUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
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
			m_ubo = makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "MorphingUbo" );
		}
	}

	void MorphingUbo::cleanup()
	{
		m_ubo.reset();
	}

	void MorphingUbo::update( float time )const
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.time = time;
		m_ubo->upload();
	}
}
