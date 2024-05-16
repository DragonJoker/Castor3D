#include "Castor3D/Shader/Ubos/FontUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

CU_ImplementSmartPtr( castor3d, FontUbo )

namespace castor3d
{
	FontUbo::FontUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	FontUbo::~FontUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void FontUbo::cpuUpdate( castor::Size const & imgSize
		, bool sdfFont
		, float pixelRange )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.imgSize->x = float( imgSize->x );
		configuration.imgSize->y = float( imgSize->y );
		configuration.sdfFont = sdfFont ? 1u : 0u;
		configuration.pixelRange = float( pixelRange );
	}
}
