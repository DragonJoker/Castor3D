/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Pipeline/ColourBlendState.hpp"

namespace renderer
{
	namespace
	{
		ColourBlendState doCreateDefault()
		{
			ColourBlendState result;
			result.attachs.push_back( ColourBlendStateAttachment{} );
			return result;
		}
	}

	ColourBlendState const & ColourBlendState::createDefault()
	{
		static ColourBlendState const defaultValue = doCreateDefault();
		return defaultValue;
	}
}
