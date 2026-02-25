#include "Castor3D/Shader/Ubos/FontUbo.hpp"

CU_ImplementSmartPtr( c3d, FontUbo )

namespace c3d
{
	FontUbo::FontUbo( RenderDevice const & device )
		: UboT{ device, MemoryPropertyFlags::eDeviceLocal }
	{
	}

	void FontUbo::cpuUpdate( Size const & imgSize
		, bool sdfFont
		, float pixelRange )
	{
		auto & configuration = getNCData();
		configuration.imgSize->x = float( imgSize->x );
		configuration.imgSize->y = float( imgSize->y );
		configuration.sdfFont = sdfFont ? 1u : 0u;
		configuration.pixelRange = float( pixelRange );
	}
}
