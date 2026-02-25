#include "FxaaPostEffect/FxaaUbo.hpp"

#include <CastorUtils/Graphics/Size.hpp>

namespace fxaa
{
	//*********************************************************************************************

	c3d::MbString const FxaaUbo::Buffer = "Fxaa";
	c3d::MbString const FxaaUbo::Data = "c3d_fxaaData";

	FxaaUbo::FxaaUbo( c3d::RenderDevice const & device
		, c3d::Size const & size )
		: UboT{ device }
	{
		auto & data = getNCData();
		data.pixelSize = c3d::Point2f{ 1.0f / float( size.getWidth() )
			, 1.0f / float( size.getHeight() ) };
	}

	void FxaaUbo::cpuUpdate( float shift
		, float span
		, float reduce )
	{
		auto & data = getNCData();
		data.subpixShift = shift;
		data.spanMax = span;
		data.reduceMul = reduce;
	}

	//************************************************************************************************
}
