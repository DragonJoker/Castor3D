/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlAttachmentType
		: GLenum
	{
		GL_ATTACHMENT_TYPE_COLOR = 0x1800,
		GL_ATTACHMENT_TYPE_DEPTH = 0x1801,
		GL_ATTACHMENT_TYPE_STENCIL = 0x1802,
		GL_ATTACHMENT_TYPE_DEPTH_STENCIL = 0x84F9,
	};
	std::string getName( GlAttachmentType value );
}
