#include "DrawEdgesPostEffect/DrawEdgesUbo.hpp"

namespace draw_edges
{
	//*********************************************************************************************

	c3d::MbString const DrawEdgesUbo::Buffer = "DrawEdges";
	c3d::MbString const DrawEdgesUbo::Data = "c3d_drawEdgesData";

	DrawEdgesUbo::DrawEdgesUbo( c3d::RenderDevice const & device )
		: UboT{ device }
	{
		auto & data = getNCData();
		data.normalDepthWidth = 1;
		data.objectWidth = 1;
	}

	void DrawEdgesUbo::cpuUpdate( int normalDepthWidth
		, int objectWidth )
	{
		auto & data = getNCData();
		data.normalDepthWidth = normalDepthWidth;
		data.objectWidth = objectWidth;
	}

	//************************************************************************************************
}
