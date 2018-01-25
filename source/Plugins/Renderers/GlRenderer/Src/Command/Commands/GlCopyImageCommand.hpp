/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Miscellaneous/ImageCopy.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de copie du contenu d'une image dans une autre.
	*/
	class CopyImageCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] copyInfo
		*	Les informations de copie.
		*\param[in] src
		*	L'image source.
		*\param[in] dst
		*	L'image destination.
		*/
		CopyImageCommand( renderer::ImageCopy const & copyInfo
			, renderer::TextureView const & src
			, renderer::TextureView const & dst );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		TextureView const & m_src;
		TextureView const & m_dst;
		renderer::ImageCopy m_copyInfo;
		GlFormat m_srcFormat;
		GlType m_srcType;
		GlTextureType m_srcTarget;
		GlFormat m_dstFormat;
		GlType m_dstType;
		GlTextureType m_dstTarget;
	};
}
