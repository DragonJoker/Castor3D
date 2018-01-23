/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline/RasterisationState.hpp"

namespace renderer
{
	RasterisationState::RasterisationState( RasterisationStateFlags flags
		, bool depthClampEnable
		, bool rasteriserDiscardEnable
		, PolygonMode polygonMode
		, CullModeFlags cullMode
		, FrontFace frontFace
		, bool depthBiasEnable
		, float depthBiasConstantFactor
		, float depthBiasClamp
		, float depthBiasSlopeFactor
		, float lineWidth )
		: m_flags{ flags }
		, m_depthClampEnable{ depthClampEnable }
		, m_rasteriserDiscardEnable{ rasteriserDiscardEnable }
		, m_polygonMode{ polygonMode }
		, m_cullMode{ cullMode }
		, m_frontFace{ frontFace }
		, m_depthBiasEnable{ depthBiasEnable }
		, m_depthBiasConstantFactor{ depthBiasConstantFactor }
		, m_depthBiasClamp{ depthBiasClamp }
		, m_depthBiasSlopeFactor{ depthBiasSlopeFactor }
		, m_lineWidth{ lineWidth }
	{
	}
}
