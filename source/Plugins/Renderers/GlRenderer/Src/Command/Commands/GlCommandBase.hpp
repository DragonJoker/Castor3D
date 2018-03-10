/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	class CommandBase
	{
	public:
		virtual ~CommandBase()noexcept;

		virtual void apply()const = 0;
		virtual CommandPtr clone()const = 0;
	};
}
