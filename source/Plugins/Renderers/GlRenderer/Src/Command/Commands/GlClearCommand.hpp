/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de vidage d'une image.
	*/
	class ClearCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] image
		*	L'image � vider.
		*\param[in] colour
		*	La couleur de vidage.
		*/
		ClearCommand( renderer::TextureView const & image
			, renderer::RgbaColour const & colour );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		TextureView const & m_image;
		renderer::RgbaColour m_colour;
		GlFormat m_format;
		GlType m_type;
	};
}
