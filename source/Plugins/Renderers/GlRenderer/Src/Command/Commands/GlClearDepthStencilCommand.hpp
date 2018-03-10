/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <RenderPass/ClearValue.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de vidage d'une image.
	*/
	class ClearDepthStencilCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] image
		*	L'image à vider.
		*\param[in] colour
		*	La couleur de vidage.
		*/
		ClearDepthStencilCommand( renderer::TextureView const & image
			, renderer::DepthStencilClearValue const & value );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		TextureView const & m_image;
		renderer::DepthStencilClearValue m_value;
		GlInternal m_internal;
		GlFormat m_format;
		GlType m_type;
	};
}
