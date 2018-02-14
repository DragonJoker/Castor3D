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
		, m_hash
		{ uint16_t(
			( uint16_t( m_depthClampEnable ) << 15 )
			| ( uint16_t( m_rasteriserDiscardEnable ) << 14 )
			| ( uint16_t( m_polygonMode ) << 11 )
			| ( uint16_t( m_cullMode ) << 9 )
			| ( uint16_t( m_frontFace ) << 7 )
			| ( uint16_t( m_depthBiasEnable ) << 6 )
		) }
	{
	}
}
