#include "WavesUbo.hpp"

namespace waves
{
	WavesUbo::WavesUbo( c3d::RenderDevice const & device )
		: UboT{ device }
	{
	}

	void WavesUbo::cpuUpdate( WavesConfiguration const & config )
	{
		setData( config );
	}
}
