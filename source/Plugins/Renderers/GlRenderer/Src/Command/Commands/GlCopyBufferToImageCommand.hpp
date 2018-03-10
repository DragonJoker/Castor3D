/*
This file belongs to RendererLib.
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
		CopyBufferToImageCommand( renderer::BufferImageCopyArray const & copyInfo
			, renderer::BufferBase const & src
			, renderer::Texture const & dst );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		void applyOne( renderer::BufferImageCopy const & copyInfo )const;

	private:
		Buffer const & m_src;
		Texture const & m_dst;
		renderer::BufferImageCopyArray m_copyInfo;
		GlInternal m_internal;
		GlFormat m_format;
		GlType m_type;
		GlTextureType m_copyTarget;
	};
}
