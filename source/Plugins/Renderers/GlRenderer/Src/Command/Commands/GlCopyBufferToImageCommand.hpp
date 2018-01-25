/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Miscellaneous/BufferImageCopy.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de copie du contenu d'un tampon dans une image.
	*/
	class CopyBufferToImageCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] copyInfo
		*	Les informations de copie.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	L'image destination.
		*/
		CopyBufferToImageCommand( renderer::BufferImageCopy const & copyInfo
			, renderer::BufferBase const & src
			, renderer::TextureView const & dst );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Buffer const & m_src;
		TextureView const & m_dst;
		renderer::BufferImageCopy m_copyInfo;
		GlFormat m_format;
		GlType m_type;
		GlTextureType m_target;
		GlTextureType m_copyTarget;
	};
}
