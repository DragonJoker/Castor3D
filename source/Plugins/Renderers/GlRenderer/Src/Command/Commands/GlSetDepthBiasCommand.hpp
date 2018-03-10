/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	class SetDepthBiasCommand
		: public CommandBase
	{
	public:
		SetDepthBiasCommand( float constantFactor
			, float clamp
			, float slopeFactor );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		float m_constantFactor;
		float m_clamp;
		float m_slopeFactor;
	};
}
